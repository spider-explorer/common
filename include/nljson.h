#ifndef NLJSON_H
#define NLJSON_H

#include <nlohmann/json.hpp>
using nljson = nlohmann::json;

static inline nlohmann::json::binary_t create_json_binary(const std::vector<std::uint8_t> &v)
{
    nlohmann::json::binary_t result(v);
#if 0x0
    for(size_t i=0; i<v.size(); i++)
    {
        result.push_back(v[i]);
    }
#endif
    return result;
}

static inline nlohmann::json::binary_t create_json_binary(const std::string &s, std::uint64_t sub_type = 0)
{
    std::vector<uint8_t> vec(s.begin(), s.end());
    nlohmann::json::binary_t result = nlohmann::json::binary_t(vec);
    result.set_subtype(sub_type);
    return result;
}

static inline std::string json_binary_to_string(const nlohmann::json::binary_t &bin)
{
    return std::string(bin.begin(), bin.end());
}

static inline std::vector<std::uint8_t> json_binary_to_vector(const nlohmann::json::binary_t &bin)
{
    return std::vector<std::uint8_t>(bin.begin(), bin.end());
}

static inline std::string json_binary_as_string(const nlohmann::json &x)
{
    return json_binary_to_string(x.get<nlohmann::json::binary_t>());
}

static inline std::vector<std::uint8_t> json_binary_as_vector(const nlohmann::json &x)
{
    return json_binary_to_vector(x.get<nlohmann::json::binary_t>());
}

//#include <QtCore>
#if defined(Q_UNUSED)
#include <QtCore>
static inline QString toQString(const nlohmann::json &x, const int indent = -1)
{
    return QString::fromStdString(x.dump(indent));
}
static inline QVariant convert_json_to_variant(const nlohmann::json &x)
{
    QString type;
    switch (x.type())
    {
    case nlohmann::json::value_t::null: ///< null value
    {
        type = "null";
        return QVariant();
    }
        break;
    case nlohmann::json::value_t::object: ///< object (unordered set of name/value pairs)
    {
        type = "object";
        QVariantMap result;
        for (auto &el : x.items())
        {
            result[QString::fromStdString(el.key())] = convert_json_to_variant(el.value());
        }
        return result;
    }
        break;
    case nlohmann::json::value_t::array: ///< array (ordered collection of values)
    {
        type = "array";
        QVariantList result;
        for (auto &el : x)
        {
            result.append(convert_json_to_variant(el));
        }
        return result;
    }
        break;
    case nlohmann::json::value_t::string: ///< string value
    {
        type = "string";
        return QString::fromStdString(x.get<std::string>());
    }
        break;
    case nlohmann::json::value_t::boolean: ///< boolean value
    {
        type = "boolean";
        return x.get<bool>();
    }
        break;
    case nlohmann::json::value_t::number_integer: ///< number value (signed integer)
    {
        type = "integer";
        return x.get<std::int64_t>();
    }
        break;
    case nlohmann::json::value_t::number_unsigned: ///< number value (unsigned integer)
    {
        type = "unsigned";
        return x.get<std::uint64_t>();
    }
        break;
    case nlohmann::json::value_t::number_float: ///< number value (floating-point)
    {
        type = "float";
        return x.get<double>();
    }
        break;
    case nlohmann::json::value_t::binary: ///< binary array (ordered collection of bytes)
    {
        type = "binary";
        const nlohmann::json::binary_t &bin = x.get_binary();
        std::string s(bin.begin(), bin.end());
        QByteArray bytes = QByteArray::fromStdString(s);
        if (bin.subtype() == 1)
        {
            QVariant v;
            QDataStream in(&bytes, QIODevice::ReadOnly);
            in.setVersion(QDataStream::Qt_5_1);
            in >> v;
            return v;
        }
        return bytes;
    }
        break;
    case nlohmann::json::value_t::discarded: ///< discarded by the parser callback function
    {
        type = "discarded";
    }
        break;
    default:
    {
        type = "?";
    }
        break;
    }
    return type;
}
static inline nlohmann::json convert_variant_to_json(const QVariant &x)
{
    QString typeName = x.typeName();
    nlohmann::json result = typeName.toStdString();
    if (!x.isValid() || x.isNull())
    {
        result = nullptr;
    }
    else if (typeName == "QVariantMap")
    {
        result = nlohmann::json::object();
        QVariantMap map = x.toMap();
        QStringList keys = map.keys();
        foreach(QString key, keys)
        {
            result[key.toStdString()] = convert_variant_to_json(map[key]);
        }
    }
    else if (typeName == "QVariantList")
    {
        result = nlohmann::json::array();
        QVariantList list = x.toList();
        for(int i=0; i<list.size(); i++)
        {
            result[i] = convert_variant_to_json(list[i]);
        }
    }
    else if (typeName == "double")
    {
        result = x.toDouble();
    }
    else if (typeName == "qlonglong")
    {
        result = x.toLongLong();
    }
    else if (typeName == "qulonglong")
    {
        result = x.toULongLong();
    }
    else if (typeName == "bool")
    {
        result = x.toBool();
    }
    else if (typeName == "QString")
    {
        result = x.toString().toStdString();
    }
    else if (typeName == "QByteArray")
    {
        std::string s = x.toByteArray().toStdString();
        result =  create_json_binary(s);
    }
    else
    {
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_1);
        out << x;
        std::string s = bytes.toStdString();
        result =  create_json_binary(s, 1);
    }
    return result;
}
static inline QJsonDocument convert_json_to_jsondoc(const nlohmann::json &x)
{
    QVariant v = convert_json_to_variant(x);
    return QJsonDocument::fromVariant(v);
}
static inline nlohmann::json convert_jsondoc_to_json(const QJsonDocument &x)
{
    QVariant v = x.toVariant();
    return convert_variant_to_json(v);
}
#endif

static inline std::string convert_json_to_msgpack(const nlohmann::json &x)
{
    std::vector<std::uint8_t> mpack = nlohmann::json::to_msgpack(x);
    return std::string((const char *)mpack.data(), mpack.size());
}
static inline nlohmann::json convert_msgpack_to_json(const std::string &x)
{
    std::vector<std::uint8_t> mpack(x.begin(), x.end());
    return nlohmann::json::from_msgpack(mpack);
}

#if defined(Q_UNUSED)
static inline std::string convert_variant_to_msgpack(const QVariant &x)
{
    return convert_json_to_msgpack(convert_variant_to_json(x));
}
static inline QVariant convert_msgpack_to_variant(const std::string &x)
{
    return convert_json_to_variant(convert_msgpack_to_json(x));
}
#endif

#endif // NLJSON_H
