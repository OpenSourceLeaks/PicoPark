/*!
 * テンプレート用アロケータ
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TEMPLATE_ALLOCATOR_STD_H_
#define _INCLUDED_TEMPLATE_ALLOCATOR_STD_H_

#include <base/util/tb_type_traits.h>
#include <base/algorithm/tb_algorithm_misc.h>
#include <base/memory/tb_allocator_holder.h>

namespace toybox
{

template<typename T>
class TbTemplateAllocatorStd
{
public:
    // 別の型にバインド
    template <typename U>
    struct Rebind
    {
        typedef TbTemplateAllocatorStd<U> Other;
    };
public:
    //! コンストラクタ
    TbTemplateAllocatorStd(){}
    //! デストラクタ
    ~TbTemplateAllocatorStd(){}
public:

    /*!
     * メモリを割り当てる
     * @author Miyake_Shunsuke
     * @since 2012.06.03
     */
    T* Allocate( TbUint32 count )
    {
#if 0
        return (T*)( ::malloc( count * sizeof(T) ) );
#else
        using namespace toybox;
        TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
        return reinterpret_cast<T*>(allocator.Allocate(count * sizeof(T)));
#endif
    }

    /*!
     * 割当て済みの領域を初期化する
     * @author Miyake_Shunsuke
     * @since 2012.06.03
     */
    void Construct(T* p, const T& value)
    {
        new( (void*)p ) T(value);
    }

    /*!
     * 割当て済みの領域をデフォルトコンストラクタで初期化する
     * @author Miyake_Shunsuke
     * @since 2014.11.03
     */
    void Construct(T* p)
    {
        new( (void*)p ) T();
    }

    /*!
     * メモリを解放する
     * @author Miyake_Shunsuke
     * @since 2012.06.03
     */
    void Deallocate(T* p, TbUint32 count)
    {
#if 0
        ::free( (void*)p );
#else
        using namespace toybox;
        TbHeapAllocator& allocator = TbAllocatorHolderGlobal::GetInstance().GetValue();
        allocator.Deallocate(p);
#endif
    }

    /*!
     * 初期化済みの領域を削除する
     * @author Miyake_Shunsuke
     * @since 2012.06.03
     */
    void Destroy(T* p)
    {
        TbDestroy(p);
    }

};

}

#endif
