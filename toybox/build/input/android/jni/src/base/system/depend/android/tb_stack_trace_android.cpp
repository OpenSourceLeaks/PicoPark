/*!
 * Android用スタックトレース
 * @author Miyake_Shunsuke
 */

#include <tb_fwd.h>
#include "base/system/tb_stack_trace.h"
#include <unwind.h>
#include <dlfcn.h>

#if TB_PLATFORM_IS_ANDROID

namespace
{
    struct BacktraceState
    {
        TbAddr* current;
        TbAddr* end;
        TbUint32 beginCount;
    };
    
    static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arg)
    {
        BacktraceState* state = static_cast<BacktraceState*>(arg);
        if( 0 == state->beginCount ) {
            uintptr_t pc = _Unwind_GetIP(context);
            if (pc) {
                if (state->current == state->end) {
                    return _URC_END_OF_STACK;
                } else {
                    *state->current = static_cast<TbAddr>(pc);
                    ++state->current;
                }
            }
        }else{
            --state->beginCount;
        }
        return _URC_NO_REASON;
    }

    size_t captureBacktrace(TbAddr* buffer, size_t max , TbUint32 offset )
    {
        BacktraceState state = {buffer, buffer + max , offset };
        _Unwind_Backtrace(unwindCallback, &state);
        return state.current - buffer;
    }
    
#if 0
    void dumpBacktrace(std::ostream& os, void** buffer, size_t count)
    {
        for (size_t idx = 0; idx < count; ++idx) {
            const void* addr = buffer[idx];
            const char* symbol = "";

            Dl_info info;
            if (dladdr(addr, &info) && info.dli_sname) {
                symbol = info.dli_sname;
            }

            os << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol << "\n";
        }
    }
#endif

}

namespace toybox
{

/*!
 * スタックトレース取得
 * @author Miyake_Shunsuke
 * @since 2014.08.13
 */
TbUint32  TbStackTrace::GetCallAddress( TbAddr* dst , TbUint32 dstMax , TbUint32 offset )
{
    return captureBacktrace( dst , dstMax , offset );
}

}

#endif
