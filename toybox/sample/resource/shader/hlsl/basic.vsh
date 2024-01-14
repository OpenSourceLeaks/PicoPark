#include "basic.h"

VsOut main( const float3 pos : POSITION , 
             const float4 color : COLOR0 )
{
    VsOut output;
    output.pos.xyz = pos;
    output.pos.w = 1.0;
    output.color = color;
    return output;
}
