/*!
* 特定のプレイヤーが押さないと動かない箱
* @author teco
*/

#ifndef _INCLUDED_SIDE_ACTION_COLOR_BLOCK_H_
#define _INCLUDED_SIDE_ACTION_COLOR_BLOCK_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"
#include "common/stage/action_stage.h"
#include <base/util/tb_dynamic_cast.h>

class SideActionColorBlock : public ActionActor
{
public:

    typedef ActionActor         Super;
    typedef SideActionColorBlock  Self;

public:

    // コンストラクタ
    SideActionColorBlock(const TbRectF32& rect, TbSint32 playerIndex, TbSint32 exceptPlayerIndex);

    // デストラクタ
    virtual ~SideActionColorBlock();

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

private:

    // 初期化
    void initialize(const TbRectF32& rect);

    // 任意の方向から押されているか
    TbBool isPushed(CmnContactFace type);

    // 押している数
    TbUint32 isPushsdByColor(ActionActor* actor, CmnContactFace type);

private:

    ActionActorElasticSprite m_Sprite;
    TbRectF32                m_Rect;
    TbSint32                 m_PlayerIndex;

};

class SideActionColorBlockConstraint : public ActionActor
{
public:

    // コンストラクタ
    SideActionColorBlockConstraint();

    // デストラクタ
    virtual ~SideActionColorBlockConstraint();

public:

    // 距離設定
    void SetDistance(TbFloat32 distance) {
        m_Distance = distance;
    }

    // ブロック追加
    void AddBlock(SideActionColorBlock* block ) {
        m_BlockList.PushBack(block);
    }

    // ブロック追加
    void RemoveBlock(SideActionColorBlock* block) {
        BlockList::Iterator it = TbFind(m_BlockList,block);
        if (it != m_BlockList.End()) {
            m_BlockList.Erase(it);
        }
    }

protected:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

private:

    static const TbUint32 BLOCK_MAX = 32;
    struct CalcInfo
    {
        CalcInfo() : block(nullptr) {}
        SideActionColorBlock* block;
        TbVector2   f;
        TbVector2   prevTension;
        TbVector2   nextTension;
    };

private:

    // 解決
    void calcF(CalcInfo* player, CalcInfo* prev, CalcInfo* next);

    // 解決
    TbVector2 calcF(SideActionColorBlock* src, SideActionColorBlock* target, const TbVector2& srcTension, const TbVector2& targetTension);

    // 引っ張り計算
    TbVector2 calcPrevTension(TbSint32 index );

    // 引っ張り計算
    TbVector2 calcNextTension(TbSint32 index );

    // 引っ張り計算
    TbVector2 calcTension(SideActionColorBlock* src, SideActionColorBlock* target);

private:

    typedef TbStaticArray<SideActionColorBlock*, BLOCK_MAX> BlockList;

private:

    TbFloat32                       m_Distance;
    BlockList                       m_BlockList;

};


#endif
