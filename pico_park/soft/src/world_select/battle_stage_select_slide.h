/*!
 * バトルステージ選択用ページ
 * @author teco
 */

#ifndef _INCLUDED_BATTLE_STAGE_SELECT_SLIDE_H_
#define _INCLUDED_BATTLE_STAGE_SELECT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"
#include "ui/ui_message_window.h"

class BattleStageSelectPage : public CmnSlidePage
{
public:
    static const TbUint32 STAGE_MAX_IN_GROUP = 6;
    typedef BattleStageSelectPage Self;
public:

    // コンストラクタ
    BattleStageSelectPage();

    // デストラクタ
    virtual ~BattleStageSelectPage() {}

public:

    // 初期化
    void Initialize(TbSint32 begin , TbBool hasPrev, TbBool hasNext);

    // 確認ウィン小津
    void SetConfirmWindow(UiMessageWindow* confirmWindow) {
        m_ConfirmWindow = confirmWindow;
    }

public:

    // 選択ワールド取得
    TbSint32 GetStageIndex() const {
        return getStageIndex(m_FocusPoint.x, m_FocusPoint.y) + m_StageRange.begin;
    }

    // 選択したか
    TbBool IsSelected() const {
        return  m_IsSelected;
    }

public:

    // 定期処理
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // アクティブ状態が変化した
    void OnChangeActive( TbBool isActive ) TB_OVERRIDE;

    // 描画処理
    void OnDraw(const TbVector2& offset) TB_OVERRIDE;

private:

    struct Group
    {
        CmnSimpleSprite sprite[STAGE_MAX_IN_GROUP];
    };

private:

    // カーソル移動
    TbBool moveCursor( TbSint32 x , TbSint32 y );

    // 位置からステージインデックス取得
    TbSint32 getStageIndex( TbSint32 x, TbSint32 y ) const;

    // ステージインデックスから座標取得
    TbPointS32 getPoint( TbSint32 stageIndex ) const;

private:
    TbRangeS32                      m_StageRange;
    TbBool                          m_HasPrev;
    TbBool                          m_HasNext;
    TbUint32                        m_StageWidth;
    TbUint32                        m_StageHeight;
    TbBool                          m_IsSelected;
    TbBool                          m_IsConfirming;
    Group                           m_Group;
    CmnCursorSprite                 m_CursorSprite;
    TbPointS32                      m_FocusPoint;
    UiMessageWindow*                m_ConfirmWindow;
};

class BattleStageSelectSlide : public CmnSlide
{
	typedef CmnSlide Super;
    typedef BattleStageSelectSlide Self;
public:

	// コンストラクタs
    BattleStageSelectSlide();

	// デストラクタ
	virtual ~BattleStageSelectSlide();

public:

    // ステージID取得
    TbUint32 GetStageId() const;

protected:

    // 定期処理
    virtual void OnUpdate();

    // 描画
    virtual void OnDraw();

    // ページ変更
    virtual void OnChangingPage(TbBool isNext) TB_OVERRIDE;

    // 次のページ取得
    virtual CmnSlidePage* GetNextPage() TB_OVERRIDE;

private:

    // 確認ウィンドウのコールバック
    void onDecidedBackConfirm( UiMessageWindow::Result result );

private:
    TbUint32                                m_StageIndex;
    TbStaticArray<BattleStageSelectPage,10>	m_BattleStageSelectPage;
    TbUint32                                m_BackConfirmSleep;
    UiMessageWindow                         m_BackConfirmWindow;
    UiMessageWindow                         m_ConfirmWindow;
};

#endif