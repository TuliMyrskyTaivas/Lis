////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	lis.cpp
//
// summary:	planetary weather phenomena emulator
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>

// Qt part
#include <QtGui/QGuiApplication>
#include <QtGui/QSurfaceFormat>

#include "PlanetWindow.h"
#include "Logger.h"

int main(int argc, char *argv[])
try
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);
    format.setOption(QSurfaceFormat::DebugContext);

    Lis::PlanetWindow window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
catch (const std::exception& e)
{
    Lis::Logger::GetInstance().Error() << "terminated: " << e.what();
    return EXIT_FAILURE;
}
