#! bash -uvx
pacman -S --needed --noconfirm --disable-download-timeout \
    svn

rm -rf rapidjson
svn export https://github.com/Tencent/rapidjson/trunk/include/rapidjson

