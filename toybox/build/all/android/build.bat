rem �x�[�X
cd ..\..\base\android
call .\..\..\common/android/build_module.bat

rem �t���[�����[�N
cd ..\..\framework/android
call .\..\..\common/android/build_module.bat

rem �O���t�B�b�N�X
cd ..\..\graphics/android
call .\..\..\common/android/build_module.bat

rem ����
cd ../../input/android
call .\..\..\common/android/build_module.bat

rem �l�b�g���[�N
cd ../../network/android
call .\..\..\common/android/build_module.bat

rem UI
cd ../../ui/android
call .\..\..\common/android/build_module.bat

pause;
