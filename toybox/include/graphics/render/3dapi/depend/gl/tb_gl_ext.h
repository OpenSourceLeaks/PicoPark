/*!
 * GL拡張を吸収するヘッダー
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_GL_EXT_H_
#define _INCLUDED_TB_GL_EXT_H_

#if TB_GLAPI_IS_GLES
// Android/IOS

#   if TB_PLATFORM_IS_IOS // IOSでは16bit floatは使えないようにする
#       define TB_GL_FLOAT16    GL_INVALID_ENUM
#   else
#       define TB_GL_FLOAT16    GL_HALF_FLOAT_OES
#   endif

#   define TbGlClearDepth   glClearDepthf

namespace toybox
{
    // カラーアタッチメント (MRT非対応)
    const GLenum TB_GL_COLOR_ATTACKMENT_TYPE[] = {
        GL_COLOR_ATTACHMENT0 ,
    };
}

// RenderBuffer用
#   define TbGlBindRenderbuffer             glBindRenderbuffer
#   define TbGlGenRenderbuffers             glGenRenderbuffers
#   define TbGlRenderbufferStorage          glRenderbufferStorage
#   define TB_GL_RENDERBUFFER               GL_RENDERBUFFER
// FrameBuffer用
#   define TbGlBindFramebuffer              glBindFramebuffer
#   define TbGlGenFramebuffers              glGenFramebuffers
#   define TbGlFramebufferStorage           glFramebufferStorage
#   define TbGlFramebufferTexture2D         glFramebufferTexture2D
#   define TbGlFramebufferRenderbuffer      glFramebufferRenderbuffer
#   define TB_GL_FRAMEBUFFER                GL_FRAMEBUFFER
#   define TB_GL_DEPTH_ATTACHMENT           GL_DEPTH_ATTACHMENT

// 使えません
#   define TB_GL_MIN               GL_INVALID_ENUM
#   define TB_GL_MAX               GL_INVALID_ENUM
#ifdef GL_BGRA
#   define TB_GL_BGRA              GL_BGRA
#else
#   define TB_GL_BGRA              GL_INVALID_ENUM
#endif
#ifdef GL_BGR
#   define TB_GL_BGR               GL_BGR
#else
#   define TB_GL_BGR              GL_INVALID_ENUM
#endif
#   define TB_GL_RGBA32F           GL_INVALID_ENUM
#   define TB_GL_DEPTH_COMPONENT24 GL_INVALID_ENUM
#   define TB_GL_DEPTH_COMPONENT32 GL_INVALID_ENUM

#elif (TB_PLATFORM_IS_WIN)
// Windows
#   define TB_GL_MIN                GL_MIN
#   define TB_GL_MAX                GL_MAX
#   define TB_GL_FLOAT16            GL_FLOAT16_NV
#   define TB_GL_BGR                GL_BGR
#   define TB_GL_BGRA               GL_BGRA
#   define TB_GL_RGBA32F            GL_RGBA32F
#   define TB_GL_DEPTH_COMPONENT24  GL_DEPTH_COMPONENT24
#   define TB_GL_DEPTH_COMPONENT32  GL_DEPTH_COMPONENT32
#   define TbGlClearDepth           glClearDepth
namespace toybox
{
    // カラーアタッチメント (MRT対応)
    const GLenum TB_GL_COLOR_ATTACKMENT_TYPE[] = 
    {
        GL_COLOR_ATTACHMENT0_EXT ,
        GL_COLOR_ATTACHMENT1_EXT ,
        GL_COLOR_ATTACHMENT2_EXT ,
        GL_COLOR_ATTACHMENT3_EXT ,
        GL_COLOR_ATTACHMENT4_EXT ,
        GL_COLOR_ATTACHMENT5_EXT ,
        GL_COLOR_ATTACHMENT6_EXT ,
        GL_COLOR_ATTACHMENT7_EXT ,
        GL_COLOR_ATTACHMENT8_EXT ,
        GL_COLOR_ATTACHMENT9_EXT ,
    };
}

// RenderBuffer用
#   define TbGlBindRenderbuffer             glBindRenderbufferEXT
#   define TbGlGenRenderbuffers             glGenRenderbuffersEXT
#   define TbGlRenderbufferStorage          glRenderbufferStorageEXT
#   define TB_GL_RENDERBUFFER               GL_RENDERBUFFER_EXT
// FrameBuffer用
#   define TbGlBindFramebuffer              glBindFramebufferEXT
#   define TbGlGenFramebuffers              glGenFramebuffersEXT
#   define TbGlFramebufferStorage           glFramebufferStorageEXT
#   define TbGlFramebufferTexture2D         glFramebufferTexture2DEXT
#   define TbGlFramebufferRenderbuffer      glFramebufferRenderbufferEXT
#   define TB_GL_FRAMEBUFFER                GL_FRAMEBUFFER_EXT
#   define TB_GL_DEPTH_ATTACHMENT           GL_DEPTH_ATTACHMENT_EXT

#endif

#endif
