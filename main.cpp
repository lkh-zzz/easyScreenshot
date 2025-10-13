#include <QApplication>
#include "screenshotwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ScreenshotWidget w;
    w.show();

    return app.exec();
}
