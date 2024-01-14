/*!
 * Stage3dの頂点フォーマット依存コード
 * @author Miyake Shunsuke
 * @since 2013.04.07
 */

#ifndef _INCLUDED_TB_VERTEX_FORMAT_S3D_H_
#define _INCLUDED_TB_VERTEX_FORMAT_S3D_H_

namespace toybox
{

// 頂点要素
struct TbVertexElementS3d
{
    typedef AS3::ui::flash::display3D::Context3DVertexBufferFormat Format;
    TbUint16 stream;     // ストリーム番号
    TbUint16 offset;     // データの開始オフセット値
    TbUint16 data32Count; // 32bit単位数
    AS3::ui::String   type; // Stage3D用タイプ
    TbUint8  method;     // 方法
    TbUint8  usage;      // 利用用途
    TbUint8  usageIndex; // 利用用途内番号
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
    TB_INLINE const TbSint32 GetElementCount() const { 
        return m_ElementCount; 
    }

    //! エレメントリスト取得
    TB_INLINE const TbVertexElementS3d* GetElements() const { 
        return m_Elements; 
    }
    
    //! エレメント取得
    const TbVertexElementS3d* GetElement( TbSizeT index ) const {
        if( index < m_ElementCount ) {
            return &m_Elements[index];
        }
        return NULL;
    }

private:
    enum {
        FLAG_ELEMENT_STATIC,
    };
private:
    TbBitArray16            m_BitArray;             //!< ビット配列
    TbSizeT                 m_ElementCount;         //!< エレメント数
    TbVertexElementS3d*     m_Elements;             //!< エレメントリスト
};

}

#endif