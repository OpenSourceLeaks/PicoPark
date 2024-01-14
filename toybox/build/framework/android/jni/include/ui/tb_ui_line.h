/*!
 * ライン
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_LINE_H_
#define _INCLUDED_TB_UI_LINE_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <graphics/sprite/tb_sprite.h>
#include "ui/tb_ui_widget.h"

namespace toybox
{

class TbUiLine : public TbUiWidget
{
public:
    typedef TbUiWidget Super;
public:
    
    // コンストラクタ
    TbUiLine();
    
    // デストラクタ
    virtual ~TbUiLine();

public:

    // 生成時コールバック
    virtual void OnCreated();

    // 破棄時コールバック
    virtual void OnDeleted();

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltaSec );

private:

    toybox::TbSprite     m_Sprite;        // スプライト

};

}

#endif
