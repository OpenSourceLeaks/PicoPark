#!/bin/sh

# �x�[�X
cd ../../base/flash
make
make debug

# �t���[�����[�N
cd ../../framework/flash
make
make debug

# �O���t�B�b�N�X
cd ../../graphics/flash
make
make debug

# �l�b�g���[�N
#cd ../../network/android
#./../../common/android/build_module.sh
