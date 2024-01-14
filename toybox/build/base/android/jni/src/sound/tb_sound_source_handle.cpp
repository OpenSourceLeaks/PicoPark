/*!
 * 音源
 * @author teco
 */

#include "tb_fwd.h"
#include "sound/tb_sound_source_handle.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundSourceHandle::TbSoundSourceHandle()
    : m_Source(nullptr)
{
    m_Source = new TbSoundSource();
}

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundSourceHandle::TbSoundSourceHandle(TbSoundId soundId)
    : m_Source(nullptr)
{
    m_Source = new TbSoundSource(soundId);
}

/*!
 * コンストラクタ
 * @author teco
 */
TbSoundSourceHandle::TbSoundSourceHandle(const char* name)
    : m_Source(nullptr)
{
    m_Source = new TbSoundSource(name);
}

/*!
 * デストラクタ
 * @author teco
 */
TbSoundSourceHandle::~TbSoundSourceHandle()
{
    if (m_Source) {
        m_Source->Stop();
    }
    TB_SAFE_RELEASE(m_Source);
}

}
