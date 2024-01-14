uniform sampler2D u_Texture0;//テクスチャユニット0

//フラグメント
void main(void)
{
//      gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);;
      gl_FragColor = texture2D( u_Texture0 , gl_TexCoord[0].st );
}
