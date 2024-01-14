/*!
 * ステージ
 * @author teco
 */

#ifndef _INCLUDED_CR_STAGE_H_
#define _INCLUDED_CR_STAGE_H_

#include <base/math/tb_vector2.h>
#include <base/actor/tb_actor_scene.h>
#include <base/util/tb_dynamic_cast.h>
#include <graphics/sprite/tb_sprite_scene.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <graphics/resource/tb_resource_texture.h>
#include <graphics/render/tb_draw_2d.h>
#include <framework/debug/menu/tb_debug_menu_all.h>
#include <crayon/actor/cr_actor_factory.h>
#include <crayon/input/cr_player_input.h>
#include <base/container/tb_assoc_array.h>
#include <base/math/tb_math_util.h>

namespace crayon
{

class CrActor;

class CrStage
{
public:

    typedef CrStage Self;

public:

    // コンストラクタ
    CrStage( TbUint32 actorLayerMax , 
             TbUint32 spriteLayerMax ,
             TbUint32 passIndex , 
             toybox::TbTexture* commonTexture );

    // デストラクタ
    virtual ~CrStage();

public:

    TB_DEFINE_BASE_CLASS(CrStage);

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
    void SetScroll( TbFloat32 x , TbFloat32 y ) {
        m_Scroll.Set(toybox::TbMax(0.0f,x),y);
    }

    // スクロール量設定
    void SetScrollOffset(TbFloat32 x, TbFloat32 y) {
        m_ScrollOffset.Set(x, y);
    }


    // スクロール量設定
    void SetScrollOffsetX(TbFloat32 x) {
        m_ScrollOffset.SetX(x);
    }

    // スクロール移動量取得
    toybox::TbVector2 GetScrollMov() const {
        return m_Scroll - m_OldScroll;
    }

    // スクロール量加算
    void AddScroll( TbFloat32 x , TbFloat32 y ) {
        m_Scroll.Add(x,y);
        m_Scroll.SetX( toybox::TbMax(0.0f,m_Scroll.GetX()) );
    }

    // スクロール量取得
    TbFloat32 GetScrollX() const {
        return m_Scroll.GetX() + m_ScrollOffset.GetX();
    }

    // スクロール量取得
    TbFloat32 GetScrollY() const {
        return m_Scroll.GetY() + m_ScrollOffset.GetY();
    }

    //　オフセットスクロール量取得
    TbFloat32 GetScrollOffsetX() const {
        return m_ScrollOffset.GetX();
    }

    // スクロール量取得
    toybox::TbVector2 GetScroll() const {
        return m_Scroll + m_ScrollOffset;
    }

    // スケール設定
    void SetScale(TbFloat32 scale) {
        m_Scale = scale;
    }

    // スケール取得
    TbFloat32 GetScale() const {
        return m_Scale;
    }

    // プレイ時間設定
    void SetPlaySec(TbFloat32 sec) {
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
    const CrPlayerInput& GetPlayerInput() const {
        return m_PlayerInput;
    }

    // プレイヤー入力ポーズ設定
    void SetPausePlayerInput( TbBool isPause ) {
        m_PlayerInput.SetPause(isPause);
    }

    // カスタム入力設定
    void SetPlayerCustomInput( CrPlayerCustomInput* customInput ) {
        m_PlayerInput.SetCustom( customInput );
    }

    // 
    void SetPoseMatrix(const toybox::TbMatrix& pose) {
        m_PoseMatrix = pose;
    }

    // 
    const toybox::TbMatrix& GetPoseMatrix() const {
        return m_PoseMatrix;
    }

    // アクターリセット
    void ResetActor();

    // アクターリセット
    void ResetActor( const char* target );

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

    // 画面揺らし
    void RequestScreenQuake();

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
    
    // 更新
    void Update();

    // 描画
    void Draw();

public:

    // デバッグメニュー生成
    virtual void OnCreateDebugMenu( toybox::TbDebugMenu* parent ) {}

public:

    // アクターシーン取得
    toybox::TbActorScene& GetActorScene() {
        return m_ActorScene;
    }

    // スプライトシーン取得
    toybox::TbSpriteScene& GetSpriteScene( TbUint32 layer ) {
        return m_SpriteScene[layer];
    }

public:

    // アクターを外部から生成
    CrActor* CreateActor( const CrActorFactory::CreateParam& param ) {
        return OnCreatedActor(param);
    }

protected:

    // 更新
    virtual void OnPre( TbFloat32 deltatime ) {}

    // 更新
    virtual void OnUpdate( TbFloat32 deltatime ) {}

    // 描画
    virtual void OnDraw( TbFloat32 deltatime ) {}

    // 描画
    virtual void OnDrawFront( TbFloat32 deltatime ) {}

    // 有効状態が変化した
    virtual void OnChangedActive( TbBool isActive ){ TB_UNUSE(isActive); }

    // アクター生成
    virtual CrActor* OnCreatedActor( const CrActorFactory::CreateParam& param ) { return nullptr; }

public:

    // アクター検索
    CrActor* FindActor( const char* name );

    // アクター検索
    const CrActor* FindActor( const char* name ) const;

    // イベント通知
    virtual TbSint32 NotifyEvent( const char* name , TbSint32 eve , void* param );

protected:

    // デバッグメニュー生成
    toybox::TbDebugMenu* CreateDebugMenu( toybox::TbDebugMenu* parent , const char* name ) {
        if( !m_DebugMenu ){
            if( parent ){
                m_DebugMenu = parent->CreateItemMenu(name);
            }else{
                m_DebugMenu = toybox::TbDebugMenuManager::GetInstance()->CreateMenu(name);
            }
            return m_DebugMenu;
        }
        return NULL;
    }

protected:

    // スプライト用マテリアル取得
    toybox::TbSpriteFixedMaterial& GetSpriteMaterial() {
        return m_SpriteMaterial;
    }

    // アクター生成取得
    CrActorFactory& GetActorFactory() {
        return m_ActorFactory;
    }

    // アクターを追加
    void AddActor( CrActor* actor );

    // 生成閾値
    virtual TbFloat32 GetActorCreateThreshold() const {
        return m_Scroll.GetX();
    }

private:

    enum Flag
    {
        FLAG_READY , 
        FLAG_REQUEST_RETRY , 
        FLAG_REQUEST_CLEAR , 
        FLAG_REQUEST_GAMEOVER , 
        FLAG_ACTIVE , 
        FLAG_SCREEN_QUAKE ,
    };
    
    typedef toybox::TbAssocArray<TbUint32,CrActor*> ActorSearchMap;
    typedef toybox::TbArray<toybox::TbSpriteScene> SpriteSceneList;
private:
    
    TbUint32                        m_Id;               // ID
    toybox::TbBitArray32            m_BitArray;         // ビット配列
    SpriteSceneList                 m_SpriteScene;      // スプライトシーン
    toybox::TbSpriteFixedMaterial   m_SpriteMaterial;   // スプライト用マテリアル
    toybox::TbVector2               m_OldScroll;
    toybox::TbVector2               m_Scroll;           // スクロール量
    toybox::TbVector2               m_ScrollOffset;
    TbFloat32                       m_Scale;            // 
    CrActorFactory                  m_ActorFactory;     // アクター生成
    toybox::TbActorScene            m_ActorScene;       // シーン  
    TbFloat32                       m_PlaySec;          // プレイ時間
    CrPlayerInput                   m_PlayerInput;
    ActorSearchMap                  m_ActorSearchMap;        // 検索マップ
    toybox::TbMatrix                m_PoseMatrix;
    TbFloat32                       m_ScreenQuakeSec;   // 画面

private:

    toybox::TbDebugMenu*                    m_DebugMenu;

};

}

#endif
