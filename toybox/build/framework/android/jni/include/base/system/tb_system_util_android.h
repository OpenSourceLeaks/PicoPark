/*!
 * @file
* @brief 基本的マクロ(Android用)
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SYSTEM_MACRO_ANDROID_H_
#define _INCLUDED_TB_SYSTEM_MACRO_ANDROID_H_

namespace toybox
{

// Javaから呼び出すための関数開始マクロ
#define TB_BEGIN_JNI_FUNC extern "C" {

// Javaから呼び出すための関数終了マクロ
#define TB_END_JNI_FUNC }
    
}

#endif