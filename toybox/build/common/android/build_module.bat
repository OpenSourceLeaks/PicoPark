rem モジュール毎のビルド

call ndk-build.cmd APP_ABI=all
call ndk-build.cmd DEBUG=1 APP_ABI=all
xcopy /Y obj\local\arm64-v8a\libtb_*.a ..\..\..\lib\android\arm64-v8a
xcopy /Y obj\local\armeabi\libtb_*.a ..\..\..\lib\android\armeabi
xcopy /Y obj\local\armeabi-v7a\libtb_*.a ..\..\..\lib\android\armeabi-v7a
xcopy /Y obj\local\mips\libtb_*.a ..\..\..\lib\android\mips
xcopy /Y obj\local\mips64\libtb_*.a ..\..\..\lib\android\mips64
xcopy /Y obj\local\x86\libtb_*.a ..\..\..\lib\android\x86
xcopy /Y obj\local\x86_64\libtb_*.a ..\..\..\lib\android\x86_64
