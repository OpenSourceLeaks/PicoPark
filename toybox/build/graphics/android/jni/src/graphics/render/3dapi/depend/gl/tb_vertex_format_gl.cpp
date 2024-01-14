/*!
 * OpenGL依存の頂点フォーマット
 * @author Miyake Shunsuke
 * @since 2010.08.08
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_format.h"

#if TB_3DAPI_IS_GL

namespace toybox
{

// TbVertexElement::Typeに対応したGL用型とサイズ
struct TbVertexElementTypeGL{
    GLint size;
    GLenum type;
};

// TbVertexElement::Typeとの対応テーブル
static const TbVertexElementTypeGL ELEMENT_TYPE[] =
{
    { 1 , GL_FLOAT },          // TYPE_FLOAT1
    { 2 , GL_FLOAT },          // TYPE_FLOAT2
    { 3 , GL_FLOAT },          // TYPE_FLOAT3
    { 4 , GL_FLOAT },          // TYPE_FLOAT4
    { 4 , GL_FLOAT },          // TYPE_COLOR 
    { 4 , GL_UNSIGNED_BYTE },  // TYPE_UBYTE4
    { 2 , GL_SHORT },          // TYPE_SHORT2
    { 4 , GL_SHORT },          // TYPE_SHORT4

    { 4 , GL_UNSIGNED_BYTE },  // TYPE_UBYTE4N
    { 2 , GL_SHORT },          // TYPE_SHORT2N
    { 4 , GL_SHORT },          // TYPE_SHORT4N
    { 2 , GL_UNSIGNED_SHORT }, // TYPE_USHORT2N
    { 4 , GL_UNSIGNED_SHORT }, // TYPE_USHORT4N
    { 3 , GL_FLOAT },          // TYPE_UDEC3
    { 3 , GL_FLOAT },          // TYPE_DEC3N
    { 2 , TB_GL_FLOAT16 },     // TYPE_FLOAT16_2
    { 4 , TB_GL_FLOAT16 },     // TYPE_FLOAT16_4
    { 0 , 0 },                 // TYPE_UNSUSED
};

struct TbVertexElementGLStatic
{
    TbVertexElementGL* elements;
    TbUint32           elementNum;
};

// TYPE_XYZ
static TbVertexElementGL STATIC_ELEMENT_XYZ[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE }
};

// TYPE_XYZ_COLOR
static TbVertexElementGL STATIC_ELEMENT_XYZ_COLOR[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 4 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 , TB_FALSE }
};

// TYPE_XYZ_NORMAL
static TbVertexElementGL STATIC_ELEMENT_XYZ_NORMAL[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 , TB_FALSE }
};

// TYPE_XYZ_NORMAL_COLOR
static TbVertexElementGL STATIC_ELEMENT_XYZ_NORMAL_COLOR[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 , TB_FALSE } , 
    { 0 , 24 , 4 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 , TB_FALSE }
};

// TYPE_XYZ_TEX0SIZE2
static TbVertexElementGL STATIC_ELEMENT_XYZ_TEX0SIZE2[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 2 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 , TB_FALSE }
};

// TYPE_XYZ_COLOR_TEX0SIZE2
static TbVertexElementGL STATIC_ELEMENT_XYZ_COLOR_TEX0SIZE2[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 4 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 , TB_FALSE } , 
    { 0 , 28 , 2 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 , TB_FALSE }
};

// TYPE_XYZ_NORMAL_TEX0SIZE2
static TbVertexElementGL STATIC_ELEMENT_XYZ_NORMAL_TEX0SIZE2[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 , TB_FALSE } ,
    { 0 , 24 , 2 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 , TB_FALSE }
};

// TYPE_XYZ_NORMAL_COLOR_TEX0SIZE2
static TbVertexElementGL STATIC_ELEMENT_XYZ_NORMAL_COLOR_TEX0SIZE2[] = {
    { 0 , 0 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 , TB_FALSE } ,
    { 0 , 12 , 3 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 , TB_FALSE } , 
    { 0 , 24 , 4 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 , TB_FALSE } ,
    { 0 , 40 , 2 , GL_FLOAT , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 , TB_FALSE }
};

static TbVertexElementGLStatic STATIC_ELEMENTS[] = 
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
    , m_Elements( nullptr )
{
    m_Elements = TB_NEW TbVertexElementGL[ m_ElementCount ];
    for( TbSint32 i = 0; i < m_ElementCount; ++i ){
        m_Elements[ i ].stream = elements[ i ].stream;
        m_Elements[ i ].offset = elements[ i ].offset;
        m_Elements[ i ].size = ELEMENT_TYPE[ elements[ i ].type ].size;
        m_Elements[ i ].type = ELEMENT_TYPE[ elements[ i ].type ].type;
        m_Elements[ i ].method = elements[ i ].method;
        m_Elements[ i ].usage = elements[ i ].usage;
        m_Elements[ i ].usageIndex = elements[ i ].usageIndex;
        m_Elements[ i ].isNormalized = ( TbVertexElement::USAGE_COLOR == m_Elements[ i ].usage && TbVertexElement::TYPE_UBYTE4 == elements[ i ].type );
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
    m_ElementCount = STATIC_ELEMENTS[ type ].elementNum;
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
    const TbVertexElementGL* elements = m_Depend.GetElements();
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