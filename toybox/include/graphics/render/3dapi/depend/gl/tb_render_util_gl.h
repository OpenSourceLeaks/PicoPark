/*!
 * @file
 * @brief GL用Utiluty関数
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */
 
#ifndef _INCLUDED_TB_RENDER_STD_GL_UTIL_H_
#define _INCLUDED_TB_RENDER_STD_GL_UTIL_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include "base/system/tb_assert.h"

#if TB_3DAPI_TARGET == TB_3DAPI_GL

namespace toybox
{
    inline void TbCheckErrorGL()
    {
#if !TB_PLATFORM_IS_ANDROID && TB_IS_DEBUG
        GLenum error = glGetError();
        TB_ASSERT(0==error);
#endif
    }

    // toybox用カラーフォーマットから3DAPIに適したフォーマットに変換
    inline TbColorFormat TbGetOptimizedColorFormat( const TbColorFormat src )
    {
        static const TbColorFormat CONVERT_TABLE[] = 
        {
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_UNKNOWN

            TB_COLOR_FORMAT_A4G4B4R4 , // TB_COLOR_FORMAT_A4G4B4R4
            TB_COLOR_FORMAT_R5G6B5   , // TB_COLOR_FORMAT_R5G6B5

            TB_COLOR_FORMAT_B8G8R8 ,   // TB_COLOR_FORMAT_R8G8B8
            TB_COLOR_FORMAT_R8G8B8 ,   // TB_COLOR_FORMAT_B8G8R8

            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_A8R8G8B8
            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_B8G8R8A8
            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_A8B8G8R8
            TB_COLOR_FORMAT_A8B8G8R8 ,  // TB_COLOR_FORMAT_X8R8G8B8

            TB_COLOR_FORMAT_A32R32G32B32 , // TB_COLOR_FORMAT_A32R32G32B32

            TB_COLOR_FORMAT_D16 , // TB_COLOR_FORMAT_D16
            TB_COLOR_FORMAT_D32 , // TB_COLOR_FORMAT_D32
            TB_COLOR_FORMAT_D24S8 , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM );
        if( TB_COLOR_FORMAT_BEGIN <= src && src < TB_COLOR_FORMAT_END ){
            return CONVERT_TABLE[ src - TB_COLOR_FORMAT_BEGIN ];
        }
        return TB_COLOR_FORMAT_UNKNOWN;
    }

    // toybox用フィルタモードをGL用に変換
    inline TbBool TbTextureFilterModeToGLenum( GLenum* dst , const TbTextureFilterMode& filterMode )
    {
        static const GLenum CONVERT_TABLE[] = {
            GL_NEAREST , 
            GL_LINEAR ,
        };
        TbBool isValid = ( TB_TEXTURE_FILTER_MODE_BEGIN <= filterMode && filterMode < TB_TEXTURE_FILTER_MODE_END );
        TB_ASSERT(isValid);
        *dst = isValid ? CONVERT_TABLE[ filterMode - TB_TEXTURE_FILTER_MODE_BEGIN ] : GL_NEAREST;
        return isValid;
    }
    // toybox用アドレスモードをGL用に変換
    inline TbBool TbTextureAddressModeToGLenum( GLenum* dst , const TbTextureAddressMode& mode )
    {
        static const GLenum CONVERT_TABLE[] = {
            GL_REPEAT , 
            GL_CLAMP_TO_EDGE , // GL_CLAMPはOpenGL 3.0以降非推奨なため
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_TEXTURE_ADDRESS_MODE_MAX );
        TbBool isValid = ( TB_TEXTURE_ADDRESS_MODE_BEGIN <= mode && mode < TB_TEXTURE_ADDRESS_MODE_END );
        TB_ASSERT(isValid);
        *dst = isValid ? CONVERT_TABLE[ mode - TB_TEXTURE_ADDRESS_MODE_BEGIN ] : GL_REPEAT;
        return isValid;
    }

    // toybox用カラーフォーマットをGLenumに変換
    inline TbBool TbColorFormatToGLenum( GLenum* dst , const TbColorFormat& mode )
    {
        static const GLenum CONVERT_TABLE[] = 
        {
            GL_RGB ,   // D3DFMT_UNKNOWN

            GL_RGBA4 , // TB_COLOR_FORMAT_A4G4B4R4
            GL_RGB   , // TB_COLOR_FORMAT_R5G6B5

            TB_GL_BGR ,   // TB_COLOR_FORMAT_R8G8B8
            GL_RGB    ,   // TB_COLOR_FORMAT_B8G8R8

            TB_GL_BGRA ,  // TB_COLOR_FORMAT_A8R8G8B8
            TB_GL_BGRA ,  // TB_COLOR_FORMAT_B8G8R8A8
            GL_RGBA    ,  // TB_COLOR_FORMAT_A8B8G8R8
            TB_GL_BGRA ,  // TB_COLOR_FORMAT_X8R8G8B8

            TB_GL_RGBA32F , // TB_COLOR_FORMAT_A32R32G32B32

            GL_DEPTH_COMPONENT16 , // TB_COLOR_FORMAT_D16
            TB_GL_DEPTH_COMPONENT24 , // TB_COLOR_FORMAT_D32
            TB_GL_DEPTH_COMPONENT32 , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM );
        if( TB_COLOR_FORMAT_BEGIN <= mode && mode < TB_COLOR_FORMAT_END ){
            *dst = CONVERT_TABLE[ mode - TB_COLOR_FORMAT_BEGIN ];
            if( TB_VERIFY(GL_INVALID_ENUM != *dst) ){
                return TB_TRUE;
            }
        }
        *dst = GL_RGB;
        TB_ASSERT(!"Invalid mode");
        return TB_FALSE;
    }

    // toybox用カラーフォーマットをテクスチャ内部フォーマットのGLenumに変換
    inline TbBool TbColorFormatToTexInternalFormatGLenum( GLenum* dst , const TbColorFormat& mode )
    {
        static const GLenum CONVERT_TABLE[] = 
        {
            GL_RGB ,   // D3DFMT_UNKNOWN

            GL_RGBA , // TB_COLOR_FORMAT_A4G4B4R4
            GL_RGB  , // TB_COLOR_FORMAT_R5G6B5

            GL_RGB ,   // TB_COLOR_FORMAT_R8G8B8
            GL_RGB ,   // TB_COLOR_FORMAT_B8G8R8

            GL_RGBA ,  // TB_COLOR_FORMAT_A8R8G8B8
            GL_RGBA ,  // TB_COLOR_FORMAT_B8G8R8A8
            GL_RGBA ,  // TB_COLOR_FORMAT_A8B8G8R8
            GL_RGBA ,  // TB_COLOR_FORMAT_X8R8G8B8

            TB_GL_RGBA32F , // TB_COLOR_FORMAT_A32R32G32B32

            GL_DEPTH_COMPONENT16 , // TB_COLOR_FORMAT_D16
            TB_GL_DEPTH_COMPONENT24 , // TB_COLOR_FORMAT_D32
            TB_GL_DEPTH_COMPONENT32 , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM );
        if( TB_COLOR_FORMAT_BEGIN <= mode && mode < TB_COLOR_FORMAT_END ){
            *dst = CONVERT_TABLE[ mode - TB_COLOR_FORMAT_BEGIN ];
            if( GL_INVALID_ENUM != *dst ){
                return TB_TRUE;
            }
        }
        *dst = GL_RGB;
        TB_ASSERT(!"Invalid mode");
        return TB_FALSE;
    }

    // toybox用ブレンド係数 -> GLenum
    inline TbBool TbBlendFactorToGLenum( GLenum* dst , const TbBlendFactor& src )
    {
        static const GLenum CONVERT_TABLE[] =
        {
            GL_ZERO , 
            GL_ONE , 
            GL_SRC_COLOR ,
            GL_ONE_MINUS_SRC_COLOR ,
            GL_DST_COLOR ,
            GL_ONE_MINUS_DST_COLOR ,
            GL_SRC_ALPHA ,
            GL_ONE_MINUS_SRC_ALPHA ,
            GL_DST_ALPHA , 
            GL_ONE_MINUS_DST_ALPHA ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_FACTOR_MAX );
        if( TB_BLEND_FACTOR_BEGIN <= src && src < TB_BLEND_FACTOR_END ){
            *dst = CONVERT_TABLE[src-TB_BLEND_FACTOR_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }       

    // toybox用ブレンド演算子 -> GLenum
    inline TbBool TbBlendOpToGLenum( GLenum* dst , const TbBlendOp& src )
    {
        static const GLenum CONVERT_TABLE[] =
        {
            GL_FUNC_ADD , 
            GL_FUNC_SUBTRACT , 
            GL_FUNC_REVERSE_SUBTRACT ,
            TB_GL_MIN , 
            TB_GL_MAX , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_OP_MAX );
        if( TB_BLEND_OP_BEGIN <= src && src < TB_BLEND_OP_END ){
            *dst = CONVERT_TABLE[src-TB_BLEND_OP_BEGIN];
            if( *dst!=GL_INVALID_ENUM ){
                return TB_TRUE;
            }
        }
        TB_ASSERT(!"Invalid TbBlendOp");
        *dst = GL_FUNC_ADD;
        return TB_FALSE;
    }

    // toybox用比較関数 -> GLenum
    inline TbBool TbCmpFuncToGLenum( GLenum* dst , const TbCmpFunc& func )
    {
        static const GLenum CONVERT_TABLE[] =
        {
            GL_NEVER ,
            GL_LESS ,
            GL_EQUAL ,
            GL_LEQUAL ,
            GL_GREATER ,
            GL_NOTEQUAL ,
            GL_GEQUAL ,
            GL_ALWAYS 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_CMP_FUNC_MAX );
        if( TB_CMP_FUNC_BEGIN <= func && func < TB_CMP_FUNC_END ){
            *dst = CONVERT_TABLE[func-TB_CMP_FUNC_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // GLenum -> toybox用比較関数
    inline TbBool GLenumToTbCmpFunc( TbCmpFunc* dst , const GLenum& src )
    {
        TbCmpFunc result;
        switch( src )
        {
        case GL_NEVER:
            result = TB_CMP_FUNC_NEVER;
            break;
        case GL_LESS:
            result = TB_CMP_FUNC_LESS;
            break;
        case GL_EQUAL:
            result = TB_CMP_FUNC_EQUAL;
            break;
        case GL_LEQUAL:
            result = TB_CMP_FUNC_LESS_EQUAL;
            break;
        case GL_GREATER:
            result = TB_CMP_FUNC_GREATER;
            break;
        case GL_NOTEQUAL:
            result = TB_CMP_FUNC_NOT_EQUAL;
            break;
        case GL_GEQUAL:
            result = TB_CMP_FUNC_GREATER_EQUAL;
            break;
        case GL_ALWAYS:
            result = TB_CMP_FUNC_ALWAYS;
            break;
        default:
            return TB_FALSE;
        };
        if( dst ){
            *dst = result;
        }
        return TB_TRUE;
    }

    // toybox用ステンシル関数 -> GLenum
    inline TbBool TbStencilOpToGLenum( GLenum* dst , const TbStencilOp& op )
    {
        static const GLenum CONVERT_TABLE[] =
        {
            GL_KEEP ,
            GL_ZERO ,
            GL_REPLACE ,
            GL_INCR_WRAP ,
            GL_DECR_WRAP ,
            GL_INVERT ,
            GL_INCR ,
            GL_DECR , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_STENCIL_OP_MAX );
        if( TB_STENCIL_OP_BEGIN <= op && op < TB_STENCIL_OP_END ){
            *dst = CONVERT_TABLE[ op - TB_STENCIL_OP_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用シェーダタイプ -> GLenum
    inline TbBool TbShaderTypeToGLenum( GLenum* dst , const TbShaderType& src )
    {
        static const GLenum CONVERT_TABLE[] =
        {
            GL_VERTEX_SHADER , 
            GL_FRAGMENT_SHADER ,
#if 0
            GL_GEOMETRY_SHADER ,
#else
            0
#endif
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_SHADER_TYPE_MAX );
        if( TB_SHADER_TYPE_BEGIN <= src && src < TB_SHADER_TYPE_END ){
            *dst = CONVERT_TABLE[ src - TB_SHADER_TYPE_BEGIN ];
            return TB_TRUE;
        }
        return TB_FALSE;
    }
       
}
#endif

#endif
