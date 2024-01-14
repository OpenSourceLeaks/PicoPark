/*!
 * メインメニュー用ページ
 * @author teco
 */

#ifndef _INCLUDED_WORLD_SELECT_SLIDE_H_
#define _INCLUDED_WORLD_SELECT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"

class WorldSelectPage : public CmnSlidePage
{
public:

    // コンストラクタ
    WorldSelectPage();

    // デストラクタ
    virtual ~WorldSelectPage() {}

public:

    // ワールド設定
    void SetWorld(TbSint32 world) {
        m_World = world;
    }

    // 選択ワールド取得
    TbSint32 GetWorld() const {
        return m_World;
    }

public:

    // 定期処理
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // 描画処理
    void Draw(const TbVector2& offset) TB_OVERRIDE;

private:

    static const TbUint32 STAGE_MAX_IN_GROUP = 6;
    struct Group
    {
        CmnSimpleSprite sprite[STAGE_MAX_IN_GROUP];
    };


private:
    TbSint32		                m_World;
    Group                           m_Group;
    CmnCursorSprite                 m_CursorSprite;
    TbPointS32                      m_FocusPoint;
};

class WorldStageSelectPage : public CmnSlidePage
{
public:

    // コンストラクタ
    WorldStageSelectPage();

    // デストラクタ
    virtual ~WorldStageSelectPage() {}

public:

    // ワールド設定
    void SetWorld( TbSint32 world );

    // ステージ設定
    void SetStage(TbUint32 stage) {
        if( stage < getStageCount() ){
            m_SelectedStage = stage;
        }
    }

    // ステージ取得
    TbUint32 GetSelectedStage() const {
        return m_SelectedStage;
    }

public:

    // 定期処理
    void OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // 描画処理
    void Draw(const TbVector2& offset) TB_OVERRIDE;

private:

    struct Stage
    {
        Stage()
            : ratio(0.0f)
        {
        }
        TbBool IsActive() const {
            return TB_FLOAT32_EQUAL(ratio,1.0f);
        }
        CmnSimpleSprite sprite;
        CmnWhiteSprite  line;
        TbFloat32       ratio;
    };

private:

    // ステージ数取得
    TbSint32 getStageCount() const;

private:

    TbUint32		    m_SelectedStage;
    TbSint32            m_World;
    CmnCursorSprite     m_CursorSprite;
    Stage*              m_Stage;

};

class WorldSelectSlide : public CmnSlide
{
	typedef CmnSlide Super;
public:

	// コンストラクタs
    WorldSelectSlide();

	// デストラクタ
	virtual ~WorldSelectSlide();

public:

    // ワールド設定
    void SetWorld(TbSint32 world) {
        m_WorldSelectPage.SetWorld(world);
        m_WorldStageSelectPage.SetWorld(world);
    }
        // ワールド取得
    TbUint32 GetWorld() const {
        return m_WorldSelectPage.GetWorld();
    }

    // ステージ設定
    void SetStage(TbSint32 stage) {
        m_WorldStageSelectPage.SetStage(stage);
    }

    // ステージ取得
    TbUint32 GetStageIndex() const {
        return m_WorldStageSelectPage.GetSelectedStage();
    }

protected:

    // ページ変更
    virtual void OnChangingPage(TbUint32 page) TB_OVERRIDE;

private:

    enum
    {
		PAGE_WORLD_SELECT ,
		PAGE_STAGE_SELECT ,
		PAGE_MAX
    };

private:
    WorldSelectPage	            m_WorldSelectPage;
    WorldStageSelectPage		m_WorldStageSelectPage;
};

#endif