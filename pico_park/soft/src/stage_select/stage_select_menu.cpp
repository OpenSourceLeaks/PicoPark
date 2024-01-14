/*!
 * UI用イメージボックス
 * @author teco
 */

#include "fwd.h"
#include "stage_select/stage_select_menu.h"
#include "sequence/seq_manager.h"
#include <crayon/input/cr_input.h>
#include "root.h"

#include <framework/sub/tb_framework_simple.h>
#include <base/util/tb_bind.h>

static const TbFloat32 BUTTON_OFFSET_Y = -80.0f;
static const TbFloat32 BUTTON_INTERVAL_X = 20.0f;
static const TbFloat32 CHANGE_PAGE_SPEED = 12.0f;
static const TbRectF32 IMAGE_UV(0.0f,0.0f,1.0f,315.0f/512.0f);

/*!
 * コンストラクタ
 * @author teco
 */
StageSelectMenu::StageSelectMenu( TbSint32 initIndex , TbSint32 stageCount )
    : Super(TB_TRUE)
    , m_Phase(PHASE_IDLE)
    , m_PageIndex(0)
    , m_RequestPageIndex(-1)
    , m_StageIndex(initIndex)
{
    TbSint32 pageCount = (stageCount / Page::STAGE_MAX);
    TbBool lastPageItemCount = Page::STAGE_MAX;
    if( (stageCount%Page::STAGE_MAX) != 0 ) {
        pageCount += 1;
        lastPageItemCount = stageCount%Page::STAGE_MAX;
    }
    m_NullWidget[NULL_WIDGET_BUTTON_PARENT].Enter(this);
    m_NullWidget[NULL_WIDGET_ARROW_PARENT].Enter(this);
    m_NullWidget[NULL_WIDGET_ARROW_NULL].Enter(&m_NullWidget[NULL_WIDGET_ARROW_PARENT]);

    m_Page.PushBackDefault(pageCount);
    for( TbSint32 i = 0; i < pageCount; ++i ){
        TbSint32 itemCount = Page::STAGE_MAX;
        if( i == pageCount-1 ) {
            itemCount = lastPageItemCount;
        }
        m_Page[i].item.PushBackDefault(itemCount);
        for( TbSint32 b = 0; b < itemCount; ++b ) 
        {
            // ボタン
            TbSint32 stageIndex = (i*Page::STAGE_MAX) + b;
            TbStaticString32 name = TbStaticString32::Format("%02d",stageIndex+1);
            m_Page[i].item[b].button.Enter(&m_NullWidget[NULL_WIDGET_BUTTON_PARENT]);
            m_Page[i].item[b].button.SetText(name.GetBuffer());
            m_Page[i].item[b].button.SetPressedCallback(TbBind(&Self::onDecideStage,this,stageIndex));
            m_Page[i].item[b].button.SetBasePos(TbUiWidget::BASE_POS_CENTER_DOWN);
            m_Page[i].item[b].button.SetFocusChangedCallback(TbBind(&Self::onFocusChangedStage,this,stageIndex));
            m_Page[i].item[b].button.SetEnable(TB_FALSE);

            float buttonWidth = m_Page[i].item[0].button.GetWidth();
            float totalWidth = m_Page[i].item.GetCount()*(BUTTON_INTERVAL_X+buttonWidth)-BUTTON_INTERVAL_X;
            TbVector2 pos( -totalWidth/2.0f+(buttonWidth+BUTTON_INTERVAL_X)*b , BUTTON_OFFSET_Y );
            m_Page[i].item[b].pos = pos;
            m_Page[i].item[b].button.SetPos(pos);

            // テクスチャ
            TbStaticString32 stageName = SeqManager::GetInstance().GetStageName(stageIndex);
            TbStaticString64 path = TbStaticString64::Format("tex/tex_%s.tga",stageName.GetBuffer());
            m_Page[i].item[b].tex.Create(path.GetBuffer(),0,TB_FILE_ASYNC);
            m_Page[i].item[b].button.SetVisible(TB_FALSE);
        }
    }

    SetPlayerBit(TB_UI_PLAYER_BIT_MAX);

    m_TextBox.Enter(this);
    m_TextBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_TextBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    m_TextBox.SetPos(0.0f,-24.0f);

    m_BestTimeTextBox.Enter(this);
    m_BestTimeTextBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_BestTimeTextBox.SetBasePos(TbUiWidget::BASE_POS_CENTER);
    m_BestTimeTextBox.SetPos(0.0f,190.0f);

    m_StageImageBox.Enter(this);
    m_StageImageBox.SetAlignX(TB_UI_ALIGN_X_CENTER);
    m_StageImageBox.SetBasePos(TbUiWidget::BASE_POS_CENTER_TOP);
    {
#if TB_PLATFORM_IS_WIN
        TbFloat32 sizeScale = 1.5f;
#else
        TbFloat32 sizeScale = 1.35f;
#endif
        m_StageImageBox.SetSize(512.0f*1.35f,315.0f*1.35f); // 1.5
        m_StageImageBox.SetPos(0.0f,48.0f);
        m_StageImageBox.SetEnableFrame(TB_TRUE);
    }

    if( 0 < m_Page.GetCount() )
    {
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowButton); ++i ){
            m_ArrowButton[i].Initilaize(static_cast<UiArrowButtonSkin::Type>(i+UiArrowButtonSkin::TYPE_BEGIN) );
            m_ArrowButton[i].Enter(&m_NullWidget[NULL_WIDGET_ARROW_PARENT]);
            m_ArrowButton[i].SetAlign(TB_UI_ALIGN_X_CENTER,TB_UI_ALIGN_Y_CENTER);
            m_ArrowButton[i].SetBasePos(TbUiWidget::BASE_POS_CENTER);
            m_ArrowButton[i].SetEnable(true);
            m_ArrowButton[i].SetEnablePress(TB_UI_INPUT_DEVICE_BUTTON,TB_FALSE);
        }
        m_ArrowButton[0].SetPos(-450.0f,-16.0f);
        m_ArrowButton[0].SetPressedCallback(TbBind(&Self::addItemIndex,this,TB_FALSE));
        m_ArrowButton[1].SetPos(450.0f,-16.0f);
        m_ArrowButton[1].SetPressedCallback(TbBind(&Self::addItemIndex,this,TB_TRUE));
    }

    SetStageIndex(m_StageIndex);
    setActivePage(m_PageIndex,TB_TRUE);
}

/*! 
 * デストラクタ
 * @author teco
 */
StageSelectMenu::~StageSelectMenu()
{
    ClearWidget();
}

/*! 
 * ステージ番号設定
 * @author teco
 */
void StageSelectMenu::setStageIndex( TbSint32 index )
{
    m_StageIndex = TbClamp<TbSint32>(index,0,SeqManager::GetInstance().GetStageCount()-1);
}

/*! 
 * ステージ番号設定
 * @author teco
 */
void StageSelectMenu::SetStageIndex( TbSint32 index )
{
    if( PHASE_IDLE != m_Phase ){
        return;
    }
    m_StageIndex = index;
    TbSint32 itemCount = m_Page[0].item.GetCount();
    TbSint32 pageIndex = m_StageIndex / itemCount;
    TbSint32 itemIndex = m_StageIndex % itemCount;
    setCurrentPage(pageIndex);
    m_Page[m_PageIndex].item[itemIndex].button.SetFocus();
}


// 
/*!
 * 定期処理
 * @author teco
 */
void StageSelectMenu::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsFocus() ){
        return;
    }

    switch( m_Phase )
    {
    case PHASE_IDLE:
        {
            // フォーカス切り替え
            if( CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_LEFT,GetPlayerBit()) ){
                addItemIndex(TB_FALSE);
            }else if( CrInput::IsAnyoneOnRepeat(CrInput::BUTTON_RIGHT,GetPlayerBit()) ){
                addItemIndex(TB_TRUE);
            }
        }
        break;
    case PHASE_PREV_PAGE:
        {
            TbFloat32 width = GetRegion().width;
            TbFloat32 offsetX = m_Page[m_PageIndex].offsetX + CHANGE_PAGE_SPEED;
            if( width <= offsetX ){
                offsetX = width;

                setActivePage(m_PageIndex,TB_FALSE);
                setCurrentPage(m_RequestPageIndex);
                m_RequestPageIndex = -1;
                m_Page[m_PageIndex].item.GetBack().button.SetFocus();
                m_Phase = PHASE_IDLE;
            }else{
                setPageOffsetX( m_PageIndex , offsetX );
            }
            setPageOffsetX( m_RequestPageIndex , offsetX-width );
        }
        break;
    case PHASE_NEXT_PAGE:
        {
            TbFloat32 width = GetRegion().width;
            TbFloat32 offsetX = m_Page[m_PageIndex].offsetX - CHANGE_PAGE_SPEED;
            if( offsetX <= -width ){
                offsetX = -width;

                setActivePage(m_PageIndex,TB_FALSE);
                setCurrentPage(m_RequestPageIndex);
                m_RequestPageIndex = -1;
                m_Page[m_PageIndex].item.GetFront().button.SetFocus();
                m_Phase = PHASE_IDLE;

            }else{
                setPageOffsetX( m_PageIndex , offsetX );
            }
            setPageOffsetX( m_RequestPageIndex , offsetX+width );
        }
        break;
    }

    TbBool isFocusArrow = TB_FALSE;
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowButton); ++i ){
        if( m_ArrowButton[i].IsCursorOver() ){
            isFocusArrow = TB_TRUE;
            break;
        }
    }
    if( !isFocusArrow ) {
        m_NullWidget[NULL_WIDGET_ARROW_NULL].SetFocus();
    }

}

/*!
 * 生成時コールバック
 * @author teco
 */
void StageSelectMenu::OnCreated()
{
    SetFullscreen(TB_TRUE);
    
    m_PageIndex = 0;
    m_StageIndex = 0;
    m_Page[m_PageIndex].item[0].button.SetFocus();
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void StageSelectMenu::OnDeleted()
{
}

/*!
 * ステージ決定
 * @author teco
 */
void StageSelectMenu::onDecideStage( TbSint32 stageIndex )
{
    SeqManager::GetInstance().RequestSequence(SEQ_STAGE,stageIndex);
}

/*!
 * フォーカス変更
 * @author teco
 */
void StageSelectMenu::onFocusChangedStage( TbSint32 stageIndex )
{
    setStageIndex(stageIndex);

    StageItem* item = getStageItem( stageIndex );
    if( item ){
        TbStaticString256 text;
        m_StageImageBox.SetImage( item->tex.GetTexture(),IMAGE_UV);
        TbUint32 playerCount = Root::GetInstance().GetPlayerCount();

        switch( SeqManager::GetInstance().GetStageType(stageIndex) )
        {
        case CMN_STAGE_TYPE_BEST_TIME:
            {
                TbUint32 bestTime = SaveFile::GetInstance().GetBestMillSec(stageIndex,playerCount);
                if(SaveDataStage::BEST_TIME_INVALID == bestTime  ){
                    text.SetFormatedString("%u PLAYERS BEST TIME --:--",playerCount);
                }else{
                    TbUint32 playSec = bestTime/1000;
                    TbSint32  minute = playSec/60;
                    TbSint32  sec = playSec%60;
                    text.SetFormatedString("%u PLAYERS BEST TIME %02d:%02d",playerCount,minute,sec);
                }
            }
            break;
        case CMN_STAGE_TYPE_BATTLE:
            text.SetFormatedString("BATTLE STAGE");
            break;
        case CMN_STAGE_TYPE_SCORE:
            text.SetFormatedString("%u PLAYERS BEST SCORE --",playerCount);
            break;
        }
        m_BestTimeTextBox.SetText(text.GetBuffer());
    }
}

/*!
 * ステージアイテム取得
 * @author teco
 */
StageSelectMenu::StageItem* StageSelectMenu::getStageItem( TbSint32 stageIndex )
{
    TbUint32 pageIndex = stageIndex / Page::STAGE_MAX;
    TbUint32 itemIndex = stageIndex % Page::STAGE_MAX;
    if( TB_VERIFY( pageIndex < m_Page.GetCount() && itemIndex < m_Page[pageIndex].item.GetCount() ) ){
        return &m_Page[pageIndex].item[itemIndex];
    }
    return nullptr;
}

/*!
 * 前のページ
 * @author teco
 */
void StageSelectMenu::requestPrevPage()
{
    if( 1 == m_Page.GetCount() ){
        TbSint32 index = m_Page[m_PageIndex].item.GetCount()-1;
        m_Page[m_PageIndex].item[index].button.SetFocus();
    }else{
        if( m_PageIndex == 0 ){
            m_RequestPageIndex = m_Page.GetCount()-1;
        }else{
            m_RequestPageIndex = (m_PageIndex-1);
        }
        setActivePage(m_RequestPageIndex,TB_TRUE);
        TbFloat32 width = GetRegion().width;
        setPageOffsetX( m_RequestPageIndex , -width );
        m_Phase = PHASE_PREV_PAGE;
    }
}
    
/*!
 * 次のページ
 * @author teco
 */
void StageSelectMenu::requestNextPage()
{
    if( 1 == m_Page.GetCount() ){
        m_Page[m_PageIndex].item[0].button.SetFocus();
    }else{
        m_RequestPageIndex = (m_PageIndex+1) % m_Page.GetCount();
        setActivePage(m_RequestPageIndex,TB_TRUE);
        TbFloat32 width = GetRegion().width;
        setPageOffsetX( m_RequestPageIndex , width );
        m_Phase = PHASE_NEXT_PAGE;
    }
}

/*!
 * ページのアクティブ設定
 * @author teco
 */
void StageSelectMenu::setActivePage( TbSint32 page , TbBool isActive )
{
    if( 0 <= page && page < static_cast<TbSint32>(m_Page.GetCount()) ) {
        TbForIterator( it , m_Page[page].item ) {
            it->button.SetVisible(isActive);
            if( isActive ) {
                it->tex.Activate();
            }else{
                it->tex.Deactivate();
            }
        }
    }
}

/*!
 * ページのオフセットX座標
 * @author teco
 */
void StageSelectMenu::setPageOffsetX( TbSint32 page , TbFloat32 x )
{
    if( 0 <= page && page < static_cast<TbSint32>(m_Page.GetCount()) ) {
        TbForIterator( it , m_Page[page].item ) {
            TbVector2 pos = it->pos;
            pos.Add(x,0.0f);
            it->button.SetPos(pos);
        }
        m_Page[page].offsetX = x;
    }
}

/*!
 * ページ設定
 * @author teco
 */
void StageSelectMenu::setCurrentPage( TbSint32 index )
{
    TB_RESQUE(0<=index&&index<static_cast<TbSint32>(m_Page.GetCount()));
    TbForIterator( ite , m_Page[m_PageIndex].item ){
        ite->button.SetEnable(TB_FALSE);
    }
    m_PageIndex = index;
    TbForIterator( ite , m_Page[index].item ){
        ite->button.SetEnable(TB_TRUE);
    }
#if CMN_ENABLE_BUILD_TETRIS
    m_TextBox.SetText("10 CO-OP TE??IS");
#else
    TbStaticString512 text;
    if( 1 == m_Page.GetCount() ){
        text = "STAGE SELEC";
    }else{
        text.SetFormatedString("STAGE SELECT (%d/%u)",index+1,m_Page.GetCount());
    }
    m_TextBox.SetText(text.GetBuffer());
#endif
}

/*!
 * アイテムインデックスカウントアップ
 * @author teco
 */
void StageSelectMenu::addItemIndex( TbBool isCountUp )
{
    if( PHASE_IDLE != m_Phase ){
        return;
    }
    TbSint32 itemCount = m_Page[m_PageIndex].item.GetCount();
    TbSint32 itemIndex = m_StageIndex % Page::STAGE_MAX;
    if( isCountUp ){
        TbSint32 index = (itemIndex+1);
        if( index < itemCount ) { 
            m_Page[m_PageIndex].item[index].button.SetFocus();
        }else{
            if( m_PageIndex < m_Page.GetCount()-1 ){
                requestNextPage();
            }
        }
    }else{
        TbSint32 index = 0;
        if( 0 == itemIndex ){
            if( m_PageIndex != 0 ){
                requestPrevPage();
            }
        }else{
            index = (itemIndex-1);
            m_Page[m_PageIndex].item[index].button.SetFocus();
        }
    }
}

