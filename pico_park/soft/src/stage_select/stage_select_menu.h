/*!
 * ステージセレクトメニュー
 * @author teco
 * @since 2014.09.30
 */

#ifndef _INCLUDED_STAGE_SELECT_MENU_H_
#define _INCLUDED_STAGE_SELECT_MENU_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <graphics/render/3dapi/tb_texture.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <graphics/resource/tb_resource_texture.h>
#include "ui/ui_button.h"
#include "ui/ui_arrow_button.h"

#include <ui/tb_ui_window.h>
#include <ui/tb_ui_widget.h>
#include <ui/tb_ui_image_box.h>
#include <ui/tb_ui_text_box.h>

class StageSelectMenu : public TbUiWindow
{
public:

    typedef TbUiWindow Super;
    typedef StageSelectMenu Self;

public:
    
    // コンストラクタ
    StageSelectMenu( TbSint32 initIndex , TbSint32 stageCount );
    
    // デストラクタ
    virtual ~StageSelectMenu();

public:

    // ステージ番号設定
    void SetStageIndex( TbSint32 index );

public:

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltaSec );

    // 生成時コールバック
    virtual void OnCreated();

    // 破棄時コールバック
    virtual void OnDeleted();

private:

    struct StageItem
    {
        UiButton                button;
        TbResourceHandleTexture tex;
        TbVector2               pos;
    };

    struct Page
    {
        Page() : offsetX(0.0f) {}
        static const TbSint32               STAGE_MAX = 10;
        TbStaticArray<StageItem,STAGE_MAX>  item;                 // ボタン
        TbFloat32                           offsetX;
    };

    enum Phase
    {
        PHASE_IDLE ,
        PHASE_PREV_PAGE = 10 ,
        PHASE_NEXT_PAGE = 20 ,
    };
    
    enum NullWidgetType
    {
        NULL_WIDGET_BUTTON_PARENT , 
        NULL_WIDGET_ARROW_PARENT , 
        NULL_WIDGET_ARROW_NULL , 
        NULL_WIDGET_MAX
    };

private:

    // ステージ決定
    void onDecideStage( TbSint32 stageIndex );

    // フォーカス変更
    void onFocusChangedStage( TbSint32 stageIndex );

    // ステージアイテム取得
    StageItem* getStageItem( TbSint32 stageIndex );

    // 前のページ
    void requestPrevPage();
    
    // 次のページ
    void requestNextPage();

    // ページのアクティブ設定
    void setActivePage( TbSint32 page , TbBool isActive );
    
    // ページのオフセットX座標
    void setPageOffsetX( TbSint32 page , TbFloat32 x );

    // ページ設定
    void setCurrentPage( TbSint32 index );

private:

    // アイテムインデックスカウントアップ
    void addItemIndex( TbBool isCountUp );

    // ステージ番号設定
    void setStageIndex( TbSint32 index );

private:

    TbBitArray32                m_BitArray;
    TbSint32                    m_Phase;

    TbUiTextBox                 m_TextBox;
    TbUiTextBox                 m_BestTimeTextBox;
    TbUiImageBox                m_StageImageBox;
    TbUiWidget                  m_NullWidget[NULL_WIDGET_MAX];
    UiArrowButton               m_ArrowButton[2];

    TbSint32                    m_PageIndex;
    TbStaticArray<Page,10>      m_Page;
    TbSint32                    m_RequestPageIndex;

    TbSint32                    m_StageIndex;


};

#endif
