/*!
 * 
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_ALLOCATOR_H_
#define _INCLUDED_TB_ALLOCATOR_H_

namespace toybox
{

class TbAllocator
{
public:

    //! メモリ確保
    virtual void* Allocate( TbUint32 size ) = 0;

    //! メモリ確保
    virtual void* Allocate(TbUint32 size,TbUint32 align) = 0;

    //! メモリ解放
    virtual void Deallocate( void* ptr ) = 0;

};

}

#endif
