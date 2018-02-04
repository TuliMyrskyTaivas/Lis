////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	C:\Users\iceni\Work\Lis\Lis\GlWindow.cpp
//
// summary:	Implements the gl Windows Form
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GlWindow.h"

#include <QtGui/QPainter>
#include <QtGui/QOpenGLContext>
#include <QtCore/QDebug>

namespace Lis
{
////////////////////////////////////////////////////////////////////////////////////////////////////
GlWindow::GlWindow(QWindow* parent)
    : QWindow(parent)
    , m_animating(false)
    , m_context(nullptr)
    , m_device(nullptr)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::render(QPainter* painter)
{
    Q_UNUSED(painter);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::initialize()
{
    // Any OpenGL initialization needed can be done by overriding the initialize() function,
    // which is called once before the first call to render(),
    // with a valid current QOpenGLContext.
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::render()
{
    if (!m_device)
        m_device = std::make_unique<QOpenGLPaintDevice>();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_device->setSize(size());

    QPainter painter(m_device.get());
    render(&painter);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::renderLater()
{
    requestUpdate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool GlWindow::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::exposeEvent(QExposeEvent* event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;
    if (!m_context)
    {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);
    if (needsInitialize)
    {
        initializeOpenGLFunctions();
        initialize();

        const char* glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        const char* glgsVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
        qInfo() << "GL vendor: " << glVendor;
        qInfo() << "GL version: " << glVersion;
        qInfo() << "GLGS version: " << glgsVersion;
    }

    render();

    m_context->swapBuffers(this);
    if (m_animating)
        renderLater();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::setAnimating(bool animating)
{
    m_animating = animating;
    if (m_animating)
        renderLater();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GlWindow::setCurrentContext()
{
    m_context->makeCurrent(this);
}
}
