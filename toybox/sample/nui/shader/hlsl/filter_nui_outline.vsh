#include "format_types_multi_tex.h"

uniform float4x4 u_OffsetMatrix : register(c6);

VsOut MainVS( const float3 pos : POSITION , 
            const float4 color : COLOR0 ,
            const float2 uv0 : TEXCOORD0 ,
            const float2 uv1 : TEXCOORD1  )
{
    VsOut output;
    output.pos.xyz = pos;
    output.pos.w = 1.0;
//    output.pos = mul( output.pos , u_OffsetMatrix );
//    output.pos = /mul( output.pos , u_OffsetMatrix );
    output.color = color;
    output.uv0 = uv0;
    output.uv1 = uv1;
    return output;
}
