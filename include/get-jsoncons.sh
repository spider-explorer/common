#! bash -uvx
pacman -S --needed --noconfirm --disable-download-timeout \
    svn

rm -rf jsoncons*
#svn export https://github.com/danielaparker/jsoncons/trunk/include/jsoncons
#svn export https://github.com/danielaparker/jsoncons/trunk/include/jsoncons_ext

git clone  https://github.com/danielaparker/jsoncons jsoncons.tmp
cp -rp jsoncons.tmp/include/* .
