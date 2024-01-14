/*!
 * アクションステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_STAGE_H_
#define _INCLUDED_ACTION_STAGE_H_

#include "common/stage/stage.h"
#include "common/map/action_map.h"
#include "common/collision/action_collision_scene.h"
#include <script/lua/tb_script_lua.h>
#include <base/container/tb_assoc_array.h>
#include <physics/tb_physics_scene_2d.h>

class Actor;
class ActionActor;

class ActionStage : public Stage
{
    typedef ActionStage Self;
    typedef Stage Super;
public:

    // コンストラクタ
    ActionStage( TbScriptLuaNode* node , TbUint32 actorLayerMax , ActionCollisionScene* collisionScene = NULL );

    // デストラクタ
    virtual ~ActionStage();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public:

    // コリジョンシーン取得
    ActionCollisionScene& GetCollisionScene() {
        return *m_CollisionScene;
    }

    // 物理シーン取得
    TbPhysicsScene2d& GetPhysicsScene() {
        return m_PhysicsScene;
    }
    
    // マップ取得
    ActionMap& GetMap() {
        return m_Map;
    }

    // マップ取得
    const ActionMap& GetMap() const {
        return m_Map;
    }

    // ランダム入力
    TbBool IsEnableRandomInput() const {
        return m_BitArray.Test(FLAG_RANDOM_INPUT);
    }

    // アクター生成
    template<typename T>
    T* CreateActorWithName( const TbVector2& pos ,const char* name )
    {
        T* actor = new T();
        actor->SetName(name);
        AddActor(actor,pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T>
    T* CreateActor( const TbVector2& pos )
    {
        T* actor = new T();
        AddActor(actor,pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T,typename Arg >
    T* CreateActor(  const TbVector2& pos , const Arg& arg )
    {
        T* actor = new T(arg);
        AddActor(actor,pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T,typename Arg1,typename Arg2 >
    T* CreateActor(  const TbVector2& pos , const Arg1& arg1 , const Arg2& arg2 )
    {
        T* actor = new T(arg1,arg2);
        AddActor(actor,pos);
        actor->Release();
        return actor;
    }

    // アクター検索
    ActionActor* FindActor( const char* name );

    // アクター検索
    const ActionActor* FindActor( const char* name ) const;

public:

    // イベント通知
    virtual void NotifyEvent( const char* name , TbSint32 eve , void* param ) TB_OVERRIDE;

public:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent ) TB_OVERRIDE;

protected:

    void UpdateCollisionScene() {
        m_CollisionScene->Update();
    }

    void UpdartePhysicsScene( TbFloat32 deltasec ) {
        m_PhysicsScene.Step(deltasec);
    }

    // アクターを追加
    void AddActor( ActionActor* actor , const TbVector2& initPos );

protected:

    // 描画
    virtual void OnDrawFront( TbFloat32 deltatime ) TB_OVERRIDE;

private:
    
    enum Flag
    {
        FLAG_READY , 
        FLAG_ACTIVE , 
        FLAG_RANDOM_INPUT ,
    };

    typedef TbAssocArray<TbUint32,ActionActor*> ActorSearchMap;

private:

    // ランダム入力有効
    void setEnableRandomInput( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_RANDOM_INPUT,isEnable);
    }

private:

    static TbBool               s_IsEnableDebugDrawCollision;
    static TbBool               s_IsEnableDebugDrawPlayerCount;
    
private:

    ActionMap                   m_Map;
    ActionCollisionScene*       m_CollisionScene;
    TbPhysicsScene2d            m_PhysicsScene;
    TbBitArray32                m_BitArray;
    ActorSearchMap              m_ActorSearchMap;
    TbFloat32                   m_PoseMatrix;

};

#endif
