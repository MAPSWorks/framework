#include <QApplication>
#include <QSurfaceFormat>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    QSurfaceFormat glFormat;
    glFormat.setSamples(4);
    glFormat.setVersion( 4, 0 );
    glFormat.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    glFormat.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
    QSurfaceFormat::setDefaultFormat(glFormat);

    // MainWindow is a singleton class
    MainWindow::getInstance().showNormal();

    return application.exec();
}
