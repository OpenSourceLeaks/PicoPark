/*!
 * 既存のイテレータから取得する値を別の値に変換するイテレータ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_TbAdapter_ITERATOR_H_
#define _INCLUDED_TB_TbAdapter_ITERATOR_H_

#include <base/util/tb_type_traits.h>

namespace toybox
{

// アダプタイテレータ
template<typename ValueType,typename Iterator,typename Getter>
class TbAdapterIterator
{
public:
    
    // コンストラクタ
    TbAdapterIterator( const Iterator& ite , Getter getter = Getter() )
        : m_Ite(ite)
        , m_Getter(getter)
    {}
    
    // コピーコンストラクタ
    TbAdapterIterator( const TbAdapterIterator& ite )
        : m_Ite(ite.m_Ite)
        , m_Getter(ite.m_Getter)
    {}

    ~TbAdapterIterator() {}
    
public:
    
    //! *演算子
    ValueType operator*() {
        return m_Getter(*m_Ite);
    }

    //! 代入演算子
    TbAdapterIterator& operator=(const TbAdapterIterator& ite) {
        m_Ite = ite.m_Ite;
        m_Getter = ite.m_Getter;
        return *this;
    }
    
    //! 前置インクリメント
    TbAdapterIterator& operator++() {
        ++m_Ite;
        return *this;
    }
    
    //! 後値インクリメント
    TbAdapterIterator operator++(int) {
        return TbAdapterIterator(m_Ite++);
   }
    
    //! 加算演算子
    TbAdapterIterator operator+(int value) {
        return TbAdapterIterator(m_Ite+value);
    }

    //! 前置デクリメント
    TbAdapterIterator& operator--() {
        --m_Ite;
        return *this;
    }
    
    //! 後値デクリメント
    TbAdapterIterator operator--(int) {
        return TbAdapterIterator(m_Ite--);
    }
    
    //! 減算演算子
    TbAdapterIterator operator-(int value) {
        return TbAdapterIterator(m_Ite-value);
    }
    
    //! 減算演算子
    int operator-(const TbAdapterIterator& ite) {
        return static_cast<int>(m_Ite - ite.m_Ite);
    }
    
    //! 比較演算子
    TbBool operator==(const TbAdapterIterator& ite) const{
        return ( m_Ite == ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator!=(const TbAdapterIterator& ite) const{
        return ( m_Ite != ite.m_Ite );
    }
        
    //! 比較演算子
    TbBool operator>(const TbAdapterIterator& ite) const{
        return ( m_Ite > ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator>=(const TbAdapterIterator& ite) const{
        return ( m_Ite >= ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator<(const TbAdapterIterator& ite) const{
        return ( m_Ite < ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator<=(const TbAdapterIterator& ite) const{
        return ( m_Ite <= ite.m_Ite );
    }

private:
    Iterator m_Ite;
    Getter   m_Getter;
};

// アダプタConstイテレータ
template<typename ValueType,typename Iterator,typename Getter>
class TbAdapterConstIterator
{
public:
    
    // コンストラクタ
    TbAdapterConstIterator( const Iterator& ite , Getter getter = Getter() )
        : m_Ite(ite)
        , m_Getter(getter)
    {}
    
    // コピーコンストラクタ
    TbAdapterConstIterator( const TbAdapterConstIterator& ite )
        : m_Ite(ite.m_Ite)
        , m_Getter(ite.m_Getter)
    {}

    ~TbAdapterConstIterator() {}
    
public:
    
    //! *演算子
    ValueType operator*() const {
        return m_Getter(*m_Ite);
    }

    //! 代入演算子
    TbAdapterConstIterator& operator=(const TbAdapterConstIterator& ite) {
        m_Ite = ite;
        m_Getter = ite.m_Getter;
        return *this;
    }
    
    //! 前置インクリメント
    TbAdapterConstIterator& operator++() {
        ++m_Ite;
        return *this;
    }
    
    //! 後値インクリメント
    TbAdapterConstIterator operator++(int) {
        return TbAdapterConstIterator(m_Ite++);
   }
    
    //! 加算演算子
    TbAdapterConstIterator operator+(int value) {
        return TbAdapterConstIterator(m_Ite+value);
    }

    //! 前置デクリメント
    TbAdapterConstIterator& operator--() {
        --m_Ite;
        return *this;
    }
    
    //! 後値デクリメント
    TbAdapterConstIterator operator--(int) {
        return TbAdapterConstIterator(m_Ite--);
    }
    
    //! 減算演算子
    TbAdapterConstIterator operator-(int value) {
        return TbAdapterConstIterator(m_Ite-value);
    }
    
    //! 減算演算子
    int operator-(const TbAdapterConstIterator& ite) {
        return m_Ite - ite.m_Ite;
    }
    
    //! 比較演算子
    TbBool operator==(const TbAdapterConstIterator& ite) const{
        return ( m_Ite == ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator!=(const TbAdapterConstIterator& ite) const{
        return ( m_Ite != ite.m_Ite );
    }
        
    //! 比較演算子
    TbBool operator>(const TbAdapterConstIterator& ite) const{
        return ( m_Ite > ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator>=(const TbAdapterConstIterator& ite) const{
        return ( m_Ite >= ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator<(const TbAdapterConstIterator& ite) const{
        return ( m_Ite < ite.m_Ite );
    }

    //! 比較演算子
    TbBool operator<=(const TbAdapterConstIterator& ite) const{
        return ( m_Ite <= ite.m_Ite );
    }

private:
    Iterator m_Ite;
    Getter   m_Getter;
};

template<typename ValueType,typename Iterator,typename Getter>
inline TbAdapterIterator<ValueType,Iterator,Getter> TbGetAdapterIterator(Iterator ite,Getter getter)
{
    return TbAdapterIterator<ValueType,Iterator,Getter>(ite,getter);
}

template<typename ValueType,typename Iterator,typename Getter>
inline TbAdapterConstIterator<ValueType,Iterator,Getter> TbGetAdapterConstIterator(Iterator ite,Getter getter)
{
    return TbAdapterConstIterator<ValueType,Iterator,Getter>(ite,getter);
}
}

#endif
