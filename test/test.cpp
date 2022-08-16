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
