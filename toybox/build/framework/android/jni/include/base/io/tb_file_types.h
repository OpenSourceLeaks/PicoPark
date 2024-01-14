/*!
 * @file
 * @brief TbFile用のパスID
 * @author Miyake Shunsuke
 */

#ifndef _INCLUDE_TB_FILE_TYPES_H_
#define _INCLUDE_TB_FILE_TYPES_H_

#include <base/util/tb_function.h>

namespace toybox
{

class TbFile;

#if TB_PLATFORM_IS_WIN
#define TB_FILE_SEPARATOR "\\"
#else
#define TB_FILE_SEPARATOR "/"
#endif

// ファイル書き込みすることが可能なプラットフォームか
#if TB_PLATFORM_IS_EMSCRIPTEN
#   define TB_IO_IS_ENABLE_WRITE_FILE (0)
#else
#   define TB_IO_IS_ENABLE_WRITE_FILE (1)
#endif

    // パスID
typedef TbUint64 TbFilePathId;
#define TB_FILE_PATH_ID_NONE 0 // 絶対パス
#define TB_FILE_PATH_ID_PRESET_BEGIN 1
#define TB_FILE_PATH_ID_PRESET_SYSTEM TB_FILE_PATH_ID_PRESET_BEGIN  // システム用ルートパス
#define TB_FILE_PATH_ID_PRESET_SHADER 2  // アプリケーション用シェーダーインクルード用パス
#define TB_FILE_PATH_ID_PRESET_ROOT 3  // アプリケーション用ルートパス
#define TB_FILE_PATH_ID_PRESET_SAVE 4  // アプリケーション用セーブパス
#define TB_FILE_PATH_ID_PRESET_MAX 4  // プリセット数

#define TB_FILE_PATH_ID_IS_APPLICATION( id ) \
    ( ( id & 0xFFFFFFFF00000000LL ) != 0LL )

// 挙動方式
enum TbFileOperateMode
{
    TB_FILE_OPERATE_READ,   // 読み込み
    TB_FILE_OPERATE_WRITE,  // 書き込み
    TB_FILE_OPERATE_MAX , 
};

// メモリ取得方法
enum TbFileAllocMode 
{
    TB_FILE_ALLOC_FORWARD ,    // 下位から取得
    TB_FILE_ALLOC_REVERSE ,    // 上位から取得
    TB_FILE_ALLOC_USER,        // ユーザー側で取得
    TB_FILE_ALLOC_MAX , 
};

// 動作方式
enum TbFileSyncMode
{
    TB_FILE_ASYNC = TB_FALSE,  //!< 非同期処理
    TB_FILE_SYNC = TB_TRUE     //!< 同期処理
};

// オープンするタイミング
enum TbFileOpenMode
{
    TB_FILE_OPEN_SEQUENTIALLY = TB_FALSE,   // 順番オープン
    TB_FILE_OPEN_DIRECTLY = TB_TRUE,        // 直接オープン
};

enum TbFileSearchFlagBit
{
    TB_FILE_SEARCH_FLAG_NAME = TB_BIT(0)     , // 名前だけの一覧(パスは必要なし)
    TB_FILE_SEARCH_FLAG_SUBDIR = TB_BIT(1)   , // サブディレクトリも検索する
    TB_FILE_SEARCH_FLAG_RELATIVE = TB_BIT(2) , // 検索フォルダからの相対パス
};

typedef TbStaticFunction32<void ()> TbFileCallback;   // IO処理終了コールバック
typedef TbStaticFunction32<void()> TbFileUserFunc;   // IO処理ユーザー処理

}

#endif