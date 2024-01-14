/*!
 * レンダービュー(Stage3D)
 * @author Miyake Shunsuke
 * @since 2013.04.28
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_S3D_H_
#define _INCLUDED_TB_RENDER_VIEW_S3D_H_

namespace toybox
{

class TbRenderDevice;
class TbVertexFormat;
class TbRenderView;
class TbRenderTarget;

class TbRenderViewDepend
{
public:

    // コンストラクタ
    TbRenderViewDepend();

    // デストラクタ
    ~TbRenderViewDepend();

public: // 依存関数(アプリケーション層からは原則呼ばない)

    //! レンダーターゲット取得
    TbRenderTarget* GetRenderTarget() {
        return m_RenderTarget;
    }

private:

    enum {
        FLAG_CREATE_DEPTH_STENCIL , // 深度ステンシルを生成した
    };

private:

    // セットアップ
    void setup( TbRenderView* view );

private:
    
    TbBitArray32            m_BitArray;         //!< ビット配列
    TbRenderTarget*         m_RenderTarget; //!< レンダーターゲットリスト

private:

    friend class TbRenderView;

};

}

#endif