/*!
 * デフォルトなプレイヤーステート
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_STATE_DEFAULT_H_
#define _INCLUDED_ACTION_PLAYER_STATE_DEFAULT_H_

#include "side_action_player_state.h"

class SideActionPlayerStateDefault : public SideActionPlayerState
{
public:

    typedef SideActionPlayerState Super;

public:

    // コンストラクタ
    SideActionPlayerStateDefault();

    // デストラクタ
    virtual ~SideActionPlayerStateDefault();

public:

    // 更新
    virtual void Update( SideActionPlayer& player , TbFloat32 deltatime );

    // 後処理
    virtual void Post( SideActionPlayer& player , TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(SideActionPlayer& player, TbUint32 event , void* param );

protected:

    // 接触開始コールバック
    virtual void OnContactAdded(SideActionPlayer& player , const TbVector2& normal ,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& info);

    // 前景描画
    virtual void OnDrawText(SideActionPlayer& player,
        const TbMatrix& poseMatrix,
        TbSpriteMaterial* material);

private:

    enum Flag
    {
        FLAG_FORCE_JUMP , 
        FLAG_FORCE_JUMP_FOR_SWITCH ,
        FLAG_REQUEST_VISIBLE , 
        FLAG_CATCHED ,
        FLAG_MAX 
    };

private:

    // ジャンプ可能
    TbBool updateEnableJump( SideActionPlayer& player , TbFloat32 deltaSec );

    // 可視設定
    void setVisible(SideActionPlayer& player , TbBool isVisible );

private:

    TbBitArray32        m_BitArray;
    TbVector2           m_ForceJumpVelo;// 強制ジャンプ初速度
    TbUint32            m_JumpFrame;    // ジャンプカウント
    TbUint32            m_JumpCount;    // 連続ジャンプ数
    TbFloat32           m_EnableJumpSec;
    TbVector2           m_RequestedPos; 

};

#endif
