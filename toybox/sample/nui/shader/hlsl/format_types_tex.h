struct VsOut
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float2 texcoord : TEXCOORD0;
};

// �s�N�Z���V�F�[�_�o��
struct PsOut{
    float4 color : COLOR0;
};

sampler g_Texture : register(s0);
