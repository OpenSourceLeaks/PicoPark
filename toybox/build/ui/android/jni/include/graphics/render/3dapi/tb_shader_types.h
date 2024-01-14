/*!
 * シェーダー用定義
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */

#ifndef _INCLUDED_TB_SHADER_TYPES_H_
#define _INCLUDED_TB_SHADER_TYPES_H_

#include <base/container/tb_pool.h>

namespace toybox
{

// シェーダID
typedef TbUint64 TbShaderId;

// エフェクトシェーダID
typedef TbUint64 TbShaderEffectId;

// シェーダID不正値
const TbShaderId TB_SHADER_ID_INVALID = TB_POOL_ID_INVALID;

enum TbShaderBufferType
{
    TB_SHADER_BUFFER_TYPE_OBJECT, //!< コンパイル済
    TB_SHADER_BUFFER_TYPE_SOURCE, //!< ソース文字列
};

enum TbShaderConstIndex
{
    TB_SHADER_CONST_INDEX_SYSTEM_BEGIN , 
    TB_SHADER_CONST_INDEX_WVP = TB_SHADER_CONST_INDEX_SYSTEM_BEGIN , 
    TB_SHADER_CONST_INDEX_SYSTEM_END , 
    TB_SHADER_CONST_INDEX_USER_BEGIN = TB_SHADER_CONST_INDEX_SYSTEM_END ,
    // ユーザー定義のインデックスはTB_SHADER_CONST_INDEX_USER_BEGINから


    // 最大数
    TB_SHADER_CONST_INDEX_SYSTEM_MAX = TB_SHADER_CONST_INDEX_SYSTEM_END - TB_SHADER_CONST_INDEX_SYSTEM_BEGIN
};

}

#endif
