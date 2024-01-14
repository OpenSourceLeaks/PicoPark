/*!
 * タイマー
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_JUDGE_BREAKOUT_H_
#define _INCLUDED_SIDE_ACTION_JUDGE_BREAKOUT_H_

#include "side_action_judge_base.h"
#include <crayon/actor/cr_action_actor.h>
#include "ui/ui_message_window.h"

class SideActionJudgeBreakout : public SideActionJudgeBase
{
public:
    
    typedef SideActionJudgeBreakout Self;
    typedef SideActionJudgeBase Super;

    struct RegistInfo
    {
        TbSint32 x;
        TbSint32 y;
        TbSint32 playerIndex;
    };

    enum Type
    {
        TYPE_BREAKOUT ,
        TYPE_TERRITORY
    };

public:

    // コンストラクタ
    SideActionJudgeBreakout( TbUint32 tileW , TbUint32 tileH );

    // デストラクタ
    virtual ~SideActionJudgeBreakout();

public:

    // 活動状態か
    TbBool IsActive() const {
        return m_Phase != PHASE_IDLE;
    }
    
    // タイプ設定
    void SetType( Type type ) {
        m_Type = type;
    }

public:

    // ロジック更新
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Phase
    {
        PHASE_IDLE ,
        PHASE_JUDGE = 10 ,
        PHASE_RESULT = 20 ,
    };

    class TerritoryMap
    {
    public:

        TerritoryMap()
            : m_Map(nullptr)
        {
        }

        ~TerritoryMap()
        {
            TB_SAFE_DELETE_ARRAY(m_Map);
        }

    public:

        // 初期化
        void Initialize( TbUint32 w, TbUint32 h )
        {
            TB_RESQUE(!m_Map);
            m_Size.width = w;
            m_Size.height = h;
            TbUint32 size = w*h;
            m_Map = new TbSint32[size];
            TbFill(m_Map,m_Map+(size),-1);
        }

        // サイズ取得
        const TbDimensionS32& GetSize() const {
            return m_Size;
        }

    public:

        // 設定
        void Set( TbSint32 x , TbSint32 y , TbSint32 value )
        {
            if( 0<=x && x<m_Size.width&& 0 <= y &&y<m_Size.height )
            {
                m_Map[y*m_Size.width+x] = value;
            }
        }

        // 取得
        TbSint32 Get( TbSint32 x , TbSint32 y ) const
        {
            if( TB_VERIFY(x<m_Size.width&&y<m_Size.height))
            {
                return m_Map[y*m_Size.width+x];
            }
            return -1;
        }

    private:
        TbDimensionS32  m_Size;
        TbSint32*       m_Map;
    };

    struct Result
    {
        Result() 
            : index(-1) 
            , playerIndex(-1)
            , count(0) 
        {}

        TbBool operator<( const Result& r ) const
        {
            return count < r.count;
        }

        TbSint32 index;
        TbSint32 playerIndex;
        TbSint32 count;
    };

private:

    // バック描画
    void onDrawBack( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // フロント描画
    void onDrawFront( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // フェーズ設定
    void setPhase( TbSint32 phase ){
        m_PhaseSec = 0.0f;
        m_Phase = phase;
    }

    // 領土更新
    void updateTerritory();

    // 領土取得
    TbUint32 getTerritory( TbUint32 playerIndex );

    // 領土削減
    void deleteTerrirory( TbUint32 playerIndex );

private:

    TbSint32                m_Phase;
    TbFloat32               m_PhaseSec;
    Type                    m_Type;
    CrActionActorSpriteHooker m_SpriteHookerBack;
    CrActionActorSpriteHooker m_SpriteHookerFront;
    TerritoryMap            m_TerritoryMap;

private:

    TbUint32                m_JudgeCount;
    UiMessageWindow         m_JudgeWindow;
    Result                  m_Result[CMN_PLAYER_MAX];

};

#endif
