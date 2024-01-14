/*! 
 * TbWindowの機種別定義
 * @author Miyake Shunsuke
 * @since 2011.03.07
 */
#ifndef _INCLUDED_TB_WINDOW_DEPEND_TYPES_H_
#define _INCLUDED_TB_WINDOW_DEPEND_TYPES_H_

namespace toybox
{

// ファイル分けはしない
#if (TB_PLATFORM_IS_WIN)
    typedef TbAddr TbWindowId;
#elif (TB_PLATFORM_IS_ANDROID) || (TB_PLATFORM_IS_FLASH) || (TB_PLATFORM_IS_EMSCRIPTEN) || (TB_PLATFORM_IS_IOS) || (TB_PLATFORM_IS_NX)
    // 必ずシングルウィンドウなので特に意味はない
    typedef TbUint32 TbWindowId;
#endif

}

#endif
