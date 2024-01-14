/*!
 * 画像クラス
 * @author Miyake_Shunsuke
 * @since 2014.10.16
 */

#ifndef _INCLUDED_TB_IMAGE_H_
#define _INCLUDED_TB_IMAGE_H_

#include <graphics/image/tb_image_types.h>

namespace toybox
{

class TbImage
{
public:

    enum Type
    {
        TYPE_INVALID , 
        TYPE_BMP , 
        TYPE_TGA , 
        TYPE_MAX
    };

public:
    
    // コンストラクタ
    TbImage() 
        : m_Type(TYPE_INVALID)
    {}

    // デストラクタ
    ~TbImage() {
        Reset();
    }

public:

    // 設定
    void Set( void* buffer , TbSizeT bufferSize , Type type );

    // リセット
    void Reset();

public:

    // 横幅取得
    TbUint32 GetWidth() const {
        if( TYPE_INVALID==m_Type) {
            return 0;
        }
        return m_ImportResult.width;
    }

    // 縦幅取得
    TbUint32 GetHeight() const {
        if( TYPE_INVALID==m_Type) {
            return 0;
        }
        return m_ImportResult.height;
    }

    // 色取得
    TbColor GetColor( TbUint32 x , TbUint32 y ) const;

private:
    Type                m_Type;
    TbImageImportResult m_ImportResult;
};

}

#endif