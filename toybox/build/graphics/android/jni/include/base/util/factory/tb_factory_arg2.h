/*!
 * 引数2つのクラスファクトリーテンプレートクラス
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_FACTORY_ARG2_H_
#define _INCLUDED_TB_FACTORY_ARG2_H_

namespace toybox
{

/*!
 * 引数2つのクラスファクトリーテンプレート
 */
template <typename T,typename ARG1,typename ARG2>
class TbFactory2
{

typedef T* (*createCallback)(void *buf,ARG1,ARG2);

public:
    
    /*!
     * コンストラクタ
     * @param keySize 登録最大数
     */
    TbFactory2(TbUint32 keyMax)
        : m_KeyMax(keyMax)
    {
        m_CallbackList = TB_NEW createCallback[m_KeyMax];
        m_ClassSizeList = TB_NEW TbUint32[m_KeyMax]s;
        TbMemClear(m_CallbackList,sizeof(createCallback) * m_KeyMax);
        TbMemClear(m_ClassSizeList,sizeof(TbUint32) * m_KeyMax);
    }
    
    /*!
     * デストラクタ
     */
    ~TbFactory2()
    {
        TB_SAFE_DELETE_ARRAY(m_CallbackList);
        TB_SAFE_DELETE_ARRAY(m_ClassSizeList);
    }
    
public:
    
    /*!
     * 型登録
     */
    template <class U>
    TbBool RegisterClass(TbUint32 key)
    {
        if(key < 0 || key >= m_KeyMax || m_CallbackList[key]){
            return TB_FALSE;
        }
        struct Create
        {
            static T* callback(void *buf,ARG1 arg1,ARG2 arg2)
            {
                if(buf){
                    return new(buf) U(arg1,arg2);
                }
                return TB_NEW U(arg1,arg2);
            }
        };
        m_CallbackList[key] = Create::callback;
        m_ClassSizeList[key] = sizeof(U);
        return TB_TRUE;
    }
    
    /*!
     * 生成
     */
    T* Create(const TbUint32 key,ARG1 arg1,ARG2 arg2)
    {
        if(key < 0 || key >= m_KeyMax || !m_CallbackList[key]){
            return NULL;
        }
        return m_CallbackList[key](NULL,arg1,arg2);
    }
    
    /*!
     * 破棄(デストラクタ呼び出し無し)
     */
    void Free(T* ptr)
    {
        TB_SAFE_DELETE(ptr);
    }
    
    /*!
     * 破棄(デストラクタ呼び出し有り)
     */
    void Delete(T* ptr)
    {
        TB_SAFE_DELETE(ptr);
    }

private:
    createCallback*    m_CallbackList;  //関数配列
    TbUint32*        m_ClassSizeList; //クラスのサイズ配列
    TbUint32        m_KeyMax;         //登録最大数
    Allocator*        m_Allocator;     //アロケータ
};

}

#endif