/*!
 * サウンドリスナー
 * @author Miyake Shunsuke
 * @since 2011.07.15
 */

#include <sound/tb_sound_types.h>
#include <base/math/tb_vector4.h>
#include "./depend/tb_sound_listener_depend.h"

namespace toybox
{

class TbSoundListener
{
    friend class TbSoundManager;
public:

    // コンストラクタ
    TbSoundListener();
    // デストラクタ
    ~TbSoundListener();

public:
    
    // 位置情報
    TbResult SetPos( const TbVector4& pos );
    const TbVector4& GetPos() const { return m_Pos; }

    // 速度情報
    TbResult SetVelo( const TbVector4& velo );
    const TbVector4& GetVelo() const { return m_Velo; }
    
    // 方向情報
    TbResult SetDirection( const TbVector4& dir );
    const TbVector4& GetDirection() const { return m_Direction; }

    // アクティブ設定
    TbBool IsActive() const { return m_BitArray.Test( FLAG_ACTIVE ); }

public: // 依存部分

    TbSoundListenerDepend& GetDepend() { return m_Depend; }
    const TbSoundListenerDepend& GetDepend() const { return m_Depend; }

private:

    enum{
        FLAG_ACTIVE ,            //!< カレント
    };

private:

    void setActive( TbBool isActive );

private:

    TbBitArray32          m_BitArray;   //!< ビット配列
    TbVector4             m_Pos;        //!< 位置情報
    TbVector4             m_Velo;       //!< 速度情報
    TbVector4             m_Direction;  //!< 方向情報
    TbSoundListenerDepend m_Depend;     //!< 依存情報

};

}
