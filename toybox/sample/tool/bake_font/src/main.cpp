// main.cpp : アプリケーションのエントリ ポイントを定義します。

#include "fwd.h"
#include <graphics/render/3dapi/tb_3dapi.h>
#include <graphics/render/tb_draw_2d.h>
#include <graphics/window/tb_window_manager.h>
#include <graphics/render/tb_render_pass_manager.h>
#include <base/io/tb_file.h>
#include <base/io/tb_file_manager.h>
#include <base/string/tb_string_code_page.h>
#include <base/container/tb_str_assoc_array.h>
#include <input/tb_keyboard.h>
#include <input/tb_mouse.h>
#include <input/tb_pad.h>
#include <framework/tb_framework.h>
#include <framework/sub/tb_framework_simple.h>
#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>
#include <base/time/tb_tick.h>
#include <graphics/image/tb_image_bmp.h>
#include <graphics/image/tb_image_tga.h>
#include <graphics/font/tb_font_info_ttf.h>
#include <base/string/encoding/tb_string_encoding_sjis.h>

using namespace toybox;

namespace
{

TbRenderDevice* s_Device = NULL;                         // 描画デバイス
TbFont*         s_Font = NULL;                           // フォント
TbTexture*      s_BakedTexture = NULL;
TbTextureSampler* s_TextureSampler = NULL;
TbBlendState*     s_BlendState = NULL;
TbRasterizerState*  s_RasterizerState = NULL;

void* alloc( TbUint32 size ){
    return new TbUint8[size];
}

}

/*!
 * フルスクリーン
 * @author Miyake Shunsuke
 * @since 2011.05.11
 */
void draw()
{
    static TbFloat64 begin = 0.0;
    static TbFloat64 end = 0.0;
    static TbFloat64 interval = 0.0;
    begin = TbTick::GetMilliSeconds();

    TbDraw2dSetRenderPass(TbFrameworkSimple::RENDER_PASS_2D);
    TbDraw2dTextureBegin();
    TbDrawRectTextureParam param( s_BakedTexture );
    param.SetRect( 10.0f , 10.0f , 512.0f , 512.0f );
    TbDrawRectTexture( param );
    TbDraw2dTextureEnd();

    end = TbTick::GetMilliSeconds();
    interval = end - begin;
}

namespace toybox
{

/*!
 * エントリーポイント
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{
    // 初期化
    TbFrameworkSimple::Initialize();
    {
        s_BlendState = new TbBlendState();
        TbBlendState::Param param;
        param.isEnableBlend = TB_TRUE;
        param.blendSrcFactor = TB_BLEND_FACTOR_SRC_ALPHA;
        param.blendDstFactor = TB_BLEND_FACTOR_INV_SRC_ALPHA;
        s_BlendState->Initialize(param);
    }
    {
        TbRasterizerState::Param param;
        param.cullMode = TB_CULL_MODE_NONE;
        s_RasterizerState = new TbRasterizerState();
        s_RasterizerState->Initialize(param);
    }

    // 設定
    TbRenderDevice& device = TbRenderDevice::GetInstance();
    device.SetBlendState(s_BlendState);
    TbRenderPass* pass = TbGetRenderPass(TbFrameworkSimple::RENDER_PASS_2D);
    TbFrameworkSimple::SetClearColor(TbColor::Red());
    device.SetRasterizerState(s_RasterizerState);
 
    // 焼きこみ用テクスチャ
    {
        TbTextureDesc texDesc;
        texDesc.colorFormat = TB_COLOR_FORMAT_A8R8G8B8;
        texDesc.flags = 0;
        texDesc.width = 1024;
        texDesc.height = 1024;
        texDesc.levels = 1;
        s_BakedTexture = new TbTexture( texDesc );
    }

    // フォント情報
    {
        TbStringEncodingSJIS encoding;
#if 0
        TbFontInfoTTF ttfInfo( 64 , "FAMania" );
        TbChar8 string[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+/=:><←→?., ";

#elif 0
        TbFontInfoTTF ttfInfo( 64 , "ＭＳ ゴシック" , TbFontInfoTTF::FLAG_BIT_EDGE );
        TbChar8 string[] = "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺc 　\
あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをんがぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽぁぃぅぇぉっゃゅょ\
アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲンガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォッャュョ\
０１２３４５６７８９０！？ー┌─━┐└│┃┘＝┤：＞∨→、「」『』";
#elif 1
        TbFontInfoTTF ttfInfo( 64 , "ＭＳ ゴシック" , TbFontInfoTTF::FLAG_BIT_EDGE );
        TbChar8 string[] = "式も終わって、さぁ帰ろう！あれ？なかじは？ただいまーお互いお疲れ様だねこちらこそありがとうこれからも一緒に幸せになってくれますかこれからもよろしくね？\
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789=.-!?*+[]><: _";
#elif 0
        TbFontInfoTTF ttfInfo( 64 , "DRAGON QUEST" );
        TbChar8 string[] = "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺc 　\
あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをんがぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽぁぃぅぇぉっゃゅょ\
アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲンガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォッャュョ\
０１２３４５６７８９０！？ー┌─━┐└│┃┘＝┤：＞∨→";
#elif 0
        TbFontInfoTTF ttfInfo( 64 , "ＭＳ ゴシック" );
        TbChar8 string[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789=.-!?*+[]><: _\
アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲンガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォッャュョ";
#else
        TbFontInfoTTF ttfInfo( 64 , "美咲ゴシック" );
        TbChar8 string[] = "ここは・・どこ・・？出るぞ！出れる！開かないコンセントだリモコンがあるベットだ動きそうチャンネル\
                            ねむいねむくないでれるあかないテレビリモコンねれそうおもそうチュートリアル出口Zた0123456789";
#endif
        TbString str = "";
        {
            TbStr16AssocArray<TbSint32> map;
            map.SetCapacity(1000);
            TbChar8* p = string;
            TbSint32 strCount = TbStrCharCount(string);
            TbSint32 byteCount = 0;
            for( TbSint32 i = 0; i < strCount; i += byteCount , p += byteCount ){
                byteCount = encoding.GetCharByteCount(0,p);
                TbChar8 shortChar[3];
                TbStrCopy(shortChar,TB_ARRAY_COUNT_OF(shortChar),p,byteCount);
                map.Insert(shortChar,0);
            }
            TbForIterator( it , map ) {
                str.Append( it->key.GetString().GetBuffer() );
            }
        }

        // テクスチャ焼きこみ
        ttfInfo.WriteTexture( s_BakedTexture , str.GetBuffer() );

        // 情報をヘッダーファイル出力
        TbStaticString512 writeString;
        TbFile texInfoFile( "font_info_custom.h" , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
        texInfoFile.Open();
        
        TbUint32 len = encoding.GetCharCount( str.GetBuffer() );
        TbUint32 step = 0;
        TbFontInfoTTF::FontInfo fontInfo = ttfInfo.GetInfo();
        TbFontInfoTTF::OutlineInfo outlineInfo;
        TbMemClear( &outlineInfo , sizeof(outlineInfo) );
        outlineInfo.charSize = 1;
        TbChar8 fontChar[3] = { 0 , 0 , 0 };
        TbUint32 fontCount = 0;
        TbFloat32 defaultWidth = 64.0f/static_cast<TbFloat32>(s_BakedTexture->GetWidth(0));
        TbFloat32 defaultHeight = 64.0f/static_cast<TbFloat32>(s_BakedTexture->GetHeight(0));
        for( TbSint32 i = 0; fontCount < len; i += outlineInfo.charSize , ++fontCount ){
            if( TB_SUCCEEDED( ttfInfo.GetCharOutline( outlineInfo , &str.GetBuffer()[i] ) ) ){
                TbMemCopy( fontChar , &str.GetBuffer()[i] , outlineInfo.charSize );

                fontChar[ outlineInfo.charSize ] = 0;

                // 情報をヘッダーに書き込み
                writeString = TbStaticString512::Format( "{ \"%s\" , %f , %f , %f , %f } , \n" ,
                                                           fontChar , 
                                                           defaultWidth * static_cast<TbFloat32>((fontCount) % (s_BakedTexture->GetWidth(0)/64)) ,
                                                           defaultHeight * static_cast<TbFloat32>((fontCount) / (s_BakedTexture->GetWidth(0)/64)) ,
                                                           static_cast<TbFloat32>(outlineInfo.cellIncX)/static_cast<TbFloat32>(s_BakedTexture->GetWidth(0)) , // outlineInfo.glyphOrigin.x + outlineInfo.blackBoxX , 
                                                           defaultHeight );
                texInfoFile.Write( writeString.GetBuffer() , writeString.GetCharCount() );

            }else{
                break;
            }
        }
    }

    // 焼きこんだテクスチャをファイル出力
    {
        TbFile texFile( "font.tga" , TB_FILE_OPERATE_WRITE , TB_FILE_SYNC , TB_FILE_PATH_ID_NONE );
        texFile.Open();
        void* texBuffer = NULL;
        TbUint32 pitch;
        TbUint32 height;
        if( TB_S_OK == s_BakedTexture->Lock( 0 , TB_LOCK_FLAG_BIT_READ_ONLY , &texBuffer , pitch , height ) ){
            TbImageExportInfo exportInfo;
            exportInfo.buffer = texBuffer;
            exportInfo.bufSize = pitch * height;
            exportInfo.width = s_BakedTexture->GetWidth(0);
            exportInfo.height = s_BakedTexture->GetHeight(0);
            exportInfo.colorFormat = s_BakedTexture->GetFormat(0);
            exportInfo.flag = 0;
            TbImageExportResult exportResult;
            TbMemClear( &exportResult , sizeof(exportResult) );
            if( TbImageTga::Export( exportResult , exportInfo ) ){
                texFile.Write( exportResult.fileBuf , exportResult.fileBufSize ); 
            }
            TbImageAllocatorDefault::Free( exportResult.fileBuf );
        }
        s_BakedTexture->Unlock( 0 );
    }
    
    // メインループ
    TbFrameworkSimple::StartMainLoop( NULL ,NULL , draw , NULL );
}

/*!
 * 終了
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFramework::OnExitPoint()
{
    TbFrameworkSimple::Finalize();
}

/*!
 * 画面の縦、横サイズが変わった
 * @author Miyake Shunsuke
 * @since 2012.07.14
 */
void TbFramework::OnResizeDisplay( TbSint32 w , TbSint32 h )
{
    // 特に何もしない
}

}
