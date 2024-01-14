/*!
 * @file
 * @brief 参照カウンタ基底クラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_REF_COUNTER_H_
#define _INCLUDED_TB_REF_COUNTER_H_

namespace toybox
{

/*!
 * 参照カウンタ基底クラス
 */
class TbRefCounter
{
public:
    
    // コンストラクタ
    TbRefCounter()
        : m_RefCount(1)
    {
    }
    
protected:

    // デストラクタ(直接呼出し禁止)
    virtual ~TbRefCounter(){}

public:

    // 保持関数(参照カウント+1)
    void Retain(){ 
        ++m_RefCount; 
    }

    // 開放関数(参照カウント-1)
    TbBool Release(){ 
        if( TB_VERIFY(m_RefCount>0) ){
            --m_RefCount; 
            if(m_RefCount == 0){ 
                TB_DELETE this; 
                return TB_TRUE;
            } 
        }
        return TB_FALSE;
    }

    // 参照カウント取得
    TbUint32 GetRefCount() {
        return m_RefCount;
    }

private:

    // 参照カウント
    TbUint32 m_RefCount;
};

template<typename T>
class TbRefCountedData : public TbRefCounter
{
public:
    
    // コンストラクタ
    TbRefCountedData() {}

public:

    // 代入演算子
    TbRefCountedData<T>& operator=( const T& data ) {
        m_Data = data;
        return *this;
    }

    // 比較演算子
    TbBool operator==( T& data ) {
        return m_Data == data;
    }

    // 比較演算子
    TbBool operator!=( T& data ) {
        return m_Data != data;
    }

public:

    // 取得
    T& Get() {
        return m_Data;
    }

    // 取得
    const T& Get() const {
        return m_Data;
    }

private:
    T   m_Data;
};


template<typename T>
class TbRefCountedArray : public TbRefCounter
{
public:

    // コンストラクタ
    TbRefCountedArray( TbUint32 count ) {
        m_Data = TB_NEW T[count];
        m_Count = count;
    }

    // デストラクタ
    virtual ~TbRefCountedArray() {
        TB_SAFE_DELETE_ARRAY(m_Data);
    }

public:

    //! アクセス演算子
    T& operator[]( TbSint32 index ) {
        return m_Data[index];
    }
    
    //! アクセス演算子
    const T& operator[]( TbSint32 index ) const {
        return m_Data[index];
    }


public:

    // 取得
    T* Get() {
        return m_Data;
    }

    // 取得
    const T* Get() const {
        return m_Data;
    }

    // 数取得
    TbUint32 GetCount() const {
        return m_Count;
    }

private:
    T* m_Data;
    TbUint32 m_Count;
};

}

#endif