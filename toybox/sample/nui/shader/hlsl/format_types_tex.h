struct VsOut
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

// ピクセルシェーダ出力
struct PsOut{
    float4 color : COLOR0;
};

sampler g_Texture : register(s0);
