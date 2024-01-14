/*!
 * ステージ
 * @author teco
 * @since 2013.09.04
 */

#include "fwd.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/actor/side_action_ghost.h"
#include "side_action/actor/side_action_player_gun.h"
#include "root.h"
#include "common/cmn_util.h"
#include <graphics/font/tb_font_buffer_2d.h>

 // アクションパート用マップチップのUV矩形座標
static TB_NDEBUG_CONST TbRectF32 MAP_CHIP_UV_RECT[] =
{
    TbRectF32() , // TILE_MAP_CHIP_INVALID , 
    TbRectF32() , // TILE_MAP_CHIP_NONE , 
    TbRectF32(CMN_TEX_UV_RECT(0,368,24,24)) , // TILE_MAP_CHIP_BLOCK ,
    TbRectF32(CMN_TEX_UV_RECT(16,528,24,24)) , // 床
    TbRectF32(CMN_TEX_UV_RECT(0,528,24,24)) , // 床+左壁
    TbRectF32(CMN_TEX_UV_RECT(24,528,24,24)) , // 床+右壁 
    TbRectF32(CMN_TEX_UV_RECT(16,552,24,24)) , // 天井
    TbRectF32(CMN_TEX_UV_RECT(0,552,24,24)) , // 天井+左壁
    TbRectF32(CMN_TEX_UV_RECT(24,552,24,24)) , // 天井+右壁
    TbRectF32(CMN_TEX_UV_RECT(0,544,24,24)) , // 左壁
    TbRectF32(CMN_TEX_UV_RECT(24,544,24,24)) , // 右壁
    TbRectF32(CMN_TEX_UV_RECT(16,544,24,24)) , // 内側
    TbRectF32(CMN_TEX_UV_RECT(0,464,16,16)) , // 内側(右壁と床に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(16,464,16,16)) , // 内側(左壁と床に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(0,480,16,16)) , // 内側(右壁と天井に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(16,480,16,16)) , // 内側(左壁と天井に挟まれ)
    TbRectF32(CMN_TEX_UV_RECT(32,464,16,16)) , // 内側(上)
    TbRectF32(CMN_TEX_UV_RECT(32,480,16,16)) , // 内側(下)
    TbRectF32(CMN_TEX_UV_RECT(48,464,16,16)) , // 内側(左)
    TbRectF32(CMN_TEX_UV_RECT(48,480,16,16)) , // 内側(右)
    TbRectF32(CMN_TEX_UV_RECT(64,528,24,24)) , // 長ブロック上
    TbRectF32(CMN_TEX_UV_RECT(64,544,24,24)) ,  // 長ブロック中
    TbRectF32(CMN_TEX_UV_RECT(64,552,24,24)) , // 長ブロック下
    TbRectF32(CMN_TEX_UV_RECT(0,592,24,24)) , // 長ブロック左
    TbRectF32(CMN_TEX_UV_RECT(16,592,24,24)) ,  // 長ブロック中
    TbRectF32(CMN_TEX_UV_RECT(24,592,24,24)) , // 長ブロック右
    TbRectF32(CMN_TEX_UV_RECT(48,368,16,16)) ,  // 土管左上
    TbRectF32(CMN_TEX_UV_RECT(48,384,16,16)) ,  // 土管左下
    TbRectF32(CMN_TEX_UV_RECT(64,368,16,16)) , // 土管右上
    TbRectF32(CMN_TEX_UV_RECT(64,384,16,16)) , // 土管右下
    TbRectF32(CMN_TEX_UV_RECT(0+1,352+1,12,12)) , // ブロックB
    TbRectF32(CMN_TEX_UV_RECT(16+1,352+1,12,12)) , // ブロックC
    TbRectF32(CMN_TEX_UV_RECT(32+1,352+1,12,12)) , // ブロックD
    TbRectF32(CMN_TEX_UV_RECT(48+1,352+1,12,12)) , // ブロックE
    TbRectF32(CMN_TEX_UV_RECT(64+1,352+1-16,12,12)) , // ブロックF
};

static TB_NDEBUG_CONST TbFloat32 PLAYER_RANGE_OFFSET = 16.0f;

struct CompareFunCY
{
    TbBool operator()( SideActionPlayer* left , SideActionPlayer* right )
    {
        return (left)->GetPosY() > (right)->GetPosY();
    }
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.08
 */
SideActionStage::SideActionStage( TbScriptLuaNode* node )
    : Super(node,
            SIDE_ACTION_ACTOR_LAYER_MAX,
            TbFrameworkSimple::RENDER_PASS_2D,
            Root::GetInstance().GetCommonTexture(),
            MAP_CHIP_UV_RECT,
            new SideActionCollisionScene(),
            Root::GetInstance().GetPlayerCount())
    , m_ClearedPlayerCount(0)
    , m_FailFallY(0.0f)
    , m_FailUpY(0.0f)
    , m_ClearFontColor(CmnUtil::GetMainColor())
{
    TbRectF32 windowRect(0.0f, 0.0f, CMN_WINDOW_WIDTH, CMN_WINDOW_HEIGHT);
    m_DarkSprite.Initialize(windowRect);
    m_DarkSprite.SetColor(TbClearColor::FromRgbaU8(0x10, 0x10, 0x10, 0xFF));

    TbUint32 playerCount = Root::GetInstance().GetPlayerCount();
    GetActorFactory().SetPlayerCount(playerCount);
    SetEnableCreateActor(TB_TRUE);
    SetEnablePause(TB_TRUE);

    // プロパティ取得
    {
        TbUint32 flag = 0;
        node->TryToFloat32(m_ScrollParam.autoSpeed, "autoScrollSpeed");
        TbFloat32 autoScrollSpeedOffset;
        if (TbScriptLuaNode::RESULT_OK == node->TryToFloat32(autoScrollSpeedOffset, "autoScrollSpeedOffset")) {
            m_ScrollParam.autoSpeed += autoScrollSpeedOffset * (Root::GetInstance().GetPlayerCount() - CMN_PLAYER_MIN);
        }
        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(flag, "autoScroll")) {
            if(flag)
            {
                m_ScrollParam.type = SCROLL_AUTO;
                node->TryToFloat32(m_ScrollParam.autoEndOffset, "autoScrollEndOffset");
                m_ScrollParam.autoEndOffset *= Root::GetInstance().GetPlayerCount() - CMN_PLAYER_MIN;
            }
        }
        else if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(flag, "scrollable")) {
            if(flag){
                m_ScrollParam.type = SCROLL_NORMAL;
            }
        }

        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(flag, "unlimitScroll")) {
            // スクロールの右端制限はない
            m_BitArray.SetBit(FLAG_UNLIMIT_SCROLL, flag);
        }
        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(flag, "unreturnScroll")) {
            // スクロールが戻らない
            m_BitArray.SetBit(FLAG_UNRETURN_SCROLL, flag);
        }

        TbFloat32 scale = 1.0f;
        if(TbScriptLuaNode::RESULT_OK == node->TryToFloat32(scale, "scale") )
        {
            SetScale(scale);
            TbMatrix pose;
            pose.Scale(scale, scale, scale);
            SetPoseMatrix(pose);
            GetSpriteScene(0).SetPoseScale(scale);
            Root::GetInstance().GetLocalFont()->SetPoseMatrix(pose);
        }
        TbUint32 limitPlayer = 0;
        if( TbScriptLuaNode::RESULT_OK==node->TryToUint32(limitPlayer,"limitPlayer") ){
            m_ActorManager.SetEnableLimitPlayer(limitPlayer);
        }
        TbFloat32 failWindowScale = 1.0f;
        if (TbScriptLuaNode::RESULT_OK == node->TryToFloat32(failWindowScale, "failWindowScale") ){
            m_FailFallY = GetWindowHeight()*failWindowScale;
        } else {
            m_FailFallY = GetWindowHeight()*3.0f;
        }
        m_FailUpY = 0.0f;
        node->TryToFloat32(m_FailUpY, "failUpY");
        TbUint32 isWhiteFontColor = 0;
        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(isWhiteFontColor, "whiteClearFont")) {
            if(isWhiteFontColor){
                m_ClearFontColor = TbColor::White();
            }
        }
        TbUint32 isDarkness = 0;
        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(isDarkness, "isDarkness")) {
            m_BitArray.SetBit(FLAG_DARKNESS,isDarkness);
            GetMap().SetVisible(!isDarkness);
        }
        TbUint32 enableShufflePlayer = 0;
        if (TbScriptLuaNode::RESULT_OK == node->TryToUint32(enableShufflePlayer, "enableShufflePlayer")) {
            m_BitArray.SetBit(FLAG_SHUFFLE_PLAYER, enableShufflePlayer);
            if(enableShufflePlayer){
                m_ActorManager.ShufflePlayer();
            }
        }
    }

    // コリジョンのレイヤー設定
    CrActionCollisionScene& collisionScene = GetCollisionScene();
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_PLAYER,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_ENEMY,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER, SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_KEY,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_OTHER,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_GOAL,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_TO_PLAYER,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB,SIDE_ACTION_COLLISION_LAYER_TO_PLAYER,TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER,SIDE_ACTION_COLLISION_LAYER_OBJECT,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB,SIDE_ACTION_COLLISION_LAYER_OBJECT,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_OBJECT, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_OBJECT,SIDE_ACTION_COLLISION_LAYER_OBJECT,TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_OBJECT_2, TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_ENEMY,SIDE_ACTION_COLLISION_LAYER_ENEMY,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT, TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_KEY,SIDE_ACTION_COLLISION_LAYER_GOAL,TB_TRUE);
    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_KEY,SIDE_ACTION_COLLISION_LAYER_KEY_TARGET,TB_TRUE);

    collisionScene.SetEnableCheck(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB,SIDE_ACTION_COLLISION_LAYER_OTHER,TB_TRUE);

#if TB_IS_DEBUG
    if( Root::GetInstance().IsAutoTest() )
    {
        SetEnableRandomInput(TB_TRUE);
    }
#endif
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.08
 */
SideActionStage::~SideActionStage()
{
    GetActorScene().Clear();
}

/*!
 * 有効状態が変化した
 * @author teco
 */
void SideActionStage::OnChangedActive(TbBool isActive)
{
    Super::OnChangedActive(isActive);
    if(isActive)
    {
        // 初期スクロール設定
        if (m_ScrollParam.type == SCROLL_NORMAL)
        {
            if (!m_BitArray.Test(FLAG_INIT_SCROLL))
            {
                if (0 < GetPlayerCount())
                {
                    TbRangeF32 range = calcPlayerRange(nullptr, nullptr, TB_FALSE);
                    TbFloat32 centerX = (range.begin + range.end) / 2.0f;
                    TbFloat32 windowWidth = GetWindowWidth();
                    TbFloat32 targetScroll = TbMax(0.0f, centerX - (windowWidth / 2.0f));
                    SetScroll(targetScroll, 0.0f);
                    m_BitArray.SetBit(FLAG_INIT_SCROLL);
                }
            }
        }
    }
}

/*!
 * 更新
 * @author teco
 * @since 2013.10.02
 */
void SideActionStage::OnUpdate( TbFloat32 deltatime )
{
    if( !IsActive() ){
        return;
    }

    // 物理シーン更新
    UpdartePhysicsScene(deltatime);

    // コリジョンシーン
    UpdateCollisionScene();

    // スクロール更新
    updateScroll();

    // カリング更新
    {
        // カリング情報
        TbFloat32 width = GetWindowWidth();
        TbFloat32 chipSize = static_cast<TbFloat32>(GetMap().GetChipSize());
        TbUint32 chipBegin = static_cast<TbUint32>(GetScroll().GetX()/chipSize);
        TbUint32 chipWidth = static_cast<TbUint32>(width / chipSize)+2;
        GetMap().SetCullingRect(TbRectU32(chipBegin,0,chipWidth,TB_UINT32_MAX));
    }

    // 状態遷移チェック
    updateClear();
    if( isCleared() ){
        RequestClear();
    }else if( isGameOver() ) {
        RequestRetry();
    }

    // Y軸プレイヤーソート
    if( IsEnablePlayerSortY() )
    {
        TbUint32 playerCount = GetPlayerCount();
        TbStaticArray<SideActionPlayer*,CMN_PLAYER_MAX> playerList;
        for( TbUint32 i = 0; i < playerCount; ++i ) {
            playerList.PushBack(GetPlayer(i));
        }
        TbInsertionSort(playerList.Begin(),playerList.End(), CompareFunCY());
        for (TbUint32 i = 0; i < playerCount; ++i) {
            playerList[i]->SetZ(i);
        }
    }
}

/*!
 * 描画
 * @author Miyake_Shunske
 * @since 2015.01.27
 */ 
void SideActionStage::OnDraw( TbFloat32 deltatime )
{
    if(IsDarkness()){
        m_DarkSprite.Calculate();
        m_DarkSprite.Draw(TbMatrix::IDENTITY);
    }
    Super::OnDraw(deltatime);
#if 0
    TbForIterator( ite , m_Player )
    {
        if( !(*ite)->IsGameOver() ) {
            TbVector4 pos = (*ite)->GetPos();
         
            
            TbSint32 x = pos.GetX();
            TbSint32 y = pos.GetY();
            Root::GetInstance().GetFont()->Printf(x,
                                                  y-100.0f,
                                                  "%d,%d",
                                                  x,
                                                  y);
        }
    }
#endif
}

/*!
 * 描画
 * @author Miyake_Shunske
 * @since 2015.01.27
 */ 
void SideActionStage::OnDrawFront( TbFloat32 deltatime )
{
    Super::OnDrawFront(deltatime);
#if 0
    TbUint32 minute = GetPlaySec()/60.0f;
    TbUint32 sec    = static_cast<TbUint32>(GetPlaySec())%60;
    TbFontBuffer2d* font = Root::GetInstance().GetLocalFont();
    font->PushSetting();
    font->Printf(8,8,"TIME %02d:%02d",minute,sec);
    font->PopSetting();
    font->Flush();
#endif
}

TbFloat32 getEdgeX(CrActionCollision* target , TbFloat32 sign)
{
    TbFloat32 x = target->GetActor()->GetDrawPos().GetX();
    CrActionCollision* contactCollision[CrActionCollision::CONTACT_GET_MAX];
    TbUint32 contactCount = target->GetContactCollision(contactCollision, CR_CONTACT_UP);
    for (TbUint32 i = 0; i < contactCount; ++i)
    {
        if(contactCollision[i]->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER){
            continue;
        }
        TbFloat32 tx = contactCollision[i]->GetActor()->GetDrawPos().GetX();
        if( 0.0f<  sign )
        {
            if( x < tx ) {
                x = tx;
            }
            TbFloat32 tx = getEdgeX(contactCollision[i],sign);
            if (x < tx) {
                x = tx;
            }
        }
        else
        {
            if (tx < x) {
                x = tx;
            }
            TbFloat32 tx = getEdgeX(contactCollision[i], sign);
            if (tx < x) {
                x = tx;
            }
        }
    }
    return x;
}

/*!
 * 画面端を移動可能か
 * @author teco
 * @since 2013.10.20
 */
TbFloat32 SideActionStage::CalcScreenEndWalkVelo(SideActionPlayer* player, TbFloat32 velo )
{
#if 1
    if (TB_FLOAT32_EQUAL_ZERO(velo)) {
        return 0.0f;
    }
    TbFloat32 x = getEdgeX(player->GetCollision(),velo);
    // 一人の時
    TbUint32 playerCount = GetPlayerCount();
    
    // スクロールしない場合、またはオートスクロールの場合は端の外に出ない
    if (  m_ScrollParam.type == SCROLL_NONE || 
          m_ScrollParam.type == SCROLL_AUTO ||
          playerCount == 1 ||
          (0.0f <= m_ScrollParam.limit && m_ScrollParam.limit < GetScrollX()+velo) )
    {
        if(x + velo < 0.0f){
            return -x;
        }
        TbFloat32 widnowWidth = GetWindowWidth();
        if(widnowWidth < x +velo ){
            return widnowWidth - x;
        }
        return velo;
    }

    // スクロールできる時は、左端キャラと右端キャラが画面の外に出ないように
    TbFloat32 playerRightX = -TB_FLOAT32_MAX;
    TbFloat32 playerLeftX = TB_FLOAT32_MAX;
    TbRangeF32 range = calcPlayerRange(nullptr,nullptr,TB_TRUE);
    playerRightX = range.end;
    playerLeftX = range.begin;

    TbFloat32 length = playerRightX - playerLeftX;
    TbFloat32 rest = GetWindowWidth()-(PLAYER_RANGE_OFFSET*2.0f)-length;

    if (velo < 0.0f) {
        if (x + velo < playerLeftX) {
            TbFloat32 tmp = (x + velo) - playerLeftX;
            return TbMax( -rest, tmp );
        }
    }
    else {
        if (playerRightX < x + velo) {
            TbFloat32 tmp = (x + velo) - playerRightX;
            return TbMin( rest , velo);
        }
    }
#endif
    return velo;
}

/*!
 * アクター生成
 * @author teco
 * @since 2013.10.20
 */
CrActor* SideActionStage::OnCreatedActor(const CrActorFactory::CreateParam& param)
{
    if( !IsEnableCreateActor() ){
        return nullptr;
    }
    return m_ActorManager.CreateActor(param,this);
}

/*!
* プレイヤー範囲取得
* @author teco
* @since 2013.10.19
*/
TbRangeF32 SideActionStage::calcPlayerRange(TbFloat32* leftVelo, TbFloat32* rightVelo , TbBool isDrawPos) const
{
    // スクロールできる時は、左端キャラと右端キャラが画面の外に出ないように
    TbUint32 playerCount = GetPlayerCount();
    TbFloat32 playerRightX = -TB_FLOAT32_MAX;
    TbFloat32 playerLeftX = TB_FLOAT32_MAX;
    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        const SideActionPlayer* player = GetPlayer(i);
        if (player->GetState() == SideActionPlayer::STATE_DEFAULT &&
            !player->IsVisible())
        {
            continue;
        }
        TbFloat32 x = isDrawPos ? player->GetDrawPos().GetX() : player->GetPos().GetX();
        TbFloat32 vx = player->GetPosX() - player->GetPrePos().GetX();
        if (playerRightX < x) {
            playerRightX = x;
        }
        if (x < playerLeftX) {
            playerLeftX = x;
        }
        if (leftVelo && rightVelo) {
            if (vx < 0.0f) {
                *leftVelo = TbMin(*leftVelo, vx);
            }
            else if (0.0f < vx) {
                *rightVelo = TbMax(*rightVelo, vx);
            }
        }
    }
    // オフセット(画面から見切れないようにするため)
#if 1
    // 幽霊がいる時は表示範囲にいれる
    SideActionGhost* ghost = m_ActorManager.GetGhost();
    if (ghost) {
        TbFloat32 x = isDrawPos ? ghost->GetDrawPos().GetX() : ghost->GetPosX();
        if (playerRightX < x) {
            playerRightX = x;
        }
        if (x < playerLeftX) {
            playerLeftX = x;
        }
    }
#endif
    TbRangeF32 rangeF32(playerLeftX, playerRightX);
    return rangeF32;
}

#if 0

/*!
 * スクロール更新
 * @author teco
 * @since 2013.10.19
 */
void SideActionStage::updateScroll()
{
    static TbFloat32 SCROLL_BEGIN_X = 0.3f;
    static TbFloat32 SCROLL_BEGIN_WEIGHT = 0.6f;
    static TbFloat32 SCROLL_END_X = 0.5f;

    // スクロールの正規化
    CrActionMap& map = GetMap();
    TbFloat32 windowWidth = GetWindowWidth();
    TbFloat32 mapWidth = static_cast<TbFloat32>(map.GetWidth()) + m_AutoScrollEndOffset;
    TbFloat32 right = GetScrollX() + windowWidth;
    if (m_BitArray.Test(FLAG_UNLIMIT_SCROLL)) {
        mapWidth = right + windowWidth;
    }
    TbFloat32 scrollMaxR = mapWidth - right; // 可能なスクロール量

    // プレイヤーの左右端位置を計算する
    TbFloat32 playerRightX = -TB_FLOAT32_MAX;
    TbFloat32 playerLeftX = TB_FLOAT32_MAX;
    TbFloat32 rightVelo = 0.0f;
    TbFloat32 leftVelo = 0.0f;
    m_PlayerRangeX = calcPlayerRange(&leftVelo,&rightVelo,TB_FALSE);
    playerLeftX = m_PlayerRangeX.begin - PLAYER_RANGE_OFFSET;
    playerRightX = m_PlayerRangeX.end + PLAYER_RANGE_OFFSET;

    if( !m_BitArray.Test(FLAG_SCROLLABLE) ){
        return;
    }
    if (IsPauseScroll()) {
        if (scrollMaxR < 0.0f) {
            AddScroll(scrollMaxR,0.0f);
        }
        GetMap().SetScroll(GetScrollX());
        return;
    }

#if 1

    TbFloat32 scrollBeginBorderR = GetScrollX() + windowWidth*SCROLL_BEGIN_X;
    TbFloat32 scrollEndBorderR = GetScrollX() + windowWidth*SCROLL_END_X;
       
    TbFloat32 scrollBeginBorderL = GetScrollX() + windowWidth*(1.0f-SCROLL_BEGIN_X);
    TbFloat32 scrollEndBorderL = GetScrollX() + windowWidth*(1.0f-SCROLL_END_X);

    TbFloat32 scrollMaxL = -GetScrollX(); // 可能なスクロール量

    // スクロール量
    TbFloat32 scroll = 0.0f;

    if (m_BitArray.Test(FLAG_AUTO_SCROLL))
    {
        scroll = CalcAutoScrollSpeed();
    }
    else
    // まず一番右にいるキャラの位置でスクロールが可能かどうか決める
    {
        if( 0.0f <= m_RequestScroll )
        {
            static TB_NDEBUG_CONST TbFloat32 REQ_AUTO_SCROLL = 3.0f;
            if( GetScrollX() < m_RequestScroll ) {
                TbFloat32 scr = REQ_AUTO_SCROLL;
                scroll = TbMin(scr, m_RequestScroll- GetScrollX() )+0.01f;
            }else{
                m_RequestScroll = -1.0f;
            }
        }
        else
        {
            // 右スクロール
            if( playerRightX >= scrollEndBorderR ) {
                TbFloat32 scale = 1.0f+1.0f*(playerRightX-scrollEndBorderR)/(windowWidth-(windowWidth*SCROLL_END_X));
                TbFloat32 scr = TbMax( 0.0f , rightVelo * scale);
                scroll += scr;
            }else if( playerRightX > scrollBeginBorderR ) {
                scroll += TbMax( 0.0f , (rightVelo) * SCROLL_BEGIN_WEIGHT );
            }

            // 左スクロール
            if (playerLeftX <= scrollEndBorderL) {
                TbFloat32 scale = 1.0f + 1.0f*(scrollEndBorderL-playerLeftX) / (scrollEndBorderL);
                TbFloat32 scr = 0.0f;
                scr = TbMin(0.0f, leftVelo * scale);
                scroll += scr;
            }
            else if (playerLeftX > scrollBeginBorderL) {
                scroll += TbMin(0.0f, leftVelo * SCROLL_BEGIN_WEIGHT);
            }
        }
        if (m_BitArray.Test(FLAG_UNRETURN_SCROLL)) {
            scroll = TbMax(0.0f, scroll);
        }

        // スクロール
        if( !TB_FLOAT32_EQUAL_ZERO(scroll) ) {

            // スクロールの限界値を超えないように
            scroll = TbClamp(scroll , scrollMaxL, scrollMaxR);

            // 左端と右端にいるキャラが見切れないように調整
            TbFloat32 leftOffset = playerLeftX - GetScrollX();
            scroll = TbMin( leftOffset , scroll );
            TbFloat32 rightOffset = playerRightX - (windowWidth+GetScrollX());
            rightOffset = TbMin( rightOffset , scrollMaxR );
            scroll = TbMax( rightOffset , scroll );
            if (0.0f <= m_ScrollLimit) {
                TbFloat32 limit = m_ScrollLimit - GetScrollX();
                scroll = TbMin(limit, scroll);
            }

            if ((mapWidth - windowWidth) < (static_cast<TbFloat32>(map.GetChipSize()) / 2.0f)) {
                scroll = 0.0f;
            }
        }
    }

    AddScroll(scroll,0.0f);
    GetMap().SetScroll(GetScrollX());
#endif
}

#else

/*!
* スクロール更新
* @author teco
* @since 2013.10.19
*/
void SideActionStage::updateScroll()
{
    m_PlayerRangeX = calcPlayerRange(nullptr, nullptr, TB_FALSE);
    if (!IsScrollable()) {
        return;
    }

    // スクロールの正規化
    CrActionMap& map = GetMap();
    TbFloat32 mapWidth = static_cast<TbFloat32>(map.GetWidth()) + m_ScrollParam.autoEndOffset;
    TbFloat32 windowWidth = GetWindowWidth();

    // 画面右の座標
    TbFloat32 right = GetScrollX() + windowWidth; 

    // スクロール無制限の時はマップの端を偽装
    if (m_BitArray.Test(FLAG_UNLIMIT_SCROLL)) {
        mapWidth = right + windowWidth;
    }

    // 左右に対するスクロール限界
    TbFloat32 scrollMaxL = -GetScrollX();
    TbFloat32 scrollMaxR = mapWidth - right;

    // プレイヤーの左右端位置を計算する
    TbRangeF32 range = calcPlayerRange(nullptr, nullptr, TB_FALSE);

    if (IsPauseScroll()) {
        if (scrollMaxR < 0.0f) {
            AddScroll(scrollMaxR, 0.0f);
        }
        GetMap().SetScroll(GetScrollX());
        return;
    }

#if 1
    // スクロール量
    TbFloat32 scroll = 0.0f;
    if (0.0f <= m_ScrollParam.request)
    {
        static TB_NDEBUG_CONST TbFloat32 REQ_AUTO_SCROLL = 3.0f;
        if (GetScrollX() < m_ScrollParam.request) {
            TbFloat32 scr = REQ_AUTO_SCROLL;
            scroll = TbMin(scr, m_ScrollParam.request - GetScrollX()) + 0.01f;
        }else {
            m_ScrollParam.request = -1.0f;
        }
    }
    else
    {
        switch(m_ScrollParam.type)
        {
        case SCROLL_AUTO:
            scroll = CalcAutoScrollSpeed();
            break;
        case SCROLL_NORMAL:
            {
                TbFloat32 targetSpeedRatio = 1.0f;
                if (!IsEnableScrollNormalSpeedMax()) {
                    targetSpeedRatio = 0.0f;
                }
                if (targetSpeedRatio != m_ScrollParam.normalSpeedMaxRatio) {
                    TbFloat32 deltaSign = TbSign(targetSpeedRatio - m_ScrollParam.normalSpeedMaxRatio);
                    m_ScrollParam.normalSpeedMaxRatio += (deltaSign * 0.05f);
                }
                // 左右の中心点が画面の真ん中になるように
                TbFloat32 centerX = (range.begin + range.end) / 2.0f;
                TbFloat32 targetScroll = TbMax(0.0f, centerX - (windowWidth/2.0f));
                TbFloat32 delta = targetScroll - GetScrollX();

                TbFloat32 scale = TbLerp( 1.0f , 0.1f , m_ScrollParam.normalSpeedMaxRatio );
                scroll = delta * scale;
                TbFloat32 speedMax = TbLerp( TbAbs(scroll),
                                             m_ScrollParam.normalSpeedMax, 
                                             m_ScrollParam.normalSpeedMaxRatio );
                scroll = TbClamp(scroll,-speedMax ,speedMax);
            }
            break;
        }
    }
    {
        if (m_BitArray.Test(FLAG_UNRETURN_SCROLL)) {
            scroll = TbMax(0.0f, scroll);
        }
        // スクロール
        if (!TB_FLOAT32_EQUAL_ZERO(scroll)) {
            // スクロールの限界値を超えないように
            scroll = TbClamp(scroll, scrollMaxL, scrollMaxR);
        }
    }

    AddScroll(scroll, 0.0f);
    GetMap().SetScroll(GetScrollX());
#endif
}

#endif

/*!
 * ステージクリア更新
 * @author teco
 * @since 2013.10.23
 */
void SideActionStage::updateClear()
{
    m_ClearedPlayerCount = 0;
    // 全プレイヤーがクリア状態になったかどうか
    // ワープ銃対応
    TbUint32 playerCount = GetPlayerCount();

    // クリアしたワープ銃に入ってるプレイヤーもクリア扱い
    const SideActionPlayer* warpPlayerList[CMN_PLAYER_MAX];
    TbUint32             warpPlayerCount = 0;
    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        const SideActionPlayer* player = GetPlayer(i);
        if (player->IsCleared())
        {
            calcClearedWarpPlayer(player, warpPlayerList,&warpPlayerCount);
        }
    }
    for (TbUint32 i = 0; i < playerCount; ++i)
    {
        const SideActionPlayer* player = GetPlayer(i);
        if( player->IsCleared() ) 
        {
            ++m_ClearedPlayerCount;
        }
    }
    m_ClearedPlayerCount += warpPlayerCount;
}

/*!
 * クリアと判断可能なワープ銃にいるプレイヤー
 * @author teco
 */
void SideActionStage::calcClearedWarpPlayer(const SideActionPlayer* clearedPlayer, const SideActionPlayer** list, TbUint32* count)
{
    const CrActionActor* equip = clearedPlayer->GetEquip(SideActionPlayer::EQUIO_PART_HAND);
    if (equip)
    {
        const SideActionPlayerWarpGun* warpGun = TbDynamicCast<const SideActionPlayerWarpGun*>(equip);
        if (warpGun)
        {
            const CrActionActor* target = warpGun->GetTarget();
            if (target)
            {
                const SideActionPlayer* player = TbDynamicCast<const SideActionPlayer*>(target);
                list[*count] = player;
                ++(*count);
                calcClearedWarpPlayer(player,list,count);
            }
        }
    }
}

/*!
 * ゲームオーバー
 * @author teco
 * @since 2013.10.27
 */
TbBool SideActionStage::isGameOver() const
{
    if( m_BitArray.Test(FLAG_DISABLE_GAMEOVER) ){
        return TB_FALSE;
    }

    TbUint32 count = GetPlayerCount();
    TbBool result = 0 < count;
    for( TbUint32 i = 0; i < count; ++i )
    {
        const SideActionPlayer* player = GetPlayer(i);
        if( !player->IsGameOver() ) {
            result = TB_FALSE;
            break;
        }
    }
    if( IsRequestedGameOver() ){
        result = TB_TRUE;
    }
    return result;
}

// 自動スクロール量計算
TbFloat32 SideActionStage::CalcAutoScrollSpeed() const 
{
    if (IsAutoScroll())
    {
        // 自動スクロール
        TbBool isEnable = TB_TRUE;
        // 死亡ステートに入っている人がいたら停止
        TbUint32 playerCount = GetPlayerCount();
        for (TbUint32 i = 0; i < playerCount; ++i)
        {
            const SideActionPlayer* player = GetPlayer(i);
            if (player->GetState() == SideActionPlayer::STATE_DEAD ) {
                isEnable = TB_FALSE;
                break;
            }
        }
        if (isEnable)
        {
            const CrActionMap& map = GetMap();
            TbFloat32 windowWidth = GetWindowWidth();
            TbFloat32 mapWidth = static_cast<TbFloat32>(map.GetWidth()) + m_ScrollParam.autoEndOffset;
            TbFloat32 right = GetScrollX() + windowWidth;
            if (m_BitArray.Test(FLAG_UNLIMIT_SCROLL)) {
                mapWidth = right + windowWidth;
            }
            TbFloat32 scrMax = mapWidth - right;
            TbFloat32 scroll = TbMin(scrMax, m_ScrollParam.autoSpeed);
            return scroll;
        }
    }
    return 0.0f;
}