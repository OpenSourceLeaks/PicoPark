/*!
 * スプライト付きアクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_SPRITE_ACTOR_H_
#define _INCLUDED_CR_SPRITE_ACTOR_H_

#include <graphics/sprite/tb_sprite_all.h>
#include <base/container/tb_static_set.h>

#include <crayon/actor/cr_actor.h>
#include <crayon/stage/cr_stage.h>

namespace crayon
{

class CrActorComponent
{
public:
    CrActorComponent() : m_Owner(nullptr) {}
    virtual ~CrActorComponent() {}
private:
    TB_DISALLOW_COPY_AND_ASSIGN(CrActorComponent);
public:
    // オーナー設定
    void SetOwner( CrActor* owner ) {
        m_Owner = owner;
    }
    // オーナー取得
    CrActor* GetOwner() {
        return m_Owner;
    }
    // オーナー取得
    const CrActor* GetOwner() const {
        return m_Owner;
    }
public:
    // シーンに入った
    virtual void OnEnter( CrStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {}
    // シーンから出た
    virtual void OnExit() {}
    // 位置更新
    virtual void OnUpdatePos( const toybox::TbVector2& pos , const toybox::TbVector2& drawPos ) {}
    // 可視変更
    virtual void OnSetVisible( TbBool isVisible ) {}
    // 速度が設定された
    virtual void OnSetVelo( toybox::TbVector2* velo ) {}
private:
    CrActor*    m_Owner;    // オーナー
};

/*!
 * スプライト
 * @author teco
 */
class CrActorSprite : public CrActorComponent
{
public:

    CrActorSprite()
        : m_LayerIndex(0)
    {
        m_BitArray.SetBit(FLAG_ENABLE_OWNER_POS);
    }

public:

    // スプライト取得
    toybox::TbSprite& GetSprite() {
        return m_Sprite;
    }

    const toybox::TbSprite& GetSprite() const {
        return m_Sprite;
    }

    // ローカル座標設定
    void SetLocalPos( const toybox::TbVector2& pos ) {
        m_LocalPos = pos;
    }

    // ローカル座標設定
    void SetLocalPos( TbFloat32 x , TbFloat32 y ) {
        m_LocalPos.Set(x,y);
    }

    // ローカル座標設定
    void SetLocalX( TbFloat32 x ) {
        m_LocalPos.SetX(x);
    }

    // ローカル座標取得
    toybox::TbVector2 GetLocalPos() const {
        return m_LocalPos;
    }

    // オーナー座標利用可能
    void SetEnableOnwerPos( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_OWNER_POS,isEnable);
    }

    // オーナー座標一回だけ利用可能
    void SetEnableOnwerPosOnce( TbBool isEnable ) {
        m_BitArray.SetBit(FLAG_ENABLE_OWNER_POS_ONCE,isEnable);
    }

    // レイヤー設定
    void SetLayer( TbUint32 layer ){
        m_LayerIndex = layer;
    }

public:

    // 位置更新
    virtual void OnUpdatePos( const toybox::TbVector2& pos , const toybox::TbVector2& drawPos ) {
        if( m_BitArray.Test(FLAG_ENABLE_OWNER_POS) || m_BitArray.Test(FLAG_ENABLE_OWNER_POS_ONCE) ){
            m_BasePos = drawPos;
            m_BitArray.ResetBit(FLAG_ENABLE_OWNER_POS_ONCE);
        }
        m_Sprite.SetTranslate(m_BasePos+m_LocalPos);
    }

    // 可視設定
    virtual void OnSetVisible( TbBool isVisible ) {
        m_Sprite.SetVisible( isVisible ); 
    }

    // シーンに入った
    virtual void OnEnter( CrStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
        if( m_Sprite.IsInitialized() ){
            m_Sprite.Enter(&stage->GetSpriteScene(m_LayerIndex));
        }
    }

    // シーンから出た
    virtual void OnExit() {
        m_Sprite.Exit();
    }

private:

    enum {
        FLAG_ENABLE_OWNER_POS , 
        FLAG_ENABLE_OWNER_POS_ONCE , 
    };

private:
    toybox::TbBitArray32        m_BitArray;
    toybox::TbVector2           m_BasePos;
    toybox::TbVector2           m_LocalPos;
    toybox::TbSprite            m_Sprite;
    TbUint32                    m_LayerIndex;
};

/*!
 * スプライトフック
 * @author teco
 */
class CrActorSpriteHooker : public CrActorComponent
{
public:

    CrActorSpriteHooker()
        : m_LayerIndex(0)
    {
    }

public:

    // スプライト取得
    toybox::TbSpriteSceneHooker& GetSprite() {
        return m_Sprite;
    }

    const toybox::TbSpriteSceneHooker& GetSprite() const {
        return m_Sprite;
    }

    // レイヤー設定
    void SetLayer( TbUint32 layer ){
        m_LayerIndex = layer;
    }

public:

    // シーンに入った
    virtual void OnEnter( CrStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
        m_Sprite.Enter(&stage->GetSpriteScene(m_LayerIndex));
    }

    // シーンから出た
    virtual void OnExit() {
        m_Sprite.Exit();
    }

private:
    toybox::TbSpriteSceneHooker    m_Sprite;
    TbUint32                       m_LayerIndex;
};

/*!
 * 伸縮スプライト
 * @author teco
 */
class CrActorElasticSprite : public CrActorComponent
{
public:

    CrActorElasticSprite()
        : m_LayerIndex(0)
    {
    }

public:

    // スプライト取得
    toybox::TbElasticSprite& GetSprite() {
        return m_Sprite;
    }

    const toybox::TbElasticSprite& GetSprite() const {
        return m_Sprite;
    }

    // レイヤー設定
    void SetLayer( TbUint32 layer ){
        m_LayerIndex = layer;
    }

public:

    // 位置更新
    virtual void OnUpdatePos( const toybox::TbVector2& pos ) {
        m_Sprite.SetTranslate(pos);
    }

    // 可視設定
    virtual void OnSetVisible( TbBool isVisible ) {
        m_Sprite.SetVisible( isVisible ); 
    }

    // シーンに入った
    virtual void OnEnter( CrStage* stage , const toybox::TbVector2& pos , TbRadian angle ) {
        if( m_Sprite.IsInitialized() ){
            m_Sprite.Enter(&stage->GetSpriteScene(m_LayerIndex));
        }
    }

    // シーンから出た
    virtual void OnExit() {
        m_Sprite.Exit();
    }

private:
    toybox::TbElasticSprite    m_Sprite;
    TbUint32                   m_LayerIndex;
};

class CrSpriteActor : public CrActor
{
public:

    static const TbFloat32 WEIGHT_MAX;

private:

    typedef CrSpriteActor Self;
    typedef CrActor Super;

public:

    // コンストラクタ
    CrSpriteActor();

    // デストラクタ
    ~CrSpriteActor();

public:

    TB_DEFINE_SUB_CLASS(Self,Super);

public: 

    // コンポーネント追加
    void AddComponent( CrActorComponent* component ) {
        component->SetOwner(this);
        m_Components.PushBack(component);
    }

    // メインスプライトのオフセット座標設定
    void SetMainSpriteOffsetPos( const toybox::TbVector2& pos ) {
        m_Sprite.SetLocalPos(pos);
    }

    // スプライト取得
    toybox::TbSprite& GetMainSprite() {
        return m_Sprite.GetSprite();
    }

    // スプライト取得
    const toybox::TbSprite& GetMainSprite() const {
        return m_Sprite.GetSprite();
    }

    // レイヤー設定
    void SetMainSpriteLayer( TbUint32 layer ){
        m_Sprite.SetLayer( layer );
    }

    // 初期位置設定
    void SetInitPos( const toybox::TbVector2& pos ) {
        m_InitPos = pos;
        SetPos(pos.GetX(),pos.GetY());
    }

    // 初期位置取得
    const toybox::TbVector2& GetInitPos() const {
        return m_InitPos;
    }

    // 前フレームの位置取得
    const toybox::TbVector4& GetPrePos() const {
        return m_PrePos;
    }

    // 強制的に座標設定
    void SetPosForce( const toybox::TbVector2& pos ) {
        SetPos( pos.GetX() , pos.GetY() );
        m_PrePos.SetXY( pos.GetX() , pos.GetY() );
    }

    // 右を向いているか
    virtual TbBool IsLookingRight() const {
        return GetMainSprite().GetScale().GetX() > 0.0f;
    }

    // 左向き
    virtual void LookLeft() {
        GetMainSprite().SetScale(-1.0f,1.0f);
    }

    // 右向き
    virtual void LookRight() {
        GetMainSprite().SetScale(1.0f,1.0f);
    }

    // 可視設定
    void SetVisible( TbBool isVisible ) {
        GetMainSprite().SetVisible(isVisible);
        TbForIterator( it , m_Components ){
            (*it)->OnSetVisible(isVisible);
        }
    }

public:

    // 描画位置
    virtual toybox::TbVector2 GetDrawPos();

    // 描画補正位置設定
    void SetOffsetDrawPos( const toybox::TbVector2& pos ) {
        m_OffsetDrawPos = pos;
    }

    // 描画補正位置設定
    const toybox::TbVector2& GetOffsetDrawPos() const {
        return m_OffsetDrawPos;
    }

    // スクロールの影響を受ける
    void SetEnableScroll( TbBool isEnable )
    {
        m_BitArray.SetBit(FLAG_ENABLE_SCROLL,isEnable);
    }

    // スクロールの影響を受ける
    TbBool IsEnableScroll() const
    {
        return m_BitArray.Test(FLAG_ENABLE_SCROLL);
    }

    // 描画オフセット座標を毎フレームリセットする
    void SetEnableResetOffsetDrawPos(TbBool isEnable)
    {
        m_BitArray.SetBit(FLAG_ENABLE_RESET_OFFSET_DRAW_POS, isEnable);
    }

    // 描画オフセット座標を毎フレームリセットする
    TbBool IsEnableResetOffsetDrawPos() const
    {
        return m_BitArray.Test(FLAG_ENABLE_RESET_OFFSET_DRAW_POS);
    }

public:

    // ロジック更新
    virtual void OnUpdate( TbFloat32 deltatime ) {
        if(IsEnableResetOffsetDrawPos()){
            m_OffsetDrawPos.Clear(); // 毎フレームリセット
        }
        m_PrePos = GetPos();
        OnUpdateDerived(deltatime);
        // コリジョンと座標を同期
        toybox::TbVector2 pos( GetPos().GetX() , GetPos().GetY() );
        toybox::TbVector2 drawPos= GetDrawPos();
        TbForIterator( it , m_Components ){
            (*it)->OnUpdatePos(pos,drawPos);
        }
    }

    // ロジック更新
    virtual void OnPost( TbFloat32 deltatime ) {
        OnPostDerived(deltatime);

        toybox::TbVector2 pos( GetPos().GetX() , GetPos().GetY() );
        toybox::TbVector2 drawPos= GetDrawPos();
        TbForIterator( it , m_Components ){
            (*it)->OnUpdatePos(pos,drawPos);
        }
    }

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // 処理中断
    virtual void OnSuspend() {
        m_Sprite.GetSprite().SetEnableStep(TB_FALSE);
    }

    // 処理復帰
    virtual void OnResume() {
        m_Sprite.GetSprite().SetEnableStep(TB_TRUE);
    }

protected:

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnUpdateDerived( TbFloat32 deltatime ) {};

    // ロジック更新 (派生クラスはこちらを継承するように)
    virtual void OnPostDerived( TbFloat32 deltatime ) {};

private:

    enum
    {
        FLAG_ENABLE_SCROLL ,
        FLAG_ENABLE_RESET_OFFSET_DRAW_POS ,
        FLAG_MAX
    };

private:

    toybox::TbBitArray32                  m_BitArray;         // ビット配列
    toybox::TbVector4                     m_PrePos;
    toybox::TbVector2                     m_InitPos;          // 初期位置
    toybox::TbVector2                     m_OffsetDrawPos;    // 初期位置
    CrActorSprite                         m_Sprite;           // スプライト
    toybox::TbArray<CrActorComponent*>    m_Components;

};

}

#endif
