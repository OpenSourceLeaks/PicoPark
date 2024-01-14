/*!
 * マップ矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_map_rect.h"
#include "side_action/stage/side_action_stage.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionMapRect::SideActionMapRect()
{
};

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionMapRect::~SideActionMapRect()
{
}

/*! 
 * セットアップ
 * @author teco
 */
void SideActionMapRect::Setup(const CrActorFactory::UserData& userData)
{
    m_Rect.x = userData.AsSint32(0);
    m_Rect.y = userData.AsSint32(1);
    m_Rect.width = TbMax<TbSint32>(1,userData.AsSint32(2));
    m_Rect.height = TbMax<TbSint32>(1, userData.AsSint32(3));
}

/*! 
 * シーンに入った
 * @author teco
 */
void SideActionMapRect::OnEnter(void* userData)
{
    Super::OnEnter(userData);
    CrActionMap& map = GetActionStage()->GetMap();
    if (m_Rect.width == 1 && m_Rect.height == 1)
    {
        map.SetMapChip(m_Rect.x,m_Rect.y,CR_TILE_MAP_CHIP_BLOCK);
    }
    else if ( 1 == m_Rect.width)
    {
        map.SetMapChip(m_Rect.x, m_Rect.y, CR_TILE_MAP_CHIP_BLOCK_HD);
        for (TbSint32 y = 1; y < m_Rect.height-1; ++y) {
            map.SetMapChip(m_Rect.x, m_Rect.y - m_Rect.height - 1, CR_TILE_MAP_CHIP_BLOCK_HC);
        }
        map.SetMapChip(m_Rect.x, m_Rect.y-(m_Rect.height-1), CR_TILE_MAP_CHIP_BLOCK_HU);
    }
    else if (1 == m_Rect.height)
    {
        TbUint32 chip = CR_TILE_MAP_CHIP_BLOCK_WL;
        if (CR_TILE_MAP_CHIP_NONE != map.GetMapChip(m_Rect.x, m_Rect.y + 1)) {
            chip = CR_TILE_MAP_CHIP_FLOOR_WALL_L;
            map.SetMapChip(m_Rect.x, m_Rect.y+1, CR_TILE_MAP_CHIP_IN);
        }
        map.SetMapChip(m_Rect.x, m_Rect.y, chip);
        for (TbSint32 x = 1; x < m_Rect.width - 1; ++x) {
            chip = CR_TILE_MAP_CHIP_BLOCK_WC;
            if (CR_TILE_MAP_CHIP_NONE != map.GetMapChip(m_Rect.x+x, m_Rect.y+1)) {
                chip = CR_TILE_MAP_CHIP_IN;
                map.SetMapChip(m_Rect.x + x, m_Rect.y+1, chip);
            }
            map.SetMapChip(m_Rect.x+x, m_Rect.y, chip);
        }
        chip = CR_TILE_MAP_CHIP_BLOCK_WR;
        if (CR_TILE_MAP_CHIP_NONE != map.GetMapChip(m_Rect.x + m_Rect.width, m_Rect.y)) {
            chip = CR_TILE_MAP_CHIP_IN;
        }
        map.SetMapChip(m_Rect.x+(m_Rect.width-1), m_Rect.y, chip);
    }
    else
    {
        // 下
        map.SetMapChip(m_Rect.x, m_Rect.y, CR_TILE_MAP_CHIP_CEILING_WALL_L);
        for (TbSint32 x = 1; x < m_Rect.width - 1; ++x) {
            map.SetMapChip(m_Rect.x + x, m_Rect.y, CR_TILE_MAP_CHIP_CEILING);
        }
        TbUint32 rightChip = CR_TILE_MAP_CHIP_CEILING_WALL_R;
        if (CR_TILE_MAP_CHIP_NONE != map.GetMapChip(m_Rect.x + m_Rect.width, m_Rect.y)) {
            rightChip = CR_TILE_MAP_CHIP_CEILING;
        }
        map.SetMapChip(m_Rect.x + (m_Rect.width - 1), m_Rect.y, rightChip);

        // 中
        TbSint32 y = 0;
        for (y = 1; y < m_Rect.height - 1; ++y) {
            map.SetMapChip(m_Rect.x, m_Rect.y - y, CR_TILE_MAP_CHIP_WALL_L);
            for (TbSint32 x = 1; x < m_Rect.width - 1; ++x) {
                map.SetMapChip(m_Rect.x + x, m_Rect.y - y, CR_TILE_MAP_CHIP_IN);
            }
            map.SetMapChip(m_Rect.x + (m_Rect.width - 1), m_Rect.y - y, CR_TILE_MAP_CHIP_WALL_R);
        }

        // 上
        y = m_Rect.y -(m_Rect.height - 1);
        map.SetMapChip(m_Rect.x, y, CR_TILE_MAP_CHIP_FLOOR_WALL_L);
        for (TbSint32 x = 1; x < m_Rect.width - 1; ++x) {
            map.SetMapChip(m_Rect.x + x, y, CR_TILE_MAP_CHIP_FLOOR);
        }
        rightChip = CR_TILE_MAP_CHIP_FLOOR_WALL_R;
        if (CR_TILE_MAP_CHIP_NONE != map.GetMapChip(m_Rect.x + m_Rect.width, y)) {
            rightChip = CR_TILE_MAP_CHIP_FLOOR;
        }
        map.SetMapChip(m_Rect.x + (m_Rect.width - 1), y, rightChip);
    }
}
