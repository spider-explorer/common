#! bash -uvx
rm -rf asmjit.$MSYSTEM_CARCH
svn export https://github.com/asmjit/asmjit/trunk asmjit.$MSYSTEM_CARCH
cd asmjit.$MSYSTEM_CARCH
mkdir -p build
cd build
#cmake -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=$MINGW_PREFIX -DASMJIT_EMBED=1 ..
cmake -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=$MINGW_PREFIX -DASMJIT_STATIC=1 ..
make && make install


