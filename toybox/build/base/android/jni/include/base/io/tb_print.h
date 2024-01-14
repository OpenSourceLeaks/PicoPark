/*!
 * @brief 基本的なI/O関連関数郡のヘッダーファイル
 */
 
#ifndef _INCLUDED_TB_PRINT_H_
#define _INCLUDED_TB_PRINT_H_
 
namespace toybox
{
    // デバッグ画面出力
    void TbPrintf(const TbChar8 *fmt, ...);

    // デバッグ画面出力(２バイト文字用)
    void TbPrintf(const TbChar16 *fmt, ...);

#if TB_IS_DEBUG
    #define TbDebugPrintf(fmt,...) TbPrintf(fmt,__VA_ARGS__)
#else
    #define TbDebugPrintf(fmt,...) 
#endif
}

#endif
