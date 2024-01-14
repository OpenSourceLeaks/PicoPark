/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "tb_fwd.h"
#include "crayon/stage/cr_action_stage.h"
#include "crayon/actor/cr_action_actor.h"
#include <base/util/tb_type_id.h>

using namespace toybox;

static TbUint32 ACTOR_MAX_DEFAULT = 100;

namespace crayon 
{
    
TbBool s_IsEnableDebugDrawCollision = TB_FALSE;
TbBool s_IsEnableUndeadPlayer = TB_FALSE; // プレイヤー不死身か
TbBool s_DebugFlag = TB_FALSE;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
CrActionStage::CrActionStage( TbScriptLuaNode* node , 
                              TbUint32 actorLayerMax , 
                              TbUint32 passIndex , 
                              toybox::TbTexture* commonTexture ,
                              const toybox::TbRectF32* mapUvRecSTable ,
                              CrActionCollisionScene* collisionScene ,
                              TbUint32 playerCount )
    : Super(actorLayerMax,1,passIndex,commonTexture)
    , m_Map(mapUvRecSTable)
    , m_CollisionScene(NULL)
{
    m_Map.Initialize(node, playerCount);
    m_Map.EnterSpriteScene(&GetSpriteScene(0));

    // コリジョン
    if( collisionScene ) {
        m_CollisionScene = collisionScene;
    }else{
        m_CollisionScene = TB_NEW CrActionCollisionScene();
    }
    m_CollisionScene->SetMap(&GetMap());

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
CrActionStage::~CrActionStage()
{
    GetActorScene().Clear();
    TB_SAFE_DELETE(m_CollisionScene);
}

/*!
 * デバッグメニュー生成
 * @author teco
 * @since 2014.05.10
 */
void CrActionStage::OnCreateDebugMenu( TbDebugMenu* parent )
{
    TbDebugMenu* menu = CreateDebugMenu(parent,"CrActionStage");
    if( !menu ) {
        return;
    }
    // ランダム入力テスト
    TbDebugMenuItemBool* randomInputMenu = menu->CreateItemBool("RandomInput");
    randomInputMenu->Bind( TbCreateMemFunc(this,&Self::SetEnableRandomInput) ,
                           TbCreateMemFunc(this,&Self::IsEnableRandomInput) );

    TbDebugMenuItemBool* drawCollisionMenu = menu->CreateItemBool("DrawDebugCollision");
    drawCollisionMenu->Bind( &s_IsEnableDebugDrawCollision );

    TbDebugMenuItemBool* undeadPlayerMenu = menu->CreateItemBool("UndeadPlayer");
    undeadPlayerMenu->Bind(&s_IsEnableUndeadPlayer);
    
    TbDebugMenuItemF32* scrollCollisionMenu = menu->CreateItemF32("ScrollOffset");
    scrollCollisionMenu->Bind(  TbCreateMemFunc(static_cast<CrStage*>(this),&Self::SetScrollOffsetX),
                                TbCreateMemFunc(static_cast<CrStage*>(this), &Self::GetScrollOffsetX));
    scrollCollisionMenu->SetUnit(100.0f);

    TbDebugMenuItemBool* debugFlagMenu = menu->CreateItemBool("DebugFlag");
    debugFlagMenu->Bind(&s_DebugFlag);

}

/*!
 * プレイヤーが不死身か
 * @author teco
 */
TbBool CrActionStage::IsPlayerUndead() const
{
    return s_IsEnableUndeadPlayer;
}

/*!
 * デバッグフラグ
 * @author teco
 */
TbBool CrActionStage::TestDebugFlag(TbUint32 index) const
{
    return s_DebugFlag;
}

/*!
 * アクターを追加
 * @author teco
 */
void CrActionStage::AddActor( CrActionActor* actor , const TbVector2& initPos ) {
    actor->SetInitPos( initPos );
    Super::AddActor(actor);
}

/*!
 * 描画
 * @author teco
 */
void CrActionStage::OnDrawFront( TbFloat32 deltatime ) 
{
    if (s_IsEnableDebugDrawCollision) {
        TbMatrix mtx;
        mtx.Translate(-GetScrollX(), 0.0f, 0.0f);
        mtx = GetSpriteScene(0).GetPoseMatrix() * mtx;
        m_CollisionScene->DrawDebug(mtx);
    }
#if 0
    if (s_IsEnableDebugDrawPlayerCount) {
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        font->PushSetting();
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        font->Printf(CR_WINDOW_WIDTH / 2.0f, CMN_WINDOW_HEIGHT / 5.0f, "%d PLAYERS GAME", Root::GetInstance().GetPlayerCount());
        font->PopSetting();
    }
#endif
}

}