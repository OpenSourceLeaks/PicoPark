uniform sampler2D u_Texture0;//�e�N�X�`�����j�b�g0

//�t���O�����g
void main(void)
{
//      gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);;
      gl_FragColor = texture2D( u_Texture0 , gl_TexCoord[0].st );
}
