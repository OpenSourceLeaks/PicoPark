/*!
 * @file
 * @brief ストリームリーダー
 * @author Miyake Shunsuke
 */

#include "tb_fwd.h"

#if 0
#include "base/system/tb_assert.h"
#include "base/math/tb_math_util.h"

namespace toybox
{


/*!
 * コンストラクタ
 * @param buf 読み込むバッファ
 * @param size バッファサイズ
 * @author Miyake Shunsuke
 */
template <typename T>
TbStreamReaderBase<T>::TbStreamReaderBase(T* buf,TbUint32 size)
    : m_BufIndex(0)
    , m_BufSize(size)
{
    m_Buf = reinterpret_cast<TbAddr>(buf);
    if(!m_Buf){
        TB_ASSERT_MSG(TB_FALSE,"m_Buf is NULL");
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
template <typename T>
TbStreamReaderBase<T>::~TbStreamReaderBase()
{
}

/*!
 * TbChar8型読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbChar8   TbStreamReaderBase<T>::ReadChar()
{
    const void* buf = Read(sizeof(TbChar8));
    if(buf){
        return *reinterpret_cast<const TbChar8*>(buf);
    }
    return 0;
}

/*!
 * Uint8値読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbUint8 TbStreamReaderBase<T>::ReadUint8()
{
    const void* buf = Read(sizeof(TbUint8));
    if(buf){
        return *reinterpret_cast<const TbUint8*>(buf);
    }
    return 0U;
}

/*!
 * Uint16値読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbUint16 TbStreamReaderBase<T>::ReadUint16()
{
    const void* buf = Read(sizeof(TbUint16));
    if(buf){
        return *reinterpret_cast<const TbUint16*>(buf);
    }
    return 0U;
}

/*!
 * Sint16値読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbSint16 TbStreamReaderBase<T>::ReadSint16()
{
    const void* buf = Read(sizeof(TbSint16));
    if(buf){
        return *reinterpret_cast<const TbSint16*>(buf);
    }
    return 0;
}

/*!
 * Uint32値読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbUint32 TbStreamReaderBase<T>::ReadUint32()
{
    const void* buf = Read(sizeof(TbUint32));
    if(buf){
        return *reinterpret_cast<const TbUint32*>(buf);
    }
    return 0U;
}

/*!
 * Sint32値読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbSint32 TbStreamReaderBase<T>::ReadSint32()
{
    const void* buf = Read(sizeof(TbSint32));
    if(buf){
        return *reinterpret_cast<const TbSint32*>(buf);
    }
    return 0;
}

/*!
 * float値読み込み
 * @author Miyake Shunsuke
 */
template <typename T>
TbFloat32  TbStreamReaderBase<T>::ReadFloat()
{
    const void* buf = Read(sizeof(TbFloat32));
    if(buf){
        return *reinterpret_cast<const TbFloat32*>(buf);
    }
    return 0.0f;
}

/*!
 * 読み込む
 * @author Miyake Shunsuke
 */
template <typename T>
T* TbStreamReaderBase<T>::Read(TbUint32 size)
{
    if(m_Buf && m_BufIndex < m_BufSize){
        T* resultBuf = reinterpret_cast<T*>(m_Buf + m_BufIndex);
        Seek(size);
        return resultBuf;
    }
    return NULL;
}

/*!
 * 進める
 * @author Miyake Shunsuke
 */
template <typename T>
void TbStreamReaderBase<T>::Seek(TbUint32 size)
{
    m_BufIndex = TbClamp(m_BufIndex + size,static_cast<TbUint32>(0),m_BufSize);
}

}

#endif
