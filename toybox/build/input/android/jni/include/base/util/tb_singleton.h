/*!
 * @file
 * @brief Singletonテンプレート
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_SINGLETON_H_
#define _INCLUDED_TB_SINGLETON_H_

#include <base/system/tb_assert.h>

namespace toybox
{

/*!
 * 参照タイプSingletonテンプレート
 * アプリケーションの起動から終了まで存在し続けるモノはこちら
 * 生成はアプリケーション側に任せた
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T> class TbSingletonRef
{
protected:

    //! コンストラクタ
    TbSingletonRef()
    {
        if( !s_Instance ){
            TbAddr offset = 0;
            // calculate the static offset between the singleton and the template argument class
            offset = reinterpret_cast<TbAddr>(reinterpret_cast<T*>(1));
            offset -= reinterpret_cast<TbAddr>(reinterpret_cast<TbSingletonRef<T>*>(reinterpret_cast<T*>(1)));
            // add the offset on the given singleton pointer, to yield the address of the derived class.
            s_Instance = reinterpret_cast<T*>(reinterpret_cast<TbAddr>(this) + offset);
            s_Instance = static_cast<T*>(this);
        }else{
            TB_ASSERT_MSG(TB_FALSE,"s_Instance is already created");
        }
    }

    //! デストラクタ
    virtual ~TbSingletonRef(){ s_Instance = NULL; }

public:

    /*!
     * インスタンス取得
     * @author Miyake Shunsuke
     * @since 2010.06.12
     */
    static T& GetInstance()
    {
        TB_ASSERT_MSG( NULL != s_Instance ,"s_Instance is NULL");
        return *s_Instance;
    }

    /*!
     * インスタンス取得(ポインター)
     * @author Miyake Shunsuke
     * @since 2010.06.12
     */
    static T* GetInstancePtr()
    {
        return s_Instance;
    }

private:
    
    static T* s_Instance; //!< シングルトンインスタンス

};

template<typename T> T *TbSingletonRef<T>::s_Instance = NULL;


/*!
 * Singletonテンプレート
 * 生成はアプリケーション側に任せた
 * @author Miyake Shunsuke
 * @since 2010.06.12
 */
template<typename T> class TbSingletonPointer
{
protected:

    //! コンストラクタ
    TbSingletonPointer()
    {
        if( !s_Instance ){
            TbAddr offset = 0;
            // calculate the static offset between the singleton and the template argument class
            offset = reinterpret_cast<TbAddr>(reinterpret_cast<T*>(1));
            offset -= reinterpret_cast<TbAddr>(reinterpret_cast<TbSingletonPointer<T>*>(1));
            // add the offset on the given singleton pointer, to yield the address of the derived class.
            s_Instance = reinterpret_cast<T*>(reinterpret_cast<TbAddr>(this) + offset);
            s_Instance = static_cast<T*>(this);
        }else{
            TB_ASSERT_MSG(TB_FALSE,"s_Instance is already created");
        }
    }

    //! デストラクタ
    virtual ~TbSingletonPointer(){ s_Instance = NULL; }

public:

    /*!
     * インスタンス取得
     * @author Miyake Shunsuke
     * @since 2010.06.12
     */
    static T* GetInstance()
    {
//        TB_ASSERT_MSG( NULL != s_Instance ,"s_Instance is NULL");
        return s_Instance;
    }

private:
    
    static T* s_Instance; //!< シングルトンインスタンス


};

template<typename T> T *TbSingletonPointer<T>::s_Instance = NULL;

}

#endif