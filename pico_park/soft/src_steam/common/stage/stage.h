/*!
 * ステージ
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_STAGE_H_
#define _INCLUDED_STAGE_H_

#include <base/math/tb_vector2.h>
#include <base/actor/tb_actor_scene.h>
#include <base/util/tb_dynamic_cast.h>
#include <graphics/sprite/tb_sprite_scene.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_draw_2d.h>
#include <framework/debug/menu/tb_debug_menu_all.h>
#include "common/actor/actor_factory.h"
#include "system/sys_player_input.h"

class Stage
{
public:

    typedef Stage Self;

public:

    // コンストラクタ
    Stage( TbUint32 actorLayerMax );

    // デストラクタ
    virtual ~Stage();

public:

    TB_DEFINE_BASE_CLASS(Stage);

public:

    // 有効化
    void SetActive( TbBool isActive );

    // 有効かどうか
    TbBool IsActive() const {
        return m_BitArray.Test(FLAG_ACTIVE);
    }

    // 準備完了
    TbBool IsReady();

    // ID設定
    void SetId( TbUint32 id ) {
        m_Id = id;
    }

    // ID取得
    TbUint32 GetId() const {
        return m_Id;
    }

    // スクロール量設定
    void SetScroll( TbFloat32 scroll ) {
        m_Scroll = TbMax(0.0f,scroll);
    }

    // スクロール量加算
    void AddScroll( TbFloat32 scroll ) {
        SetScroll( m_Scroll + scroll );
    }

    // スクロール量取得
    TbFloat32 GetScroll() const {
        return m_Scroll;
    }

    // スケール設定
    void SetScale(TbFloat32 scale) {
        m_Scale = scale;
    }

    // スケール取得
    TbFloat32 GetScale() const {
        return m_Scale;
    }

    // 画面の横幅取得
    TbFloat32 GetWindowWidth() const {
        return CMN_WINDOW_WIDTH/m_Scale;
    }

    // プレイ時間設定
    void SetPlaySec( TbFloat32 sec ) {
        m_PlaySec = sec;
    }

    // プレイ時間取得
    TbFloat32 GetPlaySec() const {
        return m_PlaySec;
    }

    // プレイ時間取得
    TbFloat32 GetPlayMillSec() const {
        return m_PlaySec * 1000.0f;
    }

    // プレイヤー入力取得
    const SysPlayerInput& GetPlayerInput() const {
        return m_PlayerInput;
    }

    // プレイヤー入力ポーズ設定
    void SetPausePlayerInput( TbBool isPause ) {
        m_PlayerInput.SetPause(isPause);
    }

    // カスタム入力設定
    void SetPlayerCustomInput( SysPlayerCustomInput* customInput ) {
        m_PlayerInput.SetCustom( customInput );
    }

    // 
    void SetPoseMatrix(const TbMatrix& pose) {
        m_PoseMatrix = pose;
    }

    // 
    const TbMatrix& GetPoseMatrix() const {
        return m_PoseMatrix;
    }

public:

    // リトライをリクエスト
    void RequestRetry() {
        m_BitArray.SetBit(FLAG_REQUEST_RETRY);
    }
    
    // クリアをリクエスト
    void RequestClear() {
        m_BitArray.SetBit(FLAG_REQUEST_CLEAR);
    }

    // ゲームオーバーをリクエスト
    void RequestGameOver() {
        m_BitArray.SetBit(FLAG_REQUEST_GAMEOVER);
    }

    // 何かリクエストがある
    TbBool IsRequestedAny() const {
        return IsRequestedRetry() || IsRequestedGameOver() || IsRequestedClear();
    }

    // リトライをリクエスト
    TbBool IsRequestedRetry() const {
        return m_BitArray.Test(FLAG_REQUEST_RETRY);
    }
    
    // クリアをリクエスト
    TbBool IsRequestedClear() const {
        return m_BitArray.Test(FLAG_REQUEST_CLEAR);
    }

    // ゲームオーバーをリクエスト
    TbBool IsRequestedGameOver() const {
        return m_BitArray.Test(FLAG_REQUEST_GAMEOVER);
    }

    // ポーズ設定
    void SetPause( TbBool isEnable ) {
        m_ActorScene.SetEnableUpdate(!isEnable);
    }

public:

    // イベント通知
    virtual void NotifyEvent( const char* name , TbSint32 eve , void* param ) {}

public:
    
    // 更新
    void Update();

    // 描画
    void Draw();

public:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( TbDebugMenu* parent ) {}

public:

    // アクターシーン取得
    toybox::TbActorScene& GetActorScene() {
        return m_ActorScene;
    }

    // スプライトシーン取得
    toybox::TbSpriteScene& GetSpriteScene() {
        return m_SpriteScene;
    }

protected:

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) {}

    // 描画
    virtual void OnDraw( TbFloat32 deltatime ) {}

    // 描画
    virtual void OnDrawFront( TbFloat32 deltatime ) {}

    // 有効状態が変化した
    virtual void OnChangedActive( TbBool isActive ){ TB_UNUSE(isActive); }

    // アクター生成
    virtual void OnCreatedActor(const char* typeName,const char* name,const TbVector2& pos,const ActorFactory::UserData& userData) {}

protected:

    // デバッグメニュー生成
    TbDebugMenu* CreateDebugMenu( TbDebugMenu* parent , const char* name ) {
        if( !m_DebugMenu ){
            if( parent ){
                m_DebugMenu = parent->CreateItemMenu(name);
            }else{
                m_DebugMenu = TbDebugMenuManager::GetInstance()->CreateMenu(name);
            }
            return m_DebugMenu;
        }
        return NULL;
    }

protected:

    // スプライト用マテリアル取得
    TbSpriteFixedMaterial& GetSpriteMaterial() {
        return m_SpriteMaterial;
    }

    // アクター生成取得
    ActorFactory& GetActorFactory() {
        return m_ActorFactory;
    }

private:

    enum Flag
    {
        FLAG_READY , 
        FLAG_REQUEST_RETRY , 
        FLAG_REQUEST_CLEAR , 
        FLAG_REQUEST_GAMEOVER , 
        FLAG_ACTIVE , 
    };
    
private:
    
    TbUint32                        m_Id;               // ID
    TbBitArray32                    m_BitArray;         // ビット配列
    toybox::TbSpriteScene           m_SpriteScene;      // スプライトシーン
    toybox::TbSpriteFixedMaterial   m_SpriteMaterial;   // スプライト用マテリアル
    TbFloat32                       m_Scroll;           // スクロール量
    TbFloat32                       m_Scale;
    ActorFactory                    m_ActorFactory;     // アクター生成
    toybox::TbActorScene            m_ActorScene;       // シーン  
    TbFloat32                       m_PlaySec;          // プレイ時間
    SysPlayerInput                  m_PlayerInput;
    TbMatrix                        m_PoseMatrix;

private:

    TbDebugMenu*                    m_DebugMenu;

};

#endif
