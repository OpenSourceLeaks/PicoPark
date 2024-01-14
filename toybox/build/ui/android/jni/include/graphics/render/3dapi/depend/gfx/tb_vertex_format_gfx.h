/*!
 * @file
 * @brief OpenGLの頂点フォーマット依存コード
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_VERTEX_FORMAT_GFX_H_
#define _INCLUDED_TB_VERTEX_FORMAT_GFX_H_

namespace toybox
{

// テクスチャ座標原点位置
#define TB_TEXCOORD_BASE_TYPE TB_TEXCOORD_BASE_TYPE_LEFTDOWN

/*!
 * 頂点フォーマット依存コード
 * @author teco
 */
class TbVertexFormatDepend
{
public:
    // コンストラクタ
    TbVertexFormatDepend();
    // デストラクタ
    ~TbVertexFormatDepend();
public:
    // 初期化
    void Initialize(const TbVertexElement* elements, TbSint32 elementCount , TbBool isStatic);
public:
    // 要素数取得
    TbSint32 GetElementCount() const {
        return m_ElementCount;
    }
    // 要素リスト取得
    const TbVertexElement* GetElement(TbSint32 index) const {
        if (0 <= index && index < m_ElementCount) {
            return &m_Elements[index];
        }
        return nullptr;
    }
private:
    TbBool                  m_IsStatic;
    const TbVertexElement*  m_Elements;
    TbSint32                m_ElementCount;
};

}

#endif