// https://deno.land/manual@main/runtime/ffi_api
// ffi.ts

import { CborLibrary } from "./cbor-api.mjs";

const targetDir = "../dll/";

const [libPrefix, libSuffix] = {
  darwin: ["lib", "dylib"],
  linux: ["lib", "so"],
  windows: ["", "dll"],
}[Deno.build.os];
const libPath = `${targetDir}/${libPrefix}dll-x86_64-static.${libSuffix}`;

console.log(libPath);

let mylib = new CborLibrary(libPath, ["add2"]);
console.dir(mylib.call("add2", [111, 222]));
