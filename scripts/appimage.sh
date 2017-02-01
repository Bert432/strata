#!/bin/bash

########################################################################
# Package the binaries built on Travis-CI as an AppImage
# By Albert Kottke, 2016
# For more information, see http://appimage.org/
########################################################################

export ARCH=$(arch)

APP=Strata
LOWERAPP=${APP,,}

GIT_REV=$(git rev-parse --short HEAD)
echo $GIT_REV

mkdir -p $HOME/$APP/$APP.AppDir/usr/bin/

cd $HOME/$APP/

wget -q https://github.com/probonopd/AppImages/raw/master/functions.sh -O ./functions.sh
. ./functions.sh

cd $APP.AppDir

cp $TRAVIS_BUILD_DIR/build/release/strata ./usr/bin/
# sudo chown -R $USER ./usr/
# sed -i -e 's|/|././|g' ./usr/bin/strata

########################################################################
# Copy desktop and icon file to AppDir for AppRun to pick them up
########################################################################

get_apprun

# FIXME: Use the official .desktop file - where is it?
cat > $LOWERAPP.desktop <<EOF
[Desktop Entry]
Name=$APP
Icon=$LOWERAPP
Type=Application
Comment=$APP
Categories=Graphics;
Exec=$LOWERAPP
EOF

find ../../ -name application-icon.svg -exec cp {} strata.svg \;

########################################################################
# Copy in the dependencies that cannot be assumed to be available
# on all target systems
########################################################################

mkdir -p ./usr/lib/qt58/plugins/
# PLUGINS=/opt/qt58/plugins
cp -r /opt/qt58/plugins/imageformats ./usr/lib/qt58/plugins/
cp -r /opt/qt58/plugins/iconengines ./usr/lib/qt58/plugins/
cp -r /opt/qt58/plugins/platforms ./usr/lib/qt58/plugins/

copy_deps

# Move the libraries to usr/bin
move_lib

mv ./usr/lib/x86_64-linux-gnu/* ./usr/lib/

# ./usr/local is not used inside an AppDir/AppImage, so move it
mv usr/local/qwt-6.1.4-svn/lib/libqwt.so.6 usr/lib/
rm -rf usr/local/

# ./opt is not use inside an AppDir/AppImage, so move it
mv opt/qt58/lib/* usr/lib/
rm -rf opt/

# ./usr/lib/qt58/ is not use inside an AppDir/AppImage, so move it
mv ./usr/lib/qt58/ ./usr/lib/qt5/

########################################################################
# Delete stuff that should not go into the AppImage
########################################################################

# Delete dangerous libraries; see
# https://github.com/probonopd/AppImages/blob/master/excludelist
delete_blacklisted

# We don't bundle the developer stuff
rm -rf usr/include || true
rm -rf usr/lib/cmake || true
rm -rf usr/lib/pkgconfig || true
find . -name '*.la' | xargs -i rm {}
strip usr/bin/* usr/lib/* || true

########################################################################
# desktopintegration asks the user on first run to install a menu item
########################################################################

get_desktopintegration $LOWERAPP

########################################################################
# Determine the version of the app; also include needed glibc version
########################################################################

GLIBC_NEEDED=$(glibc_needed)
VERSION=git$GIT_REV-glibc$GLIBC_NEEDED

########################################################################
# Patch away absolute paths; it would be nice if they were relative
########################################################################

patch_usr
# Possibly need to patch additional hardcoded paths away, replace
# "/usr" with "././" which means "usr/ in the AppDir"

########################################################################
# AppDir complete
# Now packaging it as an AppImage
########################################################################

cd .. # Go out of AppImage

mkdir -p ../out/
generate_appimage

########################################################################
# Upload the AppDir
########################################################################

transfer ../out/*
