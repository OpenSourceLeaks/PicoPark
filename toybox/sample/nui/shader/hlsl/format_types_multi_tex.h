// ���_�V�F�[�_�o��
struct VsOut
{
    float4 pos : POSITION;
    float4 color : COLOR0;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

// �s�N�Z���V�F�[�_�o��
struct PsOut{
    float4 color : COLOR0;
};

// �e�N�X�`��0
sampler g_Texture0 : register(s0);

// �e�N�X�`��
sampler g_Texture1 : register(s1);
