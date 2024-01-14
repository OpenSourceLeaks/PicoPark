attribute vec4 a_Position0;
attribute vec4 a_Color0;
attribute vec4 a_Texcoord0;

// 頂点シェーダ
void main()
{
    gl_Position = a_Position0;
    gl_FrontColor = a_Color0;
    gl_TexCoord[0] = a_Texcoord0;
}
