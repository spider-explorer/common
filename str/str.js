// https://deno.land/manual@main/runtime/ffi_api
// ffi.ts

import { CborLibrary } from "./cbor-api.mjs";

const callback = new Deno.UnsafeCallback(
  {
    parameters: ["i32"],
    result: "void",
  },
  (success) => {
    console.log("success=" + success);
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

let mylib = new CborLibrary(libPath, ["add2", "returnArgs", "returnDateTime", "cbTest"]);
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
