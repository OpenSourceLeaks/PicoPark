/*!
 * 点音源
 * @author Miyake Shunsuke
 * @since 2011.07.17
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_SHAPE_POINT_H_
#define _INCLUDED_TB_SOUND_SOURCE_SHAPE_POINT_H_

#include <sound/tb_sound_types.h>

namespace toybox
{

/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.15
 */
class TbSoundSourceShapePoint : public TbSoundSourceShape
{
public:

    // コンストラクタ
    TbSoundSourceShapePoint();
    
    // デストラクタ
    ~TbSoundSourceShapePoint();

public:

    // 影響度計算
    virtual TbFloat32 CalcWeight( TbSoundListener* listener ) const;

public:

    // 位置設定
    TbResult SetRadius( const TbFloat32 radius ) { m_Radius = radius; }

    // 位置取得
    const TbFloat32& GetRadius() const { return m_Radius; }

private:

    TbFloat32   m_Radius;   // 影響範囲半径

}:

}

#endif
