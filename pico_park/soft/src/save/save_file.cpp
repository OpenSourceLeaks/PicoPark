/*!
 * セーブデータ
 */

#include "fwd.h"
#include "save_file.h"
#include "common/cmn_fade.h"
#include "common/cmn_util.h"
#include "sequence/seq_manager.h"
#include "crayon/input/cr_input.h"
#include "framework/sub/tb_framework_simple.h"
#include "world_manager.h"
#include "root.h"

static TbFloat32 ICON_SPRITE_TIME = 2.0f;

static const TbRectF32Pod SPRITE_RECT =
    { -8 , -8 , 16 , 16,};
static const TbRectF32Pod SPRITE_UV =
    { CMN_TEX_UV_RECT(400,352,16,16) };
static const TbVector2 SPRITE_POS =
    TbVector2(static_cast<TbFloat32>(CMN_WINDOW_WIDTH)*0.95f,static_cast<TbFloat32>(CMN_WINDOW_HEIGHT)*0.05f);
static const TbFloat32 SPRITE_SCALE = 4.0f;

/*!
 * コンストラクタ
 */
SaveFile::SaveFile()
    : m_Phase(PHASE_IDLE)
    , m_IsDirty(TB_TRUE)
    , m_IsLoaded(TB_FALSE)
    , m_SaveFile(nullptr)
    , m_IconSpriteMaterial(TbFrameworkSimple::RENDER_PASS_2D)
    , m_IconSpriteTimer(0.0f)
{
    SaveInitialize(&m_Data);
}

/*!
 * デストラクタ
 */
SaveFile::~SaveFile()
{
    TB_SAFE_DELETE(m_SaveFile);
}

/*!
 * 初期化
 */
void SaveFile::Initialize()
{
     m_IconSpriteMaterial.SetTexture(Root::GetInstance().GetCommonTexture());
     m_IconSprite.Initialize(SPRITE_RECT, SPRITE_UV);
     m_IconSprite.SetZ(CMN_SPRITE_Z_FRONT_2);
     m_IconSprite.SetMaterial(&m_IconSpriteMaterial);
     m_IconSprite.SetTranslate(SPRITE_POS);
     m_IconSprite.SetScale(SPRITE_SCALE);
 }

/*!
 * セーブ
 */
void SaveFile::Save()
{
    if (!m_IsLoaded) {
        return;
    }
    TB_RESQUE(m_Phase==PHASE_IDLE);
    if (m_IsDirty)
    {
        m_SaveFile = new TbSaveFile();
        m_Phase = PHASE_SAVE;
        m_IconSpriteTimer = ICON_SPRITE_TIME;
        m_IsDirty = TB_FALSE;
    }
}

/*!
 * ロード
 */
void SaveFile::Load()
{
    TB_RESQUE(m_Phase==PHASE_IDLE);
    if (!m_IsLoaded)
    {
        m_SaveFile = new TbSaveFile();
        m_SaveFile->Initialize("userdata.sav", TB_FILE_ASYNC);
        m_SaveFile->Read();
        m_Phase = PHASE_LOAD;
        m_IsLoaded = TB_TRUE;
    }
}

/*!
 * 定期処理
 */
void SaveFile::Update()
{
    switch (m_Phase)
    {
    case PHASE_IDLE:
        break;
    case PHASE_LOAD:
        if (!m_SaveFile->IsBusy()) {
            load();
            TB_SAFE_DELETE(m_SaveFile);
            m_Phase = PHASE_IDLE;
        }
        break;
    case PHASE_SAVE:
        {
            if ( m_IconSpriteTimer < (ICON_SPRITE_TIME-0.1f) )
            {
                m_SaveFile->Initialize("userdata.sav", TB_FILE_ASYNC);
                m_SaveFile->Write(&m_Data, sizeof(m_Data));
                ++m_Phase;
            }
        }
        break;
    case PHASE_SAVE+1:
        if (!m_SaveFile->IsBusy()) {
            TB_SAFE_DELETE(m_SaveFile);
            m_Phase = PHASE_IDLE;
        }
        break;
    }
    if (0.0f < m_IconSpriteTimer) {
        m_IconSpriteTimer -= 1/60.0f;
        if (m_IconSpriteTimer <= 0.0f && m_SaveFile && m_SaveFile->IsBusy() ) {
            m_IconSpriteTimer = 0.01f;
        }
        m_IconSprite.Calculate();
    }
}

/*!
 * アイコン描画
 */
void SaveFile::DrawIcon()
{
    if (0.0f < m_IconSpriteTimer) {
        m_IconSprite.Draw(TbMatrix::IDENTITY);
    }
}

/*!
 * ビジー状態か
 */
TbBool SaveFile::IsBusy() const
{
    return m_SaveFile != nullptr;
}

// プレイ回数設定
void SaveFile::SetPlayCount(TbUint32 stage, TbUint32 playerCount, TbUint32 playCount)
{
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        m_Data.stage[stage].data[playerCount-1].playCount = playCount;
//        m_IsDirty = TB_TRUE;
    }
}

// プレイ回数カウントアップ
void SaveFile::IncrementPlayCount(TbUint32 stage, TbUint32 playerCount) {
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        ++m_Data.stage[stage].data[playerCount - 1].playCount;
//        m_IsDirty = TB_TRUE;
    }
}

// プレイ回数取得
TbUint32 SaveFile::GetPlayCount(TbUint32 stage, TbUint32 playerCount) const {
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        return m_Data.stage[stage].data[playerCount - 1].playCount;
    }
    return 0;
}

// クリア回数設定
void SaveFile::SetClearCount(TbUint32 stage, TbUint32 playerCount, TbUint32 clearCount) {
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        m_Data.stage[stage].data[playerCount - 1].clearCount = clearCount;
        m_IsDirty = TB_TRUE;
    }
}

// クリア回数カウントアップ
void SaveFile::IncrementClearCount(TbUint32 stage, TbUint32 playerCount) {
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        ++m_Data.stage[stage].data[playerCount - 1].clearCount;
        m_IsDirty = TB_TRUE;
    }
}

// クリア回数取得
TbUint32 SaveFile::GetClearCount(TbUint32 stage, TbUint32 playerCount) const {
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        return m_Data.stage[stage].data[playerCount - 1].clearCount;
    }
    return 0;
}

// クリア回数取得
TbUint32 SaveFile::GetTotalClearCount(TbUint32 stage) const
{
    TbUint32 result = 0;
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Data.stage[stage].data); ++i)
        {
            TbUint32 clearCount = m_Data.stage[stage].data[i].clearCount;
            result += clearCount;
        }
    }
    return result;
}

// クリア回数取得(一番良いの)
TbUint32 SaveFile::GetClearCount(TbUint32 stage) const
{
    TbUint32 result = 0;
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage)) {
        for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(m_Data.stage[stage].data); ++i)
        {
            TbUint32 clearCount = m_Data.stage[stage].data[i].clearCount;
            if (result < clearCount) {
                result = clearCount;
            }
        }
    }
    return result;
}


// ベストタイム設定
TbBool SaveFile::SetBestMillSec(TbUint32 stage, TbUint32 playerCount, TbUint32 ms) 
{
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage) &&
        0 < playerCount && playerCount <= CMN_PLAYER_MAX &&
        ms < m_Data.stage[stage].data[playerCount-1].bestTime)
    {
        // 更新
        m_Data.stage[stage].data[playerCount - 1].bestTime = ms;
        m_IsDirty = TB_TRUE;
        return TB_TRUE;
    }
    return TB_FALSE;
}

// ベストタイム取得
TbUint32 SaveFile::GetBestMillSec(TbUint32 stage, TbUint32 playerCount) const 
{
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage) &&
        0 < playerCount && playerCount <= CMN_PLAYER_MAX)
    {
        return m_Data.stage[stage].data[playerCount - 1].bestTime;
    }
    return SaveDataStage::BEST_TIME_INVALID;
}

// ハイスコア設定
TbBool SaveFile::SetHiscore(TbUint32 stage, TbUint32 playerCount, TbUint32 score)
{
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage) &&
        0 < playerCount && playerCount <= CMN_PLAYER_MAX &&
        m_Data.stage[stage].data[playerCount - 1].hiscore < score)
    {
        // 更新
        m_Data.stage[stage].data[playerCount - 1].hiscore = score;
        m_IsDirty = TB_TRUE;
        return TB_TRUE;
    }
    return TB_FALSE;
}

// ハイスコア取得
TbUint32 SaveFile::GetHiscore(TbUint32 stage, TbUint32 playerCount) const
{
    if (stage < TB_ARRAY_COUNT_OF(m_Data.stage) &&
        0 < playerCount && playerCount <= CMN_PLAYER_MAX)
    {
        return m_Data.stage[stage].data[playerCount - 1].hiscore;
    }
    return 0;
}

// ジャンプボタン切り替えか
TbBool SaveFile::IsChangeJumpButton() const
{
    return m_Data.setting.bitArray & SaveDataSetting::BIT_CHANGE_JUMP_BUTTON;
}

// ジャンプボタン切り替えか
void SaveFile::SetChangeJumpButton(TbBool isChangeJumpButton)
{
    if (isChangeJumpButton) {
        if (!(m_Data.setting.bitArray & SaveDataSetting::BIT_CHANGE_JUMP_BUTTON)) {
            m_IsDirty = TB_TRUE;
        }
        m_Data.setting.bitArray |= SaveDataSetting::BIT_CHANGE_JUMP_BUTTON;
        CrInput::SetInputLayout(CR_INPUT_LAYOUT_B_DECIDE);
    }else {
        if ((m_Data.setting.bitArray & SaveDataSetting::BIT_CHANGE_JUMP_BUTTON)) {
            m_IsDirty = TB_TRUE;
        }
        m_Data.setting.bitArray &= ~SaveDataSetting::BIT_CHANGE_JUMP_BUTTON;
        CrInput::SetInputLayout(CR_INPUT_LAYOUT_A_DECIDE);
    }
}

// 背景カラー取得
TbUint32 SaveFile::GetBgColorType() const
{
    TbUint32 result = (m_Data.setting.bitArray & (SaveDataSetting::BIT_CHANGE_CREAM_BG)) != 0;
    return result;
}

// 背景カラー設定
void SaveFile::SetBgColorType(TbUint32 bgColor)
{
    TbColor color = TbColor::White();
    if (bgColor) {
        if (!(m_Data.setting.bitArray & SaveDataSetting::BIT_CHANGE_CREAM_BG)) {
            m_IsDirty = TB_TRUE;
        }
        m_Data.setting.bitArray |= SaveDataSetting::BIT_CHANGE_CREAM_BG;
        color = TbColor::FromRgbaU8(255, 240, 219, 255);
    }else {
        if ((m_Data.setting.bitArray & SaveDataSetting::BIT_CHANGE_CREAM_BG)) {
            m_IsDirty = TB_TRUE;
        }
        m_Data.setting.bitArray &= ~SaveDataSetting::BIT_CHANGE_CREAM_BG;
    }
    TbFrameworkSimple::SetClearColor(color);
    CmnFade::GetInstance().SetColor(color);
}

// サウンドボリュームレベル設定
void SaveFile::SetSoundBgmVolumeLevel(TbSint32 level)
{
    level = TbClamp<TbSint32>(level, 0, CMN_SOUND_VOLUME_LEVEL_MAX);;
    if (m_Data.setting.volume != level) {
        m_Data.setting.volume = level;
        m_IsDirty = TB_TRUE;
    }
    TbFloat32 volume = CmnUtil::ConvertSoundLevelToVolume(level);
    SeqManager::GetInstance().SetBgmVolume(volume);
}

// サウンドボリュームレベル取得
TbSint32 SaveFile::GetSoundBgmVolumeLevel() const
{
    return m_Data.setting.volume;
}

// プレイヤー番号表示
void SaveFile::SetVisiblePlayerNumber(TbBool isVisible)
{
    if (isVisible) {
        if (!(m_Data.setting.bitArray & SaveDataSetting::BIT_VISIBLE_PLAYER_NUMBER)) {
            m_IsDirty = TB_TRUE;
        }
        m_Data.setting.bitArray |= SaveDataSetting::BIT_VISIBLE_PLAYER_NUMBER;
    }else {
        if ((m_Data.setting.bitArray & SaveDataSetting::BIT_VISIBLE_PLAYER_NUMBER)) {
            m_IsDirty = TB_TRUE;
        }
        m_Data.setting.bitArray &= ~SaveDataSetting::BIT_VISIBLE_PLAYER_NUMBER;
    }
}

// プレイヤー番号表示
TbBool SaveFile::IsVisiblePlayerNumber() const
{
    return (m_Data.setting.bitArray & SaveDataSetting::BIT_VISIBLE_PLAYER_NUMBER) ? TB_TRUE : TB_FALSE;
}

/*!
 * 読み込み
 */
void SaveFile::load()
{
    const SaveDataHeader* header = reinterpret_cast<const SaveDataHeader*>(m_SaveFile->GetReadBuffer());
    if(header)
    {
        TB_RESQUE(header->version == m_Data.header.version);
        TB_RESQUE(header->size    == m_Data.header.size);
        m_Data = *reinterpret_cast<const SaveData*>(header);
        SetChangeJumpButton(IsChangeJumpButton());
        SetBgColorType(GetBgColorType());
        SetSoundBgmVolumeLevel(GetSoundBgmVolumeLevel());

        WorldManager& worldManager = WorldManager::GetInstance();
        TbSint32 worldCount = worldManager.GetWorldCount();
        for (TbSint32 i = 0; i < worldCount; ++i)
        {
            worldManager.UpdateInfo(i);
        }
        worldManager.UpdateOpenAllWorld();
#if CMN_ENABLE_BUILD_8BITCAFE && TB_PLATFORM_IS_WIN
        // 8bitcafeの場合は、リセット
        SetChangeJumpButton(TB_FALSE);
        SetVisiblePlayerNumber(TB_FALSE);
#endif
        m_IsDirty = TB_FALSE;
    }
}
