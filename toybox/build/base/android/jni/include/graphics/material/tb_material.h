/*!
 * マテリアルクラス
 * @author Miyake Shunsuke
 * @since 2011.06.16
 */

#ifndef _INCLUDED_TB_MATERIAL_H_
#define _INCLUDED_TB_MATERIAL_H_

#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{

class TbShaderProgram;
class TbTexture;

/*!
 * マテリアル
 * @author Miyake Shunsuke
 * @since 2011.06.17
 */
class TbMaterial
{
public:

    // コンストラクタ
    TbMaterial() : m_Program(nullptr) {}

    // デストラクタ
    virtual ~TbMaterial() {}

private:

    TB_DISALLOW_COPY_AND_ASSIGN(TbMaterial);

public:

    // プログラム取得
    TbShaderProgram* GetShaderProgram() {
        return m_Program;
    }

    // プログラム設定
    void SetShaderProgram( TbShaderProgram* program ) {
        m_Program = program;
    }

public:

    // シェーダーパラメータをGPUに送る
    virtual void SendShaderParam( TbUint32 pass , 
                                  const TbMatrix* world , 
                                  const TbMatrix43* boneMatrix , 
                                  TbUint32        boneMatrixCount ) = 0;

protected:

    // シェーダプログラムを有効化
    void ActivateProgram( TbUint32 passIndex );

private: // メンバ変数

    TbShaderProgram*    m_Program;          // プログラム

};

}

#endif
