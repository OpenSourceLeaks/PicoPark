/*!
 * DirectX依存の頂点フォーマット
 * @author Miyake Shunsuke
 * @since 2010.08.08
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_format.h"

#if TB_3DAPI_TARGET == TB_3DAPI_DX9

#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/3dapi/tb_vertex_format.h"

namespace toybox
{

static BYTE ELEMENT_TYPE_TABLE[ TbVertexElement::TYPE_MAX ] = 
{
    D3DDECLTYPE_FLOAT1        ,  // 1D float expanded to (value, 0., 0., 1.)
    D3DDECLTYPE_FLOAT2        ,  // 2D float expanded to (value, value, 0., 1.)
    D3DDECLTYPE_FLOAT3        ,  // 3D float expanded to (value, value, value, 1.)
    D3DDECLTYPE_FLOAT4        ,  // 4D float
    D3DDECLTYPE_D3DCOLOR      ,  // 4D packed unsigned bytes mapped to 0. to 1. range
                                 // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
    D3DDECLTYPE_UBYTE4        ,  // 4D unsigned byte
    D3DDECLTYPE_SHORT2        ,  // 2D signed short expanded to (value, value, 0., 1.)
    D3DDECLTYPE_SHORT4        ,  // 4D signed short

    // The following types are valid only with vertex shaders >= 2.0

    D3DDECLTYPE_UBYTE4N       ,  // Each of 4 bytes is normalized by dividing to 255.0
    D3DDECLTYPE_SHORT2N       ,  // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
    D3DDECLTYPE_SHORT4N       ,  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
    D3DDECLTYPE_USHORT2N      ,  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
    D3DDECLTYPE_USHORT4N      ,  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
    D3DDECLTYPE_UDEC3         ,  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
    D3DDECLTYPE_DEC3N         ,  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
    D3DDECLTYPE_FLOAT16_2     ,  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
    D3DDECLTYPE_FLOAT16_4     ,  // Four 16-bit floating point values
    D3DDECLTYPE_UNUSED        ,  // When the type field in a decl is unused.
};
TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF( ELEMENT_TYPE_TABLE ) == TbVertexElement::TYPE_MAX );

static BYTE ELEMENT_METHOD_TABLE[ TbVertexElement::METHOD_MAX ] = 
{
    D3DDECLMETHOD_DEFAULT,
    D3DDECLMETHOD_PARTIALU,
    D3DDECLMETHOD_PARTIALV,
    D3DDECLMETHOD_CROSSUV,              // Normal
    D3DDECLMETHOD_UV,
    D3DDECLMETHOD_LOOKUP,               // Lookup a displacement map
    D3DDECLMETHOD_LOOKUPPRESAMPLED,     // Lookup a pre-sampled displacement map
};
TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF( ELEMENT_METHOD_TABLE ) == TbVertexElement::METHOD_MAX );

static BYTE ELEMENT_USAGE_TABLE[ TbVertexElement::USAGE_MAX ] =
{
    D3DDECLUSAGE_POSITION,
    D3DDECLUSAGE_BLENDWEIGHT,   // 1
    D3DDECLUSAGE_BLENDINDICES,  // 2
    D3DDECLUSAGE_NORMAL,        // 3
    D3DDECLUSAGE_PSIZE,         // 4
    D3DDECLUSAGE_TEXCOORD,      // 5
    D3DDECLUSAGE_TANGENT,       // 6
    D3DDECLUSAGE_BINORMAL,      // 7
    D3DDECLUSAGE_TESSFACTOR,    // 8
    D3DDECLUSAGE_POSITIONT,     // 9
    D3DDECLUSAGE_COLOR,         // 10
    D3DDECLUSAGE_FOG,           // 11
    D3DDECLUSAGE_DEPTH,         // 12
    D3DDECLUSAGE_SAMPLE,        // 13
    D3DDECLUSAGE_POSITION,      // USER_DATA
};
TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF( ELEMENT_USAGE_TABLE ) == TbVertexElement::USAGE_MAX );

static const DWORD FVF_LIST[] = 
{
    D3DFVF_XYZ ,
    D3DFVF_XYZ | D3DFVF_DIFFUSE ,
    D3DFVF_XYZ | D3DFVF_NORMAL ,
    D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE , 
    D3DFVF_XYZ | D3DFVF_TEXCOORDSIZE2(0) , 
    D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) , 
    D3DFVF_XYZ | D3DFVF_NORMAL  | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) , 
    D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) , 
};

static const TbSint32 TEXCOORD_COUNT[] = 
{
    0 ,
    0 ,
    0 ,
    0 , 
    1 , 
    1 , 
    1 , 
    1 , 
};

const TbVertexFormat TbVertexFormat::XYZ( TbVertexFormat::TYPE_XYZ );
const TbVertexFormat TbVertexFormat::XYZ_COLOR( TbVertexFormat::TYPE_XYZ_COLOR );
const TbVertexFormat TbVertexFormat::XYZ_NORMAL( TbVertexFormat::TYPE_XYZ_NORMAL );
const TbVertexFormat TbVertexFormat::XYZ_NORMAL_COLOR( TbVertexFormat::TYPE_XYZ_NORMAL_COLOR );
const TbVertexFormat TbVertexFormat::XYZ_TEX0SIZE2( TbVertexFormat::TYPE_XYZ_TEX0SIZE2 );
const TbVertexFormat TbVertexFormat::XYZ_COLOR_TEX0SIZE2( TbVertexFormat::TYPE_XYZ_COLOR_TEX0SIZE2 );
const TbVertexFormat TbVertexFormat::XYZ_NORMAL_TEX0SIZE2( TbVertexFormat::TYPE_XYZ_NORMAL_TEX0SIZE2 );
const TbVertexFormat TbVertexFormat::XYZ_NORMAL_COLOR_TEX0SIZE2( TbVertexFormat::TYPE_XYZ_NORMAL_COLOR_TEX0SIZE2 );
  
// 依存コード

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.08
 */
TbVertexFormatDepend::TbVertexFormatDepend( const TbVertexElement* elements , const TbSint32 elementCount )
    : m_VertexDeclaration( nullptr )
    , m_FVF(0)
{
    D3DVERTEXELEMENT9* d3dElements = TB_NEW D3DVERTEXELEMENT9[ elementCount + 1 ];
    for( TbSint32 i = 0; i < elementCount; ++i ){
        TB_ASSERT( TbVertexElement::USAGE_USER != elements[i].usage );
        d3dElements[i].Stream = elements[i].stream;
        d3dElements[i].Offset = elements[i].offset;
        d3dElements[i].Type = ELEMENT_TYPE_TABLE[ elements[i].type ];
        d3dElements[i].Method = ELEMENT_METHOD_TABLE[ elements[i].method ];
        d3dElements[i].Usage = ELEMENT_USAGE_TABLE[ elements[i].usage ];
        d3dElements[i].UsageIndex = elements[i].usageIndex;
    }
    D3DVERTEXELEMENT9 end = D3DDECL_END();
    d3dElements[ elementCount ] = end;
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.GetDepend().GetDeviceDX9()->CreateVertexDeclaration( d3dElements , &m_VertexDeclaration );
    TB_SAFE_DELETE_ARRAY( d3dElements );
}


/*!
 * 定数フォーマット構築用コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.22
 */
TbVertexFormatDepend::TbVertexFormatDepend( const TbSint32 type )
    : m_VertexDeclaration( nullptr )
    , m_FVF(0)
{
    m_FVF = FVF_LIST[type];
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.08
 */
TbVertexFormatDepend::~TbVertexFormatDepend()
{
    TB_SAFE_RELEASE( m_VertexDeclaration );
}

//

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.22
 */
TbVertexFormat::TbVertexFormat( const TbVertexElement* elements , const TbSint32 elementCount )
    : m_TexcoordCount( 0 )
    , m_Depend( elements , elementCount )
{
    for( TbSint32 i = 0; i < elementCount; ++i ){
        if( TbVertexElement::USAGE_TEXCOORD == elements[ i ].usage ){
            ++m_TexcoordCount;
        }
    }
}

/*!
 * 定数構築用の特別なコンストラクタ( アプリケーション側で利用するべからず )
 * @author Miyake Shunsuke
 * @since 2011.03.19
 */
TbVertexFormat::TbVertexFormat( const TbVertexFormat::Type type )
    : m_TexcoordCount( 0 )
    , m_Depend( type )
{
    if( 0 <= type && type < TbVertexFormat::TYPE_MAX ){
        m_TexcoordCount = TEXCOORD_COUNT[ type ];
    }
}

/*!
 * デストラクタ
 */
TbVertexFormat::~TbVertexFormat()
{
}

}

#endif