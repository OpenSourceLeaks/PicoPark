/*!
 * IO関連の定義
 * @author Miyake Shunsuke
 * @since 2012.03.27
 */

#ifndef _INCLUDED_TB_IO_TYPES_H_
#define _INCLUDED_TB_IO_TYPES_H_

// シークタイプ
enum TbIOSeekType
{
    TB_IO_SEEK_TYPE_BEGIN ,    // 先頭
    TB_IO_SEEK_TYPE_CURRENT ,  // 現在値
    TB_IO_SEEK_TYPE_END ,      // 最後尾
    TB_IO_SEEK_TYPE_MAX , 
};

// 状態定数
enum TbIOState
{
    TB_IO_STATE_ERROR,          //!< エラー状態
    TB_IO_STATE_CLOSED ,        //!< 閉じている
    TB_IO_STATE_READY ,         //!< 読み書き可能
    TB_IO_STATE_MAX,
};

#endif
