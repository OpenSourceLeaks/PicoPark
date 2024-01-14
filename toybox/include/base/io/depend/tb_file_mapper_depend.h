/*!
 * プラットフォーム別依存クラスインクルード
 * @author teco
 */

#ifndef _INCLUDED_TB_FILE_MAPPER_DEPEND_H_
#define _INCLUDED_TB_FILE_MAPPER_DEPEND_H_

#if (TB_PLATFORM_IS_WIN)
    #include "./win32/tb_file_mapper_win32.h"
    typedef toybox::TbFileMapperWin32 TbFileMapperDefault;
#elif (TB_PLATFORM_IS_ANDROID)
    #include "./android/tb_file_mapper_android.h"
    typedef toybox::TbFileMapperAndroid TbFileMapperDefault;
#elif (TB_PLATFORM_IS_EMSCRIPTEN || TB_PLATFORM_IS_APPLE)
    #include "./std/tb_file_mapper_std.h"
    typedef toybox::TbFileMapperStd TbFileMapperDefault;
#elif (TB_PLATFORM_IS_WIIU)
    #include "./wiiu/tb_file_mapper_wiiu.h"
    typedef toybox::TbFileMapperWiiU TbFileMapperDefault;
#elif (TB_PLATFORM_IS_NX)
    #include "./nx/tb_file_mapper_nx.h"
    typedef toybox::TbFileMapperNX      TbFileMapperDefault;
#endif

#endif
