/*!
 * ��ʃL���v�`��
 * @author Miyake Shunsuke
 * @since 2011.10.24
 */

#ifndef _INCLUDED_TB_SCREEN_SHOT_H_
#define _INCLUDED_TB_SCREEN_SHOT_H_

namespace toybox
{
    class TbTexture;
    class TbRenderDevice;

    class TbScreenShot
    {
    public:

        enum FileType
        {
            FILE_BMP , 
            FILE_TGA , 
            FILE_MAX , 
        };

    public:

        // �L���v�`���摜���o��
        static TbResult CaptureAndExportSync( const char* filepath , 
                                              FileType format );

    private:

        TbScreenShot();
        ~TbScreenShot();

    };

}



#endif
