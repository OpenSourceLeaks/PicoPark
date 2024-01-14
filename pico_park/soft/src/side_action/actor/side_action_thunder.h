/*!
* サンダー
* @author teco
*/

#ifndef _INCLUDED_ACTION_PLAYER_THUNDER_H_
#define _INCLUDED_ACTION_PLAYER_THUNDER_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"
#include <base/container/tb_static_array.h>

class SideActionPlayer;

class SideActionThunder : public CrActionActor
{
public:

    typedef SideActionThunder Self;
    typedef CrActionActor Super;
    TB_DEFINE_SUB_CLASS(Self, Super);

    static const TbFloat32 LASER_LEN;

public:

    // コンストラクタ
    SideActionThunder();

    // デストラクタ
    virtual ~SideActionThunder();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

protected:

    // ロジック後処理
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

    // ロジック後処理
    virtual void OnPostAction(TbFloat32 deltatime) TB_OVERRIDE;

private:

    enum
    {
        FLAG_SETUP , 
        FLAG_THROUGH ,
        FLAG_SHOOT_BLOCK
    };

private:

    // 衝突時コールバック
    void onCollided(CrActionCollision*);

    // パーティクル描画
    void onDrawLaser(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

    // コリジョン矩形取得
    TbRectF32 getCollisionRect( TbFloat32 len ) const;

private:

    TbBitArray32                m_BitArray;
    CrActionActorSpriteHooker   m_SpriteHooker;
    CmnDir                      m_Dir;
    TbFloat32                   m_LaserLen;
    TbFloat32                   m_LaserLenOld;
    TbFloat32                   m_LaserLenMap;
    TbFloat32                   m_AnimTimer;
    TbUint32                    m_UvIndex;
    TbUint32                    m_CollidedPlayerBit;
    TbUint32                    m_CollidedPlayerBitOld;
    TbStaticArray<SideActionPlayer*,CMN_PLAYER_MAX> m_Target;
};

#endif
