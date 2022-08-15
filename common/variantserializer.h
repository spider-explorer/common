#ifndef VARIANTSERIALIZER_H
#define VARIANTSERIALIZER_H

#include <QtCore>
#include <string>
#include "MemoryModule.h"

class VariantSerializer
{
public:
    VariantSerializer();
    QByteArray serialize(const QVariant &x);
    QVariant deserialize(const QByteArray &x);
    QString serializeToString(const QVariant &x, bool verbose = false);
    QVariant deserializeFromString(const QString &x);
    std::string serializeToStdString(const QVariant &x, bool verbose = false);
    QVariant deserializeFromStdString(const std::string &x);
    QByteArray serializeToBinary(const QVariant &x);
    QVariant deserializeFromBinary(const QByteArray &x);
};

class VariantLibrary: public QObject
{
    Q_OBJECT
public:
#if 0x0
    VariantLibrary(const QString& fileName, QObject *parent = nullptr)
        : QObject(parent), m_fileName(fileName)
    {
        m_library = new QLibrary(m_fileName, this);
    }
#else
    VariantLibrary(const QFile &file, QObject *parent = nullptr)
        : QObject(parent), m_fileName(file.fileName())
    {
        m_library = new QLibrary(m_fileName, this);
    }
#endif
    VariantLibrary(const QByteArray& bytes, QObject *parent = nullptr)
        : QObject(parent)
    {
        m_memory = ::MemoryLoadLibrary(bytes.constData(), bytes.size());
    }
    QVariant call(const QString &funcName, const QVariant args)
    {
        typedef const char *(*proto_func)(const char *args);
        if (m_library)
        {
            //QLibrary myLib(m_fileName);
            proto_func func = (proto_func) m_library->resolve(funcName.toLatin1());
            if (!func) return QVariant();
            std::string argsSS = VariantSerializer().serializeToStdString(args);
            std::string resultSS = func(argsSS.c_str());
            return VariantSerializer().deserializeFromStdString(resultSS);
        }
        else if (m_memory)
        {
            proto_func func = (proto_func) MemoryGetProcAddress(m_memory, funcName.toLatin1().constData());
            if (!func) return QVariant();
            std::string argsSS = VariantSerializer().serializeToStdString(args);
            std::string resultSS = func(argsSS.c_str());
            return VariantSerializer().deserializeFromStdString(resultSS);
        }
        return QVariant();
    }
protected:
    QString m_fileName;
    QLibrary *m_library = nullptr;
    HMEMORYMODULE m_memory = nullptr;
};

#define VARIANT_GET_ARGS() QVariant args = VariantSerializer().deserializeFromStdString(__args__); \
                           QVariant result;
#define VARIANT_RET_RESULT() static thread_local std::string __result__; \
                             __result__ = VariantSerializer().serializeToStdString(result); \
                             return __result__.c_str();

#endif // VARIANTSERIALIZER_H
