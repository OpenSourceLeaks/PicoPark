/*!
 * �o�g���Ǘ��N���X
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

    // �R���X�g���N�^
    BattleManager()
        : m_Round(0)
        , m_LatestWinner(0)
    {}

    // �f�X�g���N�^
    ~BattleManager()
    {}

public:

    // �N���A
    void Clear() {
        TbFill(m_Result,PlayerResult());
        m_Round = 0;
        m_LatestWinner = 0;
    }

    // ���U���g�ݒ�
    void SetResult(TbUint32 winner)
    {
        if(winner < TB_ARRAY_COUNT_OF(m_Result))
        {
            ++m_Result[winner].winCount;
            m_LatestWinner = winner;
        }
        ++m_Round;
    }

    // ���U���g�擾
    PlayerResult GetResult(TbUint32 player) {
        if (player < TB_ARRAY_COUNT_OF(m_Result)) {
            return m_Result[player];
        }
        return PlayerResult();
    }

    // �ŐV�̏�����
    TbUint32 GetLatestWinner() const {
        return m_LatestWinner;
    }

    // �����D���Ҏ擾
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

    // ���E���h�擾
    TbUint32     GetRound() const {
        return m_Round;
    }

private:

    // �v���C���[�ʌ���
    PlayerResult m_Result[CMN_PLAYER_MAX];
    TbUint32     m_Round;
    TbUint32     m_LatestWinner;

};

#endif
