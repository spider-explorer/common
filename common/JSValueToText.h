#ifndef JSVALUETOTEXT_H
#define JSVALUETOTEXT_H

#include <QtCore>
#include <QtQml>

static inline QString escapeChar(const QChar &c)
{
    // https://en.cppreference.com/w/cpp/language/escape
    wchar_t wc = c.unicode();
    switch(wc)
    {
    case L'"':
        return "\\\"";
    case L'\\':
        return "\\\\";
    case L'\a':
        return "\\a";
    case L'\b':
        return "\\b";
    case L'\f':
        return "\\f";
    case L'\n':
        return "\\n";
    case L'\r':
        return "\\r";
    case L'\t':
        return "\\t";
    case L'\v':
        return "\\v";
    default:
        return QString(c);
        break;
    }
}
static inline QString escapeString(const QString &s)
{
    QString result = "\"";
    for(int i=0; i<s.size(); i++)
    {
        result += escapeChar(s[i]);
    }
    result += "\"";
    return result;
}
static inline QString JSValueToText(const QJSValue &x)
{
    if(x.isVariant())
    {
        return x.toVariant().toString();
    }
    else if(x.isQObject())
    {
        QObject *qobj = x.toQObject();
        return QString("QObject(%1)").arg(qobj->metaObject()->className());
    }
    else if(x.isRegExp())
    {
        return x.toString();
    }
    else if(x.isDate())
    {
        return QString("Date(%1)").arg(x.toDateTime().toString("yyyy-MM-ddThh:mm:ss.z"));
    }
    else if(x.isArray())
    {
        const QJSValue &array = x;
        QString result = "[";
        qint32 length = array.property("length").toInt();
        qint32 i;
        for(i = 0; i < length; i++)
        {
            if(i>0) result += ", ";
            result += JSValueToText(array.property(i));
        }
        QJSValueIterator it(array);
        while(it.hasNext()) {
            it.next();
            QString name = it.name();
            if(name == "length") continue;
            bool ok;
            name.toInt(&ok);
            if(ok) continue;
            if(i>0) result += ", ";
            result += JSValueToText(name);
            result += ": ";
            result += JSValueToText(it.value());
            i++;
        }
        result += "]";
        return result;
    }
    else if(x.isObject())
    {
        if(x.toString().startsWith("function"))
        {
            QString name = x.property("name").toString();
            if(name.isEmpty()) return "<anonymous function>";
            return QString("<function %1>").arg(name);
        }
        const QJSValue &object = x;
        QJSValueIterator it(object);
        QString result = "{";
        qint64 i = 0;
        while(it.hasNext()) {
            it.next();
            if(i>0) result += ", ";
            QString name = it.name();
            result += escapeString(name);
            result += ": ";
            result += JSValueToText(it.value());
            i++;
        }
        result += "}";
        return result;
    }
    else if(x.isString())
    {
        return escapeString(x.toString());
    }
    else
    {
        return x.toString();
    }
}

#endif // JSVALUETOTEXT_H
