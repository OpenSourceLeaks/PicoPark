/*!
 * @file
 * @brief DirectX9の頂点フォーマット依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_VERTEX_FORMAT_STD_DX9_H_
#define _INCLUDED_TB_VERTEX_FORMAT_STD_DX9_H_

namespace toybox
{

// テクスチャ座標原点位置
#define TB_TEXCOORD_BASE_TYPE TB_TEXCOORD_BASE_TYPE_LEFTUP

struct TbVertexElement;
class TbRenderDevice;

/*!
 * 頂点フォーマット依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbVertexFormatDepend
{
public:
    // コンストラクタ
    TbVertexFormatDepend( const TbVertexElement* elements , const TbSint32 elementCount );
    // デストラクタ
    ~TbVertexFormatDepend();
public:
    // 定数フォーマット構築用コンストラクタ
    TbVertexFormatDepend( const TbSint32 type );
public:
    // 頂点宣言取得
    TB_INLINE const LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclaration() const {  
        return m_VertexDeclaration;
    }
    // FVF取得
    TB_INLINE TbSint32 GetFVF() const { 
        return m_FVF; 
    }
private:
    LPDIRECT3DVERTEXDECLARATION9 m_VertexDeclaration; //!< 頂点宣言
    TbSint32                     m_FVF;
};

}

#endif