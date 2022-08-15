#ifndef QDEBUG_H
#define QDEBUG_H

#include <QtCore>

#if 0x0
QDebug& operator<<(QDebug& out, const std::string& str)
{
    out << QString::fromStdString(str);
    return out;
}
#endif

#endif // QDEBUG_H
