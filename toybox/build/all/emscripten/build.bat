rem �x�[�X
cd ../../base/emscripten
make
make debug

rem �t���[�����[�N
cd ../../framework/emscripten
make
make debug

rem �t���[�����[�NCUI
cd ../../framework_cui/emscripten
make
make debug

rem �O���t�B�b�N�X
cd ../../graphics/emscripten
make
make debug

rem �X�N���v�g
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