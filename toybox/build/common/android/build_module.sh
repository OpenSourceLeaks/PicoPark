#!/bin/sh

#モジュール毎のビルド

ndk-build APP_ABI=all
cp obj/local/armeabi/libtb_*.a ../../../lib/android
