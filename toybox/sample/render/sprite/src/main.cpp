#include "fwd.h"
#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/sprite/tb_sprite_all.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>

namespace 
{

using namespace toybox;

// リソース
TbResourceHandleTexture s_ResourceTexture;
TbSprite                s_Sprite;
TbSpriteFixedMaterial   s_SpriteMaterial(TbFrameworkSimple::RENDER_PASS_2D);
TbSpriteNodeTree        s_SpriteNodeTree;

}

// 更新
void update()
{
    s_Sprite.AddTranslate(0.5f,0.5f);
    s_Sprite.AddRotation(TbDegToAng32(1.0f));
    s_Sprite.Calculate();
}

// 描画
void draw()
{
    s_Sprite.Draw(TbMatrix::IDENTITY);
}

namespace toybox
{

/*!
 * エントリーポイント
 * @author Miyake Shunsuke
 * @since 2011.05.13
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{
    // 初期化
    TbFrameworkSimple::Initialize();

    // テクスチャ生成
    s_ResourceTexture.Create( "tex/sample.bmp" , 0 , TB_FILE_SYNC );
    s_ResourceTexture.Activate();

    // スプライト設定
    TbSpriteNodeTree::InitNodeParam initParam;
    {
        initParam.name = "root";
        initParam.param.color.SetColorU8(0xFF,0xFF,0xFF,0xFF);
        initParam.param.rect = TbRectF32(-100.0f,-100.0f,200.0f,200.0f); //
        initParam.param.uvRect = TbRectF32(0.0f,0.0f,1.0f,1.0f);
        s_SpriteNodeTree.Initialize(initParam);
        s_SpriteMaterial.SetTexture(s_ResourceTexture.GetTexture());
        s_Sprite.Initialize(&s_SpriteNodeTree);
        s_Sprite.SetMaterial(&s_SpriteMaterial);
    }

    TbRenderDevice::GetInstance().SetCullMode(TB_CULL_MODE_NONE);

    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL , update , draw , NULL );
}

/*!
 * 終了
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnExitPoint()
{
    s_ResourceTexture.Destroy();
    TbFrameworkSimple::Finalize();
}

/*!
 * 画面の縦、横サイズが変わった
 * @author Miyake Shunsuke
 * @since 2012.07.14
 */
void TbFramework::OnResizeDisplay( TbSint32 w , TbSint32 h )
{
    // 特に何もしない
}

}
