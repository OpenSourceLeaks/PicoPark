/*!
 * @file
 * @brief OpenGLの固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_FIXED_SHADER_GL_H_
#define _INCLUDED_TB_FIXED_SHADER_GL_H_

#include <base/math/tb_matrix.h>

namespace toybox
{

/*!
 * 固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbFixedShaderDepend
{
public:
    // コンストラクタ
    TbFixedShaderDepend();
    // デストラクタ
    ~TbFixedShaderDepend();
public:
    // デバイス生成時に呼ぶ関数
    void Setup();
    
    // 頂点属性を利用可能にする (StreamSourceを利用)
    TbBool EnableVertexAttribute();

    // 頂点属性を利用可能にする
    TbBool EnableVertexAttribute( const void* vertexs , TbUint32 stride );

    // 頂点属性の使用不可にする
    TbBool DisableVertexAttribute();

    // ワールド行列設定
    void SetWorldMatrix( const TbMatrix& matrix ) { 
        m_WorldMatrix = matrix; 
    }

    // ワールド行列取得
    const TbMatrix& GetWorldMatrix() { 
        return m_WorldMatrix; 
    }

    // ビュー行列設定
    void SetViewMatrix( const TbMatrix& matrix ) { 
        m_ViewMatrix = matrix; 
    }

    // ビュー行列取得
    const TbMatrix& GetViewMatrix() { 
        return m_ViewMatrix; 
    }

    // 射影行列設定
    void SetProjectionMatrix( const TbMatrix& matrix ) { 
        m_ProjectionMatrix = matrix; 
    }

    // 射影行列取得
    const TbMatrix& GetProjectionMatrix() { 
        return m_ProjectionMatrix; 
    }

    // ベースとなるテクスチャ行列取得
    const TbMatrix& GetBaseTexMatrix() { 
        return m_BaseTexMatrix; 
    }

private:

    // 射影行列更新
    void updateProjectionMatrix();

private:
    TbBitArray16            m_BitArray;         //!< ビット配列
    TbMatrix                m_WorldMatrix;      //!< ワールド変換行列
    TbMatrix                m_ViewMatrix;       //!< ビュー変換行列
    TbMatrix                m_ProjectionMatrix; //!< 射影変換行列
    TbMatrix                m_BaseTexMatrix;    //!< ベーステクスチャ行列
};

}

#endif