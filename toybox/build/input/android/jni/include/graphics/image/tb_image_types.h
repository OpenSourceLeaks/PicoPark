/*!
 * @file
 * @brief 画像パーサー系の共通定義
 * @author Miyake Shunsuke
 * @since 2011.5.5
 */

#ifndef _INCLUDED_TB_IMAGE_TYPES_H_
#define _INCLUDED_TB_IMAGE_TYPES_H_

#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{

/*!
 * 画像エクスポート用情報
 * @author Miyake Shunsuke
 * @since 2010.5.5
 */
struct TbImageExportInfo
{
    enum {
        FLAG_BIT_DOWN_TO_UP = TB_BIT(0) , 
        FLAG_BIT_RIGHT_TO_LEFT = TB_BIT(1) , 
    };
    const void* buffer;
    TbUint32 bufSize;
    TbUint32 width;
    TbUint32 height;
    TbUint32 flag;
    TbColorFormat colorFormat;
    TbColorFormat outputColorFormat; // 出力用
};

/*!
 * 画像エクスポート用結果格納先
 * @author Miyake Shunsuke
 * @since 2010.5.5
 */
struct TbImageExportResult
{
    void* fileBuf;
    TbUint32 fileBufSize;
};

/*!
 * 画像インポート用情報
 * @author Miyake Shunsuke
 * @since 2010.5.5
 */
struct TbImageImportInfo
{
    TbImageImportInfo() : 
        fileBuf(nullptr) , fileBufSize(0) , colorFormat(TB_COLOR_FORMAT_UNKNOWN)
    {}
    const void*   fileBuf;
    TbUint32      fileBufSize;
    TbColorFormat colorFormat; // 強制インポートフォーマット
};

/*!
 * 画像インポート用結果格納先
 * @author Miyake Shunsuke
 * @since 2010.5.5
 */
struct TbImageImportResult
{
    void*           buffer;
    TbUint32        bufSize;
    TbUint32        width;
    TbUint32        height;
    TbColorFormat   colorFormat;
};

typedef void* (*TbImageAllocFunc)( TbUint32 size );

class TbImageAllocatorDefault
{
public:

	// デフォルトメモリ確保
	static void* Alloc( TbUint32 size ){
		return TB_NEW TbUint8[ size ];
	}

	// デフォルトメモリ解放
	static void Free( void* ptr ){
		TbUint8* buf = reinterpret_cast<TbUint8*>(ptr);
		TB_SAFE_DELETE_ARRAY( buf );
	}	

private:
	TbImageAllocatorDefault();
	~TbImageAllocatorDefault();
};

}

#endif