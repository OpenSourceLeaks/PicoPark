/*!
 * 音源形状クラス
 * @author Miyake Shunsuke
 * @since 2011.07.17
 */

#ifndef _INCLUDED_TB_SOUND_SOURCE_SHAPE_H_
#define _INCLUDED_TB_SOUND_SOURCE_SHAPE_H_

#include <sound/tb_sound_types.h>

namespace toybox
{

/*!
 * 音源
 * @author Miyake Shunsuke
 * @since 2011.07.15
 */
class TbSoundSourceShape
{
public:

    // コンストラクタ
    TbSoundSourceShape();
    
    // デストラクタ
    ~TbSoundSourceShape();

public:

    // 音源と関連付け
    void BindSource( TbSoundSource* source );

    // 影響度計算
    virtual TbFloat32 CalcWeight( TbSoundListener* listener ) const = 0;

public:

    // 位置設定
    TbResult SetOffsetPos( const TbVector4& offset );

    // 位置取得
    const TbVector4& GetOffsetPos() const { return m_OffsetPos; }

private:

    TbVector4           m_OffsetPos;   //!< 位置情報

private:
    
    friend class TbSoundManager;
}:

}

#endif
