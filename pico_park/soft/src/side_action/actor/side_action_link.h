/*!
 * リンク
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_LINK_H_
#define _INCLUDED_SIDE_ACTION_LINK_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionLink;

class SideActionLinkBlock : public CrActionActor
{
    typedef CrActionActor Super;
public:

    enum State
    {
        STATE_BEGIN,
        STATE_ACTIVE,
        STATE_RETURN,
        STATE_END,
    };

public:
    
    // コンストラクタ
    SideActionLinkBlock( TbUint32 uvIndex , TbUint32 w , TbUint32 h);
    // デストラクタ
    ~SideActionLinkBlock();

public:

    // 親設定
    void SetLink(SideActionLink* link) {
        m_Link = link;
    }

    // 目標位置設定
    void SetTargetPos( const TbVector2& pos );

    // 状態取得
    State GetState() const {
        return m_State;
    }

    // 移動
    void Activate( TbBool isInterpolated , TbBool isEnableResetDelay ) {
        if( STATE_BEGIN == m_State || STATE_RETURN == m_State ) {
            if ( isEnableResetDelay ) {
                m_DelayCounter = 0U;
            }
            m_State = STATE_ACTIVE;
            m_BitArray.SetBit(FLAG_INTERPOLATE,isInterpolated);
        }
    }

    void Deactivate(TbBool isInterpolated, TbBool isEnableResetDelay)
    {
        if (STATE_ACTIVE == m_State || STATE_END == m_State) {
            if ( isEnableResetDelay ) {
                m_DelayCounter = 0U;
            }
            m_State = STATE_RETURN;
            m_BitArray.SetBit(FLAG_INTERPOLATE, isInterpolated);
        }
    }

    // 遅延スキップ
    void SetSkipDelay() {
        m_DelayCounter = TB_UINT32_MAX;
    }

    // 先頭かどうか
    void SetFront()
    {
        m_BitArray.SetBit(FLAG_FRONT);
    }

    // 先頭かどうか
    TbBool IsFront() const {
        return m_BitArray.Test(FLAG_FRONT);
    }

    // 押し出し可能か
    void SetEnablePush()
    {
        m_BitArray.SetBit(FLAG_ENABLE_PUSH);
    }

    // 押し出し可能か
    TbBool IsEnablePush() const {
        return m_BitArray.Test(FLAG_ENABLE_PUSH);
    }

    // 前フレームのターゲット座標に行く
    void GoPreTargetPos() {
        m_BitArray.SetBit(FLAG_GO_PRE_TARGET_POS);
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // 派生クラス用座標押し戻し後処理
    virtual void OnPushedBack(const toybox::TbVector2& pos, const toybox::TbVector2& pushBackVec);

private:

    enum Flag
    {
        FLAG_INTERPOLATE ,
        FLAG_FRONT , // 先頭
        FLAG_ENABLE_PUSH ,
        FLAG_GO_PRE_TARGET_POS ,
        FLAG_MAX
    };

private:

    TbBool move( const TbVector2& dest , TbFloat32 speedScale );

private:

    TbBitArray32    m_BitArray;
    State           m_State;
    TbVector2       m_TargetPos;
    TbVector2       m_PreTargetPos;
    TbUint32        m_DelayCounter;
    SideActionLink* m_Link;

};

class SideActionLink : public CrActionActor
{
public:
    
    typedef SideActionLink Self;
    typedef CrActionActor Super;

public:

    enum Type
    {
        TYPE_OUT , 
        TYPE_IN
    };

public:

    // コンストラクタ
    SideActionLink( Type type , 
                    TbUint32 blockCount , 
                    TbUint32 blockSize , 
                    const TbVector2& dir ,
                    TbBool enablePush );

    // デストラクタ
    virtual ~SideActionLink();

public:

    // シーンに入った
    virtual void OnEnter( void* userData );
    
    // シーンから出た
    virtual void OnExit();

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime);

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

    // ブロックを押し戻された
    void OnPushedBackBlock();

public:

    // 初期オフセット数
    void SetInitOffsetCount(TbUint32 count) {
        m_InitOffsetCount = count;
    }

    // 鍵によって開く
    void SetEnableOpenByKey(TbBool isEnable) {
        m_IsEnableOpenByKey = isEnable;
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
    TbBool                                         m_IsEnableOpenByKey;

};

#endif
