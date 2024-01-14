/*! 
 * @file
 * @brief atomic
 * @author teco
 */

#ifndef _INCLUDED_TB_ATOMIC_OP_GCC_H_
#define _INCLUDED_TB_ATOMIC_OP_GCC_H_

namespace toybox
{

/*! 
 * アトミックに+1する
 * @author teco
 */
TB_FORCE_INLINE void TbAtomicInc32(volatile TbUint32* target)
{
    __atomic_add_fetch(target,1, __ATOMIC_SEQ_CST);
}

/*! 
 * アトミックに-1する
 * @author teco
 */
TB_FORCE_INLINE void TbAtomicDec32(volatile TbUint32* target)
{
    __atomic_sub_fetch(target, 1, __ATOMIC_SEQ_CST);
}

/*! 
 * アトミックに加算する
 * @author teco
 */
TB_FORCE_INLINE void TbAtomicAdd32(volatile TbUint32* target,TbUint32 val)
{
    __atomic_add_fetch(target, val, __ATOMIC_SEQ_CST);
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
    __atomic_store_n(target,val, __ATOMIC_SEQ_CST);
}

/*! 
 * アトミックにメモリに書き込む(この処理よりも先の処理がアウトオブオーダーで後でされないように)
 * @author Miyake Shunsuke
 */
TB_FORCE_INLINE void TbAtomicWriteRelease32(volatile TbUint32* target,TbUint32 val)
{

    __atomic_store_n(target,val, __ATOMIC_RELEASE);
}

// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
TB_FORCE_INLINE TbBool TbAtomicCompareAndSwap32(volatile TbUint32* target,
                                              TbUint32 val,
                                              TbUint32 cmp)
{
    return __atomic_compare_exchange_n(target,&cmp,val,false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

#if 0
// CASを行う(targetがcmpだったらtargetにvalを書き込む。targetの初期値を返す)
TB_FORCE_INLINE TbBool TbAtomicCompareAndSwapPointer32(volatile void** target,
                                                      void* val,
                                                      void* cmp)
{
    return __atomic_compare_exchange(*target,cmp,val, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}
#endif

}

#endif
