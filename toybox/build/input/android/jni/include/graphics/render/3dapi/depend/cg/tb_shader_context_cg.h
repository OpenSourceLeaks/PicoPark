/*!
 * デバイス別シェーダークラス( Cg )
 * @author Miyake Shunsuke
 * @since 2010.06.09
 */
 
#ifndef _INCLUDED_TB_SHADER_CONTEXT_CG_H_
#define _INCLUDED_TB_SHADER_CONTEXT_CG_H_

#include <util/containers/tb_pool.h>
#include <util/containers/tb_array.h>

namespace toybox
{

class TbVertexFormat;

//! 頂点シェーダ
struct _TbVertexShader
{
    _TbVertexShader() : shader(NULL) , format( NULL ) , attribLocationList( 0 ){}
    CGprogram            shader;               // シェーダ
    TbVertexFormat*      format;               // 頂点フォーマット
    TbArray<CGparameter> attribLocationList;   // 頂点属性インデックスリスト
};
typedef _TbVertexShader* TbVertexShader;

//! 頂点シェーダプール
typedef TbPool<TbVertexShader,NULL> TbVertexShaderPool;

//! ピクセルシェーダ
typedef CGprogram TbPixelShader;
//! ピクセルシェーダプール
typedef TbPool<TbPixelShader,NULL> TbPixelShaderPool;

/*!
 * CgGL用シェーダー依存クラス
 * @author Miyake Shunsuke
 * @since 2010.06.11
 */
class TbShaderManagerDepend
{
public:

    enum ParameterSettingMode
    {
        PARAMETER_SETTING_MODE_IMMEDIATE = CG_IMMEDIATE_PARAMETER_SETTING,
        PARAMETER_SETTING_MODE_DEFERRED = CG_DEFERRED_PARAMETER_SETTING
    };

public:

    // コンストラクタ
    TbShaderManagerDepend( TbRenderDevice* device , TbUint32 vertexMax , TbUint32 pixelMax , TbUint32 effectMax = 0 );

    // デストラクタ
    ~TbShaderManagerDepend();

public:
    
    // パラメータ設定モード設定
    void SetParameterSettingMode( ParameterSettingMode mode );

    // パラメータ設定モード取得
    TB_INLINE ParameterSettingMode GetParameterSettingMode() { return m_ParameterSettingMode; }

    // 頂点シェーダパラメータ更新(設定モードがDEFERREDの時は呼ばなければいけない
    void UpdateVertexParameter( TbShaderId shaderId );

    // ピクセルシェーダパラメータ更新(設定モードがDEFERREDの時は呼ばなければいけない
    void UpdatePixelParameter( TbShaderId shaderId );

public: // GL用

    // パラメータID取得
    TbBool BindAttributeLocation( const char* name , TbSint32 index );

    // 頂点属性ポインタの設定( sizeは次元数 )
    void SetVertexAttribPointer( TbSint32 index , TbSint32 size , TbSint32 type , TbSint32 stride , const void* pointer );

    // 頂点属性配列の利用
    void EnableVertexAttribArray( TbSint32 index );

    // 頂点属性配列の利用
    void DisableVertexAttribArray( TbSint32 index );

private:    // 非公開関数

    // 頂点シェーダ取得
    TbVertexShader getVertexShader( TbShaderId id ){ return m_VertexShaderPool.Get(id); }

    // ピクセルシェーダ取得
    TbPixelShader getPixelShader( TbShaderId id ){ return m_PixelShaderPool.Get(id); }

    // 最新の頂点シェーダ用プロファイル取得
    CGprofile getLatestVertexProfile(){ return m_LatestVertexProfile; }

    // 最新のピクセルシェーダ用プロファイル取得
    CGprofile getLatestPixelProfile(){ return m_LatestPixelProfile; }
       
    // エラーチェック
    void checkError( const char* situation );

    // 頂点シェーダ生成
    TbShaderId createVertexShaderFromFile( CGenum programType ,
                                           const char* filePath ,
                                           const char* entry ,
                                           const char** args );

    // ピクセルシェーダ生成
    TbShaderId createPixelShaderFromFile(  CGenum programType ,
                                           const char* filePath ,
                                           const char* entry ,
                                           const char** args );

    // シェーダ生成( ファイルから )
    CGprogram createShaderFromFile( CGprofile profile ,
                                    CGenum programType ,
                                    const char* filePath ,
                                    const char* entry ,
                                    const char** args );

private:    // 非公開変数

    TbBitArray16           m_BitArray;                //!< ビット配列
    ParameterSettingMode   m_ParameterSettingMode;    //!< パラメータ設定モード
    TbRenderDevice*        m_Device;                  //!< 関連付けされているデバイス
    CGcontext              m_Context;                 //!< コンテクスト
    CGprofile              m_LatestVertexProfile;     //!< 最新の頂点プロファイル
    CGprofile              m_LatestPixelProfile;      //!< 最新のピクセルプロファイル
    TbVertexShaderPool     m_VertexShaderPool;        //!< 頂点シェーダプール
    TbPixelShaderPool      m_PixelShaderPool;         //!< ピクセルシェーダプール

private:

    friend class TbShaderManager;

};

}

#endif
