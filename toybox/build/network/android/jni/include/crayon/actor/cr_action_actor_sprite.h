/*!
 * アクション用コンポーネント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_CR_ACTION_ACTOR_SPRITE_H_
#define _INCLUDED_CR_ACTION_ACTOR_SPRITE_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor_component.h>

namespace crayon
{

/*!
 * スプライト
 * @author teco
 * @since 2014.11.30
 */
class CrActionActorSprite : public CrActionActorComponent
{
public:

    CrActionActorSprite()
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
    const toybox::TbVector2& GetLocalPos() const {
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

public:

    // 位置更新
    virtual void OnUpdatePos( const toybox::TbVector2& pos , const toybox::TbVector2& drawPos ) TB_OVERRIDE {
        if( m_BitArray.Test(FLAG_ENABLE_OWNER_POS) || m_BitArray.Test(FLAG_ENABLE_OWNER_POS_ONCE) ){
            m_BasePos = drawPos;
            m_BitArray.ResetBit(FLAG_ENABLE_OWNER_POS_ONCE);
        }
        m_Sprite.SetTranslate(m_BasePos+m_LocalPos);
    }

    // スケール更新
    virtual void OnUpdateScale( TbFloat32 scaleX , TbFloat32 scaleY ) TB_OVERRIDE
    {
        // 方向を維持
        m_Sprite.SetScale( toybox::TbSign(m_Sprite.GetScale().GetX())*scaleX , scaleY);
    }

    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) TB_OVERRIDE {
        if( m_Sprite.IsInitialized() ){
            m_Sprite.Enter(&stage->GetSpriteScene(0));
        }
    }

    // シーンから出た
    virtual void OnExit() TB_OVERRIDE {
        m_Sprite.Exit();
    }

    // 表示変更
    virtual void OnChangedVisible(TbBool isVisible) {
        m_Sprite.SetVisible(isVisible);
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
};

/*!
 * スプライトフック
 * @author teco
 * @since 2014.11.30
 */
class CrActionActorSpriteHooker : public CrActionActorComponent
{
public:

    // スプライト取得
    toybox::TbSpriteSceneHooker& GetSprite() {
        return m_Sprite;
    }

    const toybox::TbSpriteSceneHooker& GetSprite() const {
        return m_Sprite;
    }

public:

    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) TB_OVERRIDE {
        m_Sprite.Enter(&stage->GetSpriteScene(0));
    }

    // シーンから出た
    virtual void OnExit() TB_OVERRIDE {
        m_Sprite.Exit();
    }

    // 表示変更
    virtual void OnChangedVisible(TbBool isVisible) {
        m_Sprite.SetVisible(isVisible);
    }

private:
    toybox::TbSpriteSceneHooker    m_Sprite;
};

/*!
 * 伸縮スプライト
 * @author teco
 * @since 2014.11.30
 */
class CrActionActorElasticSprite : public CrActionActorComponent
{
public:

    // スプライト取得
    toybox::TbElasticSprite& GetSprite() {
        return m_Sprite;
    }

    const toybox::TbElasticSprite& GetSprite() const {
        return m_Sprite;
    }

public:

    // 位置更新
    virtual void OnUpdatePos( const toybox::TbVector2& pos , const toybox::TbVector2& drawPos ) TB_OVERRIDE {
        m_Sprite.SetTranslate(drawPos);
    }

    // シーンに入った
    virtual void OnEnter( CrActionStage* stage , const toybox::TbVector2& pos , TbRadian angle ) TB_OVERRIDE {
        if( m_Sprite.IsInitialized() ){
            m_Sprite.Enter(&stage->GetSpriteScene(0));
        }
    }

    // シーンから出た
    virtual void OnExit() TB_OVERRIDE {
        m_Sprite.Exit();
    }

    // 表示変更
    virtual void OnChangedVisible(TbBool isVisible) TB_OVERRIDE {
        m_Sprite.SetVisible(isVisible);
    }

private:
    toybox::TbElasticSprite    m_Sprite;
};

}

#endif
