rem ベース
cd ..\..\base\android
call .\..\..\common/android/build_module.bat

rem フレームワーク
cd ..\..\framework/android
call .\..\..\common/android/build_module.bat

rem グラフィックス
cd ..\..\graphics/android
call .\..\..\common/android/build_module.bat

rem 入力
cd ../../input/android
call .\..\..\common/android/build_module.bat

rem ネットワーク
cd ../../network/android
call .\..\..\common/android/build_module.bat

rem UI
cd ../../ui/android
call .\..\..\common/android/build_module.bat

pause;
