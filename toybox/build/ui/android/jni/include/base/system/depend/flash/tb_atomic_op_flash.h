/*! 
 * @file
 * @brief atomic
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_ATOMIC_OP_FLASH_H_
#define _INCLUDED_TB_ATOMIC_OP_FLASH_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <machine/atomic.h>

namespace toybox
{

/*! 
 * アトミックに+1する
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicInc32(volatile TbUint32* target)
{
    atomic_add_int(reinterpret_cast<volatile u_int*>(target),1);
}

/*! 
 * アトミックに-1する
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicDec32(volatile TbUint32* target)
{
    atomic_subtract_int(reinterpret_cast<volatile u_int*>(target),1);
}

/*! 
 * アトミックに加算する
 * @author Miyake Shunsuke
 */
#if 0
TB_FORCE_INLINE void TbAtomicAdd32(volatile TbUint32* target,TbUint32 val)
{
    AQUA_ASSERT(!"Unsupported");
}
#endif

/*! 
 * アトミックにメモリから読み込む
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE TbUint32 TbAtomicRead32(volatile const TbUint32* target)
{
    return *target;
}

/*! 
 * アトミックにメモリから読み込む(この処理よりも後の処理が先に行われないように)
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE TbUint32 TbAtomicReadAcquire32(volatile const TbUint32* target)
{
    TbUint32 result = *target;
//    _ReadWriteBarrier();
    return result;
}

/*! 
 * アトミックにメモリに書き込む
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicWrite32(volatile TbUint32* target,TbUint32 val)
{
    atomic_set_32(reinterpret_cast<volatile u_int*>(target),val);
}

/*! 
 * アトミックにメモリに書き込む(この処理よりも先の処理がアウトオブオーダーで後でされないように)
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicWriteRelease32(volatile TbUint32* target,TbUint32 val)
{
    atomic_set_rel_32(reinterpret_cast<volatile u_int*>(target),val);
}

// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
TB_FORCE_INLINE TbBool TbAtomicCompareAndSwap32(volatile TbUint32* target,
                                              TbUint32 val,
                                              TbUint32 cmp)
{
    return atomic_cmpset_32(reinterpret_cast<volatile u_int*>(target),cmp,val);
}

// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
#if 0
typedef void* PVOID2;
TB_FORCE_INLINE TbBool TbAtomicCompareAndSwapPointer32(volatile PVOID2* target,
                                                     void* val,
                                                     void* cmp)
{
    return ::InterlockedCompareExchangePointer(target,val,cmp) == cmp;
}
#endif

}

#endif
