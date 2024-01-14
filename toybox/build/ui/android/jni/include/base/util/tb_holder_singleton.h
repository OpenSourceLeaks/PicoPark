/*!
 * @file
 * @brief Singletonテンプレート
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_HOLDER_SINGLETON_H_
#define _INCLUDED_TB_HOLDER_SINGLETON_H_

#include <base/system/tb_assert.h>
#include <base/util/tb_singleton.h>

namespace toybox
{

/*!
 * 実態を保持するシングルトン
 * @author Miyake_Shunsuke
 * @since 2012.06.04
 */
template<typename T,typename IdType>
class TbHolderSingleton : public TbSingletonRef< TbHolderSingleton<T,IdType> >
{
public:

    // コンストラクタ (m_Valueはデフォルトコンストラクタが呼ばれます)
    TbHolderSingleton() : m_Value() {}

    // デストラクタ
    virtual ~TbHolderSingleton() {}

public:
    
    // 実体取得
    T& GetValue() {
        return m_Value;
    }

    // 実体取得
    const T& GetValue() const {
        return m_Value;
    }

private:
    T m_Value;
};

/*!
 * ポインターを保持するシングルトン
 * @author Miyake_Shunsuke
 * @since 2012.06.04
 */
template<typename T,typename IdType>
class TbPointerHolderSingleton : public TbSingletonRef< TbPointerHolderSingleton<T,IdType> >
{
public:

    // コンストラクタ
    TbPointerHolderSingleton() : m_Value( NULL ) {}

    // デストラクタ
    virtual ~TbPointerHolderSingleton() {}

public:
    //! ポインター設定
    void SetValue( T* value ){
        value = m_Value;
    }
    //! ポインター取得
    T* GetValue() {
        return m_Value;
    }
    //! ポインター取得
    const T* GetValue() const {
        return m_Value;
    }
private:
    T* m_Value;
};


}

#endif