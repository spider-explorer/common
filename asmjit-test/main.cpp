#include <windows.h>
#include "test_string.h"
#include "test_initval.h"
#include "test_fib_dbl.h"
#include "test_fib_i32.h"
#include "test_fib_i64.h"
#include "test_fib_u64.h"
#include "test_fib_u32.h"
#include "test_pointer.h"

#include <QtCore>
#include <QtGui>
#include <QtQml>
#include "debug_line.h"
#include "utf8LogHandler.h"
#include "app-data.h"
//#include "AJSEngine.h"

#include "gc_test.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#if 0x0
    QApplication app(argc, argv);
    qInstallMessageHandler(utf8LogHandler);
    QQmlApplicationEngine engine;
#else
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(utf8LogHandler);
    QJSEngine engine;
    engine.installExtensions(QJSEngine::AllExtensions);
#endif
    //qdebug_line1("Hello World!");
    // esprima
    QFile esprima(":/esprima.js");
    if (!esprima.open(QIODevice::ReadOnly)) throw std::logic_error(":/esprima.js");
    engine.evaluate(QString::fromUtf8(esprima.readAll()));
    // babel-parser
    engine.globalObject().setProperty("Babel", engine.importModule(":/babel-parser.mjs"));
    QJSValue parser = engine.importModule(":/typescript-parser.mjs");
    qDebug() << parser.toString();
    qDebug() << parser.property("line").toInt();
    QJSValue parse = parser.property("parse");
    engine.globalObject().setProperty("parse", parse);
    qDebug() << parse.isCallable();
    QJSValue optimize = parser.property("optimize");
    engine.globalObject().setProperty("optimize", optimize);
    qDebug() << optimize.isCallable();
    QJSValue oml2ast = engine.importModule(":/oml2ast.mjs");
    QJSValue code2do = oml2ast.property("code2do");
    engine.globalObject().setProperty("code2do", code2do);
    oml2ast = oml2ast.property("oml2ast");
    engine.globalObject().setProperty("oml2ast", oml2ast);
    QJSValue miniMAL = engine.importModule(":/miniMAL.mjs");
    miniMAL = miniMAL.property("miniMAL");
    engine.globalObject().setProperty("miniMAL", miniMAL);
    QJSValue omljs = engine.importModule(":/omljs.mjs");
    omljs = omljs.property("omljs");
    engine.globalObject().setProperty("omljs", omljs);
    engine.globalObject().setProperty("globalThis", engine.globalObject());

    debug_line();
    SetConsoleOutputCP(65001);
    debug_line();
    test_string_fn fn = test_string();
    debug_line();
    fn();
    debug_line();
    fflush(stdout);
    debug_line();
    test_initval();
    fflush(stdout);
    debug_line();
    test_fib_i32(8);
    fflush(stdout);
    debug_line();
    test_fib_dbl(8);
    fflush(stdout);
    debug_line();
    test_fib_i64(8);
    fflush(stdout);
    debug_line();
    test_fib_u64(8);
    fflush(stdout);
    debug_line();
    test_fib_u32(8);
    fflush(stdout);
    debug_line();
    test_pointer();
    fflush(stdout);
    debug_line();
    test_fib_dbl(15);
    fflush(stdout);
    debug_line();
    test_fib_dbl(20);
    fflush(stdout);
    debug_line();
    test_fib_dbl(25);
    fflush(stdout);
    debug_line();
    std::cout << "[END]\n";
    fflush(stdout);

    // evaluate
    QJSValue ast = engine.evaluate(R"***(
console.log("(1x)");
//var m = miniMAL(globalThis)
//console.log(m.eval(oml2ast(`(. console "log" (+ 22 33))`)[0][1]));
//var do_ = code2do(`(def add2 (fn (x y) (+ x y))`)
//console.log(JSON.stringify(do_));
//console.log(m.evaluate(do_));
//console.log(m.add2(77, 77));
//m.evaluate(code2do(`(def xyz 222)(js2 "console.log(E.xyz)")`));
var om = omljs(globalThis)
om.run(`
  (console.log "this is omljs!")
  (define xyz 123)
  (console.log xyz)
  (console.log (+ 11 22 33))
  (console.log @[xyz, 0, 1, 2]@)
  (set! xyz 777)
  @console.log(xyz)@
  (console.log (list 1 null 2 3))
  (console.log (list 10 undefined "und" true false))
  [console.log (JSON.stringify (dict key1 (+ 10 2) :key2 false key3 true "key4" null) null 2]
  (console.log (- xyz))
  (console.log (+ xyz))
  (console.log (+ 11 (- (* 22 10))))
`);
console.log("(2)");
/*
var bast = parse(`
@native
class Foo {
  @deco
  sayHello(name) {
    return 'This action returns all cats';
  }
}
`);
console.log("(3)");
*/
console.log("(4)");
//console.log(JSON.stringify(bast, null, 2));
var ast = esprima.parseModule(`
import Babel from "./babel-parser.mjs";
answer = 42`);
ast = optimize(ast);
console.log(JSON.stringify(ast, null, 2));
ast;
)***");
    qDebug() << ast.toString();
    if (ast.isError())
    {
        qDebug() << ast.property("filename").toString();
        qDebug() << ast.property("line").toInt();
    }
    qDebug() << ast.toVariant();
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(ast.toVariant());
    qDebug().noquote() << jsonDoc.toJson(QJsonDocument::Indented);

    //gc_test(argc, argv);

    return 0;
}
