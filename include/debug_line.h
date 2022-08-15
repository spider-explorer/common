#ifndef DEBUG_LINE_H
#define DEBUG_LINE_H

#include <iostream>
#if !defined(uout__)
#include "strconv.h"
static unicode_ostream uout__(std::cerr, ::GetConsoleOutputCP());
#endif
#if defined(Q_UNUSED)
# include <QtCore>
#endif

#if defined(_MSC_VER)
//# define __PRETTY_FUNCTION__ __FUNCSIG__
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#if defined(DEBUG_LINE)
# define debug_line() std::cerr << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << std::endl
# define debug_line1(x) std::cerr << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << " : " << (x) << std::endl
# define debug_line2(x, y) std::cerr << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << " : " << (x) << " " << (y) << std::endl
# define debug_line3(x, y, z) std::cerr << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << " : " << (x) << " " << (y) << " " << (z) << std::endl
#else
# define debug_line()
# define debug_line1(x)
# define debug_line2(x, y)
# define debug_line3(x, y, z)
#endif

#if defined(DEBUG_LINE)
# define udebug_line() uout__ << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << std::endl
# define udebug_line1(x) uout__ << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << " : " << (x) << std::endl
# define udebug_line2(x, y) uout__ << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << " : " << (x) << " " << (y) << std::endl
# define udebug_line3(x, y, z) uout__ << __FILE__ << ":" << __PRETTY_FUNCTION__ << " @line " << __LINE__ << " : " << (x) << " " << (y) << " " << (z) << std::endl
#else
# define udebug_line()
# define udebug_line1(x)
# define udebug_line2(x, y)
# define udebug_line3(x, y, z)
#endif

#if defined(Q_UNUSED)
# if defined(DEBUG_LINE)
#  define qdebug_line() qDebug().noquote() << __FILE__ << ":" << __PRETTY_FUNCTION__ << "@line" << __LINE__
#  define qdebug_line1(x) qDebug().noquote() << __FILE__ << ":" << __PRETTY_FUNCTION__ << "@line" << __LINE__ << ":" << x
#  define qdebug_line2(x, y) qDebug().noquote() << __FILE__ << ":" << __PRETTY_FUNCTION__ << "@line" << __LINE__ << ":" << x << y
#  define qdebug_line3(x, y, z) qDebug().noquote() << __FILE__ << ":" << __PRETTY_FUNCTION__ << "@line" << __LINE__ << ":" << x << y << z
# else
#  define qdebug_line()
#  define qdebug_line1(x)
#  define qdebug_line2(x, y)
#  define qdebug_line3(x, y, z)
# endif
#endif

#endif // DEBUG_LINE_H
