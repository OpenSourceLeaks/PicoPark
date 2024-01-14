/*!
 * プレイヤー用被り物
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_DRESS_H_
#define _INCLUDED_ACTION_PLAYER_DRESS_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionPlayer;

class SideActionPlayerHeaddress : public CrActionActor
{
public:

    enum Type
    {
        TYPE_HELMET , 
        TYPE_HALOWEEN1 ,
        TYPE_HALOWEEN2 ,
        TYPE_NUMBER ,
        TYPE_MAX
    };

public:

    typedef SideActionPlayerHeaddress Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerHeaddress( SideActionPlayer* player , Type type , TbBool isEnableCollision );

    // デストラクタ
    virtual ~SideActionPlayerHeaddress();

public:

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    // 前景描画
    void onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

private:

    Type                        m_Type;
    TbBitArray32                m_BitArray;
    SideActionPlayer*           m_Player;
    CrActionActorSpriteHooker   m_SpriteHooker;

};

class SideActionPlayerBackdress : public CrActionActor
{
public:

    enum Type
    {
        TYPE_POWERED , 
        TYPE_MAX
    };

public:

    typedef SideActionPlayerBackdress Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerBackdress( SideActionPlayer* player , Type type );

    // デストラクタ
    virtual ~SideActionPlayerBackdress();

public:

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    Type                m_Type;
    TbBitArray32        m_BitArray;
    SideActionPlayer*   m_Player;
};

#endif
