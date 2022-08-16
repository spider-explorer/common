import { encode, decode } from "https://deno.land/x/cbor@v1.3.3/index.js";

export function VARIANT_GET_ARGS(__args__) {
    const argsView = new Deno.UnsafePointerView(__args__);
    console.log(argsView.getCString());
    let argsBase64 = argsView.getCString();
    let argsCbor = base64ToUint8Array(argsBase64);
    //console.log(resultCbor);
    //console.log(typeof resultCbor);
    let args = decode(argsCbor);
    return args;
}

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
                console.log("(A):" + that.args);
                console.log("(A2):" + typeof that.args);
                console.log("(A3):" + typeof f);
                that.result = that.f(that.args);
                console.log("(A4):"+ that.result);
                that.resultCbor = encode(that.result);
                that.resultCborBase64 = uint8ArrayToBase64(that.resultCbor);
                that.resultString = new Uint8Array([
                    ...new TextEncoder().encode(that.resultCborBase64),
                    0,
                ]);
                that.__result__ = Deno.UnsafePointer.of(that.resultString);
                console.log("(C)");
                return that.__result__;
            },
        );
    }
    pointer() {
        return String(this.callback.pointer);
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
