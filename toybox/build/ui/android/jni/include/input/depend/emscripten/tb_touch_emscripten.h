/*!
 * タッチ入力
 * @author teco
 */

#ifndef _INCLDUED_TB_TOUCH_EMSCRIPTEN_H_
#define _INCLDUED_TB_TOUCH_EMSCRIPTEN_H_

#include <base/math/tb_vector4.h>

namespace toybox
{

class TbTouchDepend
{
public:

    static const TbUint32 TOUCH_MAX = 10;

    struct PointInfo
    {
        PointInfo() : id(-1) , isFirst(TB_TRUE) , isPress(TB_FALSE) {}
        TbVector2 pos;
        TbVector2 futurePos;
        TbSint32  id;
        TbBool    isFirst;
        TbBool    isPress;
    };

    struct Info
    {
        Info() : window(nullptr) {}
        TbWindow* window;
        PointInfo pointInfo[TOUCH_MAX];
    };

public:

    //コンストラクタ
    TbTouchDepend( const TbTouchParameter& parameter );

    //デストラクタ
    virtual ~TbTouchDepend(void);

private:

    // セットアップ
    TbResult setup( const TbTouchParameter& parameter );

    // 数取得
    TbUint32 getCount() const { 
        return TB_ARRAY_COUNT_OF(m_TouchInfo.pointInfo); 
    }

private:

    Info            m_TouchInfo;  //!< 情報クラス

private:

    friend class TbTouch;

};

}

#endif