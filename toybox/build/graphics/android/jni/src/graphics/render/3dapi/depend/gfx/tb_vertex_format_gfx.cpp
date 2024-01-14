/*!
 * OpenGL依存の頂点フォーマット
 * @author teci
 */

#include "tb_fwd.h"
#include "graphics/render/3dapi/tb_vertex_format.h"

#if TB_3DAPI_IS_GFX

namespace toybox
{

struct TbVertexElementStatic
{
    TbVertexElement* elements;
    TbUint32           elementCount;
};

/*
TbUint16 stream;    // ストリーム番号
TbUint16 offset;    // データの開始オフセット値
TbUint8  type;      // 型
TbUint8  method;     // 方法
TbUint8  usage;      // 利用用途
TbUint8  usageIndex; // 利用用途内番号
*/

// TYPE_XYZ
static TbVertexElement STATIC_ELEMENT_XYZ[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 }
};

// TYPE_XYZ_COLOR
static TbVertexElement STATIC_ELEMENT_XYZ_COLOR[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 }
};

// TYPE_XYZ_NORMAL
static TbVertexElement STATIC_ELEMENT_XYZ_NORMAL[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 }
};

// TYPE_XYZ_NORMAL_COLOR
static TbVertexElement STATIC_ELEMENT_XYZ_NORMAL_COLOR[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } ,
    { 0 , 24 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 }
};

// TYPE_XYZ_TEX0SIZE2
static TbVertexElement STATIC_ELEMENT_XYZ_TEX0SIZE2[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

// TYPE_XYZ_COLOR_TEX0SIZE2
static TbVertexElement STATIC_ELEMENT_XYZ_COLOR_TEX0SIZE2[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_COLOR , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 28 , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

// TYPE_XYZ_NORMAL_TEX0SIZE2
static TbVertexElement STATIC_ELEMENT_XYZ_NORMAL_TEX0SIZE2[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } ,
    { 0 , 24 , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

// TYPE_XYZ_NORMAL_COLOR_TEX0SIZE2
static TbVertexElement STATIC_ELEMENT_XYZ_NORMAL_COLOR_TEX0SIZE2[] = {
    { 0 , 0 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_POSITION , 0 } ,
    { 0 , 12 , TbVertexElement::TYPE_FLOAT3 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_NORMAL , 0 } ,
    { 0 , 24 , TbVertexElement::TYPE_FLOAT4 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_COLOR , 0 } ,
    { 0 , 40 , TbVertexElement::TYPE_FLOAT2 , TbVertexElement::METHOD_DEFAULT , TbVertexElement::USAGE_TEXCOORD , 0 }
};

static TbVertexElementStatic STATIC_ELEMENTS[] =
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
 */
TbVertexFormatDepend::TbVertexFormatDepend()
    : m_IsStatic(TB_FALSE)
    , m_Elements(nullptr)
    , m_ElementCount(0)
{
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbVertexFormatDepend::~TbVertexFormatDepend()
{
    if( !m_IsStatic ){
        TB_SAFE_DELETE_ARRAY(m_Elements);
    }
}

/*!
 * 初期化
 * @author Miyake Shunsuke
 */
void TbVertexFormatDepend::Initialize(const TbVertexElement* elements, TbSint32 elementCount,TbBool isStatic)
{
    TB_RESQUE(!m_Elements);

    m_ElementCount = elementCount;
    m_IsStatic = isStatic;
    if(m_IsStatic){
        m_Elements = elements;
    }
    else 
    {
        TbVertexElement* dst = new TbVertexElement[elementCount];
        for (TbSint32 i = 0; i < elementCount; ++i) {
            dst[i] = elements[i];
        }
        m_Elements = dst;
    }
}

// TbVertexFormat

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbVertexFormat::TbVertexFormat( const TbVertexElement* elements , const TbSint32 elementCount )
    : m_TexcoordCount( 0 )
    , m_Depend()
{
    m_Depend.Initialize(elements, elementCount,TB_FALSE);
    for( TbSint32 i = 0; i < elementCount; ++i ){
        if( TbVertexElement::USAGE_TEXCOORD == elements[ i ].usage ){
            ++m_TexcoordCount;
        }
    }
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 */
TbVertexFormat::TbVertexFormat( const TbVertexFormat::Type type )
    : m_TexcoordCount( 0 )
    , m_Depend()
{
    m_Depend.Initialize(STATIC_ELEMENTS[type].elements, STATIC_ELEMENTS[type].elementCount,TB_TRUE);
    TbSint32 elementCount = STATIC_ELEMENTS[type].elementCount;
    for( TbSint32 i = 0; i < elementCount; ++i ){
        if( TbVertexElement::USAGE_TEXCOORD == STATIC_ELEMENTS[type].elements[ i ].usage ){
            ++m_TexcoordCount;
        }
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 */
TbVertexFormat::~TbVertexFormat()
{
}

}

#endif