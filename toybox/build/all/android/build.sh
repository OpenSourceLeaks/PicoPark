#!/bin/sh

# ベース
cd ../../base/android
./../../common/android/build_module.sh

# フレームワーク
cd ../../framework/android
./../../common/android/build_module.sh

# グラフィックス
cd ../../graphics/android
./../../common/android/build_module.sh

# ネットワーク
#cd ../../network/android
#./../../common/android/build_module.sh
