/*!
 * メインメニュー用ページ
 * @author teco
 */

#ifndef _CMN_SLIDE_H_
#define _CMN_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <base/util/tb_dynamic_cast.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/cmn_simple_sprite.h"

class CmnSlide;

class CmnSlidePage
{
public:

    enum Result
    {
        RESULT_PENDING ,
        RESULT_NEXT , 
        RESULT_PREV ,
        RESULT_PARENT ,
        RESULT_RETURN 
    };

public:
    // コンストラクタ
    CmnSlidePage()
        : m_IsActive(TB_FALSE)
        , m_IsEnableDrawArrowLeft(TB_FALSE)
        , m_IsEnableDrawArrowRight(TB_FALSE)
        , m_Depth(0)
        , m_Phase(0)
        , m_InitPhase(0)
        , m_PhaseSec(0.0f)
        , m_Slide(nullptr)
    {
        TbRectF32 rect(-64.0f, -32.0f, 64.0f, 64.0f);
        TbRectF32 uvRect(CMN_TEX_UV_RECT(0.0f, 640.0f, 48.0f, 48.0f));
        for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(m_ArrowSprite); ++i) {
            TbRectF32 uv = uvRect;
            uv.x += CMN_TEX_POS(48.0f)*i;
            m_ArrowSprite[i].Initialize(rect, uv);
            m_ArrowSprite[i].SetColor(TbColor::White());
        }
    }
    // デストラクタ
    virtual ~CmnSlidePage() {}
public:
    TB_DEFINE_BASE_CLASS(CmnSlidePage);
public:
    // 有効
    void SetActive(TbBool isActive) {
        if (TB_BOOL_NOT_EQUAL(m_IsActive, isActive)) {
            m_IsActive = isActive;
            m_Phase = m_InitPhase;
            m_PhaseSec = 0.0f;
            OnChangeActive(m_IsActive);
        }
    }
    // 有効か
    TbBool IsActive() const {
        return m_IsActive;
    }
    // 属しているスライド設定
    void SetSlide(CmnSlide* slide) {
        TB_ASSERT(!slide || !m_Slide);
        m_Slide = slide;
        OnChangeInsert(slide!=nullptr);
    }
    // カレント取得
    TbBool IsCurrent() const;

    // 深度取得
    TbSint32 GetDepth() const {
        return m_Depth;
    }

    // 深度設定
    void SetDepth(TbSint32 layer) {
        m_Depth = layer;
    }

    // 矢印描画可能か
    void SetEnableDrawArrowLeft(TbBool isEnable) {
        m_IsEnableDrawArrowLeft = isEnable;
    }

    void SetEnableDrawArrowRight(TbBool isEnable) {
        m_IsEnableDrawArrowRight = isEnable;
    }

public:
    // 更新
    Result Update(TbFloat32 deltasec) {
        Result result = RESULT_PENDING;
        if (m_IsActive) {
            result = OnUpdate(deltasec);
            m_PhaseSec += deltasec;
        }
        return result;
    }

    // 描画
    void Draw(const TbVector2& pos);

protected:

    // 継承定期処理
    virtual Result OnUpdate(TbFloat32 deltasec) { return RESULT_PENDING; }

    // 継承描画処理
    virtual void   OnDraw(const TbVector2& pos) {}

    // アクティブ状態が変化した
    virtual void OnChangeActive( TbBool isActive ) {}

    // ページ登録された
    virtual void OnChangeInsert(TbBool isInserted) {}

    // フェーズ取得
    TbSint32 GetPhase() const {
        return m_Phase;
    }

    // フェーズ時間取得
    TbFloat32 GetPhaseSec() const {
        return m_PhaseSec;
    }

    // フェーズ進行
    void AddPhase() {
        ++m_Phase;
        m_PhaseSec = 0.0f;
    }

    // フェーズ設定
    void SetPhase(TbSint32 phase) {
        m_Phase = phase;
        m_PhaseSec = 0.0f;
    }

    // 初期フェーズ設定
    void SetInitPhase(TbSint32 phase) {
        m_InitPhase = phase;
    }

private:

    enum
    {
        ARROW_DEFAULT,
        ARROW_SELECT,
        ARROW_MAX
    };

private:

    TbBool IsChangingNext() const;

    TbBool IsChangingPrev() const;

private:

    TbBool          m_IsActive;
    TbBool          m_IsEnableDrawArrowLeft;
    TbBool          m_IsEnableDrawArrowRight;
    TbSint32        m_Depth;
    TbSint32        m_Phase;
    TbSint32        m_InitPhase;
    TbFloat32       m_PhaseSec;
    CmnSimpleSprite	m_ArrowSprite[ARROW_MAX];
    CmnSlide*       m_Slide;
};

class CmnSlide
{
public:

	// コンストラクタs
    CmnSlide();

	// デストラクタ
	virtual ~CmnSlide();

public:

    // 初期化
    void Initialize( TbUint32 pageMax );

    // ページ挿入
    void InsertPage( TbUint32 index, CmnSlidePage* page );

    // ページ除去
    void ErasePage( TbUint32 index );

public:

	// 開始
    void Start() {
        m_IsBusy = TB_TRUE;
        m_IsCanceled = TB_FALSE;
        setCurrentPage(m_CurrentPageIndex);
    }

    // 停止
    void Stop() {
        m_IsBusy = TB_FALSE;
        m_IsCanceled = TB_TRUE;
    }

	// ビジーかどうか
    TbBool IsBusy() const {
		return m_IsBusy;
    }

    // キャンセル扱いか
    TbBool IsCanceled() const {
        return  m_IsCanceled;
    }

    // ページを一時ポーズ
    void SetPausePage(TbBool isPause)
    {
        m_IsPausePage = isPause;
    }

    // ページが空か
    TbBool IsEmptyPage() const {
        return m_Page[0] == nullptr;
    }

    // ページを強制設定
    void SetPage( TbUint32 pageIndex );

    // ページ取得
    CmnSlidePage* GetPage(TbUint32 pageIndex) const {
        return m_Page[pageIndex];
    }

    // 現在のページ番号
    TbUint32 GetCurentPageIndex() const {
        return m_CurrentPageIndex;
    }

    // 現在のページ取得
    const CmnSlidePage* GetCurrentPage() const {
        return m_Page[m_CurrentPageIndex];
    }

    // 前のページを持っている
    TbBool HasPrevPage() const;

    // 次のページを持っている
    TbBool HasNextPage() const;

    // 前のページに移行中
    TbBool IsChangingPrev() const {
        return m_Phase == PHASE_PREV_PAGE;
    }

    // 次のページに移行中
    TbBool IsChangingNext() const {
        return m_Phase == PHASE_NEXT_PAGE;
    }

    // 
    void SetVisible(TbBool isVisible) {
        m_IsVisible = isVisible;
    }

public:

    // 定期処理
    void Update(TbFloat32 deltasec);

    // 描画処理
    void Draw();

protected:

    // 定期処理
    virtual void OnUpdate() {}

    // 描画
    virtual void OnDraw() {}

    // ページ変更開始
    virtual void OnChangingPage(TbBool isNext) {}

    // 次のページ取得
    virtual CmnSlidePage* GetNextPage() = 0;

private:

    enum
    {
		PHASE_UPDATE_PAGE ,
		PHASE_NEXT_PAGE   , 
		PHASE_PREV_PAGE   ,
    };

private:

    // 現在のページ設定
    void setCurrentPage(TbSint32 index) {
        TB_RESQUE(m_Page[index]);
        resetPage();
        m_CurrentPageIndex = index;
        m_Page[m_CurrentPageIndex]->SetActive(TB_TRUE);
    }

    // ページリセット
    void resetPage() {
        for (TbSizeT i = 0; i < m_PageMax; ++i) {
            if (m_Page[i]) {
                m_Page[i]->SetActive(TB_FALSE);
            }
        }
    }

private:

    TbBool			  m_IsBusy;
    TbBool            m_IsCanceled;
    TbBool            m_IsPausePage;
    TbBool            m_IsVisible;
    TbSint32		  m_Phase;
    TbSint32		  m_CurrentPageIndex;
    TbVector2		  m_OffsetPos;
    CmnSlidePage**    m_Page;
    TbUint32          m_PageMax;

};

#endif