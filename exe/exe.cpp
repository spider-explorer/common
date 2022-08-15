#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "utf8LogHandler.h"
#include "debug_line.h"
//#include "jnetwork.h"
#include <windows.h>
#include "variantserializer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qInstallMessageHandler(utf8LogHandler);
    qdebug_line1("Hello World!2");
    VariantLibrary lib(QFile(R"***(../dll/dll-x86_64-static.dll)***"));
    qDebug() << lib.call("add2", QVariantList() << 22 << 33);
    QFile file(R"***(../dll/dll-x86_64-static.dll)***");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray bytes = file.readAll();
        VariantLibrary lib2(bytes);
        qDebug() << lib2.call("add2", QVariantList() << 44 << 55);
    }
    qDebug() << lib.call("returnArgs", QVariantList {1, 2, QDateTime::currentDateTime(), QVariant()});
    return 0; // return app.exec();
}
