#include "format_types_tex.h"

VsOut MainVS( const float3 pos : POSITION , 
            const float4 color : COLOR0 ,
            const float2 texcoord : TEXCOORD0 )
{
    VsOut output;
    output.pos.xyz = pos;
    output.pos.w = 1.0;
    output.color = color;
    output.texcoord = texcoord;
    return output;
}
