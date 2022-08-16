// https://deno.land/manual@main/runtime/ffi_api
// ffi.ts

import { CborLibrary, CborCallback, VARIANT_GET_ARGS } from "./cbor-api.mjs";

const callback = new Deno.UnsafeCallback(
  {
    parameters: ["i32"],
    result: "void",
  },
  (success) => {
    console.log("success=" + success);
  },
);

let callback2 = new Deno.UnsafeCallback(
  {
    parameters: ["pointer"],
    result: "pointer",
  },
  (__args__) => {
    //console.log(`__args__: ${__args__}`);
    //const argsView = new Deno.UnsafePointerView(__args__);
    //console.log(argsView.getCString());
    let args = VARIANT_GET_ARGS(__args__);
    console.log(args);
    callback2.__result__ = __args__;
    return callback2.__result__;
  },
);

const targetDir = "../dll/";

const [libPrefix, libSuffix] = {
  darwin: ["lib", "dylib"],
  linux: ["lib", "so"],
  windows: ["", "dll"],
}[Deno.build.os];
const libPath = `${targetDir}/${libPrefix}dll-x86_64-static.${libSuffix}`;

console.log(libPath);

let mylib = new CborLibrary(libPath, ["add2", "returnArgs", "returnDateTime", "cbTest", "cbTest2"]);
console.dir(mylib.call("add2", [111, 222]));
console.dir(mylib.call("returnDateTime", undefined));
console.log(typeof mylib.call("returnDateTime", undefined));
console.log(mylib.call("returnDateTime", undefined).getFullYear());
console.log(mylib.call("returnArgs", new Date()));
console.log(callback.pointer);
console.log(mylib.call("returnArgs", String(callback.pointer)));
console.log(mylib.call("returnArgs", BigInt(callback.pointer)));

const max = 18446744073709551615n;
//9007199254740991
console.log(mylib.call("returnArgs", String(max)));
console.log(mylib.call("cbTest", String(callback.pointer)));
console.log(mylib.call("cbTest2", [String(callback2.pointer), 8888]));

let cb3 = new CborCallback(function(args) {console.log("(B):"+args);return args*100;});
console.log(mylib.call("cbTest2", [String(cb3.pointer()), 7777]));
cb3.close();
