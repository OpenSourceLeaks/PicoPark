/*!
 * プレイヤーアクター
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_ACTION_PLAYER_INPUT_H_
#define _INCLUDED_ACTION_PLAYER_INPUT_H_

#include <graphics/sprite/tb_sprite.h>
#include <input/tb_bit_input.h>
#include <crayon/input/cr_player_input.h>
#include "system/sys_majority_input.h"

class SideActionPlayerMajorityInput : public CrPlayerCustomInput
{
public:
    SideActionPlayerMajorityInput(){}
protected:
    virtual void OnUpdate( TbFloat32 deltatime );
private:
    SysMajorityInput    m_MajorityInput;
};

class SideActionPlayerRandomInput : public toybox::TbBitInput32
{
public:
    SideActionPlayerRandomInput() : m_DirectionTimer(0.0f) {}
protected:
    virtual void OnUpdate( TbFloat32 deltatime );
private:
    TbFloat32 m_DirectionTimer;
};

class SideActionPlayerTitleInput : public toybox::TbBitInput32
{
public:
    SideActionPlayerTitleInput( TbUint32 playerIndex ) 
        : m_PlayerIndex(playerIndex) 
        , m_Time(0.0f) 
    {
        m_Time -= m_PlayerIndex;
    }
protected:
    virtual void OnUpdate( TbFloat32 deltatime );
private:
    TbUint32  m_PlayerIndex;
    TbFloat32 m_Time;
};


class SideActionPlayerMainMenuInput : public toybox::TbBitInput32
{
public:
    SideActionPlayerMainMenuInput()
    {
        Press(CrInput::BUTTON_RIGHT);
    }
protected:
    virtual void OnUpdate(TbFloat32 deltatime) {}
};

class SideActionPlayerSwitchInput : public toybox::TbBitInput32
{
public:
public:
    void OnEvent( TbUint32 event , void* param );
protected:
    virtual void OnUpdate( TbFloat32 deltatime );
};

class SideActionPlayerMultiPlayerInput : public toybox::TbBitInput32
{
public:
    SideActionPlayerMultiPlayerInput();
public:
    // セットアップ
    void Initialize( TbFloat32 jumpVelo , TbFloat32 walkVelo );
    // ジャンプ速度取得
    TbFloat32 GetJumpVelo() const;
    // 歩行速度取得
    TbFloat32 GetWalkVelo() const {
        return m_WalkVelo;
    }
    // メインプレイヤー設定
    void SetMainPlayerIndex(TbUint32 index);
public:
    void OnEvent( TbUint32 event , void* param );
protected:
    virtual void OnUpdate( TbFloat32 deltatime );
private:
    struct PlayerInfo
    {
        PlayerInfo() 
            : jumpVelo(0.0f)
            , isEnableUp(TB_FALSE) 
        {}
        TbFloat32   jumpVelo;
        TbBool      isEnableUp;
    };
private:
    TbUint32    m_PlayerCount;
    PlayerInfo  m_PlayerInfo[CMN_PLAYER_MAX];
    TbSint32    m_ActivePlayer;
    TbUint32    m_MainPlayer;
    TbFloat32   m_WalkVelo;
};

#endif
