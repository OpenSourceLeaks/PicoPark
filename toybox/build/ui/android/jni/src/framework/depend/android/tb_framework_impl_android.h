/*!
 * フレームワークのAndroid用コード
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAMEWORK_IMPL_ANDROID_H_
#define _INCLUDED_TB_FRAMEWORK_IMPL_ANDROID_H_

#include <graphics/window/tb_window.h>
#include <framework/depend/android/tb_framework_util_android.h>
#include <base/io/depend/android/tb_file_android.h>
#include <input/tb_touch.h>

namespace
{
    using namespace toybox;
    // AndroidのPixelFormat→TbColorFormatへの変換テーブル
    TbColorFormat COLOR_FORMAT_CONVERT_TABLE[] = {
        TB_COLOR_FORMAT_UNKNOWN , 
        TB_COLOR_FORMAT_A8R8G8B8 , // RGBA_8888
        TB_COLOR_FORMAT_X8R8G8B8 , // RGBX_8888
        TB_COLOR_FORMAT_R8G8B8   , // RGB_888
        TB_COLOR_FORMAT_R5G6B5   , // RGB_565
    };
    TbBool s_IsEntried = TB_FALSE;
}

TB_BEGIN_JNI_FUNC

/*!
 * ディスプレイ情報設定
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkActivity,setAssetNdk)(JNIEnv * env , 
                                                                            jobject obj,
                                                                            jobject assetManager )
{
    using namespace toybox;
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    TbFileAndroid::SetAssetManager(mgr);
}

/*!
 * ディスプレイ情報設定
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkActivity,setDisplayInfoNdk)(JNIEnv * env , 
                                                                            jobject obj , 
                                                                            jint w , 
                                                                            jint h , 
                                                                            jint pixelFormat , 
                                                                            jint refreshRate )
{
    using namespace toybox;
    TB_ASSERT(pixelFormat < TB_ARRAY_COUNT_OF(COLOR_FORMAT_CONVERT_TABLE));
    TbDisplayManagerDepend::Initialize(w,h,COLOR_FORMAT_CONVERT_TABLE[pixelFormat],refreshRate);
}

/*!
 * サーフェイスが作成された
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkView,onSurfaceCreatedNdk)(JNIEnv * env, jobject obj)
{
    s_IsEntried = TB_FALSE;
}

/*!
 * サーフェイス変更
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkView,onSurfaceChangedNdk)(JNIEnv * env, jobject obj,jint w, jint h)
{
    using namespace toybox;
    TbDisplayManagerDepend::SetSize(w,h);
    
    if( !s_IsEntried ){
        // エントリー
        const TbChar8* arg[] = {""};
        TbFramework::OnEntryPoint( arg , 1 );
        s_IsEntried = TB_TRUE;
    }
    
    if( TbFramework::IsInitialized() ){
        TbWindowManager& windowManager = TbWindowManager::GetInstance();
        TbWindowManager::WindowList::Iterator ite = windowManager.GetWindowBegin();
        TbWindowManager::WindowList::Iterator endIte = windowManager.GetWindowEnd();
        for( ; ite != endIte; ++ite ){
            (*ite).value->SetSize(w,h);
        }
    }
    
    TbFramework::OnResizeDisplay(w,h);
}

/*!
 * 毎フレーム更新
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkView,onDrawFrameNdk)(JNIEnv * env, jobject obj)
{
    using namespace toybox;
    // メインループ
    if( s_Impl && s_Impl->IsEnableMainLoop() ) {
        s_Impl->Update();
        s_Impl->Draw();
        s_Impl->Post();
    }
}

/*!
 * 毎フレーム更新
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkActivity,onPauseNdk)(JNIEnv * env, jobject obj)
{
    using namespace toybox;
    TbPrintf("onPauseNdk\n");
    TbFramework::OnExitPoint();
    TbFramework::Finalize();
}

/*!
 * タッチ押した
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkActivity,touchPressNdk)(JNIEnv * env , 
                                                                            jobject obj , 
                                                                            jint id )
{
    using namespace toybox;
    TbTouch* t = TbTouch::GetInstancePtr();
    if( t ) {
        t->GetDepend().Press(id);
    }
}

/*!
 * タッチ離した
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkActivity,touchReleaseNdk)(JNIEnv * env , 
                                                                            jobject obj , 
                                                                            jint id )
{
    using namespace toybox;
    TbTouch* t = TbTouch::GetInstancePtr();
    if( t ) {
        t->GetDepend().Release(id);
    }
}

/*!
 * タッチ位置情報
 * @author teco
 */
JNIEXPORT void JNICALL TB_FRAMEWORK_JNI_FUNC(TbFrameworkActivity,touchOnNdk)(JNIEnv * env , 
                                                                            jobject obj , 
                                                                            jint id ,
                                                                            jfloat x ,
                                                                            jfloat y ,
                                                                            jfloat radiusX ,
                                                                            jfloat radiusY )
{
    using namespace toybox;
    TbTouch* t = TbTouch::GetInstancePtr();
    if( t ) {
        toybox::TbVector2 pos( x , y );
        toybox::TbVector2 radius( radiusX,radiusY );
        t->GetDepend().SetPos(id,pos,radius);
    }
}


TB_END_JNI_FUNC

#endif
