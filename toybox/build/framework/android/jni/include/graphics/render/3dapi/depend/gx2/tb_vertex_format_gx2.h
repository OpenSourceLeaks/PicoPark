/*!
 * @file
 * @brief OpenGLの頂点フォーマット依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_VERTEX_FORMAT_GL_H_
#define _INCLUDED_TB_VERTEX_FORMAT_GL_H_

namespace toybox
{

// テクスチャ座標原点位置
#define TB_TEXCOORD_BASE_TYPE TB_TEXCOORD_BASE_TYPE_LEFTDOWN

// 頂点要素
struct TbVertexElementGL
{
    TbUint16 stream;    // ストリーム番号
    TbUint16 offset;    // データの開始オフセット値
    GLint    size;      // 1,2,3,4のどれか
    GLenum   type;      // GL用型
    TbUint8  method;     // 方法
    TbUint8  usage;      // 利用用途
    TbUint8  usageIndex; // 利用用途内番号
    TbBool   isNormalized; // 正規化が必要か
};

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
    TbVertexFormatDepend( TbSint32 type );
public:
    //! エレメント数取得
    TB_INLINE const TbSint32 GetElementCount() const { return m_ElementCount; }
    //! エレメントリスト取得
    TB_INLINE const TbVertexElementGL* GetElements() const { return m_Elements; }
private:
    enum {
        FLAG_ELEMENT_STATIC,
    };
private:
    TbBitArray16            m_BitArray;         //!< ビット配列
    TbSint32                m_ElementCount;       //!< エレメント数
    TbVertexElementGL*      m_Elements;         //!< エレメントリスト
};

}

#endif