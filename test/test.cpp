#include <QtCore>
#include "utf8LogHandler.h"
#include "debug_line.h"
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define MAX_SAFE_INTEGER 9007199254740991
#define MIN_SAFE_INTEGER -9007199254740991

static inline nlohmann::json variant_to_jcon(const QVariant &x)
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
            result[key.toStdString()] = variant_to_jcon(map[key]);
        }
        if (result.contains("!"))
        {
            nlohmann::json parent;
            parent["!"] = "dictionary";
            parent["?"] = result;
            result = parent;
        }
    }
    else if (typeName == "QVariantList")
    {
        result = nlohmann::json::array();
        QVariantList list = x.toList();
        for(int i=0; i<list.size(); i++)
        {
            result[i] = variant_to_jcon(list[i]);
        }
    }
    else if (typeName == "float"||
             typeName == "double")
    {
        result = x.toDouble();
    }
    else if (typeName == "int" ||
             typeName == "qlonglong")
    {
        qlonglong value = x.toLongLong();
        result = value;
        if (value > MAX_SAFE_INTEGER)
        {
            result = nlohmann::json::object();
            result["!"] = "bigint";
            result["?"] = x.toString().toStdString();
        }
        else if (value < MIN_SAFE_INTEGER)
        {
            result = nlohmann::json::object();
            result["!"] = "bigint";
            result["?"] = x.toString().toStdString();
        }
    }
    else if (typeName == "uint" ||
             typeName == "qulonglong")
    {
        qulonglong value = x.toLongLong();
        result = value;
        if (value > MAX_SAFE_INTEGER)
        {
            result = nlohmann::json::object();
            result["!"] = "bigint";
            result["?"] = x.toString().toStdString();
        }
    }
    else if (typeName == "bool")
    {
        result = x.toBool();
    }
    else if (typeName == "QString")
    {
        result = x.toString().toUtf8().toStdString();
    }
    else if (typeName == "QByteArray")
    {
        std::string s = x.toByteArray().toBase64().toStdString();
        result = nlohmann::json::object();
        result["!"] = "bytearray";
        result["?"] = s;
    }
    else if (typeName == "QDateTime")
    {
        QDateTime dt = x.toDateTime();
        result = nlohmann::json::object();
        result["!"] = "datetime";
        result["?"] = dt.toMSecsSinceEpoch();
    }
    else
    {
        return typeName.toStdString();
    }
    return result;
}

static inline QVariant jcon_to_variant(const nlohmann::json &x)
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
            result[QString::fromStdString(el.key())] = jcon_to_variant(el.value());
        }
        if (result.contains("!"))
        {
            if (result["!"].toString()=="dictionary")
            {
                return result["?"];
            }
            else if (result["!"].toString()=="bigint")
            {
                QString n = result["?"].toString();
                if (n.startsWith("-"))
                {
                    return result["?"].toLongLong();
                }
                return result["?"].toULongLong();
            }
            else if (result["!"].toString()=="bytearray")
            {
                return QByteArray::fromBase64(result["?"].toByteArray());
            }
            else if (result["!"].toString()=="datetime")
            {
                QDateTime dt;
                dt.setMSecsSinceEpoch(result["?"].toLongLong());
                return dt;
            }
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
            result.append(jcon_to_variant(el));
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(utf8LogHandler);
    qdebug_line1("Hello World!");

    QVariant var(QDateTime::currentDateTime());
    json j;
    j = variant_to_jcon(var);
    std::cerr << j << std::endl << std::flush;
    qDebug() << jcon_to_variant(j);
    j = variant_to_jcon(QByteArray("abc"));
    qDebug() << jcon_to_variant(j);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon((float)1.23);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon((short)-123);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon((int)-124);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon((qint8)-125);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon((qint16)-126);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon((qint64)-127);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon(9007199254740992ULL);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon(-9007199254740992LL);
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon(QVariantMap {{"a", 123},{"b", 456}});
    std::cerr << j << std::endl << std::flush;
    j = variant_to_jcon(QVariantMap {{"!", 123},{"b", 456}});
    std::cerr << j << std::endl << std::flush;
    qDebug() << jcon_to_variant(j);

    return 0;
}




#if 0x0
#include <QtCore>
#include "utf8LogHandler.h"
#include "debug_line.h"
#include <iostream>

//#include <nlohmann/json.hpp>
//using json = nlohmann::json;

#include "qcbor/qcbor_encode.h"
#include "qcbor/qcbor_decode.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(utf8LogHandler);
    qdebug_line1("Hello World!");

    QCBOREncodeContext EC;
    static uint8_t spBigBuf[2200];
    QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
    //UsefulBuf_MAKE_STACK_UB(MemoryForEncoded2, 20);
    //QCBOREncode_Init(&EC, MemoryForEncoded2);

    QCBOREncode_OpenMap(&EC);
    QCBOREncode_AddBoolToMapN(&EC, 66, true);
    QCBOREncode_AddBoolToMap(&EC, "7777", false);
    QCBOREncode_AddDateEpochToMap(&EC, "8888", 1);
    QCBOREncode_CloseMap(&EC);
    UsefulBufC Encoded;
    if(QCBOREncode_Finish(&EC, &Encoded)) {
       return -1;
    }

    std::cerr << Encoded.len << std::endl;
    QByteArray bytes((char *)Encoded.ptr, Encoded.len);
    qDebug() << QCborValue::fromCbor(bytes).toVariant();

    // Decode it and see that is right
    QCBORDecodeContext DC;
    QCBORItem Item;
    QCBORDecode_Init(&DC, Encoded, QCBOR_DECODE_MODE_NORMAL);
    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_MAP) {
       return -2;
    }
    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_TRUE) {
       return -3;
    }
    if(QCBORDecode_Finish(&DC)) {
       return -4;
    }

    //json j;
    //j["pi"] = 3.141;
    //j["happy"] = true;
    //j["name"] = "Niels";
    //j["nothing"] = nullptr;
    //j["answer"]["everything"] = 42;  // 存在しないキーを指定するとobjectが構築される
    //j["list"] = { 1, 0, 2 };         // [1,0,2]
    //j["object"] = { {"currency", "USD"}, {"value", 42.99} };  // {"currentcy": "USD", "value": 42.99}

#if 0x0
    //std::cout << j << std::endl;
    //std::vector<std::uint8_t> cbor = json::to_cbor(j);
    std::string s(cbor.begin(), cbor.end());
    QByteArray bytes(&s[0], s.size());
    qDebug() << QCborValue::fromCbor(bytes).toVariant();
    QVariant var(QDateTime::currentDateTime());
    bytes = QCborValue::fromVariant(var).toCbor();
    s = bytes.toStdString();
    std::vector<std::uint8_t> cbor2(s.begin(), s.end());
    //json j2 = json::from_cbor(cbor2);
    //std::cerr << j2 << std::endl << std::flush;
    qDebug() << var;
#endif
}
#endif

#if 0x0
#include "bobl/cbor/decode.hpp"
#include <iostream>

int main() {

  std::uint8_t data[] = {0xa4, 0x67, 0x65, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x64,
                         0xf5, 0x62, 0x69, 0x64, 0x18, 0x64, 0x64, 0x6e, 0x61,
                         0x6d, 0x65, 0x68, 0x74, 0x68, 0x65, 0x20, 0x6e, 0x61,
                         0x6d, 0x65, 0x63, 0x65, 0x6e, 0x6d, 0x2};

  uint8_t const *begin = data;
  uint8_t const *end = begin + sizeof(data) / sizeof(data[0]);
  auto res =  bobl::cbor::decode<bool, int, std::string, int>(begin, end);
  std::cout << std::get<0>(res) << std::endl;
  std::cout << std::get<1>(res) << std::endl;
  std::cout << std::get<2>(res) << std::endl;
  std::cout << std::get<3>(res) << std::endl;
  return 0;
}
#endif

#if 0x0
#include "cborg/Cbor.h"

#include <stdio.h>
#include <string>

int main(void)
{
    uint8_t buffer[200];
    char str[] = "hello";
    Cbore encoder(buffer, sizeof(buffer));

    encoder.tag(1234)
      .array()
        .item("Attention, the Universe!")
        .item("Hello World!")
        .map()
          .key("key1").value("value1")
          .key("key2")
            .array()
              .item("one")
              .item("two")
              .item("three")
            .end()
          .key(3).value("value3")
          .key("key4")
            .map(4)
              .key("key5").value(Cbor::TypeNull)
              .key(6).value(Cbor::TypeFalse)
              .key(7)
                .array(3)
                  .item(1)
                  .item(str, sizeof(str) - 1)
                  .item(3)
              .key("something").value(str, sizeof(str) - 1)
              .key(str, sizeof(str) - 1).value(str, sizeof(str) - 1)
        .end()
      .item(-10)
      .item(10)
      .item(Cbor::TypeTrue)
    .end();

    encoder.print();
}
#endif

#if 0x0
#include <QtCore>
#include <iostream>
#include <sstream>
using namespace std;

#include<gmpxx.h>
#include<iostream>

#include <quadmath.h>
#include <string>
#include <vector>

std::string flt128tostr(__float128 x)
{
    int sz = quadmath_snprintf(nullptr, 0, "%.5Qf", x);
    std::vector<char> message(sz+1);
    quadmath_snprintf(&message[0], sz, "%.5Qf", x);
    return &message[0];
}

QString QStringFromLongDouble(const long double x)
{
  std::stringstream ss;
  ss << std::fixed << x;
  return QString::fromStdString(ss.str());
}

QDebug operator<< (QDebug d, const long double x) {
    std::stringstream ss;
    ss << std::fixed << x;
    d.noquote() << QString::fromStdString(ss.str());
    return d;
}

void test_gmp()
{
    mpf_set_default_prec(1024);

    mpf_class a(1.5); // a = 1.5
    mpf_class b("1.2");
    mpf_class c = 0.0;

    c = a + b;
    double d = c.get_d();

    mpf_class f = floor(c);

    std::cout << c << std::endl;
    std::cout << d << std::endl;
    std::cout << f << std::endl;

    __float128 f128 = 3.14159;
    std::cout << flt128tostr(f128) << std::endl;
    //extern __float128 modfq (__float128, __float128 *) __quadmath_throw;
    __float128 x, y;
    x = modfq(f128, &y);
    std::cout << flt128tostr(x) << " " << flt128tostr(y) << std::endl;
    long long ll = (long long)y;
    std::cout << ll << std::endl;

    std::cout << std::numeric_limits<quint64>::max() << std::endl;
    long double z = std::numeric_limits<quint64>::max();
    printf("z = %Lf\n", z);
    std::cout << "z=" << std::fixed << z << std::endl;
    // extern long double __cdecl modfl (long double, long double*);
    long double X, Y;
    X = modfl(z, &Y);
    //qDebug() << QStringFromLongDouble(X) << QStringFromLongDouble(Y);
    qDebug() << X << Y;
}


// 多倍長テンプレ
/* ---------------------- ここから ---------------------- */
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
namespace mp = boost::multiprecision;
// 任意長整数型
using BigInt = mp::cpp_int;
// 仮数部が10進数で1024桁の浮動小数点数型(TLEしたら小さくする)
//using Real = mp::number<mp::cpp_dec_float<1024>>;
//using Real = mp::number<mp::cpp_bin_float<1024>>;
using Real = mp::number<mp::cpp_bin_float<129>>;
/* ---------------------- ここまで ---------------------- */

int main() {
    test_gmp();
    // 整数
    BigInt a = 546534242;
    BigInt b("238431313536543513515132513213516353");
    BigInt c("365435435435135435143548546841324135");
    cout << a * b % c << endl;
    // 浮動小数点数
    Real d("1234567890123456789012345678901234");
    cout << fixed << setprecision(5) << d << endl;
}
#endif

#if 0x0
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

namespace mp = boost::multiprecision;

int main()
{
    mp::cpp_int x = 1;

    for (unsigned int i = 1; i <= 100; ++i)
        x *= i;

    std::cout << x << std::endl;
}
#endif

#if 0x0
#include<gmpxx.h>
#include<iostream>
int main(){
    mpf_set_default_prec(1024);

    mpf_class a(1.5); // a = 1.5
    mpf_class b("1.2");
    mpf_class c = 0.0;

    c = a + b;
    double d = c.get_d();

    std::cout << c << std::endl;
    std::cout << d << std::endl;

    return 0;
}
#endif

#if 0x0
#include <stdio.h>
#include <quadmath.h>
#define MAXN (100)

int main(void)
{
#if 0x1
    int n;
    char message[128];
    __float128 e, newe, x;
    x = 1.0Q; e = x;
    for (n = 1; n <= MAXN; n++) {
        x /= n;
        newe = e + x;
        quadmath_snprintf(message, sizeof(message), "%.40Qf", e);
        puts(message);
        //printf("%.40Qf\n", e);
        if (e == newe)
            break;
        else
            e = newe;
    }
    printf("2.7182818284590452353602874713526624977572470937000 --- N[E,50]\n");
    printf("10進法で34桁程度の精度があるというけれど、本当？\n");
    x = 1234567890123456789012345678901234.0Q;
    quadmath_snprintf(message, sizeof(message), "%0.0Qf", x);
    puts(message);
#endif
    return 0;
}
#endif

#if 0x0
#include <QtCore>
#include <stdio.h>
#include <iostream>

#define USE_FLOAT128

#ifdef  USE_FLOAT128
typedef __float128  long_double_t;
#else
typedef long double long_double_t;
#endif

bool check_long_double(long_double_t n)
{
    if (((n + 1) - n) != 1) return false;
    if (((n - 1) - n) != -1) return false;
    return true;
}
int main(void)
{
    float a = 123456789012345678901234567890.0;
    double b = 123456789012345678901234567890.0;
    long_double_t c = 123456789012345678901234567890.0;

    printf("a = %f\n", a);
    printf("b = %f\n", b);
    printf("c = %Lf\n", c);
    std::cout << std::numeric_limits<quint64>::max() << std::endl;
    c = std::numeric_limits<quint64>::max();
    std::cout << "c=" << c << std::endl;
    printf("c = %0.0Lf\n", c);

    long_double_t n = 1;
    for(;;)
    {
        if (!check_long_double(n)) break;
        printf("n = %0.0Lf\n", n);
        printf("c = %0.0Lf\n", c);
        n *= 2;
    }

    return 0;
}
#endif

#if 0x0
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
#endif
