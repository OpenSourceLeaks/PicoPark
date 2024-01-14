/*!
 * @file
 * @brief デバッグ関連ヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_DEBUG_H_
#define _INCLUDED_TB_DEBUG_H_


#if (TB_PLATFORM_IS_WIN || TB_PLATFORM_IS_NXWIN)
namespace toybox
{
#if TB_IS_DEBUG
    TB_FORCE_INLINE void TbBreak() { __debugbreak(); }
#else
    TB_FORCE_INLINE void TbBreak() {}
#endif
}
#elif (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_FLASH) || (TB_PLATFORM_IS_EMSCRIPTEN) || (TB_PLATFORM_IS_WIIU)
#include <signal.h>
namespace toybox
{
#if TB_IS_DEBUG
    TB_FORCE_INLINE void TbBreak() { raise(SIGTRAP); }
#else
    TB_FORCE_INLINE void TbBreak() {}
#endif
}
#elif (TB_PLATFORM_IS_NX)
#include <nn/nn_Assert.h>
namespace toybox
{
#if TB_IS_DEBUG
    TB_FORCE_INLINE void TbBreak() { NN_ASSERT(0); }
#else
    TB_FORCE_INLINE void TbBreak() {}
#endif
}
#elif (TB_PLATFORM_IS_WIIU)
namespace toybox
{
#if TB_IS_DEBUG
    TB_FORCE_INLINE void TbBreak() { OSDebug(); }
#else
    TB_FORCE_INLINE void TbBreak() {}
#endif
}
#else
#error "unknown platforn"
#endif

#endif
