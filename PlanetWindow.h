////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Lis/PlanetWindow.h
//
// summary:	Declares the planet widget
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GlWindow.h"
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLDebugLogger>
#include <QtGui/QOpenGLVertexArrayObject>

#include <memory>
#include <vector>

namespace Lis
{

class PlanetWindow : public GlWindow
{
    Q_OBJECT
public:
	PlanetWindow();
	virtual ~PlanetWindow();
	
	void initialize() override;
	void render() override;

public slots:
    void onGLDebugMessage(QOpenGLDebugMessage message);

private:
	void loadShader(QOpenGLShader::ShaderType type, const std::string& name);
	void generateSphereVertices();
		
	GLuint m_matrixUniform = 0;
	
	GLuint m_numLatLines = 40;
	GLuint m_numLongLines = 40;
	GLfloat m_radius = 0.7f;

    int	m_frame = 0;
	
    std::vector<GLuint> m_indexes;
	std::vector<QVector3D> m_vertices;
	std::vector<QVector2D> m_texCoords;

    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_colorBuffer;
    QOpenGLBuffer m_indexBuffer;

    std::unique_ptr<QOpenGLVertexArrayObject> m_vao;	
	std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLDebugLogger> m_glLogger;
    std::unique_ptr<QOpenGLTexture> m_texture;
};

} // namespace Lis