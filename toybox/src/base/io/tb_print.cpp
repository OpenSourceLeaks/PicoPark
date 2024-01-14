/*!
 * @file
 * @brief 基本的なI/O関連関数郡の実装ファイル
 * @author teco
 */

#include "tb_fwd.h"
#include "base/io/tb_print.h"
#include "base/io/tb_log.h"

#if (TB_PLATFORM_IS_ANDROID)
#include <android/log.h>
#elif (TB_PLATFORM_IS_NX)
#include <nn/nn_Log.h>
#endif

#define ENABLE_BUILD_PRINT_LOG (1 || TB_IS_DEBUG)

namespace toybox
{

#define BUF_SIZE 1024

/*!
 * デバッグコンソール出力
 */
void TbPrintf(const TbChar8 *fmt, ...)
{
    va_list ap;
    TbChar8    buf[BUF_SIZE];
    va_start( ap, fmt );
    TbVStrPrintf( buf, BUF_SIZE , fmt, ap );
    va_end( ap );
#if (TB_PLATFORM_IS_WIN)
    OutputDebugString(buf);
#elif (TB_PLATFORM_IS_ANDROID)
    __android_log_print( ANDROID_LOG_INFO , "toybox" , "%s" , buf );
#elif (TB_PLATFORM_IS_WIIU)
    OSReport("%s" , buf);
#elif (TB_PLATFORM_IS_NX)
    NN_LOG("%s", buf);
#else
    printf( "%s" , buf );
#endif

#if ENABLE_BUILD_PRINT_LOG
    TbPrintLog(buf);
#endif
}

/*!
 * デバッグコンソール出力(ワイド文字)
 */
void TbPrintf(const TbChar16 *fmt, ...)
{
#if TB_CHAR16_IS_WCHAR
    va_list ap;
    TbChar16    buf[BUF_SIZE];
    va_start( ap, fmt );
    TbVStrPrintf( buf,BUF_SIZE, fmt, ap );
    va_end( ap );
#if (TB_PLATFORM_IS_WIN)
    OutputDebugStringW(buf);
#else
    wprintf( buf );
#endif
#endif
}

}
