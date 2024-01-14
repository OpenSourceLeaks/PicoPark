/*!
 * シェーダープログラム ( GLSL )
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
 
#ifndef _INCLUDED_TB_SHADER_PROGRAM_GLSL_H_
#define _INCLUDED_TB_SHADER_PROGRAM_GLSL_H_

#include <base/container/tb_array.h>
#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_shader_manager.h>

namespace toybox
{

/*!
 * シェーダー管理GLSL依存クラス
 * @author Miyake Shunsuke
 * @since 2010.06.11
 */
class TbShaderProgramDepend
{
    friend class TbShaderProgram;
public:

    static const TbUint32 CONSTANT_MAX = 10;

public:

    // コンストラクタ
    TbShaderProgramDepend();

    // デストラクタ
    ~TbShaderProgramDepend();

public:

    // プログラム取得
    GLuint GetProgram() const {
        return m_Program;
    }

    // リンクされている
    TbBool IsLinked() const {
        return m_BitArray.Test(FLAG_LINK);
    }

    // 定数ハンドル取得 (名前から直接取得)
    void UpdateConstantHandle( TbShaderConstHandleDepend& handle );

private:

    // リンク
    TbResult link();

    // リンク解除
    void resetLink() {
        m_BitArray.SetBit(FLAG_LINK,TB_FALSE);
    }

private:

    enum{
        FLAG_LINK , // リンク済み
        FLAG_TURN_OVER,
    };

private:

    TbBitArray16        m_BitArray;                   //!< ビット配列
    GLuint              m_Program;                    //!< プログラム

};

}

#endif
