/*!
 * @file
 * @brief BMP画像パーサークラスのヘッダーファイル
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */

#ifndef _INCLUDED_TB_IMAGE_BMP_H_
#define _INCLUDED_TB_IMAGE_BMP_H_

#include <graphics/image/tb_image_types.h>

namespace toybox
{

/*!
 * BMP画像パーサークラス
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */
class TbImageBmp
{
private: // インスタンス生成禁止

    // コンストラクタ
    TbImageBmp();
    // デストラクタ
    ~TbImageBmp();

public:

    // ファイルからピクセル配列を生成
    static TbBool Import( TB_OUT TbImageImportResult& result , const TbImageImportInfo& info , const TbImageAllocFunc allocFunc = nullptr );

    // ピクセル配列から画像ファイル出力
    static TbBool Export( TB_OUT TbImageExportResult& result , const TbImageExportInfo& info , const TbImageAllocFunc allocFunc = nullptr );

};

}

#endif