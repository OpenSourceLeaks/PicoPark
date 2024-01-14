/*! 
 * 頂点フォーマット
 * @date 2010.08.10
 * @author Miyake Shunsuke
 */
#ifndef _INCLUDED_TB_VERTEX_FORMAT_H_
#define _INCLUDED_TB_VERTEX_FORMAT_H_

#include <graphics/render/3dapi/tb_3dapi_types.h>
#include <graphics/render/3dapi/tb_color.h>

namespace toybox
{

class TbRenderDevice;

struct TbVertexXYZ
{
    TbFloat32 x , y , z;
};

struct TbVertexXYZColor
{
    TbFloat32 x , y , z;
    TbColor   color;
};

struct TbVertexXYZNormalColor
{
    TbFloat32 x , y , z;
    TbFloat32 normX , normY , normZ;
    TbColor   color;
};

struct TbVertexXYZNormalColorTex0
{
    TbFloat32 x , y , z;
    TbFloat32 normX , normY , normZ;
    TbColor   color;
    TbFloat32 u , v;
};

struct TbVertexXYZColorTex0
{
    TbFloat32 x , y , z;
    TbColor   color;
    TbFloat32 u , v;
};

/*! 
 * 頂点エレメント
 */
struct TbVertexElement
{
    // 型
    enum Type
    {
        TYPE_FLOAT1 = 0,
        TYPE_FLOAT2 ,
        TYPE_FLOAT3 , 
        TYPE_FLOAT4 ,
        TYPE_COLOR  , 
        TYPE_UBYTE4 ,
        TYPE_SHORT2 ,
        TYPE_SHORT4 ,

        TYPE_UBYTE4N,
        TYPE_SHORT2N,
        TYPE_SHORT4N,
        TYPE_USHORT2N,
        TYPE_USHORT4N,
        TYPE_UDEC3,
        TYPE_DEC3N,
        TYPE_FLOAT16_2,
        TYPE_FLOAT16_4,
        TYPE_UNSUSED,
        TYPE_MAX,
    };
    
    // 方法
    enum Method
    {
        METHOD_DEFAULT,
        METHOD_PARTIALU,
        METHOD_PARTIALV,
        METHOD_CROSSUV,
        METHOD_UV,
        METHOD_LOOKUP,
        METHOD_LOOKUPPRESAMPLED,
        METHOD_MAX,
    };
    
    // 利用用途
    enum Usage
    {
        USAGE_POSITION,     // 位置データ
        USAGE_BLENDWEIGHT,  // ブレンディングウェイト値
        USAGE_BLENDINDICES, // ブレンディングインデックス値
        USAGE_NORMAL,       // 法線
        USAGE_PSIZE,        // ポイントサイズデータ
        USAGE_TEXCOORD,     // テクスチャ座標
        USAGE_TANGENT,      // 接線データ
        USAGE_BINORMAL,     // 従法線データ
        USAGE_TESSFACTOR,   // 
        USAGE_POSITIONT,    // トランスフォームされた位置データ
        USAGE_COLOR,        // カラー値
        USAGE_FOG,          // フォグデータ
        USAGE_DEPTH,        // 深度データ
        USAGE_SAMPLE,       // サンプルデータ( 参照するディスプレ－スメント値 )
        USAGE_USER,         // ユーザー定義
        USAGE_MAX,
    };

    TbUint16 stream;    // ストリーム番号
    TbUint16 offset;    // データの開始オフセット値
    TbUint8  type;      // 型
    TbUint8  method;     // 方法
    TbUint8  usage;      // 利用用途
    TbUint8  usageIndex; // 利用用途内番号
};

}

// 依存コード
#include "./depend/tb_vertex_format_depend.h"

namespace toybox
{

/*!
 * 頂点フォーマット
 */
class TbVertexFormat
{
public:

    static const TbVertexFormat XYZ;                        // XYZ
    static const TbVertexFormat XYZ_COLOR;                  // XYZ + カラー
    static const TbVertexFormat XYZ_NORMAL;                 // XYZ + 法線 + カラー
    static const TbVertexFormat XYZ_NORMAL_COLOR;           // XYZ + 法線 + カラー
    static const TbVertexFormat XYZ_TEX0SIZE2;              // XYZ + UV
    static const TbVertexFormat XYZ_COLOR_TEX0SIZE2;        // XYZ + カラー + UV
    static const TbVertexFormat XYZ_NORMAL_TEX0SIZE2;       // XYZ + 法線 + UV
    static const TbVertexFormat XYZ_NORMAL_COLOR_TEX0SIZE2; // XYZ + 法線 + UV

    enum Type
    {
        TYPE_XYZ,
        TYPE_XYZ_COLOR,
        TYPE_XYZ_NORMAL,
        TYPE_XYZ_NORMAL_COLOR,
        TYPE_XYZ_TEX0SIZE2,
        TYPE_XYZ_COLOR_TEX0SIZE2,
        TYPE_XYZ_NORMAL_TEX0SIZE2,
        TYPE_XYZ_NORMAL_COLOR_TEX0SIZE2,
        TYPE_MAX,
    };

public: // コンストラクタ

    // コンストラクタ
    TbVertexFormat( const TbVertexElement* elements , const TbSint32 elementCount );

    // デストラクタ
    ~TbVertexFormat();

public: // 定数構築用の特別なコンストラクタ( アプリケーション側で利用するべからず )

    explicit TbVertexFormat( const Type type );

public: // 公開関数
    
    //! ストリーム毎の必要なサイズ
    TbUint32 GetSize( TbUint32 stream );

    //! テクスチャ座標数取得
    TbSint32 GetTexcoordCount() const { 
        return m_TexcoordCount; 
    }

    //! 依存クラス参照
    const TbVertexFormatDepend& GetDepend() const { 
        return m_Depend; 
    }

private: // 内部変数
    TbSint32             m_TexcoordCount;                    //!< テクスチャ座標要素がいくつあるか
    TbVertexFormatDepend m_Depend;                           //!< 依存コード
};

}

#endif