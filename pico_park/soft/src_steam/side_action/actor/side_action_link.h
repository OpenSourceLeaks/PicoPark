/*!
 * リンク
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LINK_H_
#define _INCLUDED_SIDE_ACTION_LINK_H_

#include "common/actor/action_actor.h"

class SideActionLinkBlock : public ActionActor
{
public:
    
    // コンストラクタ
    SideActionLinkBlock( TbUint32 uvIndex );
    // デストラクタ
    ~SideActionLinkBlock();

public:

    // 目標位置設定
    void SetTargetPos( const TbVector2& pos );

    // 移動
    void Activate( TbBool isInterpolated ) {
        if( STATE_BEGIN == m_State ) {
            m_State = STATE_ACTIVE;
            m_BitArray.SetBit(FLAG_INTERPOLATE,isInterpolated);
        }
    }

public:
    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:
    
    enum State
    {
        STATE_BEGIN , 
        STATE_ACTIVE ,
        STATE_END , 
    };

    enum Flag
    {
        FLAG_INTERPOLATE ,
        FLAG_MAX
    };

private:

    TbBitArray32    m_BitArray;
    State           m_State;
    TbVector2       m_TargetPos;

};

class SideActionLink : public ActionActor
{
public:
    
    typedef SideActionLink Self;
    typedef ActionActor Super;

public:

    enum Type
    {
        TYPE_OUT , 
        TYPE_IN
    };

public:

    // コンストラクタ
    SideActionLink( Type type , TbUint32 blockCount , TbUint32 blockSize , const TbVector2& dir );

    // デストラクタ
    virtual ~SideActionLink();

public:

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

public:

    // 初期オフセット数
    void SetInitOffsetCount(TbUint32 count) {
        m_InitOffsetCount = count;
    }

    // ブロックを別途アクティブ
    void Activate( TbSint32 index , TbBool isInterpolated );

private:

    static const TbSint32 BLOCK_MAX = 32;

private:

    // オフセット初期位置設定
    void setInitOffsetPos(TbSint32 index, TbUint32 offsetBlock);

private:

    Type                                           m_Type;
    TbVector2                                      m_Direction;
    TbStaticArray<SideActionLinkBlock*,BLOCK_MAX>  m_BlockList;
    TbUint32                                       m_InitOffsetCount;
    TbUint32                                       m_BlockSize;

};

#endif
