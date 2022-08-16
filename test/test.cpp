/*
 * testfloat128.c -- __float128 のテスト
 *   gcc testfloat128.c -lquadmath
 */

#include <stdio.h>
#include <quadmath.h>
#define MAXN (100)

int main(void)
{
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
  return 0;
}

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
