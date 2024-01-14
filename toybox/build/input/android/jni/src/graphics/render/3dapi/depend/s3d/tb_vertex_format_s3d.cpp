/*!
 * OpenGL依存の頂点フォーマット
 * @author Miyake Shunsuke
 * @since 2010.08.08
 */

#include "tb_fwd.h"
#include "graphics/3dapi/tb_vertex_format.h"

#if TB_3DAPI_IS_S3D

using namespace AS3::ui::flash::display3D;

namespace toybox
{

// TbVertexElement::Typeに対応したGL用型とサイズ
struct TbVertexElementTypeS3d {
    AS3::ui::String type;
};

// TbVertexElement::Typeとの対応テーブル
static const AS3::ui::String ELEMENT_TYPE_TABLE[] =
{
    Context3DVertexBufferFormat::FLOAT_1 , // TYPE_FLOAT1
    Context3DVertexBufferFormat::FLOAT_2 , // TYPE_FLOAT2
    Context3DVertexBufferFormat::FLOAT_3 , // TYPE_FLOAT3
    Context3DVertexBufferFormat::FLOAT_4 , // TYPE_FLOAT4
    Context3DVertexBufferFormat::FLOAT_4 , // TYPE_COLOR 
    Context3DVertexBufferFormat::BYTES_4 , // TYPE_UBYTE4
    "" ,     // TYPE_SHORT2
    "" ,     // TYPE_SHORT4
    "" ,     // TYPE_UBYTE4N
    "" ,     // TYPE_SHORT2N
    "" ,     // TYPE_SHORT4N
    "" ,     // TYPE_USHORT2N
    "" ,     // TYPE_USHORT4N
    "" ,     // TYPE_UDEC3
    "" ,     // TYPE_DEC3N
    "" ,     // TYPE_FLOAT16_2
    "" ,     // TYPE_FLOAT16_4
    "" ,     // TYPE_UNSUSED
};

struct TbVertexElementS3dStatic
{
    TbVertexElementS3d* elements;
    TbUint32         elementCount;
};

// TYPE_XYZ
static TbVertexElementS3d STATIC_ELEMENT_XYZ[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 }
};

// TYPE_XYZ_COLOR
static TbVertexElementS3d STATIC_ELEMENT_XYZ_COLOR[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 4 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_COLOR] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 }
};

// TYPE_XYZ_NORMAL
static TbVertexElementS3d STATIC_ELEMENT_XYZ_NORMAL[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 }
};

// TYPE_XYZ_NORMAL_COLOR
static TbVertexElementS3d STATIC_ELEMENT_XYZ_NORMAL_COLOR[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } , 
    { 0 , 24 , 4 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_COLOR] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 }
};

// TYPE_XYZ_TEX0SIZE2
static TbVertexElementS3d STATIC_ELEMENT_XYZ_TEX0SIZE2[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 2 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT2] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

// TYPE_XYZ_COLOR_TEX0SIZE2
static TbVertexElementS3d STATIC_ELEMENT_XYZ_COLOR_TEX0SIZE2[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 4 ,  ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_COLOR] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } , 
    { 0 , 28 , 2 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT2] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

// TYPE_XYZ_NORMAL_TEX0SIZE2
static TbVertexElementS3d STATIC_ELEMENT_XYZ_NORMAL_TEX0SIZE2[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } ,
    { 0 , 24 , 2 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT2] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

// TYPE_XYZ_NORMAL_COLOR_TEX0SIZE2
static TbVertexElementS3d STATIC_ELEMENT_XYZ_NORMAL_COLOR_TEX0SIZE2[] = {
    { 0 , 0 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , 3 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT3] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } , 
    { 0 , 24 , 4 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_COLOR] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 40 , 2 , ELEMENT_TYPE_TABLE[TbVertexElement::TYPE_FLOAT2] , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

static TbVertexElementS3dStatic STATIC_ELEMENTS[] = 
{
    { STATIC_ELEMENT_XYZ                        , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ) } , 
    { STATIC_ELEMENT_XYZ_COLOR                  , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_COLOR) } , 
    { STATIC_ELEMENT_XYZ_NORMAL                 , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_NORMAL) } , 
    { STATIC_ELEMENT_XYZ_NORMAL_COLOR           , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_NORMAL_COLOR) } , 
    { STATIC_ELEMENT_XYZ_TEX0SIZE2              , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_TEX0SIZE2) } , 
    { STATIC_ELEMENT_XYZ_COLOR_TEX0SIZE2        , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_COLOR_TEX0SIZE2) } , 
    { STATIC_ELEMENT_XYZ_NORMAL_TEX0SIZE2       , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_NORMAL_TEX0SIZE2) } , 
    { STATIC_ELEMENT_XYZ_NORMAL_COLOR_TEX0SIZE2 , TB_ARRAY_COUNT_OF(STATIC_ELEMENT_XYZ_NORMAL_COLOR_TEX0SIZE2) } , 
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
TbVertexFormatDepend::TbVertexFormatDepend( const TbVertexElement* elements , TbSint32 elementCount )
    : m_BitArray()
    , m_ElementCount( elementCount )
    , m_Elements( NULL )
{
    m_Elements = TB_NEW TbVertexElementS3d[ m_ElementCount ];
    for( TbSint32 i = 0; i < m_ElementCount; ++i ){
        m_Elements[ i ].stream = elements[ i ].stream;
        m_Elements[ i ].offset = elements[ i ].offset;
        m_Elements[ i ].type = ELEMENT_TYPE_TABLE[ elements[ i ].type ];
        m_Elements[ i ].method = elements[ i ].method;
        m_Elements[ i ].usage = elements[ i ].usage;
        m_Elements[ i ].usageIndex = elements[ i ].usageIndex;
    }
}

/*!
 * 定数フォーマット構築用コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.18
 */
TbVertexFormatDepend::TbVertexFormatDepend( const TbSint32 type )
    : m_BitArray()
{
    TB_STATIC_ASSERT( TB_ARRAY_COUNT_OF(STATIC_ELEMENTS) == TbVertexFormat::TYPE_MAX );
    m_BitArray.SetBit( FLAG_ELEMENT_STATIC , TB_TRUE );
    m_Elements = STATIC_ELEMENTS[ type ].elements;
    m_ElementCount = STATIC_ELEMENTS[ type ].elementCount;
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.08
 */
TbVertexFormatDepend::~TbVertexFormatDepend()
{
    if( !m_BitArray.Test( FLAG_ELEMENT_STATIC ) ){
        TB_SAFE_DELETE_ARRAY( m_Elements );
    }
}

//

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.11
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
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbVertexFormat::TbVertexFormat( const TbVertexFormat::Type type )
    : m_TexcoordCount( 0 )
    , m_Depend( type )
{
    TbSint32 elementCount = m_Depend.GetElementCount();
    const TbVertexElementS3d* elements = m_Depend.GetElements();
    for( TbSint32 i = 0; i < elementCount; ++i ){
        if( TbVertexElement::USAGE_TEXCOORD == elements[ i ].usage ){
            ++m_TexcoordCount;
        }
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2010.08.11
 */
TbVertexFormat::~TbVertexFormat()
{
}

}

#endif