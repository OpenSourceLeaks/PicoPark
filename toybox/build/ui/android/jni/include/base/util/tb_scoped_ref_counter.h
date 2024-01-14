/*!
 * スコープ内が生存期間の参照カウンタ
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SCOPED_REF_COUNTER_H_
#define _INCLUDED_TB_SCOPED_REF_COUNTER_H_

namespace toybox
{

template <typename T>
class TbScopedRefCounter
{
public:
    
    //! コンストラクタ
    TbScopedPtr()
        : m_Value(NULL)
    {
    }
    
    //! コンストラクタ
    TbScopedPtr( T* value )
        : m_Value(value)
    {
    }

    //! デストラクタ(直接呼出し禁止)
    void ~TbScopedPtr()
    {
        TB_SAFE_RELEASE(m_Value);
    }
    
public:
    
    //! 参照
    T& operator*()
    {
        TB_ASSERT(m_Value);
        return *m_Value;
    }
    
    //! const参照
    const T& operator*() const
    {
        TB_ASSERT(m_Value);
        return *m_Value;
    }

    //! ポインタ参照
    T* operator->()
    {
        return m_Value;
    }

    //! constポインタ参照
    const T* operator->() const
    {
        return m_Value;
    }
    
public:
    
    // 設定
    void Reset( T* value ) 
    {
        TB_SAFE_RELEASE(m_Value);
        m_Value = value;
    }
    
    //! ポインタ参照
    T* Get() 
    {
        return m_Value;
    }
    
    //! constポインタ参照
    const T* Get() const 
    {
        return m_Value;
    }

private:

    T* m_Value;

};

}

#endif