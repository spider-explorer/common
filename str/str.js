// https://deno.land/manual@main/runtime/ffi_api
// ffi.ts

import { CborLibrary, CborCallback } from "./cbor-api.mjs";

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

const max = 18446744073709551615n;
//9007199254740991
console.log(mylib.call("returnArgs", String(max)));

let cb3 = new CborCallback(
  function (args) {
    console.log("(B):" + args); return args * 100;
  });
console.log(mylib.call("cbTest2", [cb3.pointer, 7777]));
cb3.close();
