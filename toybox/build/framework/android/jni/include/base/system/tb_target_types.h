/*! 
 * @file
 * @brief ビルドターゲット定義
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_TARGET_TYPES_H_
#define _INCLUDED_TB_TARGET_TYPES_H_

// プラットフォーム
#define TB_PLATFORM_WIN          1
#define TB_PLATFORM_MAXOSX       2
#define TB_PLATFORM_IOS          3
#define TB_PLATFORM_ANDROID      4
#define TB_PLATFORM_NACL         5
#define TB_PLATFORM_LINUX        6
#define TB_PLATFORM_FLASH        7
#define TB_PLATFORM_EMSCRIPTEN   8
#define TB_PLATFORM_WIIU         9
#define TB_PLATFORM_NX           10
#define TB_PLATFORM_NXWIN        11

#if defined(_WINDOWS)
#   define TB_PLATFORM_TARGET TB_PLATFORM_WIN     // Windows版
#elif defined(_IOS)
#   define TB_PLATFORM_TARGET TB_PLATFORM_IOS       // iOS版
#elif defined(__ANDROID__)
#   define TB_PLATFORM_TARGET TB_PLATFORM_ANDROID   // Android版
#   ifdef TB_USE_NATIVE_ACTIVITY
#       define TB_IS_NATIVE_ACTIVITY 1  // NativeActivityを利用
#   else
#       define TB_IS_NATIVE_ACTIVITY 0  // NativeActivityを利用しない
#   endif
#elif defined(_native_client__)
#   define TB_PLATFORM_TARGET TB_PLATFORM_NACL      // NativeClient版
#elif defined(__AVM2__)
#   define TB_PLATFORM_TARGET TB_PLATFORM_FLASH      // Flash版
#elif defined(EMSCRIPTEN)
#   define TB_PLATFORM_TARGET TB_PLATFORM_EMSCRIPTEN      // Emscripten版
#elif defined(CAFE)
#   define TB_PLATFORM_TARGET TB_PLATFORM_WIIU
#elif defined(NN_NINTENDO_SDK)
#   if defined(WIN32) || defined(WIN64)
#       define TB_PLATFORM_TARGET TB_PLATFORM_NXWIN
#   else
#       define TB_PLATFORM_TARGET TB_PLATFORM_NX
#   endif
#elif __APPLE__
#   include "TargetConditionals.h"
#   if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#       define TB_PLATFORM_TARGET TB_PLATFORM_IOS           // iOS版
#   elif TARGET_OS_MAC
#       define TB_PLATFORM_TARGET TB_PLATFORM_MAXOSX        // MaTB_PLATFORM_WINTB_PLATFORM_WINcOSX版
#   else
#       error "Unknown Apple platform"
#   endif
#else
#   error "Unknown Platform"
#endif

#define TB_PLATFORM_IS_WIN      (TB_PLATFORM_TARGET==TB_PLATFORM_WIN)
#define TB_PLATFORM_IS_MACOSX   (TB_PLATFORM_TARGET==TB_PLATFORM_MACOSX)
#define TB_PLATFORM_IS_IOS      (TB_PLATFORM_TARGET==TB_PLATFORM_IOS)
#define TB_PLATFORM_IS_APPLE    (TB_PLATFORM_IS_MACOSX||TB_PLATFORM_IS_IOS)
#define TB_PLATFORM_IS_ANDROID  (TB_PLATFORM_TARGET==TB_PLATFORM_ANDROID)
#define TB_PLATFORM_IS_NACL     (TB_PLATFORM_TARGET==TB_PLATFORM_NACL)
#define TB_PLATFORM_IS_LINUX    (TB_PLATFORM_TARGET==TB_PLATFORM_LINUX)
#define TB_PLATFORM_IS_WIIU     (TB_PLATFORM_TARGET==TB_PLATFORM_WIIU)
#define TB_PLATFORM_IS_NX       ((TB_PLATFORM_TARGET==TB_PLATFORM_NX) || (TB_PLATFORM_TARGET==TB_PLATFORM_NXWIN))
#define TB_PLATFORM_IS_NXWIN    (TB_PLATFORM_TARGET==TB_PLATFORM_NXWIN)
#define TB_PLATFORM_IS_FLASH        (TB_PLATFORM_TARGET==TB_PLATFORM_FLASH)
#define TB_PLATFORM_IS_EMSCRIPTEN   (TB_PLATFORM_TARGET==TB_PLATFORM_EMSCRIPTEN)
#define TB_PLATFORM_IS_WEB   (TB_PLATFORM_IS_EMSCRIPTEN || TB_PLATFORM_IS_FLASH || TB_PLATFORM_IS_NACL)

// プラットフォーム別デバッグ設定
#if TB_PLATFORM_IS_APPLE
#   define TB_IS_DEBUG DEBUG
#elif TB_PLATFORM_IS_NX
#   if defined(NN_SDK_BUILD_DEBUG)
#       define TB_IS_DEBUG 1
#   else
#       define TB_IS_DEBUG 0
#   endif
#else
#   if defined(_DEBUG)
#       define TB_IS_DEBUG 1
#   else
#       define TB_IS_DEBUG 0
#   endif
#endif

//プラットフォーム別インクルード
#if TB_PLATFORM_IS_WIN
    // Windows
    #include <winsock2.h>
    #include <windows.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <new.h>
    #include <math.h>
#if defined(_WIN64)
    #define TB_IS_LP64 (0)  // windowsはLLP64モデル
    #define TB_IS_64BIT (1)
#endif
#elif TB_PLATFORM_IS_ANDROID
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <wchar.h>
    #include <math.h>
    #include <jni.h>
    #include <android/api-level.h>
    #include <android/asset_manager.h>    
    #include <android/asset_manager_jni.h>    
#if defined(__LP64__)
    #define TB_IS_LP64 (1)
    #define TB_IS_64BIT (1)
#endif
#elif TB_PLATFORM_IS_APPLE
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <math.h>
    #include <wchar.h>
    #include <new>
#ifdef __x86_64__
    #define TB_IS_LP64 (1)
    #define TB_IS_64BIT (1)
#endif
#elif TB_PLATFORM_IS_FLASH
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <math.h>
    #include <wchar.h>
    #include <sys/cdefs.h>
    #include <AS3/AS3.h>
    #include <AS3/AS3++.h>
    #include <Flash++.h>
#elif TB_PLATFORM_IS_EMSCRIPTEN
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <math.h>
    #include <wchar.h>
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <new>
#elif TB_PLATFORM_IS_WIIU
    #include <cafe.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <math.h>
    #include <wchar.h>
#elif TB_PLATFORM_IS_NX
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
    #include <string.h>
    #include <math.h>
    #include <nn/init.h>
    #include <nn/fs.h>
    #include <nn/os.h>
    #include <nn/oe.h>
    #include <nn/time.h>
    #include <new>
#if defined(__LP64__) || defined(WIN64)
    #define TB_IS_LP64 (1)
    #define TB_IS_64BIT (1)
#endif
#endif

// LP64設定
#ifndef TB_IS_LP64
#   define TB_IS_LP64 (0)
#endif
#ifndef TB_IS_64BIT
#   define TB_IS_64BIT (0)
#endif

// コンパイラ
#define TB_COMPILER_MSVC  1   // MSVC
#define TB_COMPILER_GCC   2   // GCC
#define TB_COMPILER_EPPC  3   // EPPC
#define TB_COMPILER_CLANG 4   // CLANG

#if defined(__clang__)
    #define TB_COMPILER_TARGET TB_COMPILER_CLANG
#elif defined(_MSC_VER)
    #define TB_COMPILER_TARGET TB_COMPILER_MSVC
#elif defined(EPPC)
    #define TB_COMPILER_TARGET TB_COMPILER_EPPC
#else
    // とりあえず、MSVC以外はGCCとして扱ってみる
    #define TB_COMPILER_TARGET TB_COMPILER_GCC
#endif

#define TB_COMPILER_IS_MSVC (TB_COMPILER_TARGET==TB_COMPILER_MSVC)
#define TB_COMPILER_IS_GCC (TB_COMPILER_TARGET==TB_COMPILER_GCC)
#define TB_COMPILER_IS_EPPC (TB_COMPILER_TARGET==TB_COMPILER_EPPC)
#define TB_COMPILER_IS_CLANG (TB_COMPILER_TARGET==TB_COMPILER_CLANG)

// C++11対応状況
#define TB_HAS_NULLPTR      (!TB_PLATFORM_IS_WIIU)
#define TB_HAS_OVERRIDE     (!TB_PLATFORM_IS_WIIU)

#endif
