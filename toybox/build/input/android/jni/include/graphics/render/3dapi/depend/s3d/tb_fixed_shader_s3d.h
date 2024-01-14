/*!
 * 固定機能シェーダのシミュレータ
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#ifndef _INCLUDED_TB_FIXED_SHADER_AGAL_H_
#define _INCLUDED_TB_FIXED_SHADER_AGAL_H_

#include <base/math/tb_matrix.h>
#include <graphics/3dapi/tb_shader_types.h>
#include <graphics/3dapi/tb_shader_manager.h>

namespace toybox
{

/*!
 * 固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbFixedShaderDepend
{
    friend class TbFixedShader;
public:

    // コンストラクタ
    TbFixedShaderDepend();

    // デストラクタ
    ~TbFixedShaderDepend();

public:

    // デバイス生成時に呼ぶ関数
    void Setup();

    // 何か変更されている
    TbBool IsChanged() const {
        return m_BitArray.Test( FLAG_CHANGED_MVP_MATRIX ) || m_BitArray.Test( FLAG_CHANGED_FORMAT );
    }

    // 頂点属性を利用可能にする (StreamSourceを利用)
    TbBool EnableVertexAttribute();

    // 行列設定
    TbResult SetMatrix( TbFixedShaderMatrixType matrixType , const TbMatrix& matrix );

    // 行列取得
    TbResult GetMatrix( TbFixedShaderMatrixType matrixType , TbMatrix& matrix );

private:
    
    enum Flag
    {
        FLAG_ENABLE_LIGHT , 
        FLAG_CHANGED_MVP_MATRIX , // ModelViewProjection行列が変更された
        FLAG_CHANGED_FORMAT     , // フォーマットが変わった
    };

    enum FormatFlag
    {
        FORMAT_FLAG_INVALID = -1 , 
        FORMAT_FLAG_POS ,       // 位置
        FORMAT_FLAG_COLOR ,     // 頂点カラー
        FORMAT_FLAG_TEXCOORD ,  // テクスチャ
        FORMAT_FLAG_NORMAL ,    // 法線
    };
    static const FormatFlag FORMAT_FLAG_CONVERT_TABLE[];

    struct Program
    {
        Program() 
            : program(NULL)
            , mvpMatrixHandle()
        {}
        TbShaderProgram*    program;
        TbShaderConstHandle mvpMatrixHandle;
        
    };

    enum ShaderType
    {
        SHADER_TYPE_INVALID = -1 ,
        SHADER_TYPE_BEGIN , 
        SHADER_TYPE_COL = SHADER_TYPE_BEGIN ,           // 頂点カラーあり、テクスチャなし、ライトなし
        SHADER_TYPE_COL_TEX ,       // 頂点カラーあり、テクスチャあり、ライトなし
        SHADER_TYPE_COL_TEX_LIGHT , // 頂点カラーあり、テクスチャあり、ライトあり (要法泉)
        SHADER_TYPE_END ,
        SHADER_TYPE_MAX , 
    };

private:

    // シェーダ更新
    void updateShader();

    // 頂点フォーマット設定
    void setVertexFormat( const TbVertexFormat* format );

private:
    
    TbBitArray32            m_BitArray;                             //!< ビット配列
    TbBitArray32            m_FormatBitArray;                       //!< フォーマットビット配列
    Program                 m_Program[SHADER_TYPE_MAX];             //!< プログラム
    ShaderType              m_CurrentShader;                        //!< 現在のシェーダ
    TbMatrix                m_Matrix[TB_FIXED_SHADER_MATRIX_MAX];   //!< 行列
};

}

#endif