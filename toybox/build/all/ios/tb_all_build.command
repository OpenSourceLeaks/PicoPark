#!/bin/bash
 
# エラー検出用
set -e

# カレント
cd `dirname $0`

cd ../../base/ios/tb_base
sh tb_base_build.sh
cd ../../../all/ios

cd ../../graphics/ios/tb_graphics
sh tb_graphics_build.sh
cd ../../../all/ios

cd ../../framework/ios/tb_framework
sh tb_framework_build.sh
cd ../../../all/ios

cd ../../input/ios/tb_input
sh tb_input_build.sh
cd ../../../all/ios

cd ../../network/ios/tb_network
sh tb_network_build.sh
cd ../../../all/ios

cd ../../ui/ios/tb_ui
sh tb_ui_build.sh
cd ../../../all/ios
