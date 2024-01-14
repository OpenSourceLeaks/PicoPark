#include "pmx.h"

uniform float4x4 g_WvpMtx : register(c0);
uniform float4x3 g_BoneMtx[16] : register(c16);

VsOut main( const float3 pos : POSITION , 
            const float3 normal : NORMAL , 
            const float2 texcoord : TEXCOORD0 ,
            const float4 boneIndices : BLENDINDICES ,
            const float4 weight : BLENDWEIGHT )
{
    VsOut output;
    float4 pos4 = float4(pos,1.0);
//    pos4 = float4(mul(pos4,g_BoneMtx[boneIndices.x])*1.0,1.0);
    pos4 = float4(mul(pos4,g_BoneMtx[boneIndices.x])*weight.x+
                  mul(pos4,g_BoneMtx[boneIndices.y])*weight.y+
                  mul(pos4,g_BoneMtx[boneIndices.z])*weight.z+
                  mul(pos4,g_BoneMtx[boneIndices.w])*weight.w,1.0);
    output.pos = mul(pos4,g_WvpMtx);
    output.texcoord = texcoord;
    return output;
}
