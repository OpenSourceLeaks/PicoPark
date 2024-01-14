/*!
 * @file
 * @brief Stage3D用Utiluty関数
 * @author Miyake Shunsuke
 * @since 2010.4.28
 */
 
#ifndef _INCLUDED_TB_RENDER_UTIL_S3D_H_
#define _INCLUDED_TB_RENDER_UTIL_S3D_H_

#include <graphics/3dapi/tb_3dapi_types.h>
#include "base/system/tb_assert.h"

using namespace AS3::ui::flash::display;
using namespace AS3::ui::flash::display3D;

#if TB_3DAPI_IS_S3D

namespace toybox
{
    // toybox用カラーフォーマットから3DAPIに適したフォーマットに変換
    inline TbColorFormat TbGetOptimizedColorFormat( const TbColorFormat& src )
    {
        static const TbColorFormat CONVERT_TABLE[] = 
        {
            TB_COLOR_FORMAT_UNKNOWN ,   // TB_COLOR_FORMAT_UNKNOWN

            TB_COLOR_FORMAT_A4G4B4R4 , // TB_COLOR_FORMAT_A4G4B4R4
            TB_COLOR_FORMAT_R5G6B5   , // TB_COLOR_FORMAT_R5G6B5

            TB_COLOR_FORMAT_R8G8B8 ,   // TB_COLOR_FORMAT_R8G8B8

            TB_COLOR_FORMAT_A8R8G8B8 ,  // TB_COLOR_FORMAT_A8R8G8B8
            TB_COLOR_FORMAT_B8G8R8A8 ,  // TB_COLOR_FORMAT_B8G8R8A8
            TB_COLOR_FORMAT_A8R8G8B8 ,  // TB_COLOR_FORMAT_X8R8G8B8

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

    // toybox用ブレンド係数 -> AS3::ui::String
    inline TbBool TbBlendFactorToAS3String( AS3::ui::String& dst , const TbBlendFactor& src )
    {
        static const AS3::ui::String CONVERT_TABLE[] =
        {
            Context3DBlendFactor::ZERO , 
            Context3DBlendFactor::ONE , 
            Context3DBlendFactor::SOURCE_COLOR ,
            Context3DBlendFactor::ONE_MINUS_SOURCE_COLOR ,
            Context3DBlendFactor::DESTINATION_COLOR ,
            Context3DBlendFactor::ONE_MINUS_DESTINATION_COLOR  ,
            Context3DBlendFactor::SOURCE_ALPHA ,
            Context3DBlendFactor::ONE_MINUS_SOURCE_ALPHA ,
            Context3DBlendFactor::DESTINATION_ALPHA ,
            Context3DBlendFactor::ONE_MINUS_DESTINATION_ALPHA ,
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_BLEND_FACTOR_MAX );
        if( TB_BLEND_FACTOR_BEGIN <= src && src < TB_BLEND_FACTOR_END ){
            dst = CONVERT_TABLE[src-TB_BLEND_FACTOR_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }       

    // toybox用比較関数 -> AS3::ui::String
    inline TbBool TbCmpFuncToAS3String( AS3::ui::String& dst , const TbCmpFunc& func )
    {        
        static const AS3::ui::String CONVERT_TABLE[] =
        {
            Context3DCompareMode::NEVER ,
            Context3DCompareMode::LESS ,
            Context3DCompareMode::EQUAL ,
            Context3DCompareMode::LESS_EQUAL ,
            Context3DCompareMode::GREATER ,
            Context3DCompareMode::NOT_EQUAL ,
            Context3DCompareMode::GREATER_EQUAL ,
            Context3DCompareMode::ALWAYS 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_CMP_FUNC_MAX );
        if( TB_CMP_FUNC_BEGIN <= func && func < TB_CMP_FUNC_END ){
            dst = CONVERT_TABLE[func-TB_CMP_FUNC_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用ステンシル関数 -> AS3::ui::String
    inline TbBool TbStencilOpToAS3String( AS3::ui::String& dst , const TbStencilOp& op )
    {
        static const AS3::ui::String CONVERT_TABLE[] =
        {
            Context3DStencilAction::KEEP ,
            Context3DStencilAction::ZERO ,
            Context3DStencilAction::SET ,
            Context3DStencilAction::INCREMENT_WRAP ,
            Context3DStencilAction::DECREMENT_WRAP ,
            Context3DStencilAction::INVERT ,
            Context3DStencilAction::INCREMENT_SATURATE ,
            Context3DStencilAction::DECREMENT_SATURATE , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_STENCIL_OP_MAX );
        if( TB_STENCIL_OP_BEGIN <= op && op < TB_STENCIL_OP_END ){
            dst = CONVERT_TABLE[ op - TB_STENCIL_OP_BEGIN];
            return TB_TRUE;
        }
        return TB_FALSE;
    }

    // toybox用シェーダタイプ -> AS3::ui::String
    inline TbBool TbShaderTypeToAS3String( AS3::ui::String& dst , const TbShaderType& src )
    {
        static const AS3::ui::String CONVERT_TABLE[] =
        {
            Context3DProgramType::VERTEX , 
            Context3DProgramType::FRAGMENT ,
            "" , 
        };
        TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(CONVERT_TABLE) == TB_SHADER_TYPE_MAX );
        if( TB_SHADER_TYPE_BEGIN <= src && src < TB_SHADER_TYPE_END ){
            dst = CONVERT_TABLE[ src - TB_SHADER_TYPE_BEGIN ];
            return TB_TRUE;
        }
        return TB_FALSE;
    }
       
}
#endif

#endif
