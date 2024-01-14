/*!
 * @file
 * @brief レンダーシステム依存
 */

#ifndef _INCLUDED_TB_RENDER_SYSTEM_DEPEND_H_
#define _INCLUDED_TB_RENDER_SYSTEM_DEPEND_H_

namespace toybox
{

class  TbRenderSystemDepend
{
    friend class TbRenderSystem;
public:
    
    // コンストラクタ
    TbRenderSystemDepend();
    
    // デストラクタ
    ~TbRenderSystemDepend();
    
public:
    
    // 初期化
    void Initialize();
    
    // 後始末
    void Finalize();

public:
    
    // MEM1からのアロケート
    void* AllocMem1( u32 size , u32 align );
    
    // MEM1のメモリ開放
    void FreeMem1( void* ptr );

    // Bucketからのアロケート
    void* AllocBucket( u32 size , u32 align );
    
    // Bucket解放
    void FreeBucket( void* ptr );
    
    // フォアグラウンドか
    TbBool IsOnForeground() const {
        return m_IsOnForeground;
    }
    
public:
    
    static u32 acquiredForegroundCallback(void* ptr);
    static u32 releaseForegroundCallback(void* ptr);
    
private:

    TbBool          m_IsOnForeground;;
    void*           m_ScanBuffer;
    MEMHeapHandle   m_HeapHandleMem1;
    MEMHeapHandle   m_HeapHandleBucket;

};

void* TbGX2Alloc( u32 size , u32 align );
void  TbGX2Free( void* ptr );

void* TbGX2AllocMem1( u32 size , u32 align );
void  TbGX2FreeMem1( void* ptr );

void* TbGX2AllocBucket( u32 size , u32 align );
void  TbGX2FreeBucket( void* ptr );

}

#endif