/*!
 * @file
 * @brief メモリ関連ヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_MEMORY_H_
#define _INCLUDED_TB_MEMORY_H_

namespace toybox
{

/*!
 * メモリアロケーション
 * 16バイトアライメントされます
 */
void* TbMallocKernel(TbUint32 size);

/*!
 * メモリの解放
 */
void TbFreeKernel(void* ptr);

/*!
* メモリアロケーション
*/
void* TbMallocDefaultHeap(TbUint32 size , TbUint32 align);

/*!
* メモリの解放
*/
void TbFreeDefaultHeap(void* ptr);

#define TB_SAFE_FREE_DEFAULT_HEAP(p)            if(p){TbFreeDefaultHeap(p);(p)=NULL;}

/*!
 * メモリセット
 * @param dst  転送先
 * @param data セットするデータ
 * @param size サイズ(バイト)
 */
inline void TbMemSet( void *dst , const TbUint8 data, const TbSizeT size )
{
    memset(dst,data,size);
}

/*!
 * メモリーコピー
 * @param dst 転送先
 * @param src 転送元
 * @param size コピーサイズ(バイト)
 */
inline void TbMemCopy( void *dst , const void *src , const TbSizeT size)
{
    memcpy(dst,src,size);
}

/*!
 * メモリー移動
 * @param dst 転送先
 * @param src 転送元
 * @param size コピーサイズ(バイト)
 */
inline void* TbMemMove( void *dst , const void *src , const TbSizeT size)
{
    return memmove(dst,src,size);
}

/*!
 * メモリークリア
 * @param dst 転送先
 * @param size クリアサイズ(バイト)
 */
inline void TbMemClear(void *dst , const TbSizeT size)
{
    memset(dst,0,size);
}

/*!
 * メモリークリア
 * @param dst 転送先
 */
template <typename T>
inline void TbMemClear( T& dst )
{
    TbMemClear(&dst,sizeof(dst));
}

/*!
 * メモリ比較
 */
inline TbSint32 TbMemCmp(const void *dst , const void* src , TbSizeT size)
{
    return memcmp(dst,src,size);
}


}

#endif