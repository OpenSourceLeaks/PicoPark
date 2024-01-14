/*!
 * TbFramewor用のViewController
 * @author teco
 */

#import <framework/depend/ios/tb_framework_view_controller.h>
#import <framework/tb_framework.h>
#import <framework/impl/tb_framework_impl.h>
#import <graphics/render/3dapi/depend/gl/ios/tb_render_view_gl_ios_impl.h>
#import <input/tb_touch.h>

///////////////////////////////////////
//  TbFrameworkViewController
///////////////////////////////////////

@interface TbFrameworkViewController() {
}
@end

@implementation TbFrameworkViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // ビューコントローラ設定
    toybox::TbRenderViewDependImpl::SetViewController(self);
    
    if( toybox::TbFramework::IsInitialized() ){
        // もし二回目以降は一度破棄
        toybox::TbFramework::OnExitPoint();
        toybox::TbFramework::Finalize();
    }
    
    // エントリー
    const TbChar8* argv = nullptr;
    toybox::TbFramework::OnEntryPoint( &argv , 0 );
    
    self.view.multipleTouchEnabled = YES;
}

- (void)dealloc
{
    // エントリー
    toybox::TbFramework::OnExitPoint();
    toybox::TbFramework::Finalize();
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

// 定期処理
- (void)update
{
    toybox::TbFrameworkImpl::GetInstance().Post();
    toybox::TbFrameworkImpl::GetInstance().Update();
}

// 描画処理
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    toybox::TbFrameworkImpl::GetInstance().Draw();
}

// タッチイベント
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    toybox::TbTouch* t = toybox::TbTouch::GetInstancePtr();
    if( t ) {
        TbFloat32 scale = [UIScreen mainScreen].scale;
        for (UITouch *touch in touches)
        {
            CGPoint location = [touch locationInView:self.view];
            toybox::TbVector2 pos( location.x*scale , location.y*scale );
            toybox::TbVector2 radius( touch.majorRadius*0.6f , touch.majorRadius*0.6f );
            t->GetDepend().Press(touch.hash);
            t->GetDepend().SetPos(touch.hash,pos,radius);
//            TbUint32 hash = touch.hash;
//            toybox::TbPrintf("Began(%u) %f,%f\n", touch.hash ,location.x , location.y );
//            toybox::TbPrintf("Radius(%u) %f,%f\n", touch.hash ,radius.GetX() , radius.GetY() );
        }
    }
}

// タッチイベント
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    toybox::TbTouch* t = toybox::TbTouch::GetInstancePtr();
    if( t ) {
        NSSet* touches = [event allTouches];
        TbFloat32 scale = [UIScreen mainScreen].scale;
        for (UITouch *touch in touches)
        {
            CGPoint location = [touch locationInView:self.view];
            toybox::TbVector2 pos( location.x*scale , location.y*scale );
            toybox::TbVector2 radius( touch.majorRadius*0.6f , touch.majorRadius*0.6f );
            t->GetDepend().SetPos(touch.hash,pos,radius);
//            TbUint32 hash = touch.hash;
            toybox::TbPrintf("Move(%u) %f,%f\n", touch.hash ,location.x , location.y );
            toybox::TbPrintf("Radius(%u) %f,%f\n", touch.hash ,radius.GetX() , radius.GetY() );
        }
    }
}

// タッチイベント
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    toybox::TbTouch* t = toybox::TbTouch::GetInstancePtr();
    if( t ) {
        for (UITouch* touch in touches)
        {
            t->GetDepend().Release(touch.hash);
            TbUint32 hash = touch.hash;
            toybox::TbPrintf("End(%u)",hash);
        }
    }
}

// タッチイベント
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    toybox::TbTouch* t = toybox::TbTouch::GetInstancePtr();
    if( t ) {
        for (UITouch *touch in touches)
        {
            t->GetDepend().Release(touch.hash);
            TbUint32 hash = touch.hash;
            toybox::TbPrintf("End(%u)",hash);
        }
    }

}

@end

///////////////////////////////////////
//  TbFrameworkViewControllerContainer
///////////////////////////////////////

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbFrameworkViewControllerContainer::TbFrameworkViewControllerContainer()
    : m_ViewController(nil)
{
}

/*!
 * デストラクタ
 * @author teco
 */
TbFrameworkViewControllerContainer::~TbFrameworkViewControllerContainer()
{
    m_ViewController = nil;
}
    
/*!
 * 初期化
 * @author teco
 */
void TbFrameworkViewControllerContainer::Initialize()
{
    m_ViewController = [[TbFrameworkViewController alloc] init];
}

}