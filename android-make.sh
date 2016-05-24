#!/bin/sh
DEV_PREFIX=/opt
PREFIX=/data/local/tmp
ANDROID_NDK=${DEV_PREFIX}/ndk
CROSS_COMPILE=arm-linux-androideabi
ANDROID_PREFIX=/tmp/chain
SYSROOT=${ANDROID_NDK}/platforms/android-19/arch-arm
CROSS_PATH=${ANDROID_PREFIX}/bin/${CROSS_COMPILE}
CPP=${CROSS_PATH}-cpp
AR=${CROSS_PATH}-ar
AS=${CROSS_PATH}-as
NM=${CROSS_PATH}-nm
export CC=${CROSS_PATH}-gcc
CXX=${CROSS_PATH}-g++
LD=${CROSS_PATH}-ld
RANLIB=${CROSS_PATH}-ranlib
PREFIX=${PREFIX}
PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
CFLAGS="-g -UHANDLE_MULTIBYTE -UHAVE_WCTYPE_U -U HAVE_WCHAR_H -UHAVE_LOCALE_H -DHAVE_MKFIFO -UHAVE_LOCALE_H --sysroot=${SYSROOT} -I${SYSROOT}/usr/include -I${ANDROID_PREFIX}/include -I${DEV_PREFIX}/android/bionic -UHAVE_GETPWENT -UHAVE_GETPWNAM -UHAVE_GETPWUID -I/data/local/tmp/lib/include"
CPPFLAGS="${CFLAGS}"
LDFLAGS="${LDFLAGS} -L${SYSROOT}/usr/lib -L${ANDROID_PREFIX}/lib -L/data/local/tmp/lib"
./configure \
  --with-curses \
  --bindir=/data/local/tmp \
  --sysconfdir=/data/local/tmp/etc \
  --libdir=/data/local/tmp/lib \
  --includedir=/data/local/tmp/lib/include \
  --datarootdir=/data/local/tmp/lib/share \
  --localedir=/data/local/tmp/lib/share \
  --mandir=/data/local/tmp/lib/share \
  --docdir=/data/local/tmp/lib/share \
  --host=arm-linux-gnueabi \
  --enable-static-link \
  --disable-nls \
  --without-bash-malloc \
  "LDFLAGS=${LDFLAGS}" \
  "CFLAGS=${CFLAGS}" \
  "CPPFLAGS=${CPPFLAGS}" \
  && make 
