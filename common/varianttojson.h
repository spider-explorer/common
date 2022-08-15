#ifndef VARIANTTOJSON_H
#define VARIANTTOJSON_H

#include <QtCore>

static QString variantToJson(const QVariant &v, bool pretty)
{
    QJsonDocument jd = QJsonDocument::fromVariant(v);
    return QString::fromUtf8(jd.toJson(pretty ? QJsonDocument::Indented : QJsonDocument::Compact));
}

static QVariant jsonToVariant(const QString &s)
{
    QJsonDocument jd = QJsonDocument::fromJson(s.toUtf8());
    return jd.toVariant();
}

#endif // VARIANTTOJSON_H
