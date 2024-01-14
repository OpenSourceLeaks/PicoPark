/*!
 * UI管理クラス
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"

#define TB_UI_IS_ENABLE_SE (0 && TB_PLATFORM_IS_WIN)
#if TB_UI_IS_ENABLE_SE
#include <sound/tb_sound.h>
#endif

static TbUint32 SPRITE_MAX = 256;

namespace toybox
{

static const char* SE_NAME_TABLE[]
{
    "ui_open" ,
    "ui_select" ,
    "ui_cancel" ,
};
TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SE_NAME_TABLE)==TB_UI_SE_MAX);

/*!
 * コンストラクタ
 * @author teco
 */
TbUiManager::TbUiManager( TbUint32 passIndex )
    : m_SpriteScene(SPRITE_MAX)
    , m_SpriteMaterial(passIndex)
    , m_RenderPassIndex(passIndex)
    , m_FontBuffer(nullptr)
    , m_Input(nullptr)
    , m_FullscreenSize(100,100)
{
    m_SpriteScene.SetMaterial(&m_SpriteMaterial);

    m_Windows.SetCapacity(10);
    m_Windows.SetEnableSetCapacityAuto(TB_TRUE);
    m_RemoveWindows.SetCapacity(10);
    m_RemoveWindows.SetEnableSetCapacityAuto(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiManager::~TbUiManager()
{
}

/*!
 * 定期処理
 * @author teco
 */
void TbUiManager::Update()
{
    collectGabage();

    // ウィンドウの定期処理
    TbForIterator( it , m_Windows ){
        (*it)->Update(1/30.0f);
    }

    collectGabage();

    m_SpriteScene.Step(1.0f);
    m_SpriteScene.Calculate();
}

/*!
 * 描画処理
 * @author teco
 */
void TbUiManager::Draw()
{
    m_SpriteScene.Draw();
    // ウィンドウの描画処理
    TbForIterator( it , m_Windows ){
        (*it)->Draw();
    }
}

/*!
 * テクスチャ設定
 * @author teco
 */
void TbUiManager::SetTexture( toybox::TbTexture* texture )
{
    m_SpriteMaterial.SetTexture(texture);
}

/*!
 * ウィンドウ追加
 * @author teco
 */
void TbUiManager::AddWindow( TbUiWindow* window )
{
    TbArray<TbUiWindow*>::Iterator it = TbFind(m_Windows,window);
    if( it == m_Windows.End() ) {
        m_Windows.PushBack(window);
    }
}

/*!
 * ウィンドウ除去
 * @author teco
 */
void TbUiManager::RemoveWindow( TbUiWindow* window )
{
    m_RemoveWindows.PushBack(window);
}

/*!
 * SE読み込み
 * @author teco
 */
void TbUiManager::LoadSE( TbUiSeType type , const TbChar8* fpath )
{
#if TB_UI_IS_ENABLE_SE
    TbSoundManager& sound = TbSoundManager::GetInstance();
    sound.LoadSound(fpath,SE_NAME_TABLE[type],TB_SOUND_TYPE_STATIC);
#endif
}

/*!
 * SE利用は全て可能か
 * @author teco
 */
TbBool TbUiManager::IsReadySE() const
{
#if TB_UI_IS_ENABLE_SE
    TbSoundManager& sound = TbSoundManager::GetInstance();
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SE_NAME_TABLE); ++i ) 
    {
        if( !sound.IsReadySound(SE_NAME_TABLE[i]) ) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
#else
    return TB_TRUE;
#endif
}

/*!
 * SE再生
 * @author teco
 */
void TbUiManager::PlaySE( TbUiSeType type )
{
#if TB_UI_IS_ENABLE_SE
    if( IsReadySE() )
    {
        if( TB_VERIFY(0<=type&&type<TB_ARRAY_COUNT_OF(SE_NAME_TABLE)) ){ 
            TbPlaySound(SE_NAME_TABLE[type]);
        }
    }
#endif
}

//
void TbUiManager::collectGabage()
{
    TbForIterator( it , m_RemoveWindows ){
        TbArray<TbUiWindow*>::Iterator findIt = TbFind(m_Windows,*it);
        if( findIt != m_Windows.End() ) {
            m_Windows.Erase(findIt);
        }
    }
    m_RemoveWindows.Clear();
}


}
