/*!
 * Stage3Dのインデックスバッファ依存コード
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */

#ifndef _INCLUDED_TB_INDEX_BUFFER_S3D_H_
#define _INCLUDED_TB_INDEX_BUFFER_S3D_H_

namespace toybox
{

/*!
 * 頂点バッファ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
struct TbIndexBufferDepend
{
    static TbBool IsSupportedFormat( TbIndexBufferFormat format ) 
    {
        // 4バイト以外は認めない
        return TB_INDEX_BUFFER_FORMAT_U32 == format;
    }
    AS3::ui::flash::display3D::IndexBuffer3D indexBuffer;
};

}

#endif