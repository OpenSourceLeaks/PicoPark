/*!
 * @file
 * @brief ヒープクラスのヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_HEAP_H_
#define _INCLUDED_TB_HEAP_H_

#include <base/string/tb_string_util.h>
#include <base/thread/tb_mutex.h>

namespace toybox
{

#define TB_HEAP_HEADER_DEBUG_DEF 1

/*!
 * ヒープクラス
 * @author Miyake Shunsuke
 */
class TbHeap
{
public:

    static const TbSint32 ID_INVALID;    //!< 不正ID値
    static const TbUint32 FROM_MAX = 12;
    
    enum AllocType{
        ALLOC_TYPE_FORWARD,    //前から確保
        ALLOC_TYPE_REVERSE    //後から確保
    };

    //ヘッダー(デバッグ描画を外部委託するため)
    struct Header
    {
        enum{
            FLAG_USED = (1),
        };
        Header*  freeNext;     //!< フリーリストの次ヘッダー
        Header*  freePrev;     //!< フリーリストの前ヘッダー
        Header*  globalNext;   //!< グローバルリストの次ヘッダー
        Header*  globalPrev;   //!< グローバルリストの前ヘッダー
#if TB_HEAP_HEADER_DEBUG_DEF
        TbAddr   from[FROM_MAX];    //!< 呼び出し元
#endif
        TbUint32 size;         //!< ヘッダー + 余剰 + 実体サイズ
        TbUint16 flag;              //!< フラグ
        TbUint16 headerSize;        //!< ヘッダーサイズ(実データアドレスの-1*TbUint16の場所にも同じようにセット
    };

public:

    // コンストラクタ
    TbHeap();

    // デストラクタ
    ~TbHeap();

public:

    //セットアップ
    TbResult Initialize(void *address,TbUint32 size,TbUint32 align);
    
    // 後始末
    void Finalize();

    // 前からメモリ確保(デフォルトアライメント)
    TB_INLINE void* AllocateForward(TbUint32 size) { 
        return Allocate(size,m_Align,ALLOC_TYPE_FORWARD); 
    }

    // 前からメモリ確保(アライメント指定)
    TB_INLINE void* AllocateForward(TbUint32 size,TbUint32 align) {
        return Allocate(size, align, ALLOC_TYPE_FORWARD);
    }

    // 後ろからメモリ確保(デフォルトアライメント)
    TB_INLINE void* AllocateReverse(TbUint32 size) {
        return Allocate(size,m_Align,ALLOC_TYPE_REVERSE); 
    }

    // 後ろからメモリ確保(アライメント指定)
    TB_INLINE void* AllocateReverse(TbUint32 size, TbUint32 align) {
        return Allocate(size, align, ALLOC_TYPE_REVERSE);
    }

    //メモリ確保(アライメント指定)
    void* Allocate(TbUint32 size,TbUint32 align,AllocType type);

    //開放
    void Deallocate(void* ptr);

    //アライメントの取得
    TB_INLINE TbUint32 GetAlign() const { 
        return m_Align; 
    }

    //アライメントの設定
    TB_INLINE void     SetAlign(TbUint32 align) { 
        m_Align = align; 
    }

    //先頭アドレス取得
    TB_INLINE TbAddr GetStartAddress() const { 
        return m_StartAddress; 
    } 

    //末端アドレス取得
    TB_INLINE TbAddr GetEndAddress() const { 
        return m_StartAddress + m_Size; 
    }

    //ヒープサイズ取得
    TB_INLINE TbAddr GetSize() { 
        return m_Size; 
    }

    //フリーサイズ取得
    TB_INLINE TbAddr GetFreeSize() { 
        return m_FreeSize; 
    }
    
    // アロケーションAlloc数
    TB_INLINE TbUint32 GetAllocCount() const {
        return m_UseBlockCount;
    }

public: //デバッグ用途

    // 名前取得
    const TbChar8* GetName() const {
        return m_Name;
    }

    // 名前設定
    void SetName(TbChar8* name) { 
        TbStrCopy(m_Name,HEAP_NAME_MAX,name); 
    }

    // フリーリスト取得
    const TbHeap::Header* GetFreeListRoot() const { 
        return m_ForwardFreeRoot; 
    }

    // フリーリストをダンプ
    void DumpFreeList();

    // グローバルリストをダンプ
    void DumpGlobalList();

    // 断片化情報をダンプ
    void DumpFragmentList();

    // 未開放情報をダンプ
    void DumpMallocList();

    // Mallocの呼び出し元アドレス一致でブレークをかける
    void SetBreakPointFrom(TbAddr address);

    // Mallocで確保したアドレス一致でブレークをかける
    void SetBreakPointMalloc(TbAddr address);

    // 一致したヘッダーアドレスがフリーリスト追加時にブレーク
    void SetBreakPointFreeHeader(TbAddr address);

private:    //内部構造体

    static const TbUint8 HEAP_NAME_MAX = 8;

    enum Flag
    {
        FLAG_INITIALIZE , 
        FLAG_MAX , 
    };

private:    //内部関数

    // 前からヒープを分割
    TbHeap::Header* dividHeapForward(TbHeap::Header* header,TbUint32 needSize,TbUint32 surplus);

    // 後ろからヒープを分割
    TbHeap::Header* dividHeapReverse(TbHeap::Header* header,TbUint32 needSize,TbUint32 surplus);

    // グローバルリストに追加(headerの前に)
    void insertGlobalList(TbHeap::Header* header,TbHeap::Header* insert);

    // グローバルリストから削除
    void deleteGlobalList(TbHeap::Header* header);

    // 昇順フリーリストに追加（headerの後ろに）
    void insertFreeListNext( TbHeap::Header* header, TbHeap::Header* insert);

    // 昇順フリーリストに追加 (headerの前に)
    void insertFreeListPrev( TbHeap::Header* header, TbHeap::Header* insert);

    // 昇順フリーリストから削除
    void deleteFreeList( TbHeap::Header* header );

    // ヘッダーした場合のゴミを除去
    void cleanHeader( TbHeap::Header* header );

    // ヘッダーアドレス取得
    TbHeap::Header* getHeader(void* userPtr);

    // ユーザーアドレス取得
    TbAddr getUserAddr( TbHeap::Header* header );

    // ユーザーアドレス取得
    void* getUserPtr( TbHeap::Header* header );

    // ユーザーサイズ取得
    TbUint32 getUserSize( TbHeap::Header* header );

private:    //内部変数
    
    TbChar8         m_Name[HEAP_NAME_MAX];  //!< 名前(デバッグ用)
    TbBitArray32    m_BitArray;             //!< フラグ
    TbAddr          m_StartAddress;         //!< 先頭アドレス
    TbAddr          m_EndAddress;           //!< 終端アドレス
    TbUint32        m_Align;                //!< アライメントサイズ
    TbUint32        m_Size;                 //!< サイズ
    TbUint32        m_FreeSize;             //!< フリーサイズ
    TbUint32        m_UseBlockCount;        //!< 使用ブロック数
    Header*         m_ForwardFreeRoot;      //!< 昇順検索フリーリスト
    Header*         m_ReverseFreeRoot;      //!< 降順検索フリーリスト
    Header*         m_GlobalRoot;           //!< グローバルリスト(主にデバッグ用途)
    TbAddr          m_BreakPointFrom;       //!< ブレークポイント(呼び出し元)
    TbAddr          m_BreakPointMalloc;     //!< ブレークポイント(生成アドレス)
    TbAddr          m_BreakPointFreeHeader; //!< ブレークポイント(フリーリストヘッダー)
    TbMutex         m_Mutex;

};

}

#endif
