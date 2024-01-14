/*!
 * シェーダ管理クラス
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */
 
#ifndef _INCLUDED_TB_SHADER_MANAGER_H_
#define _INCLUDED_TB_SHADER_MANAGER_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_vertex_format.h>
#include <base/container/tb_pool.h>
#include <graphics/render/3dapi/tb_shader_types.h>
#include <base/math/tb_matrix.h>
#include <base/util/tb_singleton.h>

namespace toybox
{
    class TbMatrix44;
    class TbRenderDevice;
    class TbShaderProgram;
    class TbConstantBuffer;
}

#include "./depend/tb_shader_manager_depend.h" // 依存ファイル

namespace toybox
{

//! 定数ハンドル
class TbShaderConstHandle
{
    friend class TbShaderManager;
public:
    
    // コンストラクタ
    TbShaderConstHandle() {}

    // 設定
    void Set( TbUint32 index );

public:
    // ==比較演算子
    TbBool operator==(const TbShaderConstHandle& target) {
        return m_Depend == target.m_Depend;
    }
    // !=比較演算子
    TbBool operator!=(const TbShaderConstHandle& target) {
        return m_Depend != target.m_Depend;
    }

private:
    TbShaderConstHandleDepend m_Depend;
};
extern TbShaderConstHandle TB_SHADER_CONST_HANDLE_INVALID;

/*!
 * シェーダー管理クラス
 * @author Miyake Shunsuke
 * @since 2010.06.11
 */
class TbShaderManager : public TbSingletonRef<TbShaderManager>
{
public:

    friend class TbShaderManagerDepend;

    struct ProgramCreateParam
    {
        ProgramCreateParam() : vertexFormat(nullptr) {
            TbMemClear(shaderFilePath);
        }
        TbVertexFormat* vertexFormat;
        const char*     shaderFilePath[TB_SHADER_TYPE_MAX];
    };

private:

    // コンストラクタ
    TbShaderManager();

    // デストラクタ
    ~TbShaderManager();

public:

    // 初期化
    void Initialize();

    // 後始末
    void Finalize();

public: // シェーダ

    // 各シェーダの格納限界を設定
    void SetCapacity( TbUint32 vertexMax , TbUint32 pixelMax ) {
        m_Depend.SetCapacity( vertexMax , pixelMax );
    }

    // シェーダプログラム生成(メモリから)
    TbShaderId CreateShader( TbShaderType type ,
                             TbShaderBufferType bufferType , 
                             const TbChar8* buffer ,
                             const TbUint32 bufferSize = 0 , 
                             const TbChar8* entry = nullptr ,
                             const TbChar8** args = nullptr );

    // ファイルからシェーダプログラム生成(ファイル読み込みは完了復帰)
    TbShaderId CreateShaderFromFile( TbShaderType type , 
                                     TbShaderBufferType bufferType , 
                                     const TbChar8* filepath ,
                                     const TbChar8* entry = nullptr ,
                                     const TbChar8** args = nullptr );

    // シェーダリロード
    void ReloadShader(TbShaderId id, 
                      TbShaderType type,
                      TbShaderBufferType bufferType,
                      const TbChar8* buffer,
                      const TbUint32 bufferSize = 0,
                      const TbChar8* entry = nullptr,
                      const TbChar8** args = nullptr);

    // シェーダリロード
    void ReloadShaderFromFile( TbShaderId id , 
                               TbShaderType type,
                               TbShaderBufferType bufferType,
                               const TbChar8* filepath,
                               const TbChar8* entry = nullptr,
                               const TbChar8** args = nullptr);

    // シェーダ全削除
    void DeleteShaderAll();

    // 頂点フォーマットを頂点シェーダにバインド
    TbBool BindVertexFormat( const TbVertexFormat* format , const TbShaderId vertexShaderId );

public: // プログラム
    
    // プログラム作成
    TbShaderProgram* CreateProgram();

    // プログラム生成(パラメータ付き)
    TbShaderProgram* CreateProgram( const ProgramCreateParam& param );

    // プログラム削除
    void DeleteProgram( TbShaderProgram* program );

    // シェーダプログラム設定
    TbResult SetProgram( TbShaderProgram* program );

    // 現在設定されているシェーダID取得
    TbShaderProgram* GetCurrentProgram() {
        return m_ShaderProgram;
    }

    // 現在設定されているシェーダID取得
    const TbShaderProgram* GetCurrentProgram() const {
        return m_ShaderProgram;
    }

public: // シェーダ定数

    // 利用可能な定数数取得
    TbUint32 GetConstantCount() const;

    // bool型シェーダ定数設定
    void SetConstantBool( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbBool* value ,TbUint32 count );

    // Float32型シェーダ定数設定
    void SetConstantF32( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbFloat32* value , TbUint32 count );

    // Sint32型シェーダ定数設定
    void SetConstantS32( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbSint32* value , TbUint32 count );

    // 行列型シェーダ定数設定
    void SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbMatrix44* value , TbUint32 count );

    // 行列型シェーダ定数設定
    void SetConstantMatrix( TbUint32 shaderBit , TbShaderConstHandle& handle , const TbMatrix43* value , TbUint32 count );

    // 定数バッファ設定
    void SetConstantBuffer(TbUint32 shaderBit, TbShaderConstHandle& handle, TbConstantBuffer* buffer);

public: // 予約シェーダ定数

    // ワールド行列設定
    void SetWorldMatrix( const TbMatrix44& mtx ) {
        if( m_Matrix[MATRIX_WORLD] != mtx ){
            m_Matrix[MATRIX_WORLD] = mtx;
            updateWVP();
        }
    }

    // ビュー行列設定
    void SetViewMatrix( const TbMatrix44& mtx ) {
        if( m_Matrix[MATRIX_VIEW] != mtx ){
            m_Matrix[MATRIX_VIEW] = mtx;
            updateWVP();
        }
    }

    // 射影行列設定
    void SetProjectionMatrix( const TbMatrix44& mtx ) 
    {
        if( m_Matrix[MATRIX_PROJECTION] != mtx ){
            m_Matrix[MATRIX_PROJECTION] = mtx;
            updateWVP();
        }
    }

    // ビュー射影行列設定
    void SetViewProjectionMatrix( const TbMatrix44& view , const TbMatrix44& projection ) 
    {
        m_Matrix[MATRIX_VIEW] = view;
        m_Matrix[MATRIX_PROJECTION] = projection;
        updateWVP();
    }

    // ワールドビュー射影行列設定
    void SetWorldViewProjectionMatrix( const TbMatrix44& world , 
                                       const TbMatrix44& view , 
                                       const TbMatrix44& projection ) 
    {
        m_Matrix[MATRIX_WORLD] = world;
        m_Matrix[MATRIX_VIEW] = view;
        m_Matrix[MATRIX_PROJECTION] = projection;
        updateWVP();
    }

    // ワールドビュー射影行列取得
    void GetWorldViewProjectionMatrix( TbMatrix44& dst ) {
        dst = m_Matrix[MATRIX_PROJECTION] * m_Matrix[MATRIX_VIEW] * m_Matrix[MATRIX_WORLD];
    }

public:

    // 依存コード取得
    TbShaderManagerDepend& GetDepend(){ 
        return m_Depend; 
    }

    // 依存コード取得
    const TbShaderManagerDepend& GetDepend() const { 
        return m_Depend; 
    }

public:

    // TbVertexElement::Usage に対応したシェーダコード内の変数名取得
    const TbChar8* GetElementUsageVarName( TbVertexElement::Usage usage );

    // TbVertexElement::Usage に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
    void SetElementUsageVarName( TbVertexElement::Usage usage , const TbChar8* name );

    // TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名取得
    const TbChar8* GetElementUsageUserVarName( TbSint32 index );

    // TbVertexElement::USAGE_USER に対応したシェーダコード内の変数名設定( 引数はリテラル文字である必要がある )
    void SetElementUsageUserVarName( TbSint32 index , const TbChar8* name );

    // テクスチャ変数名設定
    void SetTextureVarName( const TbChar8* name );

    // テクスチャ変数名取得
    const TbChar8* GetTextureVarName();

public:

    // デバイスのBeginSceneと同時に呼ばれる関数(必要であれば利用)
    void OnBeginScene();

    // デバイスのEndSceneと同時に呼ばれる関数(必要であれば利用)
    void OnEndScene();

private:

    enum {
        MATRIX_WORLD , 
        MATRIX_VIEW , 
        MATRIX_PROJECTION , 
        MATRIX_MAX
    };

    enum {
        CONST_HANDLE_WVP , 
        CONST_HANDLE_MAX , 
    };

    enum {
        FLAG_UPDATE_WVP , 
    };

private:

    // world view projection行列更新
    void updateWVP();

private:

    TbBitArray32          m_BitArray;                            //!< ビット配列
    TbShaderProgram*      m_ShaderProgram;                       //!< 現在設定されているプログラム
    TbMatrix44            m_Matrix[MATRIX_MAX];                  //!< 行列
    TbShaderConstHandle   m_ConstHandle[CONST_HANDLE_MAX];       //1< 予約定数ハンドル
    TbShaderManagerDepend m_Depend;                              //!< 依存コード
private:

    friend class TbRenderDevice;    //!< 生成主に任せる

};

}

#endif
