// 頂点シェーダ出力
struct VsOut
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

// ピクセルシェーダ出力
struct PsOut{
    float4 color : COLOR0;
};

// テクスチャ0
sampler g_Texture0 : register(s0);

// テクスチャ
sampler g_Texture1 : register(s1);
