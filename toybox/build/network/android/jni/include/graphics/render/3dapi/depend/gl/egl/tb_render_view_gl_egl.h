/*!
 * @file
 * @brief レンダービューのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_GL_EGL_H_
#define _INCLUDED_TB_RENDER_VIEW_GL_EGL_H_

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

public: // プラットフォーム間で共通のインタフェース

    // カレントビューに設定。設定をコピーするビューの設定可
    void SetCurrent( TbRenderView* copySrc = nullptr );

public:

    // レンダリングコンテキスト取得
    EGLContext GetRenderingContext() {
        return m_Context;
    }

private:

    EGLDisplay      m_Display;
    EGLConfig       m_Config;
    EGLSurface      m_Surface;
    EGLContext      m_Context;
    TbRenderTarget* m_RenderTarget;
    
private:

    friend class TbRenderView;
};

}

#endif