#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "utf8LogHandler.h"
#include "debug_line.h"
//#include "jnetwork.h"
#include "msgpack.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qInstallMessageHandler(utf8LogHandler);
    qdebug_line1("Hello World!");
    //JNetworkManager nm;
    //qDebug().noquote() << nm.getBatchAsText(QUrl("https://raw.githubusercontent.com/cyginst/cyginst-v1/master/cyginst.bat"));
    QVariantList list;
    list << 1 << 2 << 3 << "漢字";
    QByteArray array = MsgPack::pack(list);
    QVariantList unpacked = MsgPack::unpack(array).toList();
    qDebug() << array.toBase64();
    qDebug() << unpacked;
    auto cbv = QCborValue::fromVariant(list);
    array = cbv.toCbor();
    cbv = QCborValue::fromCbor(array);
    qDebug() << cbv.toVariant().toList();
    return 0; // return app.exec();
}
