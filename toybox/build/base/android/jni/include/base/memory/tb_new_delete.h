/*!
 * new/delete
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_NEW_DELETE_H_
#define _INCLUDED_TB_NEW_DELETE_H_

// toybox用new/delete

namespace toybox
{
    struct TbNewDeleteType {};
    extern const TbNewDeleteType TB_NEW_DELETE_TYPE_VAL;
}

#if TB_COMPILER_IS_CLANG
    #define TB_THROW throw()
#else
    #define TB_THROW
#endif

// toybox用new
void* operator new(size_t size, const toybox::TbNewDeleteType&);

// toybox用delete
void operator delete(void* pv, const toybox::TbNewDeleteType&);

// toybox用new[]
void* operator new[](size_t size, const toybox::TbNewDeleteType&);

// toybox用delete[]
void operator delete[](void* pv, const toybox::TbNewDeleteType&);

#if TB_PLATFORM_IS_APPLE

#define TB_NEW new
#define TB_DELETE delete

#elif 0

#define TB_NEW new(TB_NEW_DELETE_TYPE_VAL)
#define TB_DELETE delete
//#define TB_DELETE(p) delete(p,TB_NEW_DELETE_TYPE_VAL)
//#define TB_DELETE_ARRAY(p) delete[](p,TB_NEW_DELETE_TYPE_VAL)

#else

// toybox用new
void* operator new(size_t size);

// toybox用new[]
void* operator new[](size_t size);

// toybox用delete
void operator delete(void* pv) TB_THROW;

// toybox用delete[]
void operator delete[](void* pv) TB_THROW;

#define TB_NEW new
#define TB_DELETE delete

#endif

// placement new/delete

#if !TB_PLATFORM_IS_WIN && !TB_PLATFORM_IS_NX && !TB_PLATFORM_IS_FLASH && !TB_PLATFORM_IS_EMSCRIPTEN && !TB_PLATFORM_IS_APPLE

TB_FORCE_INLINE void* operator new(size_t size , void* ptr ) {
    return ptr;
}
TB_FORCE_INLINE void operator delete(void* pv,void* ptr) {
}
TB_FORCE_INLINE void* operator new[](size_t size,void* ptr) {
    return ptr;
}
TB_FORCE_INLINE void operator delete[](void* pv,void* ptr) {
}

#endif

#endif
