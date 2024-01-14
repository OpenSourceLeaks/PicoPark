/*!
 * frameworkのユーティリティヘッダー
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_FRAMEWORK_UTIL_ANDROID_H_
#define _INCLUDED_TB_FRAMEWORK_UTIL_ANDROID_H_

// Javaから呼び出すための関数定義マクロ
#define TB_FRAMEWORK_JNI_FUNC(ClassName,FuncName) Java_toybox_framework_##ClassName##_##FuncName

#endif
