rem �x�[�X
cd ../../base/emscripten
make clean
make
make debug

rem �t���[�����[�N
cd ../../framework/emscripten
make clean
make
make debug

rem �t���[�����[�NCUI
cd ../../framework_cui/emscripten
make clean
make
make debug

rem �O���t�B�b�N�X
cd ../../graphics/emscripten
make clean
make
make debug

rem �X�N���v�g
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
