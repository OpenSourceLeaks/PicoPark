/*!
 * @file
 * @brief TGA画像パーサークラスのヘッダーファイル
 * @author Miyake Shunsuke
 * @since 2011.5.5
 */

#ifndef _INCLUDED_TB_IMAGE_TGA_H_
#define _INCLUDED_TB_IMAGE_TGA_H_

#include <graphics/image/tb_image_types.h>

namespace toybox
{

/*!
 * TGA画像クラス
 * @author Miyake Shunsuke
 * @since 2010.5.5
 */
class TbImageTga
{
private: // インスタンス生成禁止

    // コンストラクタ
    TbImageTga();

    // デストラクタ
    ~TbImageTga();

public:

    // ファイルからピクセル配列を生成
    static TbBool Import( TB_OUT TbImageImportResult& result , const TbImageImportInfo& info , const TbImageAllocFunc allocFunc = nullptr );

    // ピクセル配列から画像ファイル出力
    static TbBool Export( TB_OUT TbImageExportResult& result , const TbImageExportInfo& info , const TbImageAllocFunc allocFunc = nullptr );

};

}

#endif