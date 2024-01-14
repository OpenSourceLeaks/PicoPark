/*!
 * バトル管理クラス
 */

#ifndef _INCLUDED_BATTLE_MANAGER_H_
#define _INCLUDED_BATTLE_MANAGER_H_

class BattleManager : public TbSingletonRef<BattleManager>
{
public:

    static const TbUint32 WIN_MAX = 3;
    static const TbUint32 RESULT_DRAW = 0xFFFFFFFF;

    struct PlayerResult
    {
        PlayerResult()
            : winCount(0)
        {}
        TbUint32 winCount;
    };

public:

    // コンストラクタ
    BattleManager()
        : m_Round(0)
        , m_LatestWinner(0)
    {}

    // デストラクタ
    ~BattleManager()
    {}

public:

    // クリア
    void Clear() {
        TbFill(m_Result,PlayerResult());
        m_Round = 0;
        m_LatestWinner = 0;
    }

    // リザルト設定
    void SetResult(TbUint32 winner)
    {
        if(winner < TB_ARRAY_COUNT_OF(m_Result))
        {
            ++m_Result[winner].winCount;
            m_LatestWinner = winner;
        }
        ++m_Round;
    }

    // リザルト取得
    PlayerResult GetResult(TbUint32 player) {
        if (player < TB_ARRAY_COUNT_OF(m_Result)) {
            return m_Result[player];
        }
        return PlayerResult();
    }

    // 最新の勝利者
    TbUint32 GetLatestWinner() const {
        return m_LatestWinner;
    }

    // 総合優勝者取得
    TbBool GetTotalWinner(TbUint32* player)
    {
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Result); ++i)
        {
            if (WIN_MAX <= m_Result[i].winCount)
            {
                *player = i;
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

    // ラウンド取得
    TbUint32     GetRound() const {
        return m_Round;
    }

private:

    // プレイヤー別結果
    PlayerResult m_Result[CMN_PLAYER_MAX];
    TbUint32     m_Round;
    TbUint32     m_LatestWinner;

};

#endif
