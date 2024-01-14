/*!
 * 固定サイズアロケータ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#include "tb_fwd.h"
#include "base/memory/tb_fixed_allocator.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */
TbFixedAllocator::TbFixedAllocator()
    : m_BitArray()
    , m_Buf(0)
    , m_BufSize(0)
    , m_BlockSize(0)
    , m_CurrentBlock(NULL)
{
}

/*!
 * デストラクタ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */
TbFixedAllocator::~TbFixedAllocator()
{
}
    
/*!
 * 初期化
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */
TbResult TbFixedAllocator::Initialize( void* buf , TbUint32 bufSize , TbUint32 blockSize )
{
    if( !buf || m_BitArray.Test(FLAG_INITIALIZE) || 0 == bufSize || 0 == blockSize ) {
        return TB_E_FAIL;
    }
    
    m_Buf = TB_ALIGN_ROUND_UP( reinterpret_cast<TbAddr>(buf) , 4 );
    m_BufSize = bufSize;
    m_BlockSize = TB_ALIGN_ROUND_UP( blockSize , 4 );

    // ブロック初期化
    m_BlockCount = m_BufSize / m_BlockSize;
    m_AvailableBlockCount = m_BlockCount;
    for( TbUint32 i = 0; i < m_BlockCount-1; ++i ){
        Block* block = getBlock(i);
        block->nextIndex = i+1;
    }
    Block* endBlock = getBlock(m_BlockCount-1);
    endBlock->nextIndex = -1;

    m_CurrentBlock = reinterpret_cast<Block*>(m_Buf);
    return TB_S_OK;
}

/*!
 * メモリ確保 
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */
void* TbFixedAllocator::Allocate( TbUint32 size )
{
    if( !TB_VERIFY( (size < m_BlockSize) && m_CurrentBlock) ){
        // ブロックサイズ以上のものを要求している
        return NULL;
    }
    void* result = m_CurrentBlock;
    if( m_CurrentBlock->nextIndex >= 0 ){
        m_CurrentBlock = getBlock(m_CurrentBlock->nextIndex);
    }else{
        m_CurrentBlock = NULL;
    }
    --m_AvailableBlockCount;
    return result;
}

/*!
* メモリ確保
* @author Miyake_Shunsuke
*/
void* TbFixedAllocator::Allocate(TbUint32 size,TbUint32 align)
{
    TbUint32 alignedSize = TB_ALIGN_ROUND_UP(size,align);
    return Allocate(alignedSize);
}

/*!
 * 開放
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */
void TbFixedAllocator::Deallocate(void* ptr)
{
    TbAddr address = reinterpret_cast<TbAddr>(ptr);
    if( !TB_VERIFY( ptr && 
                   (m_Buf < address) && 
                   (address < m_Buf+m_BufSize) && 
                   0==(address-m_Buf)%m_BlockSize ) )
    {
        return;
    }
    Block* block = reinterpret_cast<Block*>(ptr);
    block->nextIndex = (reinterpret_cast<TbAddr>(m_CurrentBlock)-m_Buf)/m_BlockSize;
    ++m_AvailableBlockCount;
}

}
