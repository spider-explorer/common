set -uvx
set -e
#mingwx g++ -shared -o str.dll str.cpp -static
deno run --allow-all --unstable str.js
