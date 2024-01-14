/*!
 * @file
 * @brief GX2用Utiluty関数
 */
 
#ifndef _INCLUDED_TB_RENDER_UTIL_GX2_H_
#define _INCLUDED_TB_RENDER_UTIL_GX2_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include "base/system/tb_assert.h"

#if TB_3DAPI_IS_GX2

namespace toybox
{
    // toybox用カラーフォーマットから3DAPIに適したフォーマットに変換
    inline TbColorFormat TbGetOptimizedColorFormat( const TbColorFormat src )
    {
        static const TbColorFormat CONVERT_TABLE[] = 
        {
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_UNKNOWN

            TB_COLOR_FORMAT_A4G4B4R4 , // TB_COLOR_FORMAT_A4G4B4R4
            TB_COLOR_FORMAT_UNKNOWN   , // TB_COLOR_FORMAT_R5G6B5

            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_R8G8B8
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_B8G8R8

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

    // toybox用フィルタモードをGX2用に変換
    inline TbBool TbTextureFilterModeToGX2TexXYFilterType( GX2TexXYFilterType* dst , const TbTextureFilterMode& filterMode )
    {
        static const GX2TexXYFilterType CONVERT_TABLE[] = {
            GX2_TEX_XY_FILTER_POINT , 
            GX2_TEX_XY_FILTER_BILINEAR ,
        };
        TbBool isValid = ( TB_TEXTURE_FILTER_MODE_BEGIN <= filterMode && filterMode < TB_TEXTURE_FILTER_MODE_END );
        TB_ASSERT(isValid);
        *dst = isValid ? CONVERT_TABLE[ filterMode - TB_TEXTURE_FILTER_MODE_BEGIN ] : GL_NEAREST;
        return isValid;
    }
    
    // toybox用アドレスモードをGX2用に変換
    inline TbBool TbTextureAddressModeToGX2TexClamp( GX2TexClamp* dst , const TbTextureAddressMode& mode )
    {
        static const GLenum CONVERT_TABLE[] = {
            GX2_TEX_CLAMP_WRAP , 
            GX2_TEX_CLAMP_CLAMP , // GL_CLAMPはOpenGL 3.0以降非推奨なため
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_TEXTURE_ADDRESS_MODE_MAX );
        TbBool isValid = ( TB_TEXTURE_ADDRESS_MODE_BEGIN <= mode && mode < TB_TEXTURE_ADDRESS_MODE_END );
        TB_ASSERT(isValid);
        *dst = isValid ? CONVERT_TABLE[ mode - TB_TEXTURE_ADDRESS_MODE_BEGIN ] : GL_REPEAT;
        return isValid;
    }

    // toybox用カラーフォーマットをGX2SurfaceFormatに変換
    inline TbBool TbColorFormatToGX2SurfaceFormat( GX2SurfaceFormat* dst , const TbColorFormat& mode )
    {
        static const GLenum CONVERT_TABLE[] = 
        {
            GX2_SURFACE_FORMAT_INVALID  ,   // D3DFMT_UNKNOWN

            GX2_SURFACE_FORMAT_TC_R4_G4_B4_A4_UNORM , // TB_COLOR_FORMAT_A4G4B4R4
            GX2_SURFACE_FORMAT_INVALID   , // TB_COLOR_FORMAT_R5G6B5

            GX2_SURFACE_FORMAT_INVALID ,   // TB_COLOR_FORMAT_R8G8B8
            GX2_SURFACE_FORMAT_INVALID    ,   // TB_COLOR_FORMAT_B8G8R8

            GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM ,  // TB_COLOR_FORMAT_A8R8G8B8
            GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM ,  // TB_COLOR_FORMAT_B8G8R8A8
            GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM ,  // TB_COLOR_FORMAT_A8B8G8R8
            GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM ,  // TB_COLOR_FORMAT_X8R8G8B8

            GX2_SURFACE_FORMAT_TC_R32_G32_B32_A32_FLOAT , // TB_COLOR_FORMAT_A32R32G32B32

            GX2_SURFACE_FORMAT_INVALID , // TB_COLOR_FORMAT_D16
            GX2_SURFACE_FORMAT_INVALID , // TB_COLOR_FORMAT_D32
            GX2_SURFACE_FORMAT_D_D24_S8_UNORM  , // TB_COLOR_FORMAT_D24S8
        };

        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_COLOR_FORMAT_NUM );
        if( TB_COLOR_FORMAT_BEGIN <= mode && mode < TB_COLOR_FORMAT_END ){
            *dst = CONVERT_TABLE[ mode - TB_COLOR_FORMAT_BEGIN ];
            if( TB_VERIFY(GX2_SURFACE_FORMAT_INVALID != *dst) ){
                return TB_TRUE;
            }
        }
        *dst = GX2_SURFACE_FORMAT_INVALID;
        TB_ASSERT(!"Invalid mode");
        return TB_FALSE;
    }

    // toybox用ブレンド係数 -> GX2
    inline TbBool TbBlendFactorToGX2BlendFunction( GX2BlendFunction* dst , const TbBlendFactor& src )
    {
        static const GX2BlendFunction CONVERT_TABLE[] =
        {
            GX2_BLEND_ZERO , 
            GX2_BLEND_ONE , 
            GX2_BLEND_SRC_COLOR ,
            GX2_BLEND_ONE_MINUS_SRC_COLOR ,
            GX2_BLEND_DST_COLOR ,
            GX2_BLEND_ONE_MINUS_DST_COLOR ,
            GX2_BLEND_SRC_ALPHA ,
            GX2_BLEND_ONE_MINUS_SRC_ALPHA ,
            GX2_BLEND_DST_ALPHA , 
            GX2_BLEND_ONE_MINUS_DST_ALPHA ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_FACTOR_MAX );
        if( TB_BLEND_FACTOR_BEGIN <= src && src < TB_BLEND_FACTOR_END ){
            *dst = CONVERT_TABLE[src-TB_BLEND_FACTOR_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }       

    // toybox用ブレンド演算子 -> GX2
    inline TbBool TbBlendOpToGX2BlendCombine( GX2BlendCombine* dst , const TbBlendOp& src )
    {
        static const GX2BlendCombine CONVERT_TABLE[] =
        {
            GX2_BLEND_COMBINE_ADD , 
            GX2_BLEND_COMBINE_SRC_MINUS_DST , 
            GX2_BLEND_COMBINE_DST_MINUS_SRC ,
            GX2_BLEND_COMBINE_MIN , 
            GX2_BLEND_COMBINE_MAX , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_OP_MAX );
        if( TB_BLEND_OP_BEGIN <= src && src < TB_BLEND_OP_END ){
            *dst = CONVERT_TABLE[src-TB_BLEND_OP_BEGIN];
            return TB_TRUE;
        }
        TB_ASSERT(!"Invalid TbBlendOp");
        *dst = GX2_BLEND_COMBINE_ADD;
        return TB_FALSE;
    }

    // toybox用比較関数 -> GX2CompareFunction
    inline TbBool TbCmpFuncToGX2CompareFunction( GX2CompareFunction* dst , const TbCmpFunc& func )
    {
        static const GX2CompareFunction CONVERT_TABLE[] =
        {
            GX2_COMPARE_NEVER ,
            GX2_COMPARE_LESS ,
            GX2_COMPARE_EQUAL ,
            GX2_COMPARE_LEQUAL ,
            GX2_COMPARE_GREATER ,
            GX2_COMPARE_NOTEQUAL ,
            GX2_COMPARE_GEQUAL ,
            GX2_COMPARE_ALWAYS 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_CMP_FUNC_MAX );
        if( TB_CMP_FUNC_BEGIN <= func && func < TB_CMP_FUNC_END ){
            *dst = CONVERT_TABLE[func-TB_CMP_FUNC_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用ステンシル関数 -> GX2StencilFunction
    inline TbBool TbStencilOpToGX2StencilFunction( GX2StencilFunction* dst , const TbStencilOp& op )
    {
        static const GLenum CONVERT_TABLE[] =
        {
            GX2_STENCIL_KEEP ,
            GX2_STENCIL_ZERO ,
            GX2_STENCIL_REPLACE ,
            GX2_STENCIL_INCR_WRAP ,
            GX2_STENCIL_DECR_WRAP ,
            GX2_STENCIL_INVERT ,
            GX2_STENCIL_INCR ,
            GX2_STENCIL_DECR , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_STENCIL_OP_MAX );
        if( TB_STENCIL_OP_BEGIN <= op && op < TB_STENCIL_OP_END ){
            *dst = CONVERT_TABLE[ op - TB_STENCIL_OP_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }
       
}
#endif

#endif
