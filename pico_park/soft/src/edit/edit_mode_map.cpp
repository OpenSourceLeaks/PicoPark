/*!
 * マップ編集モード
 * @author teco
 */

#include "fwd.h"
#include "edit/edit_mode_map.h"

TbVector2 getMousePos()
{
    TbVector2 mousePos;
    TbMouse::GetPos(&mousePos, TB_MOUSE_POS_TYPE_WINDOW);
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    TbRectS32 region = device.GetView(0)->GetPresentRegion();
    TbRectF32 regionF(region.x, region.y, region.width, region.height);
    mousePos.SetX(CMN_WINDOW_WIDTH * (mousePos.GetX() - regionF.x) / regionF.width);
    mousePos.SetY(CMN_WINDOW_HEIGHT * (mousePos.GetY() - regionF.y) / regionF.height);
    return mousePos;
}

const char* getMapChipName( TbUint32 mc )
{
    static const char* TABLE[] =
    {
        "NONE"  , 
        "BLOCK" ,
        "DOKAN" ,
        "BREAK1" ,
        "BREAK2" ,
        "BREAK3" ,
        "BREAK4" ,
        "BREAK5" ,
    };
    if (mc < TB_ARRAY_COUNT_OF(TABLE))
    {
        return TABLE[mc];
    }
    return nullptr;
}

/*!
 * コンストラクタ
 * @author teco
 */
EditModeMap::EditModeMap()
    : Super("EDIT MAP")
    , m_MapChipType(MC_TYPE_NONE)
    , m_FloorBlockY(0)
{
}

/*!
 * デストラクタ
 * @author teco
 */
EditModeMap::~EditModeMap()
{
}

/*!
 * 初期化
 * @author teco
 */
void EditModeMap::Initialize()
{
}

/*!
 * 定期処理
 * @author teco
 */
void EditModeMap::OnUpdate(TbFloat32 deltasec)
{
    SideActionStage* stage = GetStage();
    CrActionMap& map = stage->GetMap();
    TbFloat32 scale = stage->GetScale();
    TbFloat32 chipSize = map.GetChipSize() * scale;
    TbVector2 mousePos = getMousePos() + (stage->GetScroll() * scale);
    m_Point.x = mousePos.GetX() / chipSize;
    m_Point.y = mousePos.GetY() / chipSize;

    TbSint32 wheelDelta = TbMouse::GetWheelDelta();
    m_MapChipType = static_cast<MapChipType>(TbClamp<TbSint32>(m_MapChipType-wheelDelta,0, MC_TYPE_MAX-1));

    TbVector2 mov;
    if (TbMouse::IsOn(TB_MOUSE_RIGHT_BIT) && TbMouse::IsMoving(&mov,0)) {
        stage->AddScroll(-mov.GetX(),0.0f);
    }
    if (TbMouse::IsOn(TB_MOUSE_LEFT_BIT) ) {
        setMapChip( &map , m_Point );
        m_MarkerEndPoint = m_Point;
    }
    if (TbMouse::IsPress(TB_MOUSE_LEFT_BIT)){
        m_MarkerBeginPoint = m_Point;
    }
    if ( TbKeyboard::IsOn(TB_KEY_CONTROL_L) &&
         TbKeyboard::IsPress(TB_KEY_U) )
    {
        // ステージ全更新
        for (TbUint32 x = 0; x < map.GetTileWidth(); ++x)
        {
            for (TbUint32 y = 0; y < map.GetTileHeight(); ++y)
            {
                updateMapChip(&map,x,y);
            }
        }
    }
}

/*!
 * アクティブ状態が変化した
 * @author teco
 */
void EditModeMap::OnChangeActive(TbBool isActive)
{
    if(isActive)
    {
        SideActionStage* stage = GetStage();
        CrActionMap& map = stage->GetMap();
        TbFloat32 scale = stage->GetScale();
        TbFloat32 chipSize = map.GetChipSize() * scale;
        m_FloorBlockY = TbMax<TbSint32>(0,(CMN_WINDOW_HEIGHT/chipSize)-1);
    }
}

/*!
 * 描画処理
 * @author teco
 */
void EditModeMap::OnDraw(TbPointS32* pos)
{
    CrActionMap& map = GetStage()->GetMap();
    TbFloat32 scale = GetStage()->GetScale();
    TbFloat32 chipSize = map.GetChipSize() * scale;
    TbUint32  xPerPage = TbRoundUp(static_cast<TbFloat32>(CMN_WINDOW_WIDTH) / chipSize);

    EditUtil::PrintfLine(pos,"MAP_CHIP=%s", getMapChipName(m_MapChipType) );
    EditUtil::PrintfLine(pos,"MOUSE_POINT=%2d:%2d", m_Point.x, m_Point.y);
    EditUtil::PrintfLine(pos,"MOUSE_PAGE=%2d:%2d:%2d", (m_Point.x/xPerPage)+1 , m_Point.x%xPerPage , m_Point.y);
    EditUtil::PrintfLine(pos,"MOUSE_RECT=%2d:%2d:%2d:%2d",
        m_Point.x,
        m_Point.y,
        m_MarkerEndPoint.x - m_MarkerBeginPoint.x + 1,
        m_MarkerEndPoint.y - m_MarkerBeginPoint.y + 1);
}

/*!
 * マップチップ設定
 * @author teco
 */
void EditModeMap::setMapChip(CrActionMap* map, const TbPointS32& point)
{
    switch (m_MapChipType)
    {
    case MC_TYPE_NONE:
        map->SetMapChip(point.x,point.y,CR_TILE_MAP_CHIP_NONE);
        break;
    case MC_TYPE_BLOCK:
        map->SetMapChip(point.x, point.y, CR_TILE_MAP_CHIP_BLOCK);
        break;
    case MC_TYPE_DOKAN:
        map->SetMapChip(point.x, point.y, CR_ACTION_MAP_CHIP_DOKAN_LU);
        break;
    case MC_TYPE_BREAKOUT1:
    case MC_TYPE_BREAKOUT2:
    case MC_TYPE_BREAKOUT3:
    case MC_TYPE_BREAKOUT4:
        {
            TbUint32 mapChip = CR_ACTION_MAP_CHIP_BREAK1 + m_MapChipType - MC_TYPE_BREAKOUT1;
            map->SetMapChip(point.x, point.y, mapChip);
        }
        break;
    }
    updateMapChip(map, point.x, point.y);
    updateMapChip(map, point.x, point.y + 1);
    updateMapChip(map, point.x+1, point.y + 1);
    updateMapChip(map, point.x-1, point.y + 1);
    updateMapChip(map, point.x, point.y - 1);
    updateMapChip(map, point.x + 1, point.y - 1);
    updateMapChip(map, point.x - 1, point.y - 1);
    updateMapChip(map, point.x+1, point.y);
    updateMapChip(map, point.x-1, point.y);
}

/*!
 * マップチップ更新
 * @author teco
 */
void EditModeMap::updateMapChip(CrActionMap* map, TbSint32 x, TbSint32 y)
{
    TbUint32 mapChip = CR_TILE_MAP_CHIP_NONE;
    MapChipType type = getMapChipType(map->GetMapChip(x,y));
    if (MC_TYPE_NONE == type) {
        // 空セルは何もしない
        return;
    }

    enum
    {
        DIR_L ,
        DIR_R ,
        DIR_U ,
        DIR_D
    };
    
    TbBool sameTypeAround[] = { type == getMapChipType(map->GetMapChip(x - 1, y)) ,
                                type == getMapChipType(map->GetMapChip(x + 1, y)) ,
                                type == getMapChipType(map->GetMapChip(x, y - 1)) ,
                                type == getMapChipType(map->GetMapChip(x, y + 1)) };
    TbUint32 sameTypeCount = 0;
    for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(sameTypeAround); ++i) {
        if (sameTypeAround[i] ) {
            ++sameTypeCount;
        }
    }

    if (MC_TYPE_BREAKOUT1 <= type && type <= MC_TYPE_BREAKOUT4)
    {
        mapChip = CR_ACTION_MAP_CHIP_BREAK1 + type - MC_TYPE_BREAKOUT1;
    }
    else if( MC_TYPE_BLOCK == type)
    {
        if (sameTypeCount == 0) {
            mapChip = CR_TILE_MAP_CHIP_BLOCK;
        }

        // 天井
        if (y == 0)
        {
            if (x == 0)
            {
                // 左端天井
                if (sameTypeCount == 2) {
                    mapChip = CR_TILE_MAP_CHIP_IN;
                }else if (sameTypeAround[DIR_R]) {
                    mapChip = CR_TILE_MAP_CHIP_CEILING;
                }else if (sameTypeAround[DIR_D]) {
                    mapChip = CR_TILE_MAP_CHIP_WALL_R;
                }else {
                    mapChip = CR_TILE_MAP_CHIP_CEILING_WALL_R;
                }
            }
            else if (x == map->GetTileWidth() - 1)
            {
                // 右端天井
                if (2 <= sameTypeCount) {
                    mapChip = CR_TILE_MAP_CHIP_IN;
                }else if (sameTypeAround[DIR_L]) {
                    mapChip = CR_TILE_MAP_CHIP_WALL_L;
                }else if (sameTypeAround[DIR_D]) {
                    mapChip = CR_TILE_MAP_CHIP_CEILING;
                }else {
                    mapChip = CR_TILE_MAP_CHIP_CEILING_WALL_L;
                }
            }
            else
            {
                // 天井
                if (3 <= sameTypeCount) {
                    mapChip = CR_TILE_MAP_CHIP_IN;
                }
                else if (sameTypeCount == 2) {
                    if (sameTypeAround[DIR_L] &&
                        sameTypeAround[DIR_R] ) 
                    {
                        mapChip = CR_TILE_MAP_CHIP_CEILING;
                    }else if (sameTypeAround[DIR_L]) {
                        mapChip = CR_TILE_MAP_CHIP_WALL_R;
                    }
                    else if (sameTypeAround[DIR_R]) {
                        mapChip = CR_TILE_MAP_CHIP_WALL_L;
                    }
                }
                else if (sameTypeAround[DIR_L]) {
                    mapChip = CR_TILE_MAP_CHIP_CEILING_WALL_R;
                }
                else if (sameTypeAround[DIR_R]) {
                    mapChip = CR_TILE_MAP_CHIP_CEILING_WALL_L;
                }
                else if (sameTypeAround[DIR_D]) {
                    mapChip = CR_TILE_MAP_CHIP_BLOCK_HC;
                }
                else {
                    mapChip = CR_TILE_MAP_CHIP_BLOCK_HD;
                }
            }
        }
        // 床
        else if (y == m_FloorBlockY)
        {
            if (x == 0)
            {
                // 左端床
                if (2 <= sameTypeCount) {
                    mapChip = CR_TILE_MAP_CHIP_IN;
                }else if (sameTypeAround[DIR_R]) {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR;
                }else if (sameTypeAround[DIR_U]) {
                    mapChip = CR_TILE_MAP_CHIP_WALL_R;
                }else {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR_WALL_R;
                }
            }
            else if (x == map->GetTileWidth() - 1)
            {
                // 右端床
                if (2 <= sameTypeCount) {
                    mapChip = CR_TILE_MAP_CHIP_IN;
                }else if (sameTypeAround[DIR_L]) {
                    mapChip = CR_TILE_MAP_CHIP_WALL_L;
                }else if (sameTypeAround[DIR_U]) {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR;
                }else {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR_WALL_L;
                }
            }
            else
            {
                // 床
                if (3 <= sameTypeCount) {
                    mapChip = CR_TILE_MAP_CHIP_IN;
                }
                else if (sameTypeCount == 2) {
                    if (sameTypeAround[DIR_L] &&
                        sameTypeAround[DIR_R])
                    {
                        mapChip = CR_TILE_MAP_CHIP_FLOOR;
                    }else if (sameTypeAround[DIR_L]) {
                        mapChip = CR_TILE_MAP_CHIP_WALL_R;
                    }
                    else if (sameTypeAround[DIR_R]) {
                        mapChip = CR_TILE_MAP_CHIP_WALL_L;
                    }
                }
                else if (sameTypeAround[DIR_L]) {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR_WALL_R;
                }
                else if (sameTypeAround[DIR_R]) {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR_WALL_L;
                }
                else if (sameTypeAround[DIR_U]) {
                    mapChip = CR_TILE_MAP_CHIP_BLOCK_HC;
                }
                else {
                    mapChip = CR_TILE_MAP_CHIP_BLOCK_HU;
                }
            }
        }
        // 左端
        else if (x == 0)
        {
            // 左壁
            if (3 <= sameTypeCount) {
                mapChip = CR_TILE_MAP_CHIP_IN;
            }
            else if (sameTypeCount == 2) {
                if (!sameTypeAround[DIR_R]){
                    mapChip = CR_TILE_MAP_CHIP_WALL_R;
                }else if (!sameTypeAround[DIR_D]) {
                    mapChip = CR_TILE_MAP_CHIP_CEILING;
                }else if (!sameTypeAround[DIR_U]) {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR;
                }else {
                    mapChip = CR_TILE_MAP_CHIP_BLOCK_WR;
                }
            }
            else if (sameTypeAround[DIR_U]) {
                mapChip = CR_TILE_MAP_CHIP_CEILING_WALL_R;
            }
            else if (sameTypeAround[DIR_D]) {
                mapChip = CR_TILE_MAP_CHIP_FLOOR_WALL_R;
            }
            else if (sameTypeAround[DIR_R]) {
                mapChip = CR_TILE_MAP_CHIP_BLOCK_WC;
            }
        }
        // 右端
        else if (x == map->GetTileWidth()-1 )
        {
            // 右端
            if (3 <= sameTypeCount) {
                mapChip = CR_TILE_MAP_CHIP_IN;
            }
            else if (sameTypeCount == 2) {
                if (!sameTypeAround[DIR_L]){
                    mapChip = CR_TILE_MAP_CHIP_WALL_L;
                }else if (!sameTypeAround[DIR_D]) {
                    mapChip = CR_TILE_MAP_CHIP_CEILING;
                }
                else if (!sameTypeAround[DIR_U]) {
                    mapChip = CR_TILE_MAP_CHIP_FLOOR;
                }else {
                    mapChip = CR_TILE_MAP_CHIP_BLOCK_WL;
                }
            }
            else if (sameTypeAround[DIR_U]) {
                mapChip = CR_TILE_MAP_CHIP_CEILING_WALL_L;
            }
            else if (sameTypeAround[DIR_D]) {
                mapChip = CR_TILE_MAP_CHIP_FLOOR_WALL_L;
            }
            else if (sameTypeAround[DIR_L]) {
                mapChip = CR_TILE_MAP_CHIP_BLOCK_WC;
            }
        }
        else
        {
            // 真ん中
            if (sameTypeCount == 4) {
                mapChip = CR_TILE_MAP_CHIP_IN;
            } else if (sameTypeCount == 3) {
                // 一つだけかけていた場合のマップチップ
                static const TbUint32 CONV_TABLE[] =
                {
                    CR_TILE_MAP_CHIP_WALL_L ,
                    CR_TILE_MAP_CHIP_WALL_R ,
                    CR_TILE_MAP_CHIP_FLOOR  ,
                    CR_TILE_MAP_CHIP_CEILING ,
                };
                for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(CONV_TABLE); ++i) {
                    if (!sameTypeAround[i]) {
                        mapChip = CONV_TABLE[i];
                        break;
                    }
                }
            }else if (sameTypeCount == 2) {
                struct PairInfo {
                    TbUint32 first;
                    TbUint32 second;
                    TbUint32 mapChip;
                };
                static const PairInfo CONV_TABLE[] =
                {
                    { DIR_L , DIR_R , CR_TILE_MAP_CHIP_BLOCK_WC } ,
                    { DIR_L , DIR_U , CR_TILE_MAP_CHIP_CEILING_WALL_R } ,
                    { DIR_L , DIR_D , CR_TILE_MAP_CHIP_FLOOR_WALL_R } ,
                    { DIR_R , DIR_U , CR_TILE_MAP_CHIP_CEILING_WALL_L } ,
                    { DIR_R , DIR_D , CR_TILE_MAP_CHIP_FLOOR_WALL_L } ,
                    { DIR_U , DIR_D , CR_TILE_MAP_CHIP_BLOCK_HC } ,
                };
                for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(CONV_TABLE); ++i) {
                    if (sameTypeAround[CONV_TABLE[i].first] && 
                        sameTypeAround[CONV_TABLE[i].second]) 
                    {
                        mapChip = CONV_TABLE[i].mapChip;
                        break;
                    }
                }
            }else{
                // 一つだけ接続していた場合のマップチップ
                static const TbUint32 CONV_TABLE[] =
                {
                    CR_TILE_MAP_CHIP_BLOCK_WR ,
                    CR_TILE_MAP_CHIP_BLOCK_WL ,
                    CR_TILE_MAP_CHIP_BLOCK_HD ,
                    CR_TILE_MAP_CHIP_BLOCK_HU ,
                };
                for (TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(CONV_TABLE); ++i) {
                    if (sameTypeAround[i]) {
                        mapChip = CONV_TABLE[i];
                        break;
                    }
                }
            }
        }
    }
    else if (MC_TYPE_DOKAN == type)
    {
        mapChip = CR_ACTION_MAP_CHIP_DOKAN_LU;
        if (sameTypeAround[DIR_U]) {
            if (sameTypeAround[DIR_L]){
                mapChip = CR_ACTION_MAP_CHIP_DOKAN_RD;
            }else if (sameTypeAround[DIR_R]) {
                mapChip = CR_ACTION_MAP_CHIP_DOKAN_LD;
            }else {
                mapChip = CR_ACTION_MAP_CHIP_DOKAN_LD;
            }
        }else {
            if (sameTypeAround[DIR_L]) {
                mapChip = CR_ACTION_MAP_CHIP_DOKAN_RU;
            }else if (sameTypeAround[DIR_R]) {
                mapChip = CR_ACTION_MAP_CHIP_DOKAN_LU;
            }
        }
    }
    map->SetMapChip(x,y,mapChip);
}

/*!
 * マップチップタイプ取得
 * @author teco
 */
EditModeMap::MapChipType EditModeMap::getMapChipType(TbUint32 mapChip)
{
    if (mapChip <= CR_TILE_MAP_CHIP_NONE)
    {
        return MC_TYPE_NONE;
    }
    else if (CR_TILE_MAP_CHIP_BLOCK <= mapChip && mapChip < CR_TILE_MAP_CHIP_USER_BEGIN)
    {
        return MC_TYPE_BLOCK;
    }
    else if (CR_ACTION_MAP_CHIP_BREAK_BEGIN<= mapChip && mapChip < CR_ACTION_MAP_CHIP_BREAK_END)
    {
        return static_cast<MapChipType>(MC_TYPE_BREAKOUT1 + mapChip- CR_ACTION_MAP_CHIP_BREAK_BEGIN);
    }
    return MC_TYPE_DOKAN;
}
