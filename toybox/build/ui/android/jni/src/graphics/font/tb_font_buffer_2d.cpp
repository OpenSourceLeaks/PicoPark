/*!
 * @file
 * @brief
 * @author Miyake Shunsuke
 * @since 2010.10.21
 */

#include "tb_fwd.h"

#include "graphics/font/tb_font_buffer_2d.h"
#include "graphics/font/tb_font.h"

#include "base/crypt/tb_crc32.h"
#include "graphics/render/tb_draw_2d.h"
#include "graphics/render/3dapi/tb_render_device.h"
#include "graphics/render/tb_render_pass_manager.h"
#include "base/math/tb_geometry.h"
#include "base/string/tb_string_code_page.h"

namespace toybox
{

namespace
{
    const TbSint32 BUF_SIZE = 512;
}

/*!
 * コンストラクタ
 * @author Miyake Shunsuke
 * @since 2011.03.26
 */
TbFontBuffer2d::TbFontBuffer2d( TbSizeT bufferSize , TbUint32 passIndex , void* userBuffer )
    : m_BitArray()
    , m_RenderPassIndex(passIndex)
    , m_Font( nullptr )
    , m_LineHeightScale(1.0f)
    , m_Setting()
    , m_Buffer( reinterpret_cast<TbUint8*>(userBuffer) )
    , m_BufferSize(bufferSize)
    , m_BufferUseSize(0)
    , m_SettingStackTail(nullptr)
    , m_CommandTop( nullptr )
    , m_CommandTail( nullptr )
{
    m_Setting.z = 0.15f;
    m_Setting.size = 16;
    m_Setting.color = TbColor::White();
    m_Setting.borderColor = TbColor::White();
    m_Setting.alignX = ALIGN_X_LEFT;
    m_Setting.alignY = ALIGN_Y_UP;
    m_Setting.isEnableMask = TB_FALSE;
    m_Setting.isEnableBorder = TB_FALSE;
    m_Setting.isEnableLineThrough = TB_FALSE;
    m_Setting.strSpace = 0.0f;
    m_Setting.lineSpace = 6.0f;

    TbTextureSamplerParam samplerParam;
    samplerParam.minFilterMode = TB_TEXTURE_FILTER_MODE_POINT;
    samplerParam.magFilterMode = TB_TEXTURE_FILTER_MODE_POINT;
    samplerParam.addressModeU = TB_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerParam.addressModeV = TB_TEXTURE_ADDRESS_MODE_CLAMP;
    m_TextureSampler.Initialize(samplerParam);

    if( !m_Buffer ){
        m_Buffer = TB_NEW TbUint8[ m_BufferSize ];
        m_BitArray.SetBit(FLAG_USE_USER_BUFFER);
    }
}

/*!
 * デストラクタ
 * @author Miyake Shunsuke
 * @since 2011.03.26
 */
TbFontBuffer2d::~TbFontBuffer2d()
{
    m_TextureSampler.Finalize();
    if( m_BitArray.Test(FLAG_USE_USER_BUFFER) ){
        TB_SAFE_DELETE_ARRAY( m_Buffer );
    }
}

/*!
 * 設定をプッシュ
 * @author Miyake Shunsuke
 * @since 2011.09.20
 */
TbResult TbFontBuffer2d::PushSetting()
{
    SettingStackHeader* header = reinterpret_cast<SettingStackHeader*>(alloc( sizeof(SettingStackHeader) ));
    if( !header ){
        return TB_E_FAIL;
    }
    header->prev = m_SettingStackTail;
    header->setting = m_Setting;
    m_SettingStackTail = header;
    return TB_S_OK;
}

/*!
 * 設定をポップ
 * @author Miyake Shunsuke
 * @since 2011.09.20
 */
TbResult TbFontBuffer2d::PopSetting()
{
    if( m_SettingStackTail ){
        SetSetting( m_SettingStackTail->setting );
        m_SettingStackTail = m_SettingStackTail->prev;
        return TB_S_OK;
    }
    return TB_E_FAIL;
}

/*!
 * プリント
 * @author Miyake Shunsuke
 * @since 2011.08.17
 */
TbResult TbFontBuffer2d::Printf( TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... )
{
    va_list ap;
    TbChar8 text[BUF_SIZE];

    va_start( ap, fmt );
    TbVStrPrintf( text , BUF_SIZE , fmt, ap );
    va_end( ap );
    CommandHeader* header = printf( x , y , text );
    return header ? TB_S_OK : TB_E_FAIL;
}

/*!
 * プリント
 * @author Miyake Shunsuke
 * @since 2011.08.17
 */
TbResult TbFontBuffer2d::Printf( TbRectF32& region , TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... )
{
    if( !TB_VERIFY(m_Font) ){
        return TB_E_FAIL;
    }

    va_list ap;
    TbChar8 text[BUF_SIZE];

    va_start( ap, fmt );
    TbVStrPrintf( text , BUF_SIZE , fmt, ap );
    va_end( ap );

    region.x = static_cast<TbFloat32>(x);
    region.y = static_cast<TbFloat32>(y);
    getRegion(region,text,m_Setting,TB_FALSE);
    CommandHeader* header = TbFontBuffer2d::printf( x , y , text );
    if( !header ) {;
        return TB_E_FAIL;
    }

    return TB_S_OK;
}

/*!
 * 領域だけ取得
 * @author Miyake_Shunsuke
 * @since 2014.12.07
 */
TbResult TbFontBuffer2d::GetRegion( TbRectF32& region , TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... )
{
    if( !TB_VERIFY(m_Font) ){
        return TB_E_FAIL;
    }

    va_list ap;
    TbChar8 text[BUF_SIZE];

    va_start( ap, fmt );
    TbVStrPrintf( text , BUF_SIZE , fmt, ap );
    va_end( ap );

    region.x = static_cast<TbFloat32>(x);
    region.y = static_cast<TbFloat32>(y);
    getRegion(region,text,m_Setting,TB_FALSE);
    return TB_S_OK;
}

/*!
 * プリント
 * @author Miyake Shunsuke
 * @since 2012.10.15
 */
TbFontBuffer2d::CommandHeader* TbFontBuffer2d::printf( TbSint32 x , TbSint32 y , const char* text )
{
    // 設定反映
    if( m_BitArray.Test( FLAG_UPDATE_SETTING ) ){
        CommandHeader* headerSetting = reinterpret_cast<CommandHeader*>(alloc( sizeof(CommandHeader) ));
        headerSetting->type = CommandHeader::TYPE_SETTING;
        headerSetting->setting = reinterpret_cast<CommandHeaderSetting*>(alloc( sizeof(CommandHeaderSetting) ));
        if( !headerSetting || !headerSetting->setting ){
            return nullptr;
        }
        headerSetting->setting->setting = m_Setting;
        addCommand( headerSetting );
        m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_FALSE );
    }

    CommandHeader* header = reinterpret_cast<CommandHeader*>(alloc( sizeof(CommandHeader) ));
    header->type = CommandHeader::TYPE_TEXT;
    header->text = reinterpret_cast<CommandHeaderText*>(alloc( sizeof(CommandHeaderText) ));
    if( !header || !header->text ){
        return nullptr;
    }
    header->text->x = x;
    header->text->y = y;
    header->next = nullptr;

    header->text->textCount = TbStrCharCount( text );
    TbSint32 textBufferSize = sizeof(text[0]) * ( header->text->textCount + 1 );
    void* textBuffer = alloc( textBufferSize );
    TbMemCopy( textBuffer , text , textBufferSize );

    addCommand( header );

    return header;
}

/*!
 * 実際にコマンド発行
 * @author Miyake Shunsuke
 * @since 2011.03.26
 */
TbResult TbFontBuffer2d::Flush()
{    
    TbRenderPass* renderPass = TbRenderPassManager::GetInstance().GetPass(m_RenderPassIndex);
    if( !renderPass ){
        return TB_E_FAIL;
    }

    // 設定
    renderPass->SetTextureSampler(0,&m_TextureSampler);
    const TbBlendState* blendState = TbRenderStatePreset::GetInstance().GetBlendState(TbRenderStatePreset::BLEND_NORMAL);
    renderPass->SetBlendState(blendState);
    TbDraw2dSetRenderPass(m_RenderPassIndex);
    TbDraw2dTextureBegin();

    // コマンドを発行
    CommandHeader* command = m_CommandTop;
    Setting* setting = &m_Setting;
    while( command ){
        switch( command->type ){
        case CommandHeader::TYPE_TEXT:
            flush( command->text , *setting );
            break;
        case CommandHeader::TYPE_SETTING:
            setting = &(command->setting->setting);
            break;
        default:
            TB_ASSERT(!"invalid");
            break;
        }
        command = command->next;
    }
    TbDraw2dTextureEnd();

    m_CommandTop = nullptr; 
    m_CommandTail = nullptr;
    m_BufferUseSize = 0; // バッファを一度リセット
    TB_ASSERT_MSG( !m_SettingStackTail , "SettingStack should be poped" );
    m_SettingStackTail = nullptr;
    return TB_S_OK;
}

/*!
 * ASCII描画
 * @author Miyake Shunsuke
 * @since 2011.08.16
 */
void TbFontBuffer2d::flush( CommandHeaderText* command , const Setting& setting )
{
    TbTexture* texture = m_Font->GetTexture();
    const TbChar8* string = reinterpret_cast<const TbChar8*>( command + 1 );
    TbFloat32 x = static_cast<TbFloat32>( command->x );
    TbFloat32 y = static_cast<TbFloat32>( command->y );

    TbRectF32 region;
    region.x = static_cast<TbFloat32>(x);
    region.y = static_cast<TbFloat32>(y);
    // アライン補正
    getRegion(region, string , setting, TB_TRUE);
    x = static_cast<TbSint32>(region.x);
    y = static_cast<TbSint32>(region.y);

    while( string && *string )
    {
        TbRectF32 uvRect;
        TbDimensionF32 size;
        TbBool isNewLine = TB_FALSE;
        string = m_Font->GetNewLine(isNewLine,string);
        if (isNewLine)
        {
            // 取り消し線
            if (setting.isEnableLineThrough)
            {
                drawLineThrough(region, setting);
            }
            // アライン補正
            x = static_cast<TbFloat32>(command->x);
            y += region.height;
            y += setting.lineSpace;
            region.x = static_cast<TbFloat32>(x);
            region.y = static_cast<TbFloat32>(y);
            getRegion(region, string, setting , TB_TRUE);
            x = static_cast<TbSint32>(region.x);
            y = static_cast<TbSint32>(region.y);
            continue;
        }
        string = m_Font->GetUV( uvRect , size , setting.size , string );

        TbDrawRectTextureParam param( texture );
        param.pos.SetXY( x , y );
        param.size = size;
        param.color = setting.color;
        param.rectUV = uvRect;
        param.matrix = setting.pose;
        x += size.width + setting.strSpace;

        if( setting.isEnableMask )
        {
            TB_ASSERT(!setting.isEnableBorder); // 現状マスクと縁取り両方対応してない

            TbFloat32 maxX = setting.maskRegion.x + setting.maskRegion.width;
            TbFloat32 maxY = setting.maskRegion.y + setting.maskRegion.height;

            // 範囲外チェック
            if( maxX <= param.pos.GetX() ) {
                continue;
            }
            if( param.pos.GetX() + param.size.width <= setting.maskRegion.x ) {
                continue;
            }
            if( maxY <= param.pos.GetY() ) {
                continue;
            }
            if( param.pos.GetY() + param.size.height <= setting.maskRegion.y ) {
                continue;
            }

            // マスキング
            TbRectF32 newRect(param.pos.GetX(),
                              param.pos.GetY(),
                              param.size.width,
                              param.size.height);
            TbRectF32 newUvRect = param.rectUV;

            if( param.pos.GetX() < setting.maskRegion.x )
            {
                newRect.x       = setting.maskRegion.x;
                newRect.width   = (param.pos.GetX()+param.size.width) - newRect.x;
                newUvRect.x     = param.rectUV.x+(param.rectUV.width*((setting.maskRegion.x-param.pos.GetX())/param.size.width));
                newUvRect.width = (param.rectUV.x+param.rectUV.width) - newUvRect.x;
            }

            if( maxX < (param.pos.GetX()+param.size.width) )
            {
                newRect.width     = maxX-newRect.x;
                TbFloat32 uvX     = param.rectUV.x + (param.rectUV.width*((maxX-param.pos.GetX())/param.size.width));
                newUvRect.width   = uvX - newUvRect.x;
            }

            if( param.pos.GetY() < setting.maskRegion.y )
            {
                newRect.y       = setting.maskRegion.y;
                newRect.height  = (param.pos.GetY()+param.size.height) - newRect.y;
                newUvRect.y     = param.rectUV.y+(param.rectUV.height*((setting.maskRegion.y-param.pos.GetY())/param.size.height));
                newUvRect.height= (param.rectUV.y+param.rectUV.height) - newUvRect.y;
            }

            if( maxY < (param.pos.GetY()+param.size.height) )
            {
                newRect.height    = maxY-newRect.y;
                TbFloat32 uvY     = param.rectUV.y + (param.rectUV.height*((maxY-param.pos.GetY())/param.size.height));
                newUvRect.height  = uvY - newUvRect.y;
            }

            param.pos.SetXY( newRect.x , newRect.y );
            param.size.width  = newRect.width;
            param.size.height = newRect.height;
            param.rectUV = newUvRect;
        }

        if (setting.isEnableBorder)
        {
            TbFloat32 BORDER_SCALE = 0.05f;
            // 縁どりは裏に8回描く(軽くない)
            TbFloat32 offset = static_cast<TbFloat32>(setting.size) * BORDER_SCALE;
            TbVector3 pos = param.pos;
            param.color = setting.borderColor;
            param.pos.SetXY( pos.GetX() + offset, pos.GetY()+ offset );
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX() + offset, pos.GetY() - offset);
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX() - offset, pos.GetY() + offset);
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX() - offset, pos.GetY() - offset);
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX() + offset, pos.GetY());
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX() - offset, pos.GetY());
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX()         , pos.GetY() + offset);
            TbDrawRectTexture(param);
            param.pos.SetXY(pos.GetX()         , pos.GetY() - offset);
            TbDrawRectTexture(param);

            param.color = setting.color;
            param.pos = pos;
        }

        TbDrawRectTexture(param);
    }
    // 取り消し線
    if (setting.isEnableLineThrough)
    {
        drawLineThrough(region, setting);
    }
}

/*!
 * 取り消し線描画
 * @author Miyake Shunsuke
 */
void TbFontBuffer2d::drawLineThrough(const TbRectF32& region, const Setting& setting)
{
    static const TbFloat32 HEIGHT_SCALE = 0.2f;
    TbRectF32 drawRegion = region;
    drawRegion.height = region.height * HEIGHT_SCALE;
    drawRegion.y = region.y + region.height/2.0f - (drawRegion.height/2.0f);

    TbDrawRectParam param;
    param.pos.SetXY(drawRegion.x, drawRegion.y);
    param.size.width = drawRegion.width;
    param.size.height = drawRegion.height;
    param.color = setting.color;
    param.matrix = setting.pose;
    TbDrawRect(param);
}

/*!
 * バッファ確保
 * @param size 確保したいバイト数
 * @author Miyake Shunsuke
 * @since 2011.03.26
 */
void* TbFontBuffer2d::alloc( TbUint32 size )
{
    TbUint32 allocSize = TB_ALIGN_ROUND_UP( size , 4 ); // 4バイトアライメントする
    TbUint32 bufferSize = m_BufferSize - m_BufferUseSize;
    // 足りない場合は
    if( bufferSize < allocSize ){
        TB_ASSERT_MSG( 0 , "CommandBuffer is Lack" );
        return nullptr;
    }
    void* result = reinterpret_cast<void*>(m_Buffer + m_BufferUseSize);
    m_BufferUseSize += allocSize;
    return result;
}

/*!
 * コマンド追加
 * @param command コマンド
 * @author Miyake Shunsuke
 * @since 2011.09.20
 */
void TbFontBuffer2d::addCommand( CommandHeader* command )
{
    // コマンドに追加
    if( !m_CommandTop ){
        m_CommandTop = command;
        m_CommandTail = command;
    }else{
        m_CommandTail->next = command;
        m_CommandTail = command;
    }
}

/*!
 * 描画慮息取得
 * @author Miyake_Shunsuke
 * @since 2013.10.21
 */
const char* TbFontBuffer2d::getRegion( TbRectF32& region , 
                                       const char* str , 
                                       const Setting& setting ,
                                       TbBool suspendNewLine )
{
    // 描画領域計算
    region.width = 0.0f;
    region.height = 0.0f;
    TbFloat32 width = 0.0f;
    TbFloat32 height = 0.0f;
    TbDimensionF32 size;
    while( str && *str != 0 ) {
        TbBool isNewLine = TB_FALSE;
        str = m_Font->GetNewLine(isNewLine,str);
        if (isNewLine) {
            if (suspendNewLine) {
                break;
            }
            height += region.height;
            height += setting.lineSpace;
            width = TbMax(width,region.width);
            region.width = 0.0f;
            region.height = 0.0f;
        }
        str = m_Font->GetSize(size,setting.size,str);
        if( !TB_VERIFY(str) ){
            break;
        }
        region.width += size.width;
        if( size.height > region.height ) {
            region.height = size.height;
        }
    }
    region.width = TbMax(width, region.width);
    region.height += height;
    switch( setting.alignX )
    {
    case ALIGN_X_LEFT:
        break;
    case ALIGN_X_CENTER:
        region.x -= region.width/2.0f;
        break;
    case ALIGN_X_RIGHT:
        region.x -= region.width;
        break;
    default:
        TB_ASSERT(!"invalid");
        break;
    }
    switch( setting.alignY )
    {
    case ALIGN_Y_UP:
        break;
    case ALIGN_Y_CENTER:
        region.y -= region.height/2.0f;
        break;
    case ALIGN_Y_DOWN:
        region.y -= region.height;
        break;
    default:
        TB_ASSERT(!"invalid");
        break;
    }
    return str;
}

}
