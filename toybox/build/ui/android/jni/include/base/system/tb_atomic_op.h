/*! 
 * @file
 * @brief atomic
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_ATOMIC_OP_H_
#define _INCLUDED_TB_ATOMIC_OP_H_

namespace toybox
{

// アトミックに+1する
void TbAtomicInc32(volatile TbUint32* target);

// アトミックに-1する
void TbAtomicDec32(volatile TbUint32* target);

// アトミックに+1する
void TbAtomicAdd32(volatile TbUint32* target,TbUint32 val);

// アトミックにメモリから読み込む
TbUint32 TbAtomicRead32(volatile const TbUint32* target);

// アトミックにメモリから読み込む(この処理よりも後の処理が先に行われないように)
TbUint32 TbAtomicReadAcquire32(volatile TbUint32* target);

// アトミックにメモリに書き込む
void TbAtomicWrite32(volatile TbUint32* target,TbUint32 val);

// アトミックにメモリに書き込む(この処理よりも先の処理がアウトオブオーダーで後でされないように)
void TbAtomicWriteRelease32(volatile TbUint32* target,TbUint32 val);
    
// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
TbBool TbAtomicCompareAndSwap32(volatile TbUint32* target,
                              TbUint32 val,
                              TbUint32 cmp);

// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
TbBool TbAtomicCompareAndSwapPointer32( volatile void** target,
                                         void* val,
                                         void* cmp);

}

#include "./depend/tb_atomic_op_depend.h"

#endif
