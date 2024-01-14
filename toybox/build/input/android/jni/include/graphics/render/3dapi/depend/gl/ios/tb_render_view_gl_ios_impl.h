/*!
 * @file
 * @brief レンダービューのクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_GL_IOS_IMPL_H_
#define _INCLUDED_TB_RENDER_VIEW_GL_IOS_IMPL_H_

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

namespace toybox
{

class TbRenderViewDependImpl
{
public:

    // コンストラクタ
    TbRenderViewDependImpl();

    //デストラクタ
    ~TbRenderViewDependImpl();

public: //
    
    // カレント設定
    void SetCurrent();
    
public:
    
    // ビューコントローラ設定
    static void SetViewController( GLKViewController* viewController ){
        TB_ASSERT(!s_ViewController || s_ViewController == viewController );
        s_ViewController =viewController;
    }
    
private:
    
    static GLKViewController* s_ViewController;

private:
    
    EAGLContext* m_Context;
    
private:

    friend class TbRenderView;
};

}

#endif