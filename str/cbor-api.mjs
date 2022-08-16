import { encode, decode } from "https://deno.land/x/cbor@v1.3.3/index.js";

export class CborLibrary {
    constructor(libPath, methodList) {
        this.libPath = libPath;
        this.methodList = methodList;
        let methodDict = {};
        for (let method of methodList) {
            methodDict[method] = { parameters: ["pointer"], result: "pointer" };
        }
        this.dylib = Deno.dlopen(libPath, methodDict);
    }
    call(methodName, args) {
        //console.log(args);
        let argsCbor = encode(args);
        //console.log(argsCbor);
        //console.log(typeof argsCbor);
        const argsCborBase64 = uint8ArrayToBase64(argsCbor);
        //console.log(argsCborBase64);
        //console.log(typeof argsCborBase64);
        const string = new Uint8Array([
            ...new TextEncoder().encode(argsCborBase64),
            0,
        ]);
        const stringPtr = Deno.UnsafePointer.of(string);
        const resultPtr = this.dylib.symbols[methodName](stringPtr);
        //console.dir(resultPtr);
        const resultView = new Deno.UnsafePointerView(resultPtr);
        //console.log(resultView.getCString());
        let resultBase64 = resultView.getCString();
        let resultCbor = base64ToUint8Array(resultBase64);
        //console.log(resultCbor);
        //console.log(typeof resultCbor);
        let result = decode(resultCbor);
        //console.log(result);
        return result;
    }
}

function arrayBufferToBase64(buffer) {
    var binary = '';
    var bytes = new Uint8Array(buffer);
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
        binary += String.fromCharCode(bytes[i]);
    }
    return window.btoa(binary);
}

function uint8ArrayToBase64(bytes) {
    var binary = '';
    var len = bytes.byteLength;
    for (var i = 0; i < len; i++) {
        binary += String.fromCharCode(bytes[i]);
    }
    return window.btoa(binary);
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