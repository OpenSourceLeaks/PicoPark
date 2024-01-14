/*! 
 * @file
 * @brief atomic
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_ATOMIC_OP_WIIU_H_
#define _INCLUDED_TB_ATOMIC_OP_WIIU_H_

namespace toybox
{

/*! 
 * アトミックに+1する
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicInc32(volatile TbUint32* target)
{
    OSIncAtomic(reinterpret_cast<volatile OSAtomicVar*>(target));
}

/*! 
 * アトミックに-1する
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicDec32(volatile TbUint32* target)
{
    OSDecAtomic(reinterpret_cast<volatile OSAtomicVar*>(target));
}

/*! 
 * アトミックに加算する
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicAdd32(volatile TbUint32* target,TbUint32 val)
{
    OSAddAtomic(reinterpret_cast<volatile OSAtomicVar*>(target),val);
}

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
    OSSwapAtomic(reinterpret_cast<volatile OSAtomicVar*>(target),val);
}

/*! 
 * アトミックにメモリに書き込む(この処理よりも先の処理がアウトオブオーダーで後でされないように)
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicWriteRelease32(volatile TbUint32* target,TbUint32 val)
{
    OSSwapAtomic(reinterpret_cast<volatile OSAtomicVar*>(target),val);
}

// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
TB_FORCE_INLINE TbBool TbAtomicCompareAndSwap32(volatile TbUint32* target,
                                              TbUint32 val,
                                              TbUint32 cmp)
{
    return OSCompareAndSwapAtomic(reinterpret_cast<volatile OSAtomicVar*>(target),cmp,val);
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
