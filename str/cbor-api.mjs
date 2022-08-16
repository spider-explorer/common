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
        let argsCbor = encode(args);
        const argsCborBase64 = uint8ArrayToBase64(argsCbor);
        const argsArray = new Uint8Array([
            ...new TextEncoder().encode(argsCborBase64),
            0,
        ]);
        const __args__ = Deno.UnsafePointer.of(argsArray);
        const __result__ = this.dylib.symbols[methodName](__args__);
        const resultView = new Deno.UnsafePointerView(__result__);
        let resultBase64 = resultView.getCString();
        let resultCbor = base64ToUint8Array(resultBase64);
        let result = decode(resultCbor);
        return result;
    }
}

export class CborCallback {
    constructor(f) {
        let that = this;
        console.log(f);
        that.f = f;
        that.callback = new Deno.UnsafeCallback(
            {
                parameters: ["pointer"],
                result: "pointer",
            },
            (__args__) => {
                that.__args__ = __args__;
                that.argsView = new Deno.UnsafePointerView(that.__args__);
                that.argsBase64 = that.argsView.getCString();
                that.argsCbor = base64ToUint8Array(that.argsBase64);
                that.args = decode(that.argsCbor);
                that.result = that.f(that.args);
                that.resultCbor = encode(that.result);
                that.resultCborBase64 = uint8ArrayToBase64(that.resultCbor);
                that.resultArray = new Uint8Array([
                    ...new TextEncoder().encode(that.resultCborBase64),
                    0,
                ]);
                that.__result__ = Deno.UnsafePointer.of(that.resultArray);
                return that.__result__;
            },
        );
        that.pointer = String(that.callback.pointer);
    }
    close() {
        this.callback.close();
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
