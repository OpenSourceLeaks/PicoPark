/*!
 * @file
 * @brief マウス入力
 * @author Miyake Shunsuke
 */

#ifndef _INCLDUED_TB_MOUSE_H_
#define _INCLDUED_TB_MOUSE_H_

#include <base/util/tb_singleton.h>
#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>

namespace toybox
{

class TbWindow;

enum TbMouseButton
{
    TB_MOUSE_LEFT,
    TB_MOUSE_RIGHT,
    TB_MOUSE_CENTER,
};

enum TbMousePosType
{
    TB_MOUSE_POS_TYPE_GLOBAL, // グローバルなマウス位置
    TB_MOUSE_POS_TYPE_WINDOW, // フォーカスしているウィンドウ上での位置
    TB_MOUSE_POS_TYPE_WINDOW_RATIO, // フォーカスしているウィンドウ上での位置(比率)
//    TB_MOUSE_POS_TYPE_MULTI_WINDOW, // マルチウィンドウを一つのウィンドウとして見た時の位置
//    TB_MOUSE_POS_TYPE_MULTI_WINDOW_RATIO, // マルチウィンドウを一つのウィンドウとして見た時の位置(比率)
};

// 生成パラメータ
struct TbMouseParameter
{
    TbWindow* window; // 対応するウィンドウ 
};

#define TB_MOUSE_LEFT_BIT TB_BIT(TB_MOUSE_LEFT)
#define TB_MOUSE_RIGHT_BIT TB_BIT(TB_MOUSE_RIGHT)
#define TB_MOUSE_CENTER_BIT TB_BIT(TB_MOUSE_CENTER)

}

#include "./depend/tb_mouse_depend.h"

namespace toybox
{

/*!
 * マウスクラス
 * @author Miyake Shunsuke
 */
class TbMouse : public TbSingletonRef<TbMouse>
{
public:

    //コンストラクタ
    TbMouse( TbMouseParameter* parameter , TbSint32 count );

    //デストラクタ
    virtual ~TbMouse(void);

public:

    // 有効なキーボード数
    static TbUint32 GetCount() { return GetInstance().getCount(); }
    //トリガーしたか
    static TbBool IsPress(const TbUint32 buttonBit,const TbUint16 index=0){return GetInstance().isPress(buttonBit,index);}
    //どれかトリガーしたか
    static TbBool IsPressAny(const TbUint32 buttonBit,const TbUint16 index=0){return GetInstance().isPressAny(buttonBit,index);}
    //離したか
    static TbBool IsRelease(const TbUint32 buttonBit,const TbUint16 index=0){return GetInstance().isRelease(buttonBit,index);}
    //どれか離したか
    static TbBool IsReleaseAny(const TbUint32 buttonBit,const TbUint16 index=0){return GetInstance().isReleaseAny(buttonBit,index);}
    //押しているか
    static TbBool IsOn(const TbUint32 buttonBit,const TbUint16 index=0){return GetInstance().isOn(buttonBit,index);}
    //どれか押しているか
    static TbBool IsOnAny(const TbUint32 buttonBit,const TbUint16 index=0){return GetInstance().isOnAny(buttonBit,index);}
    //押されているボタン情報
    static TbBool GetOn(const TbUint16 index=0){return GetInstance().getOn(index);}
    // ホイールの移動量取得
    static TbSint32 GetWheelDelta(const TbUint16 index = 0) { return GetInstance().getWheelDelta(index); };
    // ウィンドウ上での位置
    static TbResult GetPos( TbVector2* outPos,const TbMousePosType posType=TB_MOUSE_POS_TYPE_GLOBAL,const TbUint32 index=0) { return GetInstance().getPos( outPos , posType , index ); }
    //マウスオーバーしているか
    static TbBool IsOver( const TbRectF32& rect , const TbMousePosType posType=TB_MOUSE_POS_TYPE_GLOBAL, TbUint16 index=0){return GetInstance().isOver(rect,posType,index);}
    // 動いているか
    static TbBool IsMoving(TbVector2* outMov = nullptr , const TbUint16 index=0 ){return GetInstance().isMoving(index,outMov);}

public:

    //更新
    void Update();

private:

    // 有効なキーボード数
    TbUint32 getCount() const;
    // トリガーしたか
    TbBool isPress(const TbUint32 buttonBit,const TbUint32 index=0) const;
    // トリガーしたか
    TbBool isPressAny(const TbUint32 buttonBit,const TbUint32 index=0) const;
    // 離したか
    TbBool isRelease(const TbUint32 buttonBit,const TbUint32 index=0) const;
    // 離したか
    TbBool isReleaseAny(const TbUint32 buttonBit,const TbUint32 index=0) const;
    // 押しているか
    TbBool isOn(const TbUint32 buttonBit,const TbUint32 index=0) const;
    // どれか押しているか
    TbBool isOnAny(const TbUint32 buttonBit,const TbUint32 index=0) const;
    // 動いているか
    TbBool isMoving(const TbUint32 index=0, TbVector2* outMov = nullptr) const;
    // 押されている情報取得
    TbUint32 getOn(const TbUint32 index=0);
    // ウィンドウ上での位置
    TbResult getPos( TbVector2* outWindowPos,const TbMousePosType posType=TB_MOUSE_POS_TYPE_GLOBAL,const TbUint32 index=0) const;
    // ホイールの移動量取得
    TbSint32 getWheelDelta(const TbUint32 index = 0) const;
    //マウスオーバーしているか
    TbBool isOver( const TbRectF32& rect , const TbMousePosType posType=TB_MOUSE_POS_TYPE_GLOBAL, TbUint16 index=0) const 
    {
        TbVector2 pos;
        if( TB_SUCCEEDED(getPos( &pos , posType , index )) ){
            if( rect.x <= pos.GetX() && pos.GetX() < rect.x + rect.width &&
                rect.y <= pos.GetY() && pos.GetY() < rect.y + rect.height )
            {
                return TB_TRUE;
            }        }
        return TB_FALSE;
    }

private:

    TbMouseDepend m_Depend;     //!< 依存コード

};

}

#endif