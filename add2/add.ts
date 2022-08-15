// https://deno.land/manual@main/runtime/ffi_api
// ffi.ts

const targetDir = ".";

const [libPrefix, libSuffix] = {
  darwin: ["lib", "dylib"],
  linux: ["lib", "so"],
  windows: ["", "dll"],
}[Deno.build.os];
const libPath = `${targetDir}/${libPrefix}add2.${libSuffix}`;

console.log(libPath);

const dylib = Deno.dlopen(libPath, {
  "add2": { parameters: ["i32", "i32"], result: "i32" }
});

// Call the symbol `add`
const result = dylib.symbols.add2(35, 34); // 69

console.log(`Result from external addition of 35 and 34: ${result}`);
