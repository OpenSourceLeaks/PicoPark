/*!
 * メインメニュー用ページ
 * @author teco
 */

#ifndef _INCLUDED_WORLD_SELECT_SLIDE_H_
#define _INCLUDED_WORLD_SELECT_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/container/tb_static_array.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/slide/cmn_slide.h"
#include "common/cmn_simple_sprite.h"

class WorldSelectPage : public CmnSlidePage
{
public:
    typedef WorldSelectPage Self;
    typedef CmnSlidePage Super;
public:
    static const TbUint32 WORLD_MAX_IN_GROUP = 6;
    static const TbUint32 WORLD_WIDTH = 3;
    static const TbUint32 WORLD_HEIGHT = WORLD_MAX_IN_GROUP/ WORLD_WIDTH;
public:

    // コンストラクタ
    WorldSelectPage();

    // デストラクタ
    virtual ~WorldSelectPage();

public:

    TB_DEFINE_SUB_CLASS(Self, Super);

public:

    // 初期化
    void Initialize(TbSint32 begin , TbBool hasPrev, TbBool hasNext);

public:

    // ワールド全解放モード
    void SetOpenAllWorldMode();

    // ワールド設定
    void SetWorld(TbSint32 world) {
        TB_RESQUE(m_WorldRange.begin<=world&&world<m_WorldRange.end);
        TbSint32 worldIndex = world-m_WorldRange.begin;
        m_FocusPoint = getPoint(worldIndex);
    }

    // ワールド設定
    void SetWorld(TbSint32 x, TbSint32 y) {
        TbSint32 world = getWorldIndex(x, y) + m_WorldRange.begin;
        if(m_WorldRange.begin <= world&&world<m_WorldRange.end){
            SetWorld(world);
        }
    }

    // 選択ワールド取得
    TbSint32 GetWorld() const {
        return getWorldIndex(m_FocusPoint.x, m_FocusPoint.y) + m_WorldRange.begin;
    }

    // 選択したか
    TbBool IsSelected() const {
        return  m_IsSelected;
    }

    // フォーカス位置取得
    TbPointS32 GetFocusPoint() const {
        return m_FocusPoint;
    }

    // 選択可能か
    TbBool IsReadySelect() const {
        return GetPhase() < PHASE_OPEN_WORLD_CLEAR;
    }

    // 王冠更新
    void UpdateCrown();

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
        CmnSimpleSprite sprite[WORLD_MAX_IN_GROUP];
        CmnSimpleSprite crownSprie[WORLD_MAX_IN_GROUP];
    };

    enum
    {
        PHASE_DEFAULT = 0 ,
        PHASE_OPEN_WORLD_CLEAR = 10
    };

private:

    // カーソル移動
    TbBool moveCursor( TbSint32 x , TbSint32 y );

    // 位置からワールドインデックス取得
    TbSint32 getWorldIndex( TbSint32 x, TbSint32 y ) const;

    // ワールドインデックスから座標取得
    TbPointS32 getPoint( TbSint32 worldIndex ) const;

    // ワールド利用可能か
    TbBool   isEnableWorld( TbSint32 x, TbSint32 y ) const;

private:
    TbRangeS32                      m_WorldRange;
    TbBool                          m_HasPrev;
    TbBool                          m_HasNext;
    TbBool                          m_IsSelected;
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
    virtual ~WorldStageSelectPage();

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
    TbUint32 GetSelectedStageIndex() const {
        return m_SelectedStage;
    }

    // ステージ取得
    TbUint32 GetSelectedStageId() const;

    // ステージセレクト準備完了
    TbBool IsReadySelectStage() const {
        return PHASE_SELECT_STAGE <= GetPhase();
    }

    // 全ワールドオープン
    TbBool IsOpenAllWorld() const {
        return PHASE_OPEN_ALL_WORLD == GetPhase();
    }

public:

    // 定期処理
    Result OnUpdate(TbFloat32 deltasec) TB_OVERRIDE;

    // 描画処理
    void OnDraw(const TbVector2& offset) TB_OVERRIDE;

private:

    enum
    {
        PHASE_CHECK_CLEAR,
        PHASE_NEW_STAGE = 10,
        PHASE_WORLD_CLEAR = 20,
        PHASE_OPEN_ALL_WORLD = 25,
        PHASE_SELECT_STAGE = 30,
    };

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
        CmnSimpleSprite crownSprie;
        TbFloat32       ratio;
    };

private:

    // ステージ数取得
    TbSint32 getStageCount() const;

private:

    TbUint32		    m_SelectedStage;
    TbSint32            m_World;
    TbSint32            m_LatestStage;
    TbFloat32           m_WorldClearPosX;
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

    // 全ワールド解放モード設定
    void SetOpenAllWorldMode();

    // 復帰
    void Resume( TbSint32 stageId , TbBool isNextStage);

    // ワールド設定
    void SetWorld(TbSint32 world) {
        m_World = world;
        TbUint32 worldSelectIndex = getWorldSelectPage(world);
        m_WorldSelectPage[worldSelectIndex].SetWorld(world);
        m_WorldStageSelectPage.SetWorld(world);
    }

        // ワールド取得
    TbUint32 GetWorld() const {
        return m_World;
    }

    // ステージ設定
    void SetStageIndex(TbSint32 stageIndex) {
        m_WorldStageSelectPage.SetStage(stageIndex);
    }

    // ステージ取得
    TbUint32 GetStageIndex() const {
        return m_WorldStageSelectPage.GetSelectedStageIndex();
    }

    // ステージID取得
    TbUint32 GetStageId() const {
        return m_WorldStageSelectPage.GetSelectedStageId();
    }

    // BGM再生可能状態か
    TbBool IsReadyBgm() const {
        return m_IsReadyBgm;
    }

    // 全ワールドオープンか
    TbBool IsOpenAllWorld() const {
        return m_WorldStageSelectPage.IsOpenAllWorld();
    }

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

    // 該当するページを取得
    TbUint32 getWorldSelectPage(TbUint32 world) {
        return world/ WorldSelectPage::WORLD_MAX_IN_GROUP;
    }

private:
    TbUint32                            m_World;
    TbBool                              m_IsReadyBgm;
    TbStaticArray<WorldSelectPage,10>	m_WorldSelectPage;
    WorldStageSelectPage	            m_WorldStageSelectPage;
};

#endif