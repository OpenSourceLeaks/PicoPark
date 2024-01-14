/*!
 * シェーダープログラムクラス
 * @author Miyake Shunsuke
 * @since 2013.04.09
 */
 
#ifndef _INCLUDED_TB_SHADER_PROGRAM_H_
#define _INCLUDED_TB_SHADER_PROGRAM_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_vertex_format.h>
#include <base/container/tb_pool.h>
#include <graphics/render/3dapi/tb_shader_types.h>
#include <base/math/tb_matrix.h>
#include <graphics/render/3dapi/tb_shader_manager.h>

namespace toybox
{
    class TbMatrix44;
    class TbRenderDevice;
}

#include "./depend/tb_shader_program_depend.h" // 依存ファイル

namespace toybox
{

/*!
 * シェーダー管理クラス
 * @author Miyake Shunsuke
 * @since 2010.06.11
 */
class TbShaderProgram
{
    friend class TbShaderProgramDepend;
public:

    // コンストラクタ
    TbShaderProgram();

    // デストラクタ
    ~TbShaderProgram();

public:
    
    // 現在カレントプログラムとしえ設定されているか
    TbBool IsCurrent() const {
        return TbShaderManager::GetInstance().GetCurrentProgram() == this;
    }

    // 有効なプログラムか
    TbBool IsValid() const;

    // 現在の組み合わせで更新
    TbResult Update();
    
    // シェーダ設定 (Updateを呼んで初めて反映されます)
    TbResult SetShader( const TbShaderType type , const TbShaderId shaderId ) {
        if( IsCurrent() ){
            // 現在使われている状態では更新できないようにする
            return TB_E_FAIL;
        }
        if( type < TB_ARRAY_COUNT_OF(m_CurrentShaderId) ){
            m_UpdateShaderId[type] = shaderId;
            m_BitArray.SetBit(FLAG_ENABLE_UPDATE);
            return TB_S_OK;
        }
        return TB_E_FAIL;
    }

    // 現在設定されているシェーダID取得
    TbShaderId GetCurrentShader( const TbShaderType type ) const {
        if( type < TB_ARRAY_COUNT_OF(m_CurrentShaderId) ){
            return m_CurrentShaderId[type];
        }
        return TB_SHADER_ID_INVALID;
    }

    // 現在設定されているシェーダが有効
    TbBool IsActiveShader( const TbShaderType type ) const {
        if( type < TB_ARRAY_COUNT_OF(m_CurrentShaderId) ){
            return TB_SHADER_ID_INVALID != m_CurrentShaderId[type];
        }
        return TB_FALSE;
    }

    // 現在適用されているフォーマット
    const TbVertexFormat* GetVertexFormat() const {
        return m_VertexFormat;
    }

public:

    // 依存コード取得
    TbShaderProgramDepend& GetDepend(){ 
        return m_Depend; 
    }

    // 依存コード取得
    const TbShaderProgramDepend& GetDepend() const { 
        return m_Depend; 
    }

private:

    enum Flag
    {
        FLAG_ENABLE_UPDATE , // 更新が必要
        FLAG_MAX , 
    };

private:

    TbBitArray32          m_BitArray;                            //!< ビット配列
    TbShaderId            m_UpdateShaderId[TB_SHADER_TYPE_MAX];  //!< 設定待ちシェーダ
    TbShaderId            m_CurrentShaderId[TB_SHADER_TYPE_MAX]; //!< 現在設定されているシェーダ
    const TbVertexFormat* m_VertexFormat;                        //1< 対応しているフォーマット
    TbShaderProgramDepend m_Depend;                              //!< 依存コード

};

}

#endif
