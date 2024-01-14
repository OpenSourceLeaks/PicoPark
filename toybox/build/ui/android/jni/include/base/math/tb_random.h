/*!
 * @file
 * @brief 乱数生成関数郡のヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_RANDOM_H_
#define _INCLUDED_TB_RANDOM_H_

namespace toybox
{

//乱数初期化
void     TbInitRandom(TbUint32 seed);

//乱数シャッフル
void     TbShuffleRandom();

//乱数取得
TbUint32    TbRandom();
TbUint32    TbRandom( TbUint32 max);
TbFloat32   TbRandomFloat();
TbFloat32   TbRandomFloatMinMax( TbFloat32 minVal , TbFloat32 maxVal );

}

#endif