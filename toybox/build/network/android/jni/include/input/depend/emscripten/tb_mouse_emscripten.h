/*!
 * パッド入力(emscripten)
 * @author teco
 */

#ifndef _INCLDUED_TB_MOUSE_EMSCRIPTEN_H_
#define _INCLDUED_TB_MOUSE_EMSCRIPTEN_H_

#include <base/math/tb_vector4.h>

namespace toybox
{

/*!
 * マウス依存クラス
 */
class TbMouseDepend
{
public:

    struct Info
    {
        Info() : window(nullptr) , now(0) , old(0) , future(0) 
        {}
        
        TbWindow* window;
        
        TbVector2 pos;
        TbUint32 now;              //現フレーム入力情報
        TbUint32 old;              //前フレーム入力情報

        TbVector2 futurePos;
        TbUint32 future;           //次フレーム入力情報
    };

public:

    //コンストラクタ
    TbMouseDepend( TbMouseParameter* parameter , TbSint32 count );

    //デストラクタ
    virtual ~TbMouseDepend(void);

private:

    // セットアップ
    TbResult setup( TbMouseParameter* parameter );

    // 情報取得
    Info* getInfo( TbUint32 index ) {
        if( 0 == index ) {
            return &m_MouseInfo;
        }
        return nullptr;
    }

    // 情報取得
    const Info* getInfo( TbUint32 index ) const {
        if( 0 == index ) {
            return &m_MouseInfo;
        }
        return nullptr;
    }

private:

    Info                                    m_MouseInfo;  //!< 情報クラス

private:

    friend class TbMouse;

};

}

#endif