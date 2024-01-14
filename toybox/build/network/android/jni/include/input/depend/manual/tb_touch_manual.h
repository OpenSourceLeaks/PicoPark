/*!
 * タッチ入力
 * @author teco
 */

#ifndef _INCLDUED_TB_TOUCH_MANUAL_H_
#define _INCLDUED_TB_TOUCH_MANUAL_H_

#include <base/math/tb_vector4.h>

namespace toybox
{

class TbTouchDepend
{
public:

    static const TbUint32 TOUCH_MAX = 8;
    static const TbSint64 ID_INVALID = -1;

    struct PointInfo
    {
        PointInfo() : id(ID_INVALID) , isOnPre(TB_TRUE) , isOn(TB_FALSE) , isOnFuture(TB_FALSE) {}
        TbSint64  id;
        TbVector2 pos;
        TbVector2 radius;
        TbVector2 futurePos;
        TbVector2 futureRadius;
        TbBool    isOnPre;
        TbBool    isOn;
        TbBool    isOnFuture;
    };

    struct Info
    {
        Info() : window(nullptr) {}
        TbWindow*  window;
        PointInfo pointInfo[TOUCH_MAX];
    };

public:

    //コンストラクタ
    TbTouchDepend( const TbTouchParameter& parameter );

    //デストラクタ
    virtual ~TbTouchDepend(void);

public:
    
    // ボタン押した
    void Press( TbSint64 id ){
        PointInfo* info = getInfo(id,TB_TRUE);
        if( info ){
            info->isOnFuture = TB_TRUE;
        }
    }

    // ボタン離した
    void Release( TbSint64 id ){
        PointInfo* info = getInfo(id,TB_FALSE);
        if( info ) {
            // 即時にはIDを無効化しない
            info->isOnFuture = TB_FALSE;
        }
    }

    // 座標設定
    void SetPos( TbSint64 id , const TbVector2& pos , const TbVector2& radius ) {
        PointInfo* info = getInfo(id,TB_FALSE);
        if( info ){
            info->futurePos = pos;
            info->futureRadius = radius;
        }
    }
    
private:

    // セットアップ
    TbResult setup( const TbTouchParameter& parameter );

    // 数取得
    TbUint32 getCount() const { 
        return TB_ARRAY_COUNT_OF(m_TouchInfo.pointInfo); 
    }

    // 情報取得
    PointInfo* getInfo( TbSint64 id , TbBool isEnableInsert ) {
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_TouchInfo.pointInfo); ++i ) {
            if( m_TouchInfo.pointInfo[i].id == id ) {
                return &m_TouchInfo.pointInfo[i];
            }
        }
        if( isEnableInsert ){
            // 新規で追加可能
            for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_TouchInfo.pointInfo); ++i ) {
                if( ID_INVALID == m_TouchInfo.pointInfo[i].id ) {
                    m_TouchInfo.pointInfo[i].id = id;
                    return &m_TouchInfo.pointInfo[i];
                }
            }
        }
        return nullptr;
    }

private:

    Info            m_TouchInfo;  //!< 情報クラス

private:

    friend class TbTouch;

};

}

#endif