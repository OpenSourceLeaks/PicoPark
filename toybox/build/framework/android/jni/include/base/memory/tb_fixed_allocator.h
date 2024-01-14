/*!
 * 固定長サイズ用アロケータ
 * @author Miyake_Shunsuke
 * @since 2012.05.23
 */

#ifndef _INCLUDED_TB_FIXED_ALLOCATOR_H_
#define _INCLUDED_TB_FIXED_ALLOCATOR_H_

#include <base/memory/tb_allocator.h>

namespace toybox
{

class TbFixedAllocator : public TbAllocator
{
public:

    // コンストラクタ
    TbFixedAllocator();

    // デストラクタ
    ~TbFixedAllocator();

public:
    
    // 初期化
    TbResult Initialize( void* buf , TbUint32 bufSize , TbUint32 blockSize );

    //メモリ確保
    virtual void* Allocate( TbUint32 size );

    //メモリ確保
    virtual void* Allocate(TbUint32 size,TbUint32 align);

    //開放
    virtual void Deallocate(void* ptr);

    //先頭アドレス取得
    TB_INLINE TbAddr GetStartAddress() const { 
        return m_Buf;
    }

    //末端アドレス取得
    TB_INLINE TbAddr GetEndAddress() const { 
        return m_Buf + m_BufSize;
    }
    
private:

    enum Flag
    {
        FLAG_INITIALIZE , 
        FLAG_MAX , 
    };

    struct Block
    {
        TbSint32 nextIndex; // 次のインデックス
    };

private:
    
    // ブロック取得
    TB_INLINE Block* getBlock( TbUint32 index ) {
        return reinterpret_cast<Block*>(m_Buf + (m_BlockSize*index) );
    }
    
private:
    
    TbBitArray32 m_BitArray;            // ビット配列
    TbAddr       m_Buf;                 // 先頭アドレス
    TbUint32     m_BufSize;             // ブロック
    TbUint32     m_BlockSize;           // ブロックサイズ
    Block*       m_CurrentBlock;        // 次取得可能なブロック
    TbUint32     m_BlockCount;          // ブロック数
    TbUint32     m_AvailableBlockCount; // 利用可能なブロック数
    
};

}

#endif
