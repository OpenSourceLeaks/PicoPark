/*!
 * OpenAL
 * @author teco
 */

#include "tb_fwd.h"
#include "sound/tb_sound_listener.h"
#include "sound/tb_sound_manager.h"

#if TB_SOUNDAPI_IS_XAUDIO

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
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
 * @author teco
 */
TbSoundListener::~TbSoundListener()
{
    if( IsActive() ){
        TbSoundManager::GetInstance().SetCurrentListener( NULL );
    }
}

/*!
 * 位置情報
 * @author teco
 */
TbResult TbSoundListener::SetPos( const TbVector4& pos )
{
    m_Pos = pos;
    if( IsActive() ){
    }
    return TB_S_OK;
}

/*!
 * 速度情報設定
 * @author teco
 */
TbResult TbSoundListener::SetVelo( const TbVector4& velo )
{
    m_Velo = velo;
    if( IsActive() ){
    }
    return TB_S_OK;
}

/*!
 * 方向情報設定
 * @author teco
 */
TbResult TbSoundListener::SetDirection( const TbVector4& dir )
{
    m_Direction = dir;
    if( IsActive() ){
    }
    return TB_S_OK;
}

/*!
 * 有効状態設定
 * @author teco
 */
void TbSoundListener::setActive( TbBool isActive )
{
    m_BitArray.SetBit( FLAG_ACTIVE , isActive );
    if( IsActive() ){
    }
}

}

#endif