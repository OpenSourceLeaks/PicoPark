#!/bin/sh

# ベース
cd ../../base/flash
make
make debug

# フレームワーク
cd ../../framework/flash
make
make debug

# グラフィックス
cd ../../graphics/flash
make
make debug

# ネットワーク
#cd ../../network/android
#./../../common/android/build_module.sh
