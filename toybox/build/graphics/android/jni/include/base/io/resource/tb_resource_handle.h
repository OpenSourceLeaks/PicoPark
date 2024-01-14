/*!
 * リソースハンドル
 * @author Miyake Shunsuke
 * @since 2011.12.15
 */

#ifndef _INCLUDED_TB_RESOURCE_HANDLE_H_
#define _INCLUDED_TB_RESOURCE_HANDLE_H_

#include "tb_resource.h"
#include "tb_resource_manager.h"

namespace toybox
{

template <typename Data>
class TbResourceHandle
{
public:

    // デフォルトコンストラクタ
    TbResourceHandle()
        : m_Resource(NULL)
    {
    }

    // コンストラクタ
    TbResourceHandle( const char* fileName )
        : m_Resource(NULL)
    {
        Create( fileName );
    }

    // コンストラクタ
    TbResourceHandle( const TbResourceHandle& handle)
        : m_BitArray(handle.m_BitArray)
        , m_Resource(handle.m_Resource)
    {
        if( m_Resource ) {
            m_Resource->Retain();
        }
        if( IsActive() ) {
            m_Resource->Load();
        }
    }

    // デストラクタ
    ~TbResourceHandle() {
        Destroy();
    }

public:

    // 生成
    TbResult Create( const char* fileName , const TbUint32 listType = 0 , TbFileSyncMode syncMode = TB_FILE_SYNC ) {
        if( TB_VERIFY(!m_Resource) ) {
            m_Resource = TbResourceManager::GetInstance().CreateResource<TbResource<Data> >(fileName , listType , syncMode );
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }

    // 検索(既に生成されていたら使う。存在しなければ使わない)
    TbResult Find( const char* fileName , const TbUint32 listType = 0) {
        if( TB_VERIFY(!m_Resource) ) {
            m_Resource = TbResourceManager::GetInstance().FindResource<TbResource<Data> >(fileName , listType );
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }

    // 破棄
    void Destroy() {
        if( TbResourceManager::GetInstancePtr() ){
            TB_SAFE_RELEASE(m_Resource);
        }
        m_BitArray.SetBit(FLAG_ACTIVE,TB_FALSE);
    }

public:

    // データを持っているか
    TbBool HasData() const {
        return m_Resource && m_Resource->GetData();
    }

    // データ取得
    Data* GetData() {
        TB_ASSERT(m_Resource);
        if( m_Resource ){
            return m_Resource->GetData();
        }
        return NULL;
    }

    // データ取得
    const Data* GetData() const {
        TB_ASSERT(m_Resource);
        if( m_Resource ){
            return m_Resource->GetData();
        }
        return NULL;
    }

public:
    
    // 有効化
    void Activate() {
        if( m_Resource && !m_BitArray.Test(FLAG_ACTIVE) ){
            m_Resource->Load();
            m_BitArray.SetBit(FLAG_ACTIVE,TB_TRUE);
        }
    }

    // 無効化
    void Deactivate() {
        if( m_Resource && m_BitArray.Test(FLAG_ACTIVE) ){
            m_Resource->Unload();
            m_BitArray.SetBit(FLAG_ACTIVE,TB_FALSE);
        }
    }

    // 有効かどうか
    TbBool IsActive() const {
        return m_Resource && m_BitArray.Test(FLAG_ACTIVE);
    }

    // 利用可能状態か
    TbBool IsReady() const {
        return m_Resource && m_Resource->IsReady();
    }

    // ビジー状態か
    TbBool IsBusy() const {
        return m_Resource && m_Resource->IsBusy();
    }

private:

    enum Flag
    {
        FLAG_ACTIVE , 
        FLAG_MAX , 
    };

private:

    TbBitArray32      m_BitArray;
    TbResource<Data>* m_Resource; // 複数ハンドルから共有されるインスタンス

};

}

#endif
