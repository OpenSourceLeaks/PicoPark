/*!
 * 文字列キー連想配列
 * @author Miyake_Shunsuke
 * @since 2012.06.12
 */

#ifndef _INCLUDED_TB_STR_ASSOC_ARRAY_H_
#define _INCLUDED_TB_STR_ASSOC_ARRAY_H_

#include <base/container/tb_assoc_array.h>
#include <base/string/tb_static_string.h>
#include <base/crypt/tb_crc32.h>

namespace toybox
{

// 文字列キー
template<typename String,typename HashFunc=TbCrc32>
class TbStringKey
{
public:
    typedef String StringType;
public:
    TbStringKey( const String& str , HashFunc func = HashFunc() ) 
        : m_Str(str) 
    {
        m_Hash = func( str.GetBuffer() , str.GetCharCount() );
    }
    TbStringKey( const TbStringKey& src ) 
        : m_Str(src.m_Str) 
        , m_Hash(src.m_Hash)
    {
    }
public:

    // 文字列取得
    const String& GetString() const {
        return m_Str;
    }

    // ハッシュ値取得
    TbUint32 GetHash() const {
        return m_Hash;
    }

    // <演算子
    TbBool operator==( const TbStringKey& str ) const {
        return ( m_Hash == str.m_Hash && m_Str == str.m_Str );
    }

    // <演算子
    TbBool operator<( const TbStringKey& str ) const {
        return (m_Hash < str.m_Hash) || ( m_Hash == str.m_Hash && m_Str < str.m_Str );
    }

private:
    String   m_Str;
    TbUint32 m_Hash;
};

// 16文字列キー
typedef TbStringKey<TbStaticString16> TbStringKey16;
// 32文字列キー
typedef TbStringKey<TbStaticString32> TbStringKey32;
// 64文字列キー
typedef TbStringKey<TbStaticString64> TbStringKey64;
// 128文字列キー
typedef TbStringKey<TbStaticString128> TbStringKey128;
// 256文字列キー
typedef TbStringKey<TbStaticString256> TbStringKey256;

template <typename StringKey , 
          typename Value , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<Key,Value>>> 
class TbStrAssocArray
    : private TbAssocArray<StringKey,Value,TbLess<StringKey>,Allocator>
{    
public:

    typedef TbAssocArray<StringKey,Value,TbLess<StringKey>,Allocator> Super;
    typedef typename Super::Iterator Iterator;
    typedef typename Super::ConstIterator ConstIterator;

public:
    
    // コンストラクタ
    explicit TbStrAssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() )
        : Super( capacity , allocator )
    {}

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
    TbBool Insert( const StringKey& key , const Value& value ) 
    {
        return Super::Insert( key , value );
    }
    
    //! 挿入
    TbBool Insert( typename const StringKey::StringType::CharType* key , const Value& value ) 
    {
        return Super::Insert( StringKey(key) , value );
    }

    //! 除去
    void Erase( StringKey& key )
    {
        Iterator ite( Find(key) );
        if( ite != End() ){
            Super::Erase(ite);
        }
    }

    //! 除去
    void Erase( typename const StringKey::StringType::CharType* key )
    {
        Iterator ite( Find(StringKey(key)) );
        if( ite != End() ){
            Super::Erase(ite);
        }
    }
    
    //! クリア
    void Clear() {
        Super::Clear();
    }
    
    //! 検索
    TB_FORCE_INLINE Iterator Find( const StringKey& key )
    {
        return Super::Find( key );
    }
    
    //! 検索
    TB_FORCE_INLINE Iterator Find( typename const StringKey::StringType::CharType* key )
    {
        return Super::Find( StringKey(key) );
    }

    //! 検索
    TB_FORCE_INLINE ConstIterator Find( const StringKey& key ) const
    {
        return Super::Find( key );
    }
    
    //! 検索
    TB_FORCE_INLINE ConstIterator Find( typename const StringKey::StringType::CharType* key ) const
    {
        return Super::Find( StringKey(key) );
    }

    //! 下限取得
    Iterator GetLowerBound( const StringKey& key )
    {
        return Super::GetLowerBound( key );
    }
    
    //! 下限取得
    ConstIterator GetLowerBound( const StringKey& key ) const
    {
        return Super::GetLowerBound( key );
    }

    //! 下限取得
    Iterator GetUpperBound( const StringKey& key )
    {
        return Super::GetUpperBound( key );
    }
    
    //! 下限取得
    ConstIterator GetUpperBound( const StringKey& key ) const
    {
        return Super::GetUpperBound( key );
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
    
};

// 16文字キー用
template< typename Value , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<TbStringKey16,Value>>> 
class TbStr16AssocArray : public TbStrAssocArray<TbStringKey16,Value,Allocator>
{
public:
    typedef TbStrAssocArray<TbStringKey16,Value,Allocator> Super;
public:
    explicit TbStr16AssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() )
        : Super( capacity , allocator )
    {}
};
// 32文字キー用
template< typename Value , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<TbStringKey32,Value>>> 
class TbStr32AssocArray : public TbStrAssocArray<TbStringKey32,Value,Allocator>
{
public:
    typedef TbStrAssocArray<TbStringKey32,Value,Allocator> Super;
public:
    explicit TbStr32AssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() )
        : Super( capacity , allocator )
    {}
};
// 64文字キー用
template< typename Value , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<TbStringKey64,Value>>> 
class TbStr64AssocArray : public TbStrAssocArray<TbStringKey64,Value,Allocator>
{
public:
    typedef TbStrAssocArray<TbStringKey64,Value,Allocator> Super;
public:
    explicit TbStr64AssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() )
        : Super( capacity , allocator )
    {}
};
// 128文字キー用
template< typename Value , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<TbStringKey128,Value>>> 
class TbStr128AssocArray : public TbStrAssocArray<TbStringKey128,Value,Allocator>
{
public:
    typedef TbStrAssocArray<TbStringKey128,Value,Allocator> Super;
public:
    explicit TbStr128AssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() )
        : Super( capacity , allocator )
    {}
};
// 256文字キー用
template< typename Value , 
          typename Allocator = TbTemplateAllocatorStd<TbKeyValue<TbStringKey256,Value>>> 
class TbStr256AssocArray : public TbStrAssocArray<TbStringKey256,Value,Allocator>
{
public:
    typedef TbStrAssocArray<TbStringKey256,Value,Allocator> Super;
public:
    explicit TbStr256AssocArray( TbUint32 capacity = 0 , const Allocator& allocator = Allocator() )
        : Super( capacity , allocator )
    {}
};

}

#endif
