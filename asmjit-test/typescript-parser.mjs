//var exports = {};
//Object.defineProperty(exports, "__esModule", {
//  value: true
//});

console.log("typescript.parser.mjs(1)");
import Babel from "./babel-parser.mjs";

var ast = Babel.parse('answer = 42',
{
  sourceType: "module",
  plugins: ["typescript", "decorators-legacy"],
});

console.log("typescript.parser.mjs(2)" + JSON.stringify(ast, null, 2));

export function optimize(x) {
  if (x == null) return x;
  switch (typeof x) {
    case "object":
      Object.keys(x).forEach(function (key) {
        if (key === "loc" || key === "start" || key === "end") delete x[key];
        else if (key === "type") {
          x["!type"] = optimize(x[key]);
          delete x[key];
        } else if (key === "decorators") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else if (key === "id") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else if (key === "key") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else if (key === "kind") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else if (key === "params") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else if (key === "superClass") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else if (key === "sourceType") {
          x["*"+key] = optimize(x[key]);
          delete x[key];
        } else {
          x[key] = optimize(x[key]);
        }
      });
      return x;
    case "array":
      for (let i = 0; i < x.length; i++) {
        x[i] = optimize(x[i]);
      }
      return x;
    default:
      return x;
  }
}

console.log("typescript.parser.mjs(3)");
export function parse(code)
{
    console.log("typescript.parser.mjs:parse(1)");
    var ast = Babel.parse(code,
    {
      sourceType: "module",
      plugins: ["typescript", "decorators-legacy"],
    });
    return optimize(ast);
}
console.log("typescript.parser.mjs(4)");

//exports.parse = parse;
//const __esModule = exports.__esModule;
//const _parse = exports.parse;

//export { __esModule, exports as default, _parse as parse };
