#ifdef QT_STATIC_BUILD
#include <QtCore>
#include "utf8LogHandler.h"
#include "debug_line.h"
#include "strconvEx.h"
class MyQt
{
public:
    QCoreApplication *app = nullptr;
    explicit MyQt()
    {
        std::vector<char *>argv;
        argv = get_ansi_args();
        int argc = argv.size();
        this->app = new QCoreApplication(argc, &argv[0]);
        qInstallMessageHandler(utf8LogHandler);
    }
    virtual ~MyQt()
    {
        delete this->app;
    }
};
#endif // QT_STATIC_BUILD

#include "variantserializer.h"

#if 0x0
extern "C" __declspec(dllexport) int add2(int a, int b)
{
    return a + b;
}
#endif

extern "C" __declspec(dllexport) const char *add2(const char *__args__)
{
    VARIANT_GET_ARGS();
    {
        qDebug() << "add2()" << args;
        result = args.toList()[0].toInt() + args.toList()[1].toInt();
        qDebug() << result;
    }
    VARIANT_RET_RESULT();
}

extern "C" __declspec(dllexport) const char *returnArgs(const char *__args__)
{
    VARIANT_GET_ARGS();
    {
        qDebug() << "returnArgs()" << args;
        result = args;
        qDebug() << result;
    }
    VARIANT_RET_RESULT();
}

extern "C" __declspec(dllexport) const char *returnDateTime(const char *__args__)
{
    VARIANT_GET_ARGS();
    {
        qDebug() << "returnDateTime()" << args;
        result = QDateTime::currentDateTime();
        qDebug() << result;
    }
    VARIANT_RET_RESULT();
}

typedef void (*cb)(int x);

extern "C" __declspec(dllexport) const char *cbTest(const char *__args__)
{
    VARIANT_GET_ARGS();
    {
        qDebug() << "cbTest()" << args;
        cb p = (cb)args.toULongLong();
        p(1234);
        p(-7777);
        //result = args;
        qDebug() << result;
    }
    VARIANT_RET_RESULT();
}

extern "C" __declspec(dllexport) const char *cbTest2(const char *__args__)
{
    VARIANT_GET_ARGS();
    {
        qDebug() << "cbTest2()" << args;
        QVariantList list = args.toList();
        QVariant cb = list[0];
        QVariant n = list[1];
        result = VARIANT_RUN_CALLBACK(cb, n);
        qDebug() << result;
    }
    VARIANT_RET_RESULT();
}
