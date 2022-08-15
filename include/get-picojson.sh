#! bash -uvx
pacman -S --needed --noconfirm --disable-download-timeout \
    svn

rm -rf picojson.h
svn export https://github.com/kazuho/picojson/trunk/picojson.h

