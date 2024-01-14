/*!
 * 天秤
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_BALANCE_H_
#define _INCLUDED_SIDE_ACTION_BALANCE_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionBalanceUnit : public CrActionActor
{
public:

    // コンストラクタ
    SideActionBalanceUnit();

    // デストラクタ
    ~SideActionBalanceUnit() {}

public:

    // 目標位置設定
    void SetTargetY( TbFloat32 y ) {
        m_TargetOffsetY = y;
    }

    // 目標位置への移動速度設定
    void SetSpeedY( TbFloat32 y ) {
        m_SpeedY = y;
    }

    // 速度取得
    TbFloat32 GetSpeedY() const {
        return m_SpeedY;
    }

    // 乗っているキャラの数
    TbUint32 GetContactCount() const {
        return m_ContactCount;
    }

    // オフセットY座標取得
    TbFloat32 GetOffsetY() const {
        return m_OffsetY;
    }

    // 目標オフセットY座標取得
    TbFloat32 GetTargetOffsetY() const {
        return m_TargetOffsetY;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack( const TbVector2& pos , const TbVector2& pushBackVec ) {
        m_OffsetY = (pos - GetInitPos()).GetY();
        m_BitArray.SetBit(FLAG_PUSH_BACK);
    }

private:

    enum
    {
        FLAG_PUSH_BACK
    };

private:

    TbBitArray32            m_BitArray;
    TbFloat32               m_TargetOffsetY;
    TbFloat32               m_OffsetY;
    TbFloat32               m_SpeedY;
    TbUint32                m_ContactCount;

};

class SideActionBalance : public CrActionActor
{
public:

    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionBalance( TbFloat32 interval );

    // デストラクタ
    virtual ~SideActionBalance();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 入る
    virtual void OnEnter( void* userData );

private:


    enum UnitType
    {
        UNIT_LEFT , 
        UNIT_RIGHT, 
        UNIT_MAX
    };

private:

    TbBitArray32            m_BitArray;
    TbFloat32               m_Interval;
    SideActionBalanceUnit*  m_Unit[UNIT_MAX];

};

#endif
