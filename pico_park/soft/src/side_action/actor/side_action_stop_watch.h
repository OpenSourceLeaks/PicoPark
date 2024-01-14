/*!
 * ストップウォッチ
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_STOP_WATCH_H_
#define _INCLUDED_SIDE_ACTION_STOP_WATCH_H_

#include <crayon/actor/cr_action_actor.h>

class SideActionStopWatch : public CrActionActor
{

public:
    
    typedef SideActionStopWatch Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionStopWatch();

    // デストラクタ
    virtual ~SideActionStopWatch();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        if (userData.AsSint32(0) > 0) {
            SetInitSec(userData.AsFloat32(0));
        }
        if (userData.AsString(1)) {
            SetParentName(userData.AsString(1));
        }
        if( 2 <= userData.GetCount() ){ 
            m_PlayerIndex = userData.AsSint32(2);
        }
        if (3 <= userData.GetCount()) {
            if (userData.AsSint32(3)) {
                m_BitArray.SetBit(FLAG_INVISIBLE);
            }
        }
    }

public:

    // 時間を取得
    TbFloat32 GetSec() const {
        return m_Sec;
    }

    // 初期時間設定
    void SetInitSec( TbFloat32 sec ){
        m_InitSec = sec;
        m_Sec = sec;
    }

    // 親の名前設定
    void SetParentName( const char* name ) {
        m_ParentName = name;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_PAUSE ,
        FLAG_DEAD ,
        FLAG_INVISIBLE ,
        FLAG_MAX
    };

private:

    // 前景描画
    void onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // 時間表示するか
    TbBool isVisibleTime() const;

private:

    TbFloat32               m_Sec;
    TbFloat32               m_InitSec;
    TbFloat32               m_TotalSec;
    TbBitArray32            m_BitArray;
    CrActionActorSpriteHooker m_SpriteHooker;
    TbStaticString64        m_ParentName;
    TbUint32                m_PlayerIndex;
    TbColor                 m_FontColor;

};

#endif
