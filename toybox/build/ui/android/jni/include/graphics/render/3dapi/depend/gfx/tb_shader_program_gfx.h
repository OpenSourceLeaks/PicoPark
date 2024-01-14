/*!
 * シェーダープログラム ( GL4 for NX )
 * @author teco
 */
 
#ifndef _INCLUDED_TB_SHADER_PROGRAM_GFX_H_
#define _INCLUDED_TB_SHADER_PROGRAM_GFX_H_

#include <base/container/tb_array.h>
#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_shader_manager.h>

namespace toybox
{

class TbShaderProgram;

/*!
 * シェーダー管理GFX依存クラス
 */
class TbShaderProgramDepend
{
    friend class TbShaderProgram;
public:

    // コンストラクタ
    TbShaderProgramDepend();

    // デストラクタ
    ~TbShaderProgramDepend();

public:

    void SetCommon(TbShaderProgram* common) {
        m_Common = common;
    }

    // シェーダ取得
    TbShaderId GetVertexShaderId() {
        return m_VertexShaderId;
    }

    // シェーダ取得
    TbShaderId GetPixelShaderId() {
        return m_PixelShaderId;
    }

    // 定数ハンドル取得 (名前から直接取得)
    TbBool UpdateConstantHandle(TbUint32 shaderBit, TbShaderConstHandleDepend& handle);

private:

    TbBitArray16        m_BitArray;  //!< ビット配列
    TbShaderId          m_VertexShaderId;
    TbShaderId          m_PixelShaderId;
    TbShaderProgram*    m_Common;

};

}

#endif
