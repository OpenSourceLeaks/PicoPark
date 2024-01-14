/*!
 * 参照渡しするためのテンプレート
 * @author Miyake_Shunsuke
 * @since 2012.04.18
 */

#ifndef _INCLUDED_TB_REF_H_
#define _INCLUDED_TB_REF_H_

template <typename T>
class TbRefWrapper
{
public:

    //! コンストラクタ
    inline explicit TbRefWrapper(T& src)
        : m_Member (src)
    {}

    //! デストラクタ
    ~TbRefWrapper() {}

private:

    TB_DISALLOW_ASSIGN(TbRefWrapper);

public:
    
    //! 参照型変換演算子
    inline operator T& () const {
        return m_Member;
    }
    
    //! 取得
    inline T& Get() {
        return m_Member;
    }

private:
    T& m_Member;
};

/*!
 * 参照を保持
 * @author Miyake_Shunsuke
 * @since 2012.04.22
 */
template<typename T>
inline TbRefWrapper<T> TbRef(T& t)
{
    return TbRefWrapper<T>(t);
}

/*!
 * 参照を保持(const参照用)
 * @author Miyake_Shunsuke
 * @since 2012.04.22
 */
template<typename T>
inline TbRefWrapper<const T> TbConstRef(const T& t)
{
    return TbRefWrapper<const T>(t);
}

#endif