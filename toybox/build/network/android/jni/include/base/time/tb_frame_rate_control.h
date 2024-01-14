/*!
 * フレームレート管理
 * @author teco
 */

#ifndef _INCLUDED_TB_FRAME_RATE_CONTROL_H_
#define _INCLUDED_TB_FRAME_RATE_CONTROL_H_

namespace toybox
{

class TbFrameRateControl
{
public:
    // コンストラクタ
    TbFrameRateControl() {}
    // デストラクタ
    ~TbFrameRateControl() {}
public:
    // 進行フレーム取得
    virtual TbFloat32 GetStepFrame( TbFloat32 defaultStep ) const = 0;
};

}

#endif