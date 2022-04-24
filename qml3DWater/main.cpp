#include <QGuiApplication>
#include <Qt3DQuickExtras/Qt3DQuickWindow>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QQmlEngine>
#include <QQmlContext>

#include <QOpenGLContext>
#include <QGLFormat>


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL)
    {
        qDebug() << "QOpenGLContext::openGLModuleType LibGL";
    }

    if (QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGLES)
    {
        qDebug() << "QOpenGLContext::openGLModuleType LibGLES";
    }

    //qDebug() << "OpenGL flags:" << QGLFormat::openGLVersionFlags();

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 3);
    format.setDepthBufferSize(32);
    format.setSamples(0);
    format.setStencilBufferSize(8);
    QSurfaceFormat::setDefaultFormat(format);

    qDebug() << "QSurfaceFormat version=" << QSurfaceFormat::defaultFormat().version() <<
                    " profile=" << QSurfaceFormat::defaultFormat().profile() <<
                    " depthBufferSize=" << QSurfaceFormat::defaultFormat().depthBufferSize() <<
                    " samples=" << QSurfaceFormat::defaultFormat().samples() <<
                    "stencilBufferSize=" << QSurfaceFormat::defaultFormat().stencilBufferSize();

    Qt3DExtras::Quick::Qt3DQuickWindow view;

    view.setSource(QUrl("qrc:/main.qml"));
    view.resize(1600, 1280);
    view.show();

    return app.exec();
}
