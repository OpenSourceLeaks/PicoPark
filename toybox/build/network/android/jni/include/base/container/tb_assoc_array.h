/*!
 * 連想配列
 * @author Miyake_Shunsuke
 * @since 2012.06.12
 */

#ifndef _INCLUDED_TB_ASSOC_ARRAY_H_
#define _INCLUDED_TB_ASSOC_ARRAY_H_

#include <base/algorithm/tb_search.h>
#include <base/container/tb_key_value.h>
#include <base/container/tb_array.h>

namespace toybox
{

template <typename Key , 
          typename Value , 
          typename Compare = TbLess<Key> , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<Key,Value> > > 
class TbAssocArray
    : private TbArray<TbKeyValue<Key,Value>,Allocator>
{
public:
    
    typedef TbKeyValue<Key,Value> KeyValue;
    typedef TbArray<KeyValue,Allocator> Super;
    typedef typename Super::Iterator Iterator;
    typedef typename Super::ConstIterator ConstIterator;
    
    // キー比較関数オブジェクト
    struct KeyCompare
    {
        KeyCompare( Compare compare = Compare() ) 
            : m_Compare(compare) 
        {}
        // 比較 (KeyValueとKeyValue)
        TbBool operator() ( const KeyValue& left , const KeyValue& right ) const {
            return m_Compare( left.key , right.key );
        }

        // 比較 (KeyとKey)
        TbBool operator() ( const typename KeyValue::KeyType& left , const typename KeyValue::KeyType& right ) const {
            return m_Compare( left , right );
        }

        // 比較 (KeyとKeyValue)
        TbBool operator() ( const typename KeyValue::KeyType& left , const KeyValue& right ) const {
            return m_Compare( left , right.key );
        }

        // 比較 (KeyValueとKey)
        TbBool operator() ( const KeyValue& left , const typename KeyValue::KeyType& right ) const {
            return m_Compare( left.key , right );
        }

        Compare m_Compare;
    };

public:
    
    // コンストラクタ
    explicit TbAssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() , const Compare& comp = Compare() )
        : Super(capacity,allocator)
        , m_Compare(comp)
    {}
    
    // デストラクタ
    ~TbAssocArray() {}
    
public:
    
    //! 空かどうか
    TbBool IsEmpty() const {
        return Super::IsEmpty();
    }
    
    //! 一杯かどうか
    TbBool IsFull() const {
        return Super::IsFull();
    }
    
    //! 数取得
    TbUint32 GetCount() const {
        return Super::GetCount();
    }
    
    //! 最大数取得
    TbUint32 GetCapacity() const {
        return Super::GetCapacity();
    }
    
    //! 最大数設定
    void SetCapacity( TbUint32 capacity ) {
        Super::SetCapacity( capacity );
    }

    //! 自動リサイズ可能設定
    void SetEnableSetCapacityAuto( TbBool isEnableSetCapacityAuto ){
        Super::SetEnableSetCapacityAuto( isEnableSetCapacityAuto );
    }
    
    //! 自動リサイズ可能設定取得
    TbBool IsEnableSetCapacityAuto() const{
        return Super::IsEnableSetCapacityAuto();
    }

    //! 挿入
    TbBool Insert( const Key& key , const Value& value ) 
    {
        Iterator ite( GetLowerBound(key) );
        if( ite == End() || m_Compare( key , *ite ) ){
            return Super::Insert( ite , KeyValue(key,value) );
        }
        return TB_FALSE;
    }
    
    //! 除去
    void Erase( const Key& key )
    {
        Iterator ite( Find(key) );
        if( ite != End() ){
            Super::Erase(ite);
        }
    }
    
    //! 除去
    Iterator Erase( const Iterator& ite )
    {
        if( ite != End() ){
            return Super::Erase(ite);
        }
        return ite;
    }

    //! クリア
    void Clear() {
        Super::Clear();
    }
    
    //! 検索
    TB_FORCE_INLINE Iterator Find( const Key& key )
    {
        return TbBinarySearchIte(Begin(),End(),key,m_Compare);
    }
    
    //! 検索
    TB_FORCE_INLINE ConstIterator Find( const Key& key ) const
    {
        return TbBinarySearchIte(Begin(),End(),key,m_Compare);
    }
    
    //! 下限取得
    TB_FORCE_INLINE Iterator GetLowerBound( const Key& key )
    {
        return TbLowerBound(Begin(),End(),key,m_Compare);
    }
    
    //! 下限取得
    TB_FORCE_INLINE ConstIterator GetLowerBound( const Key& key ) const
    {
        return TbLowerBound(Begin(),End(),key,m_Compare);
    }

    //! 下限取得
    Iterator GetUpperBound( const Key& key )
    {
        return TbUpperBound(Begin(),End(),key,m_Compare);
    }
    
    //! 下限取得
    ConstIterator GetUpperBound( const Key& key ) const
    {
        return TbUpperBound(Begin(),End(),key,m_Compare);
    }

public: // イテレータ
    
    // 開始
    Iterator Begin() { 
        return Super::Begin();
    }
    
    // 終端
    Iterator End() { 
        return Super::End();
    }

    // 開始
    ConstIterator Begin() const {
        return Super::Begin();
    }

    // 終端
    ConstIterator End() const {
        return Super::End();
    }
    
private:
    
    KeyCompare m_Compare;  //!< 比較オブジェクト

};

};

#endif
