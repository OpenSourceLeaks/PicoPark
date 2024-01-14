/*!
 * @file
 * @brief ヒープクラスの実装ファイル
 * @author teco
 */

#include "tb_fwd.h"
#include "base/memory/tb_heap.h"
#include "base/system/tb_assert.h"
#include "base/system/tb_debug.h"
#include "base/system/tb_stack_trace.h"
#include "base/io/tb_print.h"
#include "base/thread/tb_scoped_lock.h"

namespace
{
    const static TbUint8 HEAP_FILL_BYTE = (0xcd);
};

#define TB_HEAP_IS_ENABLE_GET_CALL_ADDREES (1 && TB_HEAP_HEADER_DEBUG_DEF)

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbHeap::TbHeap()
    : m_BitArray()
    , m_Align(0)
    , m_StartAddress(0)
    , m_EndAddress(0)
    , m_Size(0)
    , m_FreeSize(0)
    , m_UseBlockCount(0U)
    , m_ForwardFreeRoot(NULL)
    , m_ReverseFreeRoot(NULL)
    , m_GlobalRoot(NULL)
    , m_BreakPointFrom(0)
    , m_BreakPointMalloc(0)
    , m_BreakPointFreeHeader(0)
{
    m_Name[0] = '\0';
}

/*!
 * デストラクタ
 * @author teco
 */
TbHeap::~TbHeap()
{
    Finalize();
}

/*!
 * セットアップ
 * 呼べるのは一回だけ
 * @param id 識別子
 * @param size サイズ
 * @param align アライメント
 * @author teco
 */
TbResult TbHeap::Initialize(void *address,TbUint32 size,TbUint32 align)
{
    if( m_BitArray.Test(FLAG_INITIALIZE) ){
        return TB_E_FAIL;
    }
    if( address){
        m_Align = align;
        if(m_Align < sizeof(Header)){
            m_Align = sizeof(Header);
        }
        m_StartAddress = TB_ALIGN_ROUND_UP_64( reinterpret_cast<TbAddr>(address) , m_Align );
        m_EndAddress = TB_ALIGN_ROUND_DOWN_64((reinterpret_cast<TbAddr>(address) + size) , m_Align );
        m_Size = m_EndAddress - m_StartAddress;
        m_FreeSize = m_Size;
        m_ForwardFreeRoot = reinterpret_cast<TbHeap::Header*>(m_StartAddress);
        TbMemClear(m_ForwardFreeRoot,sizeof(TbHeap::Header));
        m_ForwardFreeRoot->size = m_Size;
        m_ReverseFreeRoot = m_ForwardFreeRoot;
        m_GlobalRoot = m_ForwardFreeRoot;
        m_BitArray.SetBit(FLAG_INITIALIZE);
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * 後始末
 */
void TbHeap::Finalize()
{
    if( !m_BitArray.Test(FLAG_INITIALIZE) ){
        return;
    }
    if( m_UseBlockCount > 0 ) {
        DumpMallocList();
        TB_ASSERT(!"Memory Leak!!");
    }
    m_BitArray.ResetBit(FLAG_INITIALIZE);
}

/*!
 * メモリ確保
 * @param size 確保したいサイズ
 * @param align アライメント
 * @param type 確保する方向
 * @author teco
 */
void* TbHeap::Allocate(TbUint32 size,TbUint32 align,TbHeap::AllocType type)
{
    TbScopedLock lock(m_Mutex);
    TbHeap::Header* header = (type == ALLOC_TYPE_FORWARD) ? m_ForwardFreeRoot : m_ReverseFreeRoot;
    if(!header){
        return NULL;
    }

    TbAddr headerAddress = 0;
    TbUint32 headerSize = sizeof(TbHeap::Header);
    //アライメントされたサイズ
    TbUint32 alignSize = TB_ALIGN_ROUND_UP(size,align);

    while( header )
    {
        headerAddress = reinterpret_cast<TbAddr>(header);

        //アライメント余剰分計算
        TbUint32 surplus = (headerAddress + headerSize) & (align-1);
        if(surplus > 0){
            surplus = align - surplus;
        }
        
        //ヘッダー含めて必要なサイズ
        TbUint32 needSize = headerSize + surplus + alignSize;
        
        //サイズが分割できる余裕がなければそのままフリーリストから除去
        if( needSize <= header->size && header->size <= needSize + headerSize + surplus)
        {
            //フリーリストから削除
            deleteFreeList( header );
            header->flag |= TbHeap::Header::FLAG_USED;

            //サイズを2箇所に埋め込み
            header->headerSize = static_cast<TbUint16>(headerSize + surplus);
            TbUint16* sizePtr = reinterpret_cast<TbUint16*>(headerAddress + headerSize + surplus)-1;
            *sizePtr = header->headerSize;

            //初期化して返す
            TbUint8* userPtr = reinterpret_cast<TbUint8*>(getUserAddr( header ));
            TbMemSet(userPtr,HEAP_FILL_BYTE,alignSize);

#if TB_HEAP_IS_ENABLE_GET_CALL_ADDREES // デバッグコード

            //呼び出し元アドレス保存
//            TbGetCallFrom(header->from,4);
            
            //ブレークポイント
            if(m_BreakPointFrom && header->from[0] == m_BreakPointFrom){
                TbBreak();
            }
            if(m_BreakPointMalloc && userPtr == reinterpret_cast<TbUint8*>(m_BreakPointMalloc)){
                TbBreak();
            }

#endif

            //フリーサイズ現象
            m_FreeSize -= header->size;
            ++m_UseBlockCount;
            return userPtr;    
        }
        //分割する余裕があれば（次のヘッダーサイズ分含む）分割する
        else if(header->size > ( needSize + headerSize + surplus ))
        {
            TbHeap::Header* allocHeader = (type == ALLOC_TYPE_FORWARD) ? dividHeapForward(header,needSize,surplus) : dividHeapReverse(header,needSize,surplus);
            allocHeader->flag |= TbHeap::Header::FLAG_USED;

            TbUint8* userPtr = reinterpret_cast<TbUint8*>(getUserAddr( allocHeader ));
            TbMemSet(userPtr,HEAP_FILL_BYTE,alignSize);

#if TB_HEAP_HEADER_DEBUG_DEF // デバッグコード
            
            //コールスタック保存
            TbGetCallAddress(allocHeader->from,TB_ARRAY_COUNT_OF(allocHeader->from),6);
            if(type == ALLOC_TYPE_FORWARD){
                TbMemCopy(allocHeader->globalNext->from,allocHeader->from,sizeof(allocHeader->from));
            }else{
                TbMemCopy(allocHeader->globalPrev->from,allocHeader->from,sizeof(allocHeader->from));
            }
            if(m_BreakPointFrom && header->from[0] == m_BreakPointFrom){
                TbBreak();
            }
            if(m_BreakPointMalloc && userPtr == reinterpret_cast<TbUint8*>(m_BreakPointMalloc)){
                TbBreak();
            }

#endif

            m_FreeSize -= allocHeader->size;
            ++m_UseBlockCount;
            return userPtr;
        }
        header = (type == ALLOC_TYPE_FORWARD) ? header->freeNext : header->freePrev;
    }

    TbPrintf("[Warning] heap is lack. needSize = %d" , headerSize + alignSize);
    DumpFreeList();
    TB_ASSERT(TB_FALSE);

    return NULL;
}

/*!
 * 前からヒープを分割
 * @param header 分割領域のヘッダー
 * @param needSize 確保に必要なサイズ
 * @param surplus 余剰サイズ
 * @return 確保領域のヘッダー
 * @author teco
 */
TbHeap::Header* TbHeap::dividHeapForward(TbHeap::Header* header,TbUint32 needSize,TbUint32 surplus)
{
    TbAddr   headerAddress = reinterpret_cast<TbAddr>(header);
    TbUint32 headerSize = sizeof(TbHeap::Header);

    // 分割後のヘッダー
    TbHeap::Header* divHeader = reinterpret_cast<TbHeap::Header*>(headerAddress + needSize);
    TbMemSet(divHeader,0x00,sizeof(TbHeap::Header));
    divHeader->size = header->size - needSize;

    // 領域確保後のサイズ変更
    header->headerSize = static_cast<TbUint16>(headerSize + surplus);
    TbUint16* sizePtr = reinterpret_cast<TbUint16*>(headerAddress + headerSize + surplus)-1;
    *sizePtr = header->headerSize;
    header->size = needSize;
    
    // グローバルリストに追加
    insertGlobalList(header,divHeader);
    
    // フリーリストに追加
    insertFreeListNext(header,divHeader);
    
    // フリーリストから削除
    deleteFreeList( header );
    return header;
}

/*!
 * ヒープ分割(後を利用する)
 * @param header 分割領域のヘッダー
 * @param needSize 確保に必要なサイズ
 * @param surplus 余剰サイズ
 * @return 確保領域のヘッダー
 * @author teco
 */
TbHeap::Header* TbHeap::dividHeapReverse(TbHeap::Header* header,TbUint32 needSize,TbUint32 surplus)
{
    TbAddr headerAddress = reinterpret_cast<TbAddr>(header);
    TbUint32 headerSize = sizeof(TbHeap::Header);

    TbHeap::Header* divHeader = reinterpret_cast<TbHeap::Header*>(headerAddress + header->size - needSize);
    TbMemSet(divHeader,0x00,sizeof(TbHeap::Header));
    divHeader->size = needSize;
    divHeader->headerSize = TbUint16(headerSize + surplus);
    TbUint16* sizePtr = reinterpret_cast<TbUint16*>(reinterpret_cast<TbAddr>(divHeader) + headerSize + surplus)-1;
    *sizePtr = divHeader->headerSize ;

    header->size -= needSize;

    // グローバルリストに追加
    insertGlobalList( header,divHeader );
    
    // フリーリストに追加
    insertFreeListNext( header,divHeader );
    
    // フリーリストから削除
    deleteFreeList( divHeader );

    return divHeader;
}

/*!
 * 開放
 * @param ptr 開放する領域
 * @author teco
 */
void TbHeap::Deallocate(void* ptr)
{
    if(!ptr){
        //TbPrintf();
        return;
    }
    TbScopedLock lock(m_Mutex);
    TB_ASSERT(m_UseBlockCount>0);

    TbHeap::Header* header = getHeader(ptr);
    //TbPrintf("[Free]freePrev=%p globalPrev=%p this=%p freeNext=%p globalNext=%p\n"
    //         ,header->freePrev,header->globalPrev,header,header->freeNext,header->globalNext);
    TbHeap::Header* next = header->globalNext;
    TbHeap::Header* prev = header->globalPrev;

    //フリーリストに連結されているはずがない
    if( !(header->flag & TbHeap::Header::FLAG_USED) )
    {
        TbPrintf("[HeaderInfo]freePrev=%p globalPrev=%p this=%p freeNext=%p globalNext=%p headerSize=%u flag=%u \n"
                 ,header->freePrev,header->globalPrev,header,header->freeNext,header->globalNext,header->headerSize,header->flag);
        // フリーリストをダンプ
        DumpFreeList();
        //グローバルリストをダンプ
        DumpGlobalList();
        TB_ASSERT(TB_FALSE);
        return;
    }

    --m_UseBlockCount;

    //後ろがフリーの場合連結させる
    if(next && !(next->flag & TbHeap::Header::FLAG_USED)){
        m_FreeSize += header->size;
        insertFreeListPrev(next,header);
        header->flag &= ~TbHeap::Header::FLAG_USED;
        header->size += next->size;
        deleteGlobalList(next);
        deleteFreeList(next);
        
        if(prev && !(prev->flag & TbHeap::Header::FLAG_USED))
        {
            prev->size += header->size;
            if( m_ReverseFreeRoot == header ){
                m_ReverseFreeRoot = prev;
            }
            deleteGlobalList(header);
            deleteFreeList(header);
            cleanHeader(header);
        }
        return;
    }
    //前がフリーの場合連結させる
    if(prev && !(prev->flag & TbHeap::Header::FLAG_USED))
    {
        m_FreeSize += header->size;
        prev->size += header->size;
        deleteGlobalList(header);
        cleanHeader(header);
        return;
    }

    //自分を独立してフリーリストに登録
    //使い切ってる場合はそのまま
    header->flag &= ~TbHeap::Header::FLAG_USED;
    if( !m_ForwardFreeRoot && !m_ReverseFreeRoot ){
        m_ForwardFreeRoot = header;
        m_ReverseFreeRoot = header;
        m_FreeSize += header->size;
        return;
    }

    //しかし、断片化はできる限り起こさないように！！
    TbHeap::Header* forward = m_ForwardFreeRoot;
    while( forward )
    {
        if( header < forward ){
            insertFreeListPrev(forward,header);
            break;
        }
        if( !forward->freeNext ){
            insertFreeListNext(forward,header);
            break;
        }
        forward = forward->freeNext;
    }
    m_FreeSize += header->size;
}

/*!
 * グローバルリストに追加(headerの後ろに)
 * @param header 挿入場所
 * @param insert 挿入するヘッダー
 * @author teco
 */
void TbHeap::insertGlobalList(TbHeap::Header* header,TbHeap::Header* insert)
{
    insert->globalNext = header->globalNext;
    if(header->globalNext){
        header->globalNext->globalPrev = insert;
    }
    header->globalNext = insert;
    insert->globalPrev = header;
}

/*!
 * グローバルリストから削除
 * @author teco
 */
void TbHeap::deleteGlobalList(TbHeap::Header* header)
{
    if(header->globalPrev){
        header->globalPrev->globalNext = header->globalNext;
    }
    if(header->globalNext){
        header->globalNext->globalPrev = header->globalPrev;
    }
    header->globalPrev = NULL;
    header->globalNext = NULL;
}

/*!
 * 昇順フリーリストに追加（headerの後ろに）
 * @param header 挿入場所
 * @param insert 挿入するヘッダー
 * @author teco
 */
void TbHeap::insertFreeListNext( TbHeap::Header* header, TbHeap::Header* insert)
{
#if TB_HEAP_HEADER_DEBUG_DEF    //デバッグコード
    if(m_BreakPointFreeHeader && insert == reinterpret_cast<TbHeap::Header*>(m_BreakPointFreeHeader)){
        TbBreak();
    }
#endif
    //フリーのルートだったら変更する
    if( m_ForwardFreeRoot > insert){
        m_ForwardFreeRoot = insert;
    }
    if( m_ReverseFreeRoot < insert){
        m_ReverseFreeRoot = insert;
    }

    insert->freeNext = header->freeNext;
    if( header->freeNext ){
        header->freeNext->freePrev = insert;
    }

    header->freeNext = insert;
    insert->freePrev = header;
}

/*!
 * 昇順フリーリストに追加 (headerの前に)
 * @param header 挿入場所
 * @param insert 挿入するヘッダー
 * @author teco
 */
void TbHeap::insertFreeListPrev( TbHeap::Header* header, TbHeap::Header* insert)
{
#if TB_HEAP_HEADER_DEBUG_DEF    //デバッグコード
    if(m_BreakPointFreeHeader && insert == reinterpret_cast<TbHeap::Header*>(m_BreakPointFreeHeader)){
        TbBreak();
    }
#endif

    //フリーのルートだったら変更する
    if( m_ForwardFreeRoot > insert){
        m_ForwardFreeRoot = insert;
    }
    if( m_ReverseFreeRoot < insert){
        m_ReverseFreeRoot = insert;
    }

    insert->freePrev = header->freePrev;
    if( header->freePrev ){
        header->freePrev->freeNext = insert;
    }
    header->freePrev = insert;
    insert->freeNext = header;
}

/*!
 * 昇順フリーリストから削除
 * @author teco
 */
void TbHeap::deleteFreeList( TbHeap::Header* header )
{
    // フリーのルートだったら変更する
    if( m_ForwardFreeRoot == header )
    {
        m_ForwardFreeRoot = m_ForwardFreeRoot->freeNext;
    }
    if( m_ReverseFreeRoot == header )
    {
        m_ReverseFreeRoot = m_ReverseFreeRoot->freePrev;
    }

    if( header->freePrev )
    {
        header->freePrev->freeNext = header->freeNext;
    }
    if( header->freeNext )
    {
        header->freeNext->freePrev    = header->freePrev;
    }
    header->freePrev = NULL;
    header->freeNext = NULL;
}

/*!
 * ヘッダー初期化
 * 実体を真っ白にする
 * @author teco
 */
void TbHeap::cleanHeader( TbHeap::Header* header )
{
    TbMemClear(header,header->size);
}

/*!
 * ヘッダーアドレス取得
 * @author teco
 */
TbHeap::Header* TbHeap::getHeader(void* userPtr)
{
    TbAddr offset = *(reinterpret_cast<TbUint16*>(userPtr)-1);
    return reinterpret_cast<TbHeap::Header*>(reinterpret_cast<TbAddr>(userPtr) - offset);
}

/*!
 * ユーザーアドレス取得
 * @author teco
 */
TbAddr TbHeap::getUserAddr( TbHeap::Header* header )
{
    return reinterpret_cast<TbAddr>(header) + header->headerSize;
}

/*!
 * ユーザーアドレス取得
 * @author teco
 */
void* TbHeap::getUserPtr( TbHeap::Header* header )
{
    return reinterpret_cast<TbUint8*>(header) + header->headerSize;
}

/*!
 * ユーザーサイズ取得
 * @author teco
 */
TbUint32 TbHeap::getUserSize( TbHeap::Header* header )
{
    return header->size - header->headerSize;
}

/*!
 * フリーリストをダンプ
 * @author teco
 */
void TbHeap::DumpFreeList()
{
    TbHeap::Header* header = m_ForwardFreeRoot;
    TbUint32 i = 0;
    while(header){
        TbPrintf("[HeapHeader][Free:%d size:%d] prev=%p this=%p next=%p\n",i,header->size,header->freePrev,header,header->freeNext);
        header = header->freeNext;
        ++i;
    }    
}

/*!
 * グローバルリストをダンプ
 * @author teco
 */
void TbHeap::DumpGlobalList()
{
    TbHeap::Header* header = m_GlobalRoot;
    TbUint32 i = 0;
    while(header){
        TbUint32 offset = 0;
        if( (header->flag & TbHeap::Header::FLAG_USED) ){
            void* userPtr = getUserPtr(header);
            offset = *(reinterpret_cast<TbUint16*>(userPtr)-1);
        }
        TbPrintf("[HeapHeader][Global:%d size:%d headerSize:%d:%d] prev=%p this=%p next=%p\n",i,header->size,header->headerSize,offset,header->globalPrev,header,header->globalNext);
        header = header->globalNext;
        ++i;
    }
}

/*!
 * 断片化情報をダンプ
 * @author teco
 */
void TbHeap::DumpFragmentList()
{
    //未実装
}

/*!
 * 未開放情報をダンプ
 * @author teco
 */
void TbHeap::DumpMallocList()
{
#if TB_HEAP_HEADER_DEBUG_DEF
    TbHeap::Header* header = m_GlobalRoot;
    TbUint32 i = 0;
    while(header){
        if( (header->flag & TbHeap::Header::FLAG_USED) ){
            void* userPtr = getUserPtr(header);
            TbPrintf("[HeapMalloc]this=0x%p size=%u\n",userPtr,header->size);
            for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(header->from); ++i ) {
                TbPrintf("            from[%u]=0x%p\n",i,header->from[i]);
            }
        }
        header = header->globalNext;
        ++i;
    }
#endif
}

/*!
 * Mallocの呼び出し元アドレス一致でブレークをかける
 * @param address 呼び出し元
 * @author teco
 */
void TbHeap::SetBreakPointFrom(TbAddr address)
{
    m_BreakPointFrom = address;
}

/*!
 * Mallocで確保したアドレス一致でブレークをかける
 * @param address 呼び出し元
 * @author teco
 */
void TbHeap::SetBreakPointMalloc(TbAddr address)
{
    m_BreakPointMalloc = address;
}

/*!
 * 一致したヘッダーアドレスがフリーリスト追加時にブレーク
 * @param address 呼び出し元
 * @author teco
 */
void TbHeap::SetBreakPointFreeHeader(TbAddr address)
{
    m_BreakPointFreeHeader = address;
}

}
