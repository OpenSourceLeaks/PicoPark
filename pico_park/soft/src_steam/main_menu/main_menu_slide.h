/*!
 * メインメニュー用ページ
 * @author teco
 */

#ifndef _INCLUDED_MAIN_MENU_PAGE_H_
#define _INCLUDED_MAIN_MENU_PAGE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"

class MainMenuModeSelectPage : public CmnSlidePage
{
public:

    // コンストラクタ
    MainMenuModeSelectPage();

    // デストラクタ
    virtual ~MainMenuModeSelectPage() {}

public:

    // 選択インデックス取得
    TbSint32 GetSelectedIndex() const {
        return m_Index;
    }

public:

    // 定期処理
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // 描画処理
    void Draw(const TbVector2& pos) TB_OVERRIDE;

private:

	static const TbSint32 MODE_MAX = 2;

private:
    TbSint32		m_Index;
    CmnCursorSprite	m_CursorSprite;
};

class MainMenuPlayerSelectPage : public CmnSlidePage
{
public:

    // コンストラクタ
    MainMenuPlayerSelectPage();

    // デストラクタ
    virtual ~MainMenuPlayerSelectPage() {}

public:

    // プレイヤー人数取得
    TbSint32 GetPlayerCount() const {
        return m_PlayerCount;
    }

public:

    // 定期処理
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // 描画処理
    void Draw(const TbVector2& pos) TB_OVERRIDE;

private:

    enum
    {
		ARROW_DEFAULT , 
		ARROW_SELECT ,
		ARROW_MAX
    };

private:

    TbSint32		m_PlayerCount;
	TbSint32		m_ArrowSelect;
	CmnSimpleSprite	m_ArrowSprite[ARROW_MAX];

};

class MainMenuSlide : public CmnSlide
{
	typedef CmnSlide Super;
public:

	// コンストラクタs
    MainMenuSlide();

	// デストラクタ
	virtual ~MainMenuSlide();

public:

	// 選択モード取得
    TbSint32 GetMode() {
		return m_ModeSelectPage.GetSelectedIndex();
    }

	// プレイ人数取得
    TbSint32 GetPlayerCount() {
		return m_PlayerSelectPage.GetPlayerCount();
    }

private:

    enum
    {
		PAGE_MODE_SELECT ,
		PAGE_PLAYER_SELECT ,
		PAGE_MAX
    };

private:
    MainMenuModeSelectPage		m_ModeSelectPage;
    MainMenuPlayerSelectPage	m_PlayerSelectPage;
};

#endif