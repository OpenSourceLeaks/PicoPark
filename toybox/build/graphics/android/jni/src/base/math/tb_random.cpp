/*!
 * @file
 * @brief 乱数関数郡の実装ファイル
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"
#include "base/math/tb_random.h"

namespace toybox
{

static const TbUint32 RANDOM_BUF_SIZE = 56;
static TbUint32 s_RandomIndex;
static TbUint32 s_RandomBuf[RANDOM_BUF_SIZE];

/*!
 * 乱数初期化
 */
void     TbInitRandom(TbUint32 seed)
{
    TbUint32 tmpSeed = seed;

    s_RandomBuf[ RANDOM_BUF_SIZE - 1 ] = seed;

    TbUint32 k = 1;
    for(TbSint32 i = 1; i < RANDOM_BUF_SIZE -1; i++)
    {
        TbSint32 ii = (21 * i) % (RANDOM_BUF_SIZE - 1);
        s_RandomBuf[ ii ] = k;
        k = tmpSeed - k;
        tmpSeed = s_RandomBuf[ ii ];
    }
    TbShuffleRandom();
    TbShuffleRandom();
    TbShuffleRandom();
    s_RandomIndex = 55;
}

/*!
 * 乱数シャッフル
 */
void     TbShuffleRandom()
{
    for(TbSint32 i = 1; i <= 24; i++)    s_RandomBuf[i] -= s_RandomBuf[i + 31];
    for(TbSint32 i = 25; i < RANDOM_BUF_SIZE; i++)   s_RandomBuf[i] -= s_RandomBuf[i - 24];
}

/*!
 * 乱数取得
 */
TbUint32 TbRandom()
{
    if (++s_RandomIndex >= RANDOM_BUF_SIZE )
    {
        TbShuffleRandom();
        s_RandomIndex = 1;
    }
    return    s_RandomBuf[ s_RandomIndex ];
}

/*!
 * 最大値指定で乱数取得
 */
TbUint32 TbRandom( TbUint32 max)
{
    return (max == 0) ? 0 : TbRandom() % (max+1);
}

/*!
 * float32 乱数取得 
 * @author Miyake Shunsuke
 */
TbFloat32  TbRandomFloat()
{
    if (++s_RandomIndex >= RANDOM_BUF_SIZE )
    {
        TbShuffleRandom();
        s_RandomIndex = 1;
    }
    return static_cast<TbFloat32>(s_RandomBuf[ s_RandomIndex ] * 2.3283067140944855325800815320112e-10f);
}

TbFloat32   TbRandomFloatMinMax( TbFloat32 minVal , TbFloat32 maxVal )
{
    TB_RESQUE_RET(minVal<=maxVal,minVal);
    return (TbRandomFloat() * (maxVal-minVal)) + minVal;
}

}
