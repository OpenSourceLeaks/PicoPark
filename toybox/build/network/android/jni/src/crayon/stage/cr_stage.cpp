/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "tb_fwd.h"
#include <crayon/stage/cr_stage.h>
#include <crayon/actor/cr_actor.h>
#include <framework/sub/tb_framework_simple.h>
#include <base/io/resource/tb_resource_manager.h>
#include <base/time/tb_tick.h>

using namespace toybox;

static TbUint32 SPRITE_MAX = 256;

namespace crayon
{

namespace
{
    TbFloat32 quake( TbFloat32 t )
    {
        // 0で
        static TbFloat32 MAX = 12.0f;
        static TbFloat32 B = 4.0f; // 振動数
        static TbFloat32 C = 0.1f; // 値が0になるまでの時間(秒)
        static TbFloat32 D = 0.2f; // 減衰。大きいと最初はなだらかに減少。
        TbFloat32 result = TbPow(2 , -(t/D) ) * // t/Dが1で0.5
                           TbCos(t*B*(2.0f*TB_PI))*   // 1秒で一週
                           TbCos((t/(C*2.0f))*TB_PI)* // 
                           MAX; // 最大値
        return result;
    }
}

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
CrStage::CrStage(TbUint32 actorLayerMax , TbUint32 sprieLayerMax , TbUint32 passIndex , toybox::TbTexture* commonTexture)
    : m_Id(0)
    , m_ActorScene(actorLayerMax)
    , m_SpriteScene()
    , m_SpriteMaterial(passIndex)
    , m_Scale(1.0f)
    , m_PlaySec(0.0f)
    , m_ScreenQuakeSec(0.0f)
    , m_DebugMenu(NULL)
{
    m_SpriteScene.SetCapacity(sprieLayerMax);
    m_SpriteScene.PushBackDefault(sprieLayerMax);
    TbForIterator( it , m_SpriteScene ){
        it->SetCapacity(SPRITE_MAX);
        it->SetMaterial(&m_SpriteMaterial);
    }

    m_ActorSearchMap.SetEnableSetCapacityAuto(TB_TRUE);
    m_SpriteMaterial.SetTexture(commonTexture);
    m_ActorFactory.SetCreateFunc(toybox::TbCreateMemFunc(this,&Self::OnCreatedActor));
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
CrStage::~CrStage()
{
    if( m_DebugMenu ){
        m_DebugMenu->Suicide();
    }
    ResetActor();
}

/*! 
 * 有効化
 * @author teco
 * @since 2013.09.08
 */
void CrStage::SetActive( TbBool isActive ) 
{
    if( IsReady() ){
        if( TB_BOOL_NOT_EQUAL(IsActive(),isActive) ){
            m_BitArray.SetBit(FLAG_ACTIVE,isActive);
            if (m_BitArray.Test(FLAG_ACTIVE)) {
                m_ActorFactory.Update(GetActorCreateThreshold());
                m_ActorScene.Update(0.0f);
            }
            OnChangedActive(isActive);
        }
    }
}
    
/*!
 * 準備完了
 * @author teco
 */
TbBool CrStage::IsReady()
{
    return TB_TRUE;
}

/*!
 * 更新
 * @author teco
 */
void CrStage::Update() 
{
    m_OldScroll = m_Scroll;
    float deltaSec = 1/60.0f;
    OnPre(deltaSec);
    m_PlayerInput.Update(deltaSec);
    if( m_BitArray.Test(FLAG_ACTIVE) ){
        m_ActorFactory.Update(GetActorCreateThreshold());
        m_ActorScene.Update(deltaSec);
    }
    OnUpdate(deltaSec);
    if( m_BitArray.Test(FLAG_ACTIVE) ){
        static const TbFloat32 PLAY_SEC_MAX = 60.0f * 60.0f;
        m_ActorScene.Post(deltaSec);
        TbForIterator( it , m_SpriteScene ){
            it->Step(1.0f);
            it->Calculate();
        }

        m_PlaySec += static_cast<TbFloat32>(toybox::TbTick::GetFrameSpeedSeconds());
        if( PLAY_SEC_MAX < m_PlaySec ) {
            m_PlaySec = PLAY_SEC_MAX;
        }
        if (m_BitArray.Test(FLAG_SCREEN_QUAKE))
        {
            static TbFloat32 SEC = 0.3;
            m_ScreenQuakeSec += deltaSec;
            TbFloat32 offset = quake(m_ScreenQuakeSec);
            if (SEC < m_ScreenQuakeSec)
            {
                offset = 0.0f;
                m_ScreenQuakeSec = 0.0f;
                m_BitArray.ResetBit(FLAG_SCREEN_QUAKE);
            }
            TbForIterator(it, m_SpriteScene) {
                it->SetPoseTrasnlate(TbVector2(offset, offset));
            }
        }
    }
}

/*!
 * 描画
 * @author teco
 */
void CrStage::Draw() 
{
    float deltaSec = 1/60.0f;
    if( m_BitArray.Test(FLAG_ACTIVE) ){
        OnDraw(deltaSec);
        m_ActorScene.Draw(deltaSec); // 30フレームでまわしてみる
        TbForIterator( it , m_SpriteScene ){
            it->Draw();
        }
        OnDrawFront(deltaSec);
    }
}

/*!
 * アクターリセット
 * @author teco
 */
void CrStage::ResetActor()
{
    // 所有権解放
    TbForIterator(it,m_ActorSearchMap) {
        it->value->Release();
    }
    m_ActorSearchMap.Clear();
    m_ActorScene.Clear();
}

/*!
 * アクターリセット
 * @author teco
 */
void CrStage::ResetActor( const char* target ) 
{
    // 所有権解放
    for( auto it = m_ActorSearchMap.Begin(); it != m_ActorSearchMap.End(); ) {
        if( 0 <= TbStrFind( it->value->GetName() , target ) ) {
            it->value->Exit();
            it->value->Release();
            it = m_ActorSearchMap.Erase(it);
        }else{
            ++it;
        }
    }
}

/*!
 * 画面揺らし
 * @author teco
 */
void CrStage::RequestScreenQuake()
{
    m_BitArray.SetBit(FLAG_SCREEN_QUAKE);
    m_ScreenQuakeSec = 0.0f;
}

/*!
 * アクター検索
 * @author teco
 */
CrActor* CrStage::FindActor( const char* name )
{
    TB_RESQUE_RET(name,nullptr);

    TbUint32 key = TbCrc32::GetHash(name);
    auto it = m_ActorSearchMap.Find(key);
    if( it != m_ActorSearchMap.End() ){
        return it->value;
    }
    return nullptr;
}

/*!
 * アクター検索
 * @author teco
 */
const CrActor* CrStage::FindActor( const char* name ) const
{
    TB_RESQUE_RET(name,nullptr);

    TbUint32 key = TbCrc32::GetHash(name);
    auto it = m_ActorSearchMap.Find(key);
    if( it != m_ActorSearchMap.End() ){
        return it->value;
    }
    return nullptr;
}

/*! 
 * イベント通知
 * @author teco
 */
TbSint32 CrStage::NotifyEvent( const char* name , TbSint32 eve , void* param )
{
    if( name ){
        // ユニキャスト
        CrActor* actor = FindActor( name );
        if( actor ) {
            return actor->NotifyEvent( eve , param );
        }
    }else{
        // マルチキャスト
        TbForIterator( it, m_ActorSearchMap ){
            it->value->NotifyEvent( eve , param );
        }
    }
    return 0;
}


/*!
 * アクターを追加
 * @author teco
 */
void CrStage::AddActor( CrActor* actor ) 
{
    actor->SetStage(this);
    actor->Enter(&GetActorScene(),this);

    // 名前で検索できるように登録
    if( 0 != TbStrCmp(actor->GetName(),"") ) // 名前が無い場合は登録しない
    {
        TbUint32 key = TbCrc32::GetHash(actor->GetName());
        TbBool ret = m_ActorSearchMap.Insert(key,actor);
        if( ret ){
            actor->Retain();
        }
    }
}



}