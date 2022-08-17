#! bash -uvx
g++ main.cpp -DASMJIT_STATIC=1 -lasmjit -o main && ./main.exe

