/*!
 * OpenAL
 * @author Miyake Shunsuke
 * @since 2011.07.14
 */

#include "tb_fwd.h"
#include "sound/tb_sound_listener.h"
#include "sound/tb_sound_manager.h"

#if TB_SOUNDAPI_IS_AL 

namespace toybox
{

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbSoundListener::TbSoundListener()
    : m_Pos()
    , m_Velo()
    , m_Direction()
    , m_BitArray()
    , m_Depend(0)
{
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbSoundListener::~TbSoundListener()
{
    if( IsActive() ){
        TbSoundManager::GetInstance().SetCurrentListener( NULL );
    }
}

/*!
 * 位置情報
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbResult TbSoundListener::SetPos( const TbVector4& pos )
{
    m_Pos = pos;
    if( IsActive() ){
        alListener3f( AL_POSITION , m_Pos.GetX() , m_Pos.GetY() , m_Pos.GetZ() ); 
    }
    return TB_S_OK;
}

/*!
 * 速度情報設定
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbResult TbSoundListener::SetVelo( const TbVector4& velo )
{
    m_Velo = velo;
    if( IsActive() ){
        alListener3f( AL_VELOCITY , m_Velo.GetX() , m_Velo.GetY() , m_Velo.GetZ() ); 
    }
    return TB_S_OK;
}

/*!
 * 方向情報設定
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
TbResult TbSoundListener::SetDirection( const TbVector4& dir )
{
    m_Direction = dir;
    if( IsActive() ){
        alListener3f( AL_DIRECTION , m_Direction.GetX() , m_Direction.GetY() , m_Direction.GetZ() ); 
    }
    return TB_S_OK;
}

/*!
 * 有効状態設定
 * @author Miyake Shunsuke
 * @since 2011.07.18
 */
void TbSoundListener::setActive( TbBool isActive )
{
    m_BitArray.SetBit( FLAG_ACTIVE , isActive );
    if( IsActive() ){
        alListener3f( AL_POSITION , m_Pos.GetX() , m_Pos.GetY() , m_Pos.GetZ() ); 
        alListener3f( AL_VELOCITY , m_Velo.GetX() , m_Velo.GetY() , m_Velo.GetZ() ); 
        alListener3f( AL_DIRECTION , m_Direction.GetX() , m_Direction.GetY() , m_Direction.GetZ() ); 
    }
}

}

#endif