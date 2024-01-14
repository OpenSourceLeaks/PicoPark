/*!
 * ルーレット
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_ROULETTE_H_
#define _INCLUDED_SIDE_ACTION_ROULETTE_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include <crayon/actor/cr_action_actor_child.h>
#include "side_action/side_action_types.h"

class SideActionSwitch;

class SideActionRouletteManager : public CrActionActor
{
    typedef CrActionActor Super;
public:

    // コンストラクタ
    SideActionRouletteManager();

    // デストラクタ
    ~SideActionRouletteManager();

public:

    // 最大数設定
    void SetMax( TbUint32 count ){
        m_Max = count;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

protected:

    // シーンに入った
    virtual void OnEnter( void* userData );

private:

    enum Result
    {
        RESULT_NONE , 
        RESULT_FAILED , 
        RESULT_SUCCESS , 
    };

    static const TbUint32 RESULT_MAX = 10;

private:
    
    TbUint32    m_Max;
    TbUint32    m_Count;
    Result      m_Result[RESULT_MAX];

};

class SideActionRoulette : public CrActionActor
{
public:

    typedef SideActionRoulette Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionRoulette();

    // デストラクタ
    virtual ~SideActionRoulette();

public:

    // 切り替え時間
    void SetSwitchSec( TbFloat32 sec ) {
        m_SwitchSec = sec;
    }

    // 応え設定
    void SetAnswer( TbSint32 index ) {
        m_AnswerIndex = index;
    }

    // 候補追加
    void AddCandidate( const char* word ) {
        m_Candidate.PushBack(word);
    }

    // 正しい 
    TbBool IsSuccess() const {
        if( m_BitArray.Test(FLAG_SELECT) ) {
            return m_AnswerIndex == m_CurrentIndex;
        }
        return TB_FALSE;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_SELECT ,
        FLAG_MAX
    };

private:

    // ルーレット候補描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbBitArray32                m_BitArray;
    TbSint32                    m_CurrentIndex;
    TbSint32                    m_AnswerIndex;
    TbArray<TbStaticString8>    m_Candidate;
    TbFloat32                   m_SwitchSec;
    TbFloat32                   m_SwitchTimer;

    CrActionActorSpriteHooker     m_SpriteHooker;    
    CrActionActorChild            m_Child;
    SideActionSwitch*           m_Switch;

};

#endif
