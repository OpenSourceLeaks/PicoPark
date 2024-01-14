/*! 
 * アーカイブ
 * @author Miyake_Shunsuke
 * @since 2014.6.10
 */
 
#ifndef _INCLUDED_TB_ARCHIVE_TYPES_H_
#define _INCLUDED_TB_ARCHIVE_TYPES_H_

namespace toybox
{

#define TB_ARCHIVE_VERSION (0x1)

/*

-------------------
TbArchiveHeader
-------------------
TbArchiveFileInfo
       :
       :
-------------------
TbArchiveDirectoryInfo
       :
       :
-------------------
[TbUint32の配列(subdir配列),
 TbUint32の配列(file配列)]
       :
       :
-------------------

*/

// ヘッダー
struct TbArchiveHeader
{
    enum Flag
    {
        FLAG_BIT_RESOLVE = TB_BIT(0)
    };
    TbUint32                version;
    TbUint32                flag;
    TbUint32                fileCount;
    TbUint32                dirCount;
    TbUint32                offsetData; // データ部までのオフセット
};

// ファイル情報
struct TbArchiveFileInfo
{
   TbBool operator<( TbUint32 hashVal ) const {
        return hash < hashVal;
    }
    union {
        TbUint64    pathBuf;
        const char* path;       // ファイルパス
    };
    TbUint32    nameIndex;  // ファイル名インデックス
    TbUint32    hash;       // パスハッシュ
    TbUint32    offset;     // ファイルまでのオフセット
    TbUint32    size;       // ファイルサイズ
};

// ファイル情報
struct TbArchiveDirectoryInfo
{
    TbBool operator<( TbUint32 hashVal ) const {
        return hash < hashVal;
    }
    union {
        TbUint64    pathBuf;
        const char* path;       // ファイルパス
    };
    TbUint32    nameIndex;
    TbUint32    hash;
    TbUint32    subdirCount;
    TbUint32    subdirAllCount; // 子供も含むカウント
    TbUint32    subdirOffset;
    TbUint32    fileCount;
    TbUint32    fileAllCount; // 子供も含むカウント
    TbUint32    fileOffset;
};

}

#endif
