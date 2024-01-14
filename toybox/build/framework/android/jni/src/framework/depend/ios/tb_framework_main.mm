/*!
 * TbFramewor用のメイン
 * @author teco
 */

#import <UIKit/UIKit.h>
#import "framework/tb_framework.h"
#import "framework/depend/ios/tb_framework_view_controller.h"

using  namespace toybox;

static TbFrameworkViewControllerContainer s_ViewControllerContainer;

//////////////////////////////////////
// TbFrameworkAppDelegate
//////////////////////////////////////

@interface TbFrameworkAppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

@interface TbFrameworkAppDelegate ()

@end

@implementation TbFrameworkAppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    // UIWindowの生成
    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    self.window.backgroundColor = [UIColor whiteColor];
    
    // 最初に表示されるViewControllerを生成
    s_ViewControllerContainer.Initialize();
    self.window.rootViewController = s_ViewControllerContainer.GetViewController();
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    toybox::TbFramework::OnExitPoint();
    toybox::TbFramework::Finalize();
}

@end

//////////////////////////////////////
// メイン処理
//////////////////////////////////////

int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([TbFrameworkAppDelegate class]));
    }
}
