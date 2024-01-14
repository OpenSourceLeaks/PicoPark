/*!
 * @file
 * @brief レンダービューのクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_GL_IOS_H_
#define _INCLUDED_TB_RENDER_VIEW_GL_IOS_H_

namespace toybox
{

class TbRenderViewDependImpl;
class TbRenderView;
class TbRenderTarget;

class TbRenderViewDepend
{
public:

    // コンストラクタ
    TbRenderViewDepend( const TbRenderViewParameter& param  );

    //デストラクタ
    ~TbRenderViewDepend();
    
public:
    
    // カレントビューに設定。設定をコピーするビューの設定可
    void SetCurrent( TbRenderView* copySrc = nullptr );

public: // Objective-Cと混在させるため、Implクラスを隠す
    
    // 実装部取得
    TbRenderViewDependImpl* GetImpl() {
        return m_Impl;
    }
    
private:

    TbRenderViewDependImpl* m_Impl;
    TbRenderTarget*         m_RenderTarget;

private:

    friend class TbRenderView;
};

}

#endif