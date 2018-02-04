////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Lis/PlanetWindow.cpp
//
// summary:	Implements the planet widget
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PlanetWindow.h"
#include "Logger.h"

#include <QtGui/QScreen>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>

#include <stdexcept>
#include <cassert>
#include <cmath>

namespace Lis
{
////////////////////////////////////////////////////////////////////////////////////////////////////
PlanetWindow::PlanetWindow()
    : m_positionBuffer(QOpenGLBuffer::VertexBuffer)
    , m_indexBuffer(QOpenGLBuffer::IndexBuffer)
    , m_colorBuffer(QOpenGLBuffer::PixelPackBuffer)
    , m_vao(new QOpenGLVertexArrayObject(this))
    , m_program(new QOpenGLShaderProgram(this))
    , m_glLogger(new QOpenGLDebugLogger(this))
{
    generateSphereVertices();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
PlanetWindow::~PlanetWindow()
{
    // make sure the context is current when deleting the texture
    // and the buffers
    setCurrentContext();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PlanetWindow::initialize()
{
    assert(!m_texture && "PlanetWindows::initialize seems to be called twice");

    // initialize the logger
    if (m_glLogger->initialize())
    {
        connect(m_glLogger.get(), SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
            SLOT(onGLDebugMessage(QOpenGLDebugMessage)), Qt::DirectConnection);
    }

    // load the embedded texture
    m_texture = std::make_unique<QOpenGLTexture>(QImage(QString(":/images/land_ocean_ice_2048.jpg")));

    // load and link the shader program
    loadShader(QOpenGLShader::Vertex, "/vertex.shader");
    loadShader(QOpenGLShader::Fragment, "/fragment.shader");
    if (!m_program->link())
        throw std::runtime_error("failed to link shader program: " +
            m_program->log().toStdString());

    m_matrixUniform = m_program->uniformLocation("matrix");

    // Create VAO for the first object to render
    m_vao->create();
    // Setup VBO and IBO. These will be remembered by the currently bound VAO
    m_vao->bind();

    // VBO
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_positionBuffer.bind();
    const int posBufSize = static_cast<int>(m_vertices.size() * 3 * sizeof(float));
    m_positionBuffer.allocate(m_vertices.data(), posBufSize);

    m_program->enableAttributeArray("vertexPosition");
    m_program->setAttributeBuffer("vertexPosition", GL_FLOAT, 0, 3);

    // IBO
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.bind();
    const int idxBufSize = static_cast<int>(m_indexes.size() * sizeof(GLuint));
    m_indexBuffer.allocate(m_indexes.data(), idxBufSize);

    m_colorBuffer.create();
    m_colorBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_colorBuffer.bind();
    const int colorBufSize = static_cast<int>(m_texCoords.size() * 2 * sizeof(float));
    m_colorBuffer.allocate(m_texCoords.data(), colorBufSize);

    m_program->enableAttributeArray("texCoord");
    m_program->setAttributeBuffer("texCoord", GL_FLOAT, 0, 4);

    m_vao->release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PlanetWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!m_program->bind())
        throw std::runtime_error("failed to bind the shader program to active GL context");

    // Calculate the rotation matrix
    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(20.0f * m_frame / screen()->refreshRate(), 0, 1, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);

    // Draw the mesh
    m_vao->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexes.size()), GL_UNSIGNED_INT, 0);
    m_vao->release();

    m_program->release();

    ++m_frame;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PlanetWindow::loadShader(QOpenGLShader::ShaderType type, const std::string& path)
{
    const QString shaderPath = QCoreApplication::applicationDirPath() + path.c_str();
    if (!m_program->addCacheableShaderFromSourceFile(type, shaderPath))
        throw std::runtime_error("failed to compile the shader " + shaderPath.toStdString()
            + ": " + m_program->log().toStdString());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PlanetWindow::onGLDebugMessage(QOpenGLDebugMessage message)
{
    Logger::GetInstance().Debug() << message.message().toStdString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void PlanetWindow::generateSphereVertices()
{
    static float pi = std::atan(1) * 4;

    // North pole
    m_vertices.emplace_back(QVector3D(0, m_radius, 0));
    m_texCoords.emplace_back(QVector2D(0, 1));

    // +1.0f because there's a gap between the poles and the first parallel
    const float latitudeSpacing = 1.0f / (m_numLatLines + 1.0f);
    const float longitudeSpacing = 1.0f / (m_numLongLines);

    for (GLuint latitude = 0; latitude < m_numLatLines; ++latitude)
    {
        for (GLuint longitude = 0; longitude < m_numLongLines; ++longitude)
        {
            // Scale coordinates into the 0..1 texture coordinate range,
            // with north at the top (y = 1).
            const float x = longitude * longitudeSpacing;
            const float y = 1.0f - (latitude + 1) * latitudeSpacing;
            m_texCoords.emplace_back(QVector2D(x, y));

            // Convert to spherical coordinates:
            // theta is a longitude angle (around the equator) in radians;
            // phi is a latitude angle (north or south of the equator)
            const float theta = x * 2.0f * pi;
            const float phi = (y - 0.5f) * pi;

            // This determines the radius of the ring of this line of latitude.
            // It's widest at the equator, and narrows as phi increases/decreases
            const float c = std::cos(phi);

            // The current coordinate
            m_vertices.emplace_back(QVector3D(c * std::cos(theta),
                std::sin(phi),
                c * sin(theta)) * m_radius);

            // The indexes of adjacent vertices to draw the triangles
            m_indexes.push_back(latitude * m_numLongLines + (longitude % m_numLongLines));
            m_indexes.push_back((latitude + 1) * m_numLongLines + (longitude % m_numLongLines));
        }
    }

    // South pole
    m_vertices.emplace_back(QVector3D(0, -m_radius, 0));
    m_texCoords.emplace_back(QVector2D(0, 0));
}
} // namespace Lis
