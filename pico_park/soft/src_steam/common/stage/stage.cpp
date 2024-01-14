/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "common/stage/stage.h"
#include "root.h"
#include <framework/sub/tb_framework_simple.h>
#include <base/io/resource/tb_resource_manager.h>
#include <base/time/tb_tick.h>

static TbUint32 SPRITE_MAX = 100;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
Stage::Stage(TbUint32 actorLayerMax)
    : m_Id(0)
    , m_ActorScene(actorLayerMax)
    , m_SpriteScene(SPRITE_MAX)
    , m_SpriteMaterial(TbFrameworkSimple::RENDER_PASS_2D)
    , m_Scroll(0.0f)
    , m_Scale(1.0f)
    , m_PlaySec(0.0f)
    , m_DebugMenu(NULL)
{
    m_SpriteScene.SetMaterial(&m_SpriteMaterial);
    m_SpriteMaterial.SetTexture(Root::GetInstance().GetCommonTexture());
    m_ActorFactory.SetCreateFunc(TbCreateMemFunc(this,&Self::OnCreatedActor));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
Stage::~Stage()
{
    if( m_DebugMenu ){
        m_DebugMenu->Suicide();
    }
}

/*! 
 * 有効化
 * @author teco
 * @since 2013.09.08
 */
void Stage::SetActive( TbBool isActive ) 
{
    if( IsReady() ){
        if( TB_BOOL_NOT_EQUAL(IsActive(),isActive) ){
            m_BitArray.SetBit(FLAG_ACTIVE,isActive);
            OnChangedActive(isActive);
        }
    }
}
    
/*!
 * 準備完了
 * @author teco
 */
TbBool Stage::IsReady()
{
    return TB_TRUE;
}

/*!
 * 更新
 * @author teco
 */
void Stage::Update() 
{
    float deltaSec = 1/60.0f;
    if( m_BitArray.Test(FLAG_ACTIVE) ){
        m_ActorFactory.Update(m_Scroll);
        m_ActorScene.Update(deltaSec);
    }
    m_PlayerInput.Update(deltaSec);
    OnUpdate(deltaSec);
    if( m_BitArray.Test(FLAG_ACTIVE) ){
        static const TbFloat32 PLAY_SEC_MAX = 60.0f * 60.0f;
        m_ActorScene.Post(deltaSec);
        m_SpriteScene.Step(1.0f);
        m_SpriteScene.Calculate();
        m_PlaySec += TbTick::GetFrameSpeedSeconds();
        if( PLAY_SEC_MAX < m_PlaySec ) {
            m_PlaySec = PLAY_SEC_MAX;
        }
    }
}

/*!
 * 描画
 * @author teco
 */
void Stage::Draw() 
{
    float deltaSec = 1/60.0f;
    if( m_BitArray.Test(FLAG_ACTIVE) ){
        OnDraw(deltaSec);
        m_ActorScene.Draw(deltaSec); // 30フレームでまわしてみる
        m_SpriteScene.Draw();
        OnDrawFront(deltaSec);
    }
}
