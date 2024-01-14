/*!
 * アクションステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_ACTION_STAGE_H_
#define _INCLUDED_CR_ACTION_STAGE_H_

#include <crayon/stage/cr_stage.h>
#include <crayon/map/cr_action_map.h>
#include <crayon/collision/cr_action_collision_scene.h>

#include <script/lua/tb_script_lua.h>
#include <physics/tb_physics_scene_2d.h>

namespace crayon
{

class CrActor;
class CrActionActor;

class CrActionStage : public CrStage
{
    typedef CrActionStage Self;
    typedef CrStage Super;
public:

    // コンストラクタ
    CrActionStage( toybox::TbScriptLuaNode* node , 
                   TbUint32 actorLayerMax , 
                   TbUint32 passIndex , 
                   toybox::TbTexture* commonTexture ,
                   const toybox::TbRectF32* mapUvRecSTable ,
                   CrActionCollisionScene* collisionScene = nullptr ,
                   TbUint32 playerCount = 1);

    // デストラクタ
    virtual ~CrActionStage();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public:

    // コリジョンシーン取得
    CrActionCollisionScene& GetCollisionScene() {
        return *m_CollisionScene;
    }

    // 物理シーン取得
    toybox::TbPhysicsScene2d& GetPhysicsScene() {
        return m_PhysicsScene;
    }
    
    // マップ取得
    CrActionMap& GetMap() {
        return m_Map;
    }

    // マップ取得
    const CrActionMap& GetMap() const {
        return m_Map;
    }

    // ランダム入力
    TbBool IsEnableRandomInput() const {
        return m_BitArray.Test(FLAG_RANDOM_INPUT);
    }

    // ランダム入力有効
    void SetEnableRandomInput(TbBool isEnable) {
        m_BitArray.SetBit(FLAG_RANDOM_INPUT, isEnable);
    }

    // プレイヤーが不死身か
    TbBool IsPlayerUndead() const;

    // デバッグフラグ
    TbBool TestDebugFlag( TbUint32 index ) const;

public:

    // アクター生成
    template<typename T>
    T* CreateActorWithName(const toybox::TbVector2& pos, const char* name)
    {
        T* actor = new T();
        actor->SetName(name);
        AddActor(actor, pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T, typename Arg>
    T* CreateActorWithName(const toybox::TbVector2& pos, const char* name, const Arg& arg)
    {
        T* actor = new T(arg);
        actor->SetName(name);
        AddActor(actor, pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T>
    T* CreateActor(const toybox::TbVector2& pos)
    {
        T* actor = new T();
        AddActor(actor, pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T, typename Arg >
    T* CreateActor(const toybox::TbVector2& pos, const Arg& arg)
    {
        T* actor = new T(arg);
        AddActor(actor, pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T, typename Arg1, typename Arg2 >
    T* CreateActor(const toybox::TbVector2& pos, const Arg1& arg1, const Arg2& arg2)
    {
        T* actor = new T(arg1, arg2);
        AddActor(actor, pos);
        actor->Release();
        return actor;
    }

    // アクター生成
    template<typename T, typename Arg1, typename Arg2, typename Arg3 >
    T* CreateActor(const toybox::TbVector2& pos, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3)
    {
        T* actor = new T(arg1, arg2 , arg3);
        AddActor(actor, pos);
        actor->Release();
        return actor;
    }

    // アクターを追加
    void AddActor(CrActionActor* actor, const toybox::TbVector2& initPos);

public:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( toybox::TbDebugMenu* parent ) TB_OVERRIDE;

protected:

    // 描画
    virtual void OnDrawFront( TbFloat32 deltatime ) TB_OVERRIDE;

protected:

    void UpdateCollisionScene() {
        m_CollisionScene->Update();
    }

    void UpdartePhysicsScene( TbFloat32 deltasec ) {
        m_PhysicsScene.Step(deltasec);
    }

private:
    
    enum Flag
    {
        FLAG_READY , 
        FLAG_ACTIVE , 
        FLAG_RANDOM_INPUT ,
    };

    typedef toybox::TbAssocArray<TbUint32,CrActionActor*> ActorSearchMap;

private:

    CrActionMap                   m_Map;
    CrActionCollisionScene*       m_CollisionScene;
    toybox::TbPhysicsScene2d      m_PhysicsScene;
    toybox::TbBitArray32          m_BitArray;

};

}

#endif
