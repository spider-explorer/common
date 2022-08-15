set -uvx
set -e
g++ -shared -o add2.dll add2.cpp
deno run --allow-all --unstable add.ts

