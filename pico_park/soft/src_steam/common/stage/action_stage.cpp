/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "common/stage/action_stage.h"
#include "common/actor/action_actor.h"
#include "root.h"
#include <base/util/tb_type_id.h>

static TbUint32 ACTOR_MAX_DEFAULT = 100;

TbBool ActionStage::s_IsEnableDebugDrawCollision = TB_FALSE;
TbBool ActionStage::s_IsEnableDebugDrawPlayerCount = TB_FALSE;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
ActionStage::ActionStage( TbScriptLuaNode* node , TbUint32 actorLayerMax , ActionCollisionScene* collisionScene )
    : Super(actorLayerMax)
    , m_Map()
    , m_CollisionScene(NULL)
{
    m_Map.Initialize(node,Root::GetInstance().GetPlayerCount());
    m_Map.EnterSpriteScene(&GetSpriteScene());

    // コリジョン
    if( collisionScene ) {
        m_CollisionScene = collisionScene;
    }else{
        m_CollisionScene = TB_NEW ActionCollisionScene();
    }
    m_CollisionScene->SetMap(&GetMap());
    m_ActorSearchMap.SetEnableSetCapacityAuto(TB_TRUE);

    // 物理
    m_PhysicsScene.SetGravity(TbVector2(0.0f,980.0f));
    
    // アクター生成
    GetActorFactory().Initialize(node);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
ActionStage::~ActionStage()
{
    GetActorScene().Clear();
    TB_SAFE_DELETE(m_CollisionScene);

    // 所有権解放
    TbForIterator(it,m_ActorSearchMap) {
        it->value->Release();
    }
}

/*!
 * アクター検索
 * @author teco
 * @since 2015.01.15
 */
ActionActor* ActionStage::FindActor( const char* name )
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
 * @since 2015.01.15
 */
const ActionActor* ActionStage::FindActor( const char* name ) const
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
void ActionStage::NotifyEvent( const char* name , TbSint32 eve , void* param )
{
    if( name ){
        // ユニキャスト
        ActionActor* actor = FindActor( name );
        if( actor ) {
            actor->NotifyEvent( eve , param );
        }
    }else{
        // マルチキャスト
        TbForIterator( it, m_ActorSearchMap ){
            it->value->NotifyEvent( eve , param );
        }
    }
}

/*!
 * デバッグメニュー生成
 * @author teco
 * @since 2014.05.10
 */
void ActionStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"ActionStage");
    if( !menu ) {
        return;
    }
    // ランダム入力テスト
    TbDebugMenuItemBool* randomInputMenu = menu->CreateItemBool("RandomInput");
    randomInputMenu->Bind( TbCreateMemFunc(this,&Self::setEnableRandomInput) ,
                           TbCreateMemFunc(this,&Self::IsEnableRandomInput) );

    TbDebugMenuItemBool* drawCollisionMenu = menu->CreateItemBool("DrawDebugCollision");
    drawCollisionMenu->Bind( &s_IsEnableDebugDrawCollision );

    TbDebugMenuItemBool* drawPlayerCountMenu = menu->CreateItemBool("DrawPlayerCount");
    drawPlayerCountMenu->Bind( &s_IsEnableDebugDrawPlayerCount );
}

/*!
 * アクターを追加
 * @author teco
 * @since 2014.11.30
 */
void ActionStage::AddActor( ActionActor* actor , const TbVector2& initPos ) {
    actor->SetStage(this);
    actor->SetInitPos( initPos );
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

// 描画
void ActionStage::OnDrawFront( TbFloat32 deltatime ) 
{
    if( s_IsEnableDebugDrawCollision ){
        TbMatrix mtx;
        mtx.Translate(-GetScroll(),0.0f,0.0f);
        mtx = GetSpriteScene().GetPoseMatrix() * mtx;
        m_CollisionScene->DrawDebug(mtx);
    }
    if( s_IsEnableDebugDrawPlayerCount ){
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        font->PushSetting();
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        font->Printf(CMN_WINDOW_WIDTH/2.0f,CMN_WINDOW_HEIGHT/5.0f,"%d PLAYERS GAME",Root::GetInstance().GetPlayerCount());
        font->PopSetting();
    }
}
