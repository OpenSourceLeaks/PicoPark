/*!
 * @file
 * @brief DirectX9の固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_FIXED_SHADER_DX9_H_
#define _INCLUDED_TB_FIXED_SHADER_DX9_H_

namespace toybox
{

/*!
 * 固定機能シェーダ依存コード
 * @author Miyake Shunsuke
 * @since 2010.07.04
 */
class TbFixedShaderDepend
{
public:
    // コンストラクタ
    TbFixedShaderDepend();
    // デストラクタ
    ~TbFixedShaderDepend();
private:
    TbBitArray16            m_BitArray;         //!< ビット配列
};

}

#endif