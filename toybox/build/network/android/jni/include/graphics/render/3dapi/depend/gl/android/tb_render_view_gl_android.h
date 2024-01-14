/*!
 * @file
 * @brief レンダービューのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_GL_ANDROID_H_
#define _INCLUDED_TB_RENDER_VIEW_GL_ANDROID_H_

namespace toybox
{

class TbRenderDevice;
class TbRenderTarget;
class TbRenderView;

class TbRenderViewDepend
{
public:

    // コンストラクタ
    TbRenderViewDepend();

    //デストラクタ
    ~TbRenderViewDepend();

public: // プラットフォーム間で共通のインタフェース

    // カレントビューに設定。設定をコピーするビューの設定可
    void SetCurrent( TbRenderView* copySrc = nullptr );
    
private:

    TbRenderTarget*         m_RenderTarget;

private:

    friend class TbRenderView;
};

}

#endif