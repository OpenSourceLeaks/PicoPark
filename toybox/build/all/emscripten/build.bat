rem ベース
cd ../../base/emscripten
make
make debug

rem フレームワーク
cd ../../framework/emscripten
make
make debug

rem フレームワークCUI
cd ../../framework_cui/emscripten
make
make debug

rem グラフィックス
cd ../../graphics/emscripten
make
make debug

rem スクリプト
cd ../../script/lua/emscripten
make
make debug

rem input
cd ../../../input/emscripten
make
make debug

rem UI
cd ../../ui/emscripten
make
make debug

cd ../../all/emscripten

pause;