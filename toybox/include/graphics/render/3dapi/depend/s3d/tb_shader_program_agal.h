/*!
 * シェーダープログラム ( AGAL )
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
 
#ifndef _INCLUDED_TB_SHADER_PROGRAM_AGAL_H_
#define _INCLUDED_TB_SHADER_PROGRAM_AGAL_H_

#include <base/container/tb_array.h>
#include <base/math/tb_matrix.h>
#include <graphics/3dapi/tb_shader_manager.h>

namespace toybox
{

class TbShaderProgramDepend
{
    friend class TbShaderProgram;
public:

    // コンストラクタ
    TbShaderProgramDepend();

    // デストラクタ
    ~TbShaderProgramDepend();

public:

    // プログラム取得
    AS3::ui::flash::display3D::Program3D& GetProgram() {
        return m_Program;
    }

    // プログラム取得
    const AS3::ui::flash::display3D::Program3D& GetProgram() const {
        return m_Program;
    }

    // リンクされている
    TbBool IsLinked() const {
        return m_BitArray.Test(FLAG_LINK);
    }

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

    TbBitArray16                            m_BitArray;                   //!< ビット配列
    AS3::ui::flash::display3D::Program3D    m_Program;                    //!< プログラム

};

}

#endif
