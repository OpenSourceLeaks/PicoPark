/*!
 * framework用ViewController
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAMEWORK_VIEW_CONTROLLER_H_
#define _INCLUDED_TB_FRAMEWORK_VIEW_CONTROLLER_H_

#import <base/util/tb_singleton.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface TbFrameworkViewController : GLKViewController
@end

namespace toybox
{

class TbFrameworkViewControllerContainer : public TbSingletonRef<TbFrameworkViewControllerContainer>
{
public:
    
    // コンストラクタ
    TbFrameworkViewControllerContainer();
    
    // デストラクタ
    ~TbFrameworkViewControllerContainer();
    
public:
    
    // 初期化
    void Initialize();
    
    // ViewController取得
    TbFrameworkViewController* GetViewController() {
        return m_ViewController;
    }
    
private:
    TbFrameworkViewController*   m_ViewController;
};
    
}

#endif
