/*!
 * アクション用コンポーネント
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_ACTOR_SPRITE_H_
#define _INCLUDED_ACTION_ACTOR_SPRITE_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor_component.h"

/*!
 * スプライト
 * @author teco
 * @since 2014.11.30
 */
class ActionActorSprite : public ActionActorComponent
{
public:

    ActionActorSprite()
    {
        m_BitArray.SetBit(FLAG_ENABLE_OWNER_POS);
    }

public:

    // スプライト取得
    TbSprite& GetSprite() {
        return m_Sprite;
    }

    const TbSprite& GetSprite() const {
        return m_Sprite;
    }

    // ローカル座標設定
    void SetLocalPos( const TbVector2& pos ) {
        m_LocalPos = pos;
    }

    // ローカル座標設定
    void SetLocalX( TbFloat32 x ) {
        m_LocalPos.SetX(x);
    }

    // ローカル座標取得
    TbVector2 GetLocalPos() const {
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
    virtual void OnUpdatePos( const TbVector2& pos ) {
        if( m_BitArray.Test(FLAG_ENABLE_OWNER_POS) || m_BitArray.Test(FLAG_ENABLE_OWNER_POS_ONCE) ){
            m_BasePos = pos;
            m_BitArray.ResetBit(FLAG_ENABLE_OWNER_POS_ONCE);
        }
        m_Sprite.SetTranslate(m_BasePos+m_LocalPos);
    }

    // シーンに入った
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
        if( m_Sprite.IsInitialized() ){
            m_Sprite.Enter(&stage->GetSpriteScene());
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
    TbBitArray32        m_BitArray;
    TbVector2           m_BasePos;
    TbVector2           m_LocalPos;
    toybox::TbSprite    m_Sprite;
};

/*!
 * スプライトフック
 * @author teco
 * @since 2014.11.30
 */
class ActionActorSpriteHooker : public ActionActorComponent
{
public:

    // スプライト取得
    TbSpriteSceneHooker& GetSprite() {
        return m_Sprite;
    }

    const TbSpriteSceneHooker& GetSprite() const {
        return m_Sprite;
    }

public:

    // シーンに入った
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
        m_Sprite.Enter(&stage->GetSpriteScene());
    }

    // シーンから出た
    virtual void OnExit() {
        m_Sprite.Exit();
    }

private:
    toybox::TbSpriteSceneHooker    m_Sprite;
};

/*!
 * 伸縮スプライト
 * @author teco
 * @since 2014.11.30
 */
class ActionActorElasticSprite : public ActionActorComponent
{
public:

    // スプライト取得
    TbElasticSprite& GetSprite() {
        return m_Sprite;
    }

    const TbElasticSprite& GetSprite() const {
        return m_Sprite;
    }

public:

    // 位置更新
    virtual void OnUpdatePos( const TbVector2& pos ) {
        m_Sprite.SetTranslate(pos);
    }

    // シーンに入った
    virtual void OnEnter( ActionStage* stage , const TbVector2& pos , TbRadian angle ) {
        if( m_Sprite.IsInitialized() ){
            m_Sprite.Enter(&stage->GetSpriteScene());
        }
    }

    // シーンから出た
    virtual void OnExit() {
        m_Sprite.Exit();
    }

private:
    toybox::TbElasticSprite    m_Sprite;
};

#endif
