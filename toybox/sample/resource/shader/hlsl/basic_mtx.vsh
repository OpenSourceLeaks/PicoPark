VsOut main( const float3 pos : POSITION , 
            const float4 color : COLOR0 )
{
    VsOut out;
    out.pos = mul( g_MatWVP , pos );
    out.color = color;
    return out;
}
