#!/bin/sh

#���W���[�����̃r���h

ndk-build APP_ABI=all
cp obj/local/armeabi/libtb_*.a ../../../lib/android
