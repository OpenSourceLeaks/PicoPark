rem ベース
cd ../../base/emscripten
make clean
make
make debug

rem フレームワーク
cd ../../framework/emscripten
make clean
make
make debug

rem フレームワークCUI
cd ../../framework_cui/emscripten
make clean
make
make debug

rem グラフィックス
cd ../../graphics/emscripten
make clean
make
make debug

rem スクリプト
cd ../../script/lua/emscripten
make clean
make
make debug

rem input
cd ../../../input/emscripten
make clean
make
make debug

rem UI
cd ../../ui/emscripten
make clean
make
make debug

cd ../../all/emscripten
