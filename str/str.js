// https://deno.land/manual@main/runtime/ffi_api
// ffi.ts

//import { encode, decode } from "https://deno.land/x/cbor@v1.5.0/index.js";
import { encode, decode } from "https://deno.land/x/cbor@v1.3.3/index.js";

function arrayBufferToBase64( buffer ) {
    var binary = '';
    var bytes = new Uint8Array( buffer );
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
        binary += String.fromCharCode( bytes[ i ] );
    }
    return window.btoa( binary );
}

function uint8ArrayToBase64( bytes ) {
    var binary = '';
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
        binary += String.fromCharCode( bytes[ i ] );
    }
    return window.btoa( binary );
}

function base64ToArrayBuffer(base64) {
    var binary_string = window.atob(base64);
    var len = binary_string.length;
    var bytes = new Uint8Array(len);
    for (var i = 0; i < len; i++) {
        bytes[i] = binary_string.charCodeAt(i);
    }
    return bytes.buffer;
}

function base64ToUint8Array(base64) {
    var binary_string = window.atob(base64);
    var len = binary_string.length;
    var bytes = new Uint8Array(len);
    for (var i = 0; i < len; i++) {
        bytes[i] = binary_string.charCodeAt(i);
    }
    return bytes;
}

const targetDir = "../dll/";

const [libPrefix, libSuffix] = {
  darwin: ["lib", "dylib"],
  linux: ["lib", "so"],
  windows: ["", "dll"],
}[Deno.build.os];
const libPath = `${targetDir}/${libPrefix}dll-x86_64-static.${libSuffix}`;

console.log(libPath);

const dylib = Deno.dlopen(libPath, {
  "add2": { parameters: ["pointer"], result: "pointer" }
});

let args = [11, 22];
console.log(args);

let argsCbor = encode(args);
console.log(argsCbor);
console.log(typeof argsCbor);

const argsCborBase64 = uint8ArrayToBase64(argsCbor);
console.log(argsCborBase64);
console.log(typeof argsCborBase64);

const string = new Uint8Array([
  ...new TextEncoder().encode(argsCborBase64),
  0,
]);
const stringPtr = Deno.UnsafePointer.of(string);
//const stringPtrview = new Deno.UnsafePointerView(stringPtr);
//console.log(stringPtrview.getCString());

// Call the symbol `hello`
const resultPtr = dylib.symbols.add2(stringPtr);

console.dir(resultPtr);
const resultView = new Deno.UnsafePointerView(resultPtr);
console.log(resultView.getCString());
let resultBase64 = resultView.getCString();
let resultCbor = base64ToUint8Array(resultBase64);
console.log(resultCbor);
console.log(typeof resultCbor);
let result = decode(resultCbor);
console.log(result);

