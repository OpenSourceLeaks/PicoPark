/*!
 * アクターシーンクラス
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */

#include "tb_fwd.h"
#include "base/actor/tb_actor_scene.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */
TbActorScene::TbActorScene( TbUint32 layerCount )
    : m_LayerCount(layerCount)
    , m_Layer(NULL)
{
    m_Layer = TB_NEW Layer[layerCount];
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */
TbActorScene::~TbActorScene()
{
    removeActorAll();
    TB_SAFE_DELETE_ARRAY(m_Layer);
}

/*!
 * 全アクター削除
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */
void TbActorScene::removeActorAll()
{
    // 追加して強制終了
    if( m_EnterLayer.top ){
        TbActor* end = m_EnterLayer.top;
        do {
            TbActor* actor = m_EnterLayer.top;
            m_EnterLayer.top = actor->m_Next;
            actor->m_Next = NULL;
            actor->m_Prev = NULL;
            addActor( actor , m_Layer[actor->GetLayerIndex()] );
        }while( end != m_EnterLayer.top );
        m_EnterLayer.top = NULL;
    }

    for( TbUint32 i = 0; i <m_LayerCount; ++i ){
        while( m_Layer[i].top ) {
            m_Layer[i].top->exitForce();
        }
    }
}

/*!
 * 更新
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */
void TbActorScene::Update( TbFloat32 deltatime )
{
    if( !IsEnableUpdate() ){
        return;
    }

    // 追加
    if( m_EnterLayer.top ){
        TbActor* end = m_EnterLayer.top;
        do {
            TbActor* actor = m_EnterLayer.top;
            m_EnterLayer.top = actor->m_Next;
            actor->m_Next = NULL;
            actor->m_Prev = NULL;
            addActor( actor , m_Layer[actor->GetLayerIndex()] );
            if( actor->m_BitArray.Test(TbActor::FLAG_EXIT) ){
                actor->exitForce();
            }
        }while( end != m_EnterLayer.top );
        m_EnterLayer.top = NULL;
    }

    lockLayer();
    for( TbUint32 i = 0; i <m_LayerCount; ++i ){
        TbActor* actor = m_Layer[i].top;
        if( !actor ) {
            continue;
        }
        // ルートのみ次を持たないのでそこまで処理するまでループ
        do {
            TbActor* next = actor->m_Next;
            if( !actor->IsSuspended() ){
                actor->update(deltatime);
            }
            actor = next;
        }while( m_Layer[i].top && actor != m_Layer[i].top );
    }
    unlockLayer();
}

/*!
 * 後処理
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */
void TbActorScene::Post( TbFloat32 deltatime )
{
    if( !IsEnableUpdate() ){
        return;
    }
    lockLayer();
    for( TbUint32 i = 0; i <m_LayerCount; ++i ){
        TbActor* actor = m_Layer[i].top;
        if( !actor ) {
            continue;
        }
        // ルートのみ次を持たないのでそこまで処理するまでループ
        do {
            TbActor* next = actor->m_Next;
            if( !actor->IsSuspended() ){
                actor->post(deltatime);
            }
            actor = next;
        }while( m_Layer[i].top && actor != m_Layer[i].top );
    }
    unlockLayer();
}

/*
 * 描画
 * @author Miyake Shunsuke
 * @since 2013.03.23
 */
void TbActorScene::Draw( TbFloat32 deltatime )
{
    if( !IsEnableDraw() ){
        return;
    }
    lockLayer();
    for( TbUint32 i = 0; i <m_LayerCount; ++i ){
        TbActor* actor = m_Layer[i].top;
        if( !actor ) {
            continue;
        }
        // ルートのみ次を持たないのでそこまで処理するまでループ
        do {
            actor->OnDraw(deltatime);
            actor = actor->m_Next;
        }while( actor != m_Layer[i].top );
    }
    unlockLayer();
}

/*!
 * アクター登録
 * @author Miyake Shunsuke
 * @since 2013.07.10
 */
void TbActorScene::addActor( TbActor* actor , TbUint32 layerIndex )
{
    if( !TB_VERIFY(actor&&layerIndex<m_LayerCount) ) {
        return;
    }
    TB_ASSERT(!actor->m_Scene);
    addActor(actor,m_EnterLayer);
    actor->m_Scene = this;
    actor->Retain();
}

// アクター登録
void TbActorScene::addActor( TbActor* actor , Layer& layer )
{
    if( layer.top ) {
        actor->m_Next = layer.top;
        actor->m_Prev = layer.top->m_Prev;
        layer.top->m_Prev->m_Next = actor;
        layer.top->m_Prev  =actor;
    }else{
        layer.top = actor;
        actor->m_Next = actor;
        actor->m_Prev = actor;
    }
}

/*!
 * アクターを除去
 * @author Miyake Shunsuke
 * @since 2013.07.10
 */
void TbActorScene::removeActor( TbActor* actor )
{
    if( !TB_VERIFY(actor && actor->m_Scene == this) ) {
        return;
    }
    if( m_Layer[actor->m_LayerIndex].top == actor ) {
        if( actor->m_Next == actor ) {
            m_Layer[actor->m_LayerIndex].top = NULL;
        }else{
            actor->m_Prev->m_Next = actor->m_Next;
            actor->m_Next->m_Prev = actor->m_Prev;
            m_Layer[actor->m_LayerIndex].top = actor->m_Next;
        }
    }else{
        actor->m_Prev->m_Next = actor->m_Next;
        actor->m_Next->m_Prev = actor->m_Prev;
    }
    actor->m_Prev = NULL;
    actor->m_Next = NULL;
    actor->m_Scene = NULL;
    actor->Release();
}

/*!
 * レイヤーの状態ロック解除
 * @author Miyake_Shunsuke
 * @since 2014.08.02
 */
void TbActorScene::unlockLayer()
{
    m_BitArray.ResetBit(FLAG_LOCK_LAYER);
}

}
