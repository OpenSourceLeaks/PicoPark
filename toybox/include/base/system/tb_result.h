/*!
 * 結果定義
 * @author Miyake Shunsuke
 * @data 2010.04.10
 */

#ifndef _INCLUDED_TB_RESULT_H_
#define _INCLUDED_TB_RESULT_H_

typedef TbUint32 TbResult;

// 結果
enum
{
    TB_S_OK         , //!< 成功
    TB_E_FAIL       , //!< 失敗
    TB_E_PENDING    , //!< 保留
    TB_E_INVALIDARG , //!< 不正引数
    TB_E_OUTOFMEMORY, //!< メモリ不足
    TB_E_BUSY       , //!< ビジー状態
    TB_E_DENY       , //!< 呼び出し拒否
};

// 成功したかどうか
#define TB_SUCCEEDED(result) (TB_S_OK==(result))

// 失敗したかどうか
#define TB_FAILED(result) (TB_S_OK!=(result))

#endif