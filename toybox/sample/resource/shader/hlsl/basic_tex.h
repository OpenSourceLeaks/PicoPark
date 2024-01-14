struct VsOut
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

sampler g_Texture : register(s0);
