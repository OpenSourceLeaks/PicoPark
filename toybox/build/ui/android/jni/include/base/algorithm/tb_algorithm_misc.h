/*!
 * アルゴリズム集
 * @author Miyake_Shunsuke
 * @since 2012.04.22
 */

#ifndef _INCLUDED_TB_ALGORITHM_MISC_H_
#define _INCLUDED_TB_ALGORITHM_MISC_H_

#include <base/util/tb_template_util.h>
#include <base/util/tb_type_traits.h>
#include <base/math/tb_random.h>

namespace toybox
{

//! for_eachテンプレート関数
template <typename T,typename Func>
inline Func TbForEach(T begin,T end,Func func){
    for(; begin != end; ++begin){
        func(*begin);
    }
    return func;
}

//! イテレーションをforで回す
#define TbForIterator( ite , container ) for( auto ite = (container).Begin() , endIte = (container).End(); ite != endIte; ++ite )

//! 一定範囲(0～max)をループするようにインクリメント
template<typename T>
T TbIncrementLoop(T val, T max) {
    T result = ((val+1)%max);
    return result;
}

//! 一定範囲(0～max)をループするようにデクリメント
template<typename T>
T TbDecrementLoop(T val, T max) {
    T result = val;
    if (0 == val) {
        result = max-1;
    }else {
        --result;
    }
    return result;
}

//! コピーテンプレート関数
template<class SrcT,class DestT> inline
DestT TbCopy(SrcT begin, SrcT end,DestT dest )
{
    for (; begin != end; ++dest, ++begin){
        *dest = *begin;
    }
    return (dest);
}

//! 配列コピーテンプレート関数
template<class T,size_t N> inline
void TbCopyArray(const T (&src)[N] , T (&dst)[N] )
{
    for (size_t i = 0; i < N; ++i ){
        dst[i] = src[i];
    }
}

//! 配列コピーテンプレート関数
template<class T, size_t N, size_t M> inline
void TbCopyArray2(const T(&src)[N][M], T(&dst)[N][M])
{
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            dst[i][j] = src[i][j];
        }
    }
}
    
//! Fillテンプレート関数
template<class SrcT,class DestT> inline
void TbFill( SrcT begin , SrcT end , DestT dest)
{
    for (; begin != end; ++begin){
        *begin = dest;
    }
}

//! Fillテンプレート関数(配列用)
template<class T,TbSizeT N>
void TbFill( T (&ary)[N] , T dest )
{
    TbFill(ary,ary+N,dest);
}

//! Fillテンプレート関数(配列用)
template<class T,TbSizeT N>
void TbFillZero( T (&ary)[N] )
{
    TbFill(ary,ary+N,static_cast<T>(0));
}

//! Fillテンプレート関数(配列用)
template<class T, TbSizeT N>
void TbFillNull(T(&ary)[N])
{
    TbFill(ary, ary + N, nullptr);
}

//! Transformテンプレート関数
template <typename T,typename Func>
void TbTransform( T begin , T end , Func func)
{
    for(; begin != end; ++begin){
        *begin = func(*begin);
    }
}

//! 配列コピーテンプレート関数
template<class T, size_t N> inline
TbBool TbEqualsArray(const T(&src)[N], const T(&dst)[N])
{
    for (size_t i = 0; i < N; ++i) {
        if (dst[i] != src[i]) {
            return TB_FALSE;
        }
    }
    return TB_TRUE;
}


// 未初期化メモリへのコピーテンプレート関数(スカラー型)
template <typename T,typename Allocator>
T* TbUninitializedMove( T* first , T* last , T* dest , Allocator& allocator , const TbTrueType& isScalar ) {
    TbUint32 count = static_cast<TbUint32>(last-first);
    return static_cast<T*>(TbMemMove(&*dest,&*first,count * sizeof(*first))) + count;
}

// 未初期化メモリへのコピーテンプレート関数(非スカラー型)
template <typename T,typename Allocator>
T* TbUninitializedMove( T* first , T* last , T* dest , Allocator& allocator , const TbFalseType& isUnscalar )
{
    for(; first != last; ++dest , ++first ) {
        allocator.Construct(dest,*first);
    }
    return dest;
}

// 破棄(スカラー型)
template<typename T>
inline void TbDestroyImpl( T* ptr ,  const TbTrueType& isScalar ) 
{
    // 何もしない
}

// 破棄(スカラー型)
template<typename T>
inline void TbDestroyImpl( T* ptr , const TbFalseType& isUnscalar ) 
{
    // デストラクタを呼ぶ
    ptr->~T();
}

// 破棄
template<typename T>
inline void TbDestroy( T* ptr ) 
{
    TbDestroyImpl<T>(ptr,TbBoolType<TbIsScalar<T>::VALUE>());
}

// Swap
template<typename T>
inline void TbSwap( T& target1 , T& target2 )
{
    T data = target1;
    target1 = target2;
    target2 = data;
}

// iteratorのswap関数
template<typename T>
inline void TbSwapIterator(T a, T b) {
    TbSwap(*a, *b);
}

// シャッフル関数
template<typename T>
inline void TbShuffle(T begin, T end) {
    if (begin == end) {
        return;
    }
    for (auto it = begin + 1; it != end; ++it) {
        TbSizeT d = TbDistance(begin, it);
        TbSwapIterator(it, begin + TbRandom(d));
    }
}

template <typename T>
inline TbSizeT TbDistance( const T& begin , const T& end )
{
    TbSizeT result = end - begin;
    return result;
}

template <typename T1,typename T2>
inline void TbCastCopy( T1& dst , const T2& src )
{
    dst = static_cast<T1>(src);
}

}

#endif
