/*!
 * 
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_ARRAY_ITERATOR_H_
#define _INCLUDED_TB_ARRAY_ITERATOR_H_

namespace toybox
{

/*
 * @brief イテレータ
 */
template<typename T>
struct TbArrayIterator
{
public:
    typedef T ValueType;
public:
        
    //! コンストラクタ
    explicit TbArrayIterator( T* value ) : m_Value( value ) {}

    //! コピーコンストラクタ
    TbArrayIterator(const TbArrayIterator &ite) : m_Value( ite.m_Value ){}

    //! "*"演算子
    T& operator*(){
        return *m_Value;
    }

    //! "*"演算子
    const T& operator*() const{
        return *m_Value;
    }

    //! アロー演算子
    T* operator->() {
        return m_Value;
    }

    //! アロー演算子
    const T* operator->() const{
        return m_Value;
    }

    //! 代入演算子
    TbArrayIterator &operator=(const TbArrayIterator& ite){
        m_Value = ite.m_Value;
        return *this;
    }
        
    //! 前置インクリメント
    TbArrayIterator &operator++(){
        ++m_Value;
        return *this;
    }

    //! 後置インクリメント 
    TbArrayIterator operator++(int){
        TbArrayIterator ite = *this;
        m_Value++;
        return ite;
    }

    //! 加算演算子
    TbArrayIterator operator+(int value){
        TbArrayIterator ite = *this;
        ite.m_Value += value;
        return ite;
    }

    //! 前置インクリメント
    TbArrayIterator &operator--(){
        --m_Value;
        return *this;
    }

    //! 後置インクリメント 
    TbArrayIterator &operator--(int){
        m_Value--;
        return *this;
    }

    //! 引き算演算子
    TbArrayIterator operator-(int value){
        TbArrayIterator ite = *this;
        ite.m_Value -= value;
        return ite;
    }

    //! 引き算演算子
    int operator-( const TbArrayIterator &ite ) const {
        int result = 0;
        if( m_Value && ite.m_Value ){
            result = static_cast<int>( m_Value - ite.m_Value );
        }
        return result;
    }

    //! 比較演算子
    TbBool operator==(const TbArrayIterator &ite) const{
        return ( m_Value == ite.m_Value );
    }

    //! 比較演算子
    TbBool operator!=(const TbArrayIterator &ite) const{
        return ( m_Value != ite.m_Value );
    }
        
    //! 比較演算子
    TbBool operator>(const TbArrayIterator &ite) const{
        return ( m_Value > ite.m_Value );
    }

    //! 比較演算子
    TbBool operator>=(const TbArrayIterator &ite) const{
        return ( m_Value >= ite.m_Value );
    }

    //! 比較演算子
    TbBool operator<(const TbArrayIterator &ite) const{
        return ( m_Value < ite.m_Value );
    }

    //! 比較演算子
    TbBool operator<=(const TbArrayIterator &ite) const{
        return ( m_Value <= ite.m_Value );
    }

private:
    T* m_Value;
};

/*
 * @brief イテレータ
 */
template<typename T>
struct TbArrayConstIterator
{
public:
    typedef T ValueType;
public:
        
    //! コンストラクタ
    explicit TbArrayConstIterator( const T* value ) 
        : m_Value( value ) {}
        
    //! コピーコンストラクタ
    TbArrayConstIterator(const TbArrayConstIterator &ite)
        : m_Value( ite.m_Value ){}

    //! コピーコンストラクタ
    TbArrayConstIterator(const TbArrayIterator<T> &ite)
        : m_Value( &(*ite) ){}

    //! "*"演算子
    const T& operator*() const{
        return *m_Value;
    }


    //! アロー演算子
    const T* operator->() const{
        return m_Value;
    }

    //! 代入演算子
    TbArrayConstIterator &operator=(const TbArrayConstIterator& ite){
        m_Value = ite.m_Value;
        return *this;
    }
        
    //! 前置インクリメント
    TbArrayConstIterator &operator++(){
        ++m_Value;
        return *this;
    }

    //! 後置インクリメント 
    TbArrayConstIterator operator++(int){
        TbArrayConstIterator ite = *this;
        m_Value++;
        return ite;
    }

    //! 加算演算子
    TbArrayConstIterator operator+(int value){
        TbArrayConstIterator ite = *this;
        ite.m_Value += value;
        return ite;
    }

    //! 前置インクリメント
    TbArrayConstIterator &operator--(){
        --m_Value;
        return *this;
    }

    //! 後置インクリメント 
    TbArrayConstIterator &operator--(int){
        m_Value--;
        return *this;
    }

    //! 引き算演算子
    TbArrayConstIterator operator-(int value){
        TbArrayConstIterator ite = *this;
        ite.m_Value -= value;
        return ite;
    }

    //! 引き算演算子
    int operator-( const TbArrayConstIterator &ite ) const{
        int result = 0;
        if( m_Value && ite.m_Value ){
            result = ( m_Value - ite.m_Value );
        }
        return result;
    }

    //! 比較演算子
    TbBool operator==(const TbArrayConstIterator &ite) const{
        return ( m_Value == ite.m_Value );
    }

    //! 比較演算子
    TbBool operator!=(const TbArrayConstIterator &ite) const{
        return ( m_Value != ite.m_Value );
    }
        
    //! 比較演算子
    TbBool operator>(const TbArrayConstIterator &ite) const{
        return ( m_Value > ite.m_Value );
    }

    //! 比較演算子
    TbBool operator>=(const TbArrayConstIterator &ite) const{
        return ( m_Value >= ite.m_Value );
    }

    //! 比較演算子
    TbBool operator<(const TbArrayConstIterator &ite) const{
        return ( m_Value < ite.m_Value );
    }

    //! 比較演算子
    TbBool operator<=(const TbArrayConstIterator &ite) const{
        return ( m_Value <= ite.m_Value );
    }

private:
    const T* m_Value;
};

}

#endif
