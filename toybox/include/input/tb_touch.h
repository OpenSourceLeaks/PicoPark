/*!
 * タッチ入力
 * @author teco
 */

#ifndef _INCLDUED_TB_TOUCH_H_
#define _INCLDUED_TB_TOUCH_H_

#include <base/util/tb_singleton.h>
#include <base/math/tb_geometry.h>
#include <base/math/tb_geometry_util.h>
#include <base/math/tb_vector2.h>

namespace toybox
{

class TbWindow;

enum TbTouchPosType
{
    TB_TOUCH_POS_TYPE_GLOBAL, // グローバルなマウス位置
    TB_TOUCH_POS_TYPE_WINDOW, // フォーカスしているウィンドウ上での位置
    TB_TOUCH_POS_TYPE_WINDOW_RATIO, // フォーカスしているウィンドウ上での位置(比率)
};

// 生成パラメータ
struct TbTouchParameter
{
    TbWindow* window; // 対応するウィンドウ 
};

}

#include "./depend/tb_touch_depend.h"

namespace toybox
{

class TbTouch : public TbSingletonRef<TbTouch>
{
public:

    //コンストラクタ
    TbTouch( TbTouchParameter* parameter , TbSint32 count );

    //デストラクタ
    virtual ~TbTouch(void);

public:

    // 有効なタッチ数
    static TbUint32 GetCount() { return GetInstance().getCount(); }

    //トリガーしたか
    static TbBool IsPress( const TbUint16 index=0){return GetInstance().isPress(index);}

    //離したか
    static TbBool IsRelease(const TbUint16 index=0){return GetInstance().isRelease(index);}

    //押しているか
    static TbBool IsOn(const TbUint16 index=0){return GetInstance().isOn(index);}

    // ウィンドウ上での位置
    static TbResult GetPos( TbVector2* outPos,const TbTouchPosType posType=TB_TOUCH_POS_TYPE_GLOBAL,const TbUint32 index=0) {
        return GetInstance().getPos( outPos , posType , index ); 
    }

    // 半径取得
    static TbResult GetRadius( TbVector2* outRadius , const TbTouchPosType posType=TB_TOUCH_POS_TYPE_GLOBAL , const TbUint32 index=0) {
        return GetInstance().getRadius( outRadius , posType , index ); 
    }

    //オーバーしているか
    static TbBool IsOver( const TbRectF32& rect , TbBool isEnableRadius , const TbTouchPosType posType=TB_TOUCH_POS_TYPE_GLOBAL, TbUint16 index=0){
        return GetInstance().isOver(rect,isEnableRadius,posType,index);
    }

public:

    //更新
    void Update();
    
public: // 依存部分取得
    
    TbTouchDepend& GetDepend() {
        return m_Depend;
    }

    const TbTouchDepend& GetDepend() const {
        return m_Depend;
    }

private:

    // 有効なキーボード数
    TbUint32 getCount() const;
    // トリガーしたか
    TbBool isPress(const TbUint32 index=0) const;
    // 離したか
    TbBool isRelease(const TbUint32 index=0) const;
    // 押しているか
    TbBool isOn(const TbUint32 index=0) const;
    // 位置
    TbResult getPos( TbVector2* outPos,const TbTouchPosType posType,const TbUint32 index=0) const;
    // 半径
    TbResult getRadius( TbVector2* outRadius,const TbTouchPosType posType,const TbUint32 index=0) const;

    //タッチオーバーしているか
    TbBool isOver( const TbRectF32& rect , TbBool enableRadius , const TbTouchPosType posType=TB_TOUCH_POS_TYPE_GLOBAL, TbUint16 index=0) const 
    {
        TbVector2 pos;
        TbVector2 radius;
        if( TB_SUCCEEDED(getPos( &pos , posType , index )) && TB_SUCCEEDED(getRadius( &radius , posType , index )) ){
            TbBool over = TB_FALSE;
            if( !enableRadius || radius.IsZero() ) {
                if( rect.x <= pos.GetX() && pos.GetX() < rect.x + rect.width &&
                    rect.y <= pos.GetY() && pos.GetY() < rect.y + rect.height )
                {
                    over =TB_TRUE;
                }
            }
            else
            {
                TbRectF32 touchRect( pos.GetX() - radius.GetX() ,
                                     pos.GetY() - radius.GetY() ,
                                     radius.GetX()*2.0f ,
                                     radius.GetY()*2.0f );
                over = TbIsGeometryContacted( rect , touchRect );
            }
            if( over )
            {
                return TB_TRUE;
            }
        }
        return TB_FALSE;
    }

private:

    TbTouchDepend m_Depend;     //!< 依存コード

};

}

#endif