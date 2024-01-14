/*!
 * @file
 * @brief アサート関連ヘッダーファイル
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDED_TB_ASSERT_H_
#define _INCLUDED_TB_ASSERT_H_

namespace toybox
{

// エラー出力
TbUint32 TbPrintAssertMessage( const char* file , TbSint32 line , const char* func , const char* fmt , ... );
TbUint32 TbPrintWarnMessage( const char* file , TbSint32 line , const char* func , const char* fmt , ... );

// アサート
#if TB_IS_DEBUG

#define TB_WARN( enable ) if(!(enable)){ ::toybox::TbPrintWarnMessage(TB_FILENAME , TB_LINE , TB_FUNCNAME , #enable ); }
#define TB_WARN_MSG( enable , msg ) if(!(enable)){ ::toybox::TbPrintWarnMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , msg , "" ); }
#define TB_WARN_MSG_FORMAT( enable , fmt , ... ) if(!(enable)){ ::toybox::TbPrintWarnMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , fmt , __VA_ARGS__ ); }

#define TB_ASSERT( enable ) if(!(enable)){ ::toybox::TbPrintAssertMessage(TB_FILENAME , TB_LINE , TB_FUNCNAME , #enable ); }
#define TB_ASSERT_MSG( enable , msg ) if(!(enable)){ ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , msg , "" ); }
#define TB_ASSERT_MSG_FORMAT( enable , fmt , ... ) if(!(enable)){ ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , fmt , __VA_ARGS__ ); }

#define TB_VERIFY( exp ) ( (exp) || ::toybox::TbPrintAssertMessage(TB_FILENAME , TB_LINE , TB_FUNCNAME , #exp ) )
#define TB_VERIFY_MSG( exp , msg ) ( (exp) || ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , msg , "" ) )
#define TB_VERIFY_MSG_FORMAT( exp , fmt , ... ) ( (exp) || ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , fmt , __VA_ARGS__ ) )

#define TB_RESQUE( enable ) if(!(enable)){ ::toybox::TbPrintAssertMessage(TB_FILENAME , TB_LINE , TB_FUNCNAME , #enable ); return; }
#define TB_RESQUE_MSG( enable , msg ) if(!(enable)){ ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , msg , "" ); return; }
#define TB_RESQUE_MSG_FORMAT( enable , fmt , ... ) if(!(enable)){ ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , fmt , __VA_ARGS__ ); return; }
#define TB_RESQUE_RET( enable , ret ) if(!(enable)){ ::toybox::TbPrintAssertMessage(TB_FILENAME , TB_LINE , TB_FUNCNAME , #enable ); return ret; }
#define TB_RESQUE_RET_MSG( enable , ret , msg ) if(!(enable)){ ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , msg , "" ); return ret; }
#define TB_RESQUE_RET_MSG_FORMAT( enable , ret , fmt , ... ) if(!(enable)){ ::toybox::TbPrintAssertMessage( TB_FILENAME , TB_LINE , TB_FUNCNAME , fmt , __VA_ARGS__ ); return ret; }
#else
#define TB_WARN( enable ) 
#define TB_WARN_MSG( enable , fmt )
#define TB_WARN_MSG_FORMAT( enable , fmt , ... )
#define TB_ASSERT( enable ) 
#define TB_ASSERT_MSG( enable , fmt )
#define TB_ASSERT_MSG_FORMAT( enable , fmt , ... )
#define TB_VERIFY( exp ) (exp)
#define TB_VERIFY_MSG( exp , msg ) (exp)
#define TB_VERIFY_MSG_FORMAT( exp , fmt , ... ) (exp)
#define TB_RESQUE( enable ) if(!(enable)){ return; }
#define TB_RESQUE_MSG( enable , msg ) if(!(enable)){ return; }
#define TB_RESQUE_MSG_FORMAT( enable , fmt , ... ) if(!(enable)){ return; }
#define TB_RESQUE_RET( enable , ret ) if(!(enable)){ return ret; }
#define TB_RESQUE_RET_MSG( enable , ret , msg ) if(!(enable)){ return ret; }
#define TB_RESQUE_RET_MSG_FORMAT( enable , ret , fmt , ... ) if(!(enable)){ return ret; }
#endif

// 静的アサート
#if 1
template <TbBool> struct static_assert_{
  enum { okay = 1 };
};
template <> struct static_assert_<TB_FALSE> {};
#define JOIN(a, b) JOIN_(a, b)
#define JOIN_(a, b) a ## b
#define TB_STATIC_ASSERT(expr) \
    typedef int JOIN(diagnostics_, __LINE__) \
    [toybox::static_assert_<!!(expr)>::okay]

}
#else
#define TB_STATIC_ASSERT(expr) static_assert((expr),"")
#endif

#endif
