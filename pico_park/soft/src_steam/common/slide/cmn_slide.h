/*!
 * メインメニュー用ページ
 * @author teco
 */

#ifndef _CMN_SLIDE_H_
#define _CMN_SLIDE_H_

#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite.h>
#include "common/cmn_simple_sprite.h"

class CmnSlidePage
{
public:
    // コンストラクタ
    CmnSlidePage()
        : m_IsActive(TB_FALSE)
        , m_Phase(0)
        , m_PhaseSec(0.0f)
    {}
    // デストラクタ
    virtual ~CmnSlidePage() {}
public:
    // 有効
    void SetActive(TbBool isActive) {
        if (TB_BOOL_NOT_EQUAL(m_IsActive, isActive)) {
            m_IsActive = isActive;
            m_Phase = 0;
            m_PhaseSec = 0.0f;
        }
    }
    // 有効か
    TbBool IsActive() const {
        return m_IsActive;
    }
public:
    // 更新
    void Update(TbFloat32 deltasec) {
        if (m_IsActive) {
            OnUpdate(deltasec);
            m_PhaseSec += deltasec;
        }
    }
    // 描画
    virtual void Draw(const TbVector2& pos) {}
protected:

    // 継承定期処理
    virtual void OnUpdate(TbFloat32 deltasec) {}

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

private:
    TbBool       m_IsActive;
    TbSint32    m_Phase;
    TbFloat32   m_PhaseSec;
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

public:

	// 開始
    void Start() {
        m_IsBusy = TB_TRUE;
        m_IsCanceled = TB_FALSE;
        setCurrentPage(m_CurrentPageIndex);
    }

	// ビジーかどうか
    TbBool IsBusy() const {
		return m_IsBusy;
    }

    // キャンセル扱いか
    TbBool IsCanceled() const {
        return  m_IsCanceled;
    }

    // ページを強制設定
    void SetPage( TbUint32 pageIndex );

public:

    // 定期処理
    void Update(TbFloat32 deltasec);

    // 描画処理
    void Draw();

protected:

    // ページ変更開始
    virtual void OnChangingPage( TbUint32 page ) {}

private:

    enum
    {
		PHASE_UPDATE_PAGE ,
		PHASE_NEXT_PAGE   , 
		PHASE_PREV_PAGE   ,
    };

private:

    void setCurrentPage(TbSint32 index) {
        TB_RESQUE(m_Page[index]);
        resetPage();
        m_CurrentPageIndex = index;
        m_Page[m_CurrentPageIndex]->SetActive(TB_TRUE);
    }

    void resetPage() {
        for (TbSizeT i = 0; i < m_PageMax; ++i) {
            if (m_Page[i]) {
                m_Page[i]->SetActive(TB_FALSE);
            }
        }
    }

private:
    TbBool						m_IsBusy;
    TbBool                      m_IsCanceled;
    TbSint32					m_Phase;
    TbSint32					m_CurrentPageIndex;
    TbVector2			        m_OffsetPos;
    CmnSlidePage**              m_Page;
    TbUint32                    m_PageMax;

};

#endif