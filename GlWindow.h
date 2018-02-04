////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Lis/GlWindow.h
//
// summary:	Declares the gl Windows Form
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLPaintDevice>

#include <memory>

class QPainter;
class QOpenGLContext;

namespace Lis
{

class GlWindow : public QWindow, protected QOpenGLFunctions
{
	Q_OBJECT
public :
	explicit GlWindow(QWindow* parent = 0);
	virtual ~GlWindow() = default;
	
	virtual void render(QPainter* painter);
	virtual void render();
	virtual void initialize();

	////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Use setAnimating(true) for render() to be called
	/// 			at the vertical refresh rate, assuming vertical sync is enabled
	/// 			in the underlying OpenGL drivers.
	/// </summary>
	///
	/// <remarks>	Andrey Sploshnov, 23.12.2017. </remarks>
	///
	/// <param name="animating">	true to animating. </param>
	////////////////////////////////////////////////////////////////////////////////
	void setAnimating(bool animating);

    void setCurrentContext();
	
public slots :
	void renderLater();
	void renderNow();
	
protected:
	bool event(QEvent* event) override;
	void exposeEvent(QExposeEvent* event) override;
	
private:
	bool m_animating;
	QOpenGLContext* m_context;
	std::unique_ptr<QOpenGLPaintDevice> m_device;	
};

} // namespace Lis
