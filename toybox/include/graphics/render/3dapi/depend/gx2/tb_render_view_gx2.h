/*!
 * @file
 * @brief レンダービューのクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_GX2_H_
#define _INCLUDED_TB_RENDER_VIEW_GX2_H_

namespace toybox
{

class TbRenderDevice;
class TbRenderTarget;
class TbRenderView;

class TbRenderViewDepend
{
public:

    // コンストラクタ
    TbRenderViewDepend( const TbRenderViewParameter& param );

    //デストラクタ
    ~TbRenderViewDepend();

public: // 依存関数(アプリケーション層からは原則呼ばない)

    // カラーバッファ取得
    GX2ColorBuffer* GetColorBuffer() { 
        return &m_ColorBuffer; 
    }

    // カラーバッファ取得
    const GX2ColorBuffer* GetColorBuffer() const { 
        return &m_ColorBuffer; 
    }

private:

    GX2ColorBuffer  m_ColorBuffer;
    TbRenderTarget* m_RenderTarget;

private:

    friend class TbRenderView;
};

}

#endif