/*!
 * スタックトレース
 * @author Miyake_Shunsuke
 * @since 2014.08.13
 */

#ifndef _INCLUDED_TB_STACK_TRACE_H_
#define _INCLUDED_TB_STACK_TRACE_H_

#include <base/util/tb_singleton.h>

namespace toybox
{

#if TB_PLATFORM_IS_WIN

#include <dbghelp.h>

class TbStackTrace : public TbSingletonRef<TbStackTrace>
{
public:
    
    // コンストラクタ
    TbStackTrace();
    
    // デストラクタ
    virtual ~TbStackTrace();

public:
    
    // 呼び出し元アドレス取得(返り値は実際に取得した階層)
    TbUint32  GetCallAddress( TbAddr* addr , TbUint32 addrMax , TbUint32 depthOffset = 0);

private:

    typedef USHORT  ( __stdcall *PtrRtlCaptureStackBackTrace )( ULONG, ULONG, PVOID*, PULONG );
    typedef DWORD   ( __stdcall *PtrSymSetOptions )( DWORD );
    typedef BOOL    ( __stdcall *PtrSymInitialize )( HANDLE, PCTSTR, BOOL );
    typedef BOOL    ( __stdcall *PtrSymCleanup )( HANDLE );
    typedef BOOL    ( __stdcall *PtrSymFromAddr )( HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO );
    typedef BOOL    ( __stdcall *PtrSymGetLineFromAddr64 )( HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64 );

    enum Flag
    {
        FLAG_INITIALIZED , 
        FLAG_MAX
    };

private:

    TbBitArray32                m_BitArray;
    HMODULE                     m_DllKernel32;
    HMODULE                     m_DllDbgHelp;
    PtrRtlCaptureStackBackTrace m_RtlCaptureStackBackTraceFunc;
    PtrSymSetOptions            m_SymSetOptionsFunc;
    PtrSymInitialize            m_SymInitializeFunc;
    PtrSymCleanup               m_SymCleanupFunc;
    PtrSymFromAddr              m_SymFromAddrFunc;
    PtrSymGetLineFromAddr64     m_SymGetLineFromAddr64Func;

};

#elif (TB_PLATFORM_IS_ANDROID)

class TbStackTrace : public TbSingletonRef<TbStackTrace>
{
public:
    // コンストラクタ
    TbStackTrace() {}
    // デストラクタ
    virtual ~TbStackTrace() {}
public:
    // 呼び出し元アドレス取得
    TbUint32  GetCallAddress( TbAddr* addr , TbUint32 addrMax , TbUint32 depthOffset = 0);
};

#elif (TB_PLATFORM_IS_EMSCRIPTEN) || (TB_PLATFORM_IS_APPLE) || (TB_PLATFORM_IS_WIIU) || (TB_PLATFORM_IS_NX)

class TbStackTrace : public TbSingletonRef<TbStackTrace>
{
public:
    // コンストラクタ
    TbStackTrace() {}
    // デストラクタ
    virtual ~TbStackTrace() {}
public:
    // 呼び出し元アドレス取得
    TbUint32  GetCallAddress( TbAddr* addr , TbUint32 addrMax , TbUint32 depthOffset = 0) {
        return 0;
    }
};
    
#endif

inline void TbGetCallAddress( TbAddr* dst , TbUint32 dstMax , TbUint32 depthOffset )
{
    if( !dst ) {
        return;
    }
    TbStackTrace* stack = TbStackTrace::GetInstancePtr();
    if( stack ) {
        stack->GetCallAddress(dst,dstMax,depthOffset);
    }
}

}

#endif