/*!
 * @file
 * @brief バッファリングした2Dフォントクラス
 * @author Miyake Shunsuke
 * @since 2010.4.8
 */
 
#ifndef _INCLUDED_TB_BUFFERED_FONT_2D_H_
#define _INCLUDED_TB_BUFFERED_FONT_2D_H_

#include <graphics/font/tb_font_types.h>
#include <graphics/render/3dapi/tb_texture_sampler.h>
#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_geometry.h>
#include <base/math/tb_vector2.h>
#include <base/math/tb_matrix.h>
#include <base/task/tb_task.h>

namespace toybox
{

class TbFont;

class TbFontBuffer2d
{
public:

    enum AlignX
    {
        ALIGN_X_LEFT   , // 左寄せ
        ALIGN_X_RIGHT  , // 右寄せ
        ALIGN_X_CENTER , // 中央寄せ
        ALIGN_X_MAX    ,
    };

    enum AlignY
    {
        ALIGN_Y_UP   , // 上寄せ
        ALIGN_Y_DOWN  , // 下寄せ
        ALIGN_Y_CENTER , // 中央寄せ
        ALIGN_Y_MAX    ,
    };

    // 設定
    struct Setting
    {
        TbBool operator==( const Setting& setting ){
                return size == setting.size && 
                   color == setting.color && 
                   borderColor == setting.borderColor &&
                   z == setting.z && 
                   bitArray == setting.bitArray &&
                   alignX == setting.alignX &&
                   alignY == setting.alignY &&
                   isEnableMask == setting.isEnableMask &&
                   maskRegion == setting.maskRegion &&
                    isEnableBorder == setting.isEnableBorder &&
                    isEnableLineThrough == setting.isEnableLineThrough &&
                    pose == setting.pose &&
                    strSpace == setting.strSpace &&
                    lineSpace == setting.lineSpace;
        }
        TbBool operator!=( const Setting& setting )
        {
            return  size != setting.size || 
                    color != setting.color || 
                    borderColor != setting.borderColor || 
                    z != setting.z || 
                    bitArray != setting.bitArray ||
                    alignX != setting.alignX ||
                    alignY != setting.alignY ||
                    isEnableMask != setting.isEnableMask ||
                    maskRegion != setting.maskRegion ||
                    isEnableBorder != setting.isEnableBorder ||
                    isEnableLineThrough != setting.isEnableLineThrough || 
                    pose != setting.pose ||
                    strSpace != setting.strSpace ||
                    lineSpace != setting.lineSpace;
        }
        TbUint32    size;               // サイズ
        TbColor     color;              // 色
        TbColor     borderColor;
        TbFloat32   z;                  // 奥行き
        TbBitArray8 bitArray;           // フラグ
        AlignX      alignX;             // アライン
        AlignY      alignY;             // アライン
        TbMatrix    pose;               // 姿勢行列
        TbUint8     isEnableMask;       // マスク可能
        TbUint8     isEnableBorder;     // 縁どり
        TbUint8     isEnableLineThrough;// 取り消し線
        TbUint8     _dummy;
        TbRectF32   maskRegion;         // マスク範囲
        TbFloat32   strSpace;           // 文字間補正座標
        TbFloat32   lineSpace;          // 行間補正座標
    };

public:

    // コンストラクタ
    TbFontBuffer2d( TbSizeT bufferSize , TbUint32 passIndex , void* userBuffer = nullptr );

    // デストラクタ
    ~TbFontBuffer2d();

public: // 描画

    // プリント
    TbResult Printf( TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... );

    // プリント
    TbResult Printf( TbRectF32& region , TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... );

    // 領域だけ取得
    TbResult GetRegion( TbRectF32& region , TbSint32 x , TbSint32 y , const TbChar8 *fmt , ... );

    // 実際にGPUにコマンド発行
    TbResult Flush();

public: // 設定

    //! レンダーパス取得
    TbUint32 GetRenderPass() const {
        return m_RenderPassIndex;
    }

    //! フォント設定
    void SetFont( TbFont* font ) {
        m_Font = font;
    }

    // フォントサイズに対する行間比率設定
    void SetLineHeightScale( TbFloat32 scale ) {
        m_LineHeightScale = scale;
    }

    // フォントサイズに対する行間比率取得
    TbFloat32 GetLineHeightScale() const {
        return m_LineHeightScale;
    }

public:

    // 設定情報をセット
    void SetSetting( const Setting& setting ){
        if( m_Setting != setting ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting = setting; 
        }
    };

    // 設定情報取得
    const Setting& GetSetting() const {
        return m_Setting;
    };

    // フォントサイズ設定
    void SetFontSize( TbUint32 size ) 
    {
        if( m_Setting.size != size ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting.size = size; 
        }
    }

    // フォントサイズ取得
    TbUint32 GetFontSize() const { 
        return m_Setting.size; 
    }

    // フォントカラー設定
    void SetColor( const TbColor& color ) 
    {
        if( m_Setting.color != color ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting.color = color;
        }
    }

    // フォントカラー取得
    const TbColor& GetColor() const { 
        return m_Setting.color; 
    }

    //奥行き設定
    void SetZ( TbFloat32 z ) 
    {
        if( !TB_FLOAT32_EQUAL( m_Setting.z , z ) ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting.z = z;
        }
    }

    // 奥行き取得
    TbFloat32 GetZ() const { 
        return m_Setting.z;
    }

    // フォントカラー設定
    void SetAlignX( const AlignX align ) 
    {
        if( m_Setting.alignX != align ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting.alignX = align;
        }
    }

    // フォントカラー取得
    AlignX GetAlignX() const { 
        return m_Setting.alignX; 
    }

    // フォントカラー設定
    void SetAlignY( const AlignY align ) 
    {
        if( m_Setting.alignY != align ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting.alignY = align;
        }
    }

    // フォントカラー取得
    AlignY GetAlignY() const {
        return m_Setting.alignY;
    }

    // マスクリージョン
    void SetMaskRegion( const TbRectF32& region )
    {
        if( m_Setting.maskRegion != region ){
            m_BitArray.SetBit( FLAG_UPDATE_SETTING , TB_TRUE );
            m_Setting.isEnableMask = TB_TRUE;
            m_Setting.maskRegion = region;
        }
    }
    
    // 縁どり
    void SetEnableBorder( TbBool isEnable )
    {
        if (m_Setting.isEnableBorder != isEnable) {
            m_BitArray.SetBit(FLAG_UPDATE_SETTING, TB_TRUE);
            m_Setting.isEnableBorder = static_cast<TbUint8>(isEnable);
        }
    }

    // 縁カラー設定
    void SetBorderColor(const TbColor& color) {
        if (m_Setting.borderColor != color) {
            m_BitArray.SetBit(FLAG_UPDATE_SETTING, TB_TRUE);
            m_Setting.borderColor = color;
        }
    }

    // 取り消し線
    void SetEnableLineThrough(TbBool isEnable)
    {
        if (m_Setting.isEnableLineThrough != isEnable) {
            m_BitArray.SetBit(FLAG_UPDATE_SETTING, TB_TRUE);
            m_Setting.isEnableLineThrough = static_cast<TbUint8>(isEnable);
        }
    }

    // 文字間補正値
    void SetStrSpace(TbFloat32 space) {
        if (m_Setting.strSpace != space) {
            m_BitArray.SetBit(FLAG_UPDATE_SETTING, TB_TRUE);
            m_Setting.strSpace = space;
        }
    }

    // 行間補正値
    void SetLineSpace(TbFloat32 space) {
        if (m_Setting.lineSpace != space) {
            m_BitArray.SetBit(FLAG_UPDATE_SETTING, TB_TRUE);
            m_Setting.lineSpace = space;
        }
    }

    void SetPoseMatrix(const TbMatrix& pose)
    {
        if (m_Setting.pose != pose) {
            m_BitArray.SetBit(FLAG_UPDATE_SETTING, TB_TRUE);
            m_Setting.pose = pose;
        }
    }

    // 設定をプッシュ
    TbResult PushSetting();

    // 設定をポップ
    TbResult PopSetting();

private:

    enum {
        FLAG_UPDATE_SETTING ,
        FLAG_USE_USER_BUFFER , 
    };

    // 設定スタックヘッダー
    struct SettingStackHeader
    {
        Setting setting;
        SettingStackHeader* prev;
    };

    // コマンドヘッダー(テキスト)
    struct CommandHeaderText
    {
        TbSint32 x;                //!< X座標
        TbSint32 y;                //!< Y座標
        TbUint32 textCount;        //!< 文字数
    };

    // コマンドヘッダー(設定)
    struct CommandHeaderSetting
    {
        Setting setting;
    };

    // コマンドヘッダー
    struct CommandHeader
    {
        enum Type
        {
            TYPE_SETTING , 
            TYPE_TEXT , 
            TYPE_FORCE_S32 = 0xFFFFFFFF 
        };
        Type type;             //!< タイプ
        union {
            CommandHeaderSetting* setting;
            CommandHeaderText* text;
        };
        CommandHeader* next; //!< 次
    };

private:

    // プリント
    CommandHeader* printf( TbSint32 x , TbSint32 y , const char* text );

    // バッファ確保
    void* alloc( TbUint32 size );

    // コマンド追加
    void addCommand( CommandHeader* command );

    // コマンド転送
    void flush( CommandHeaderText* command , const Setting& setting );

    // 取り消し線描画
    void drawLineThrough( const TbRectF32& region , const Setting& setting );

    // 描画領域取得
    const char* getRegion( TbRectF32& region , 
                           const char* str , 
                           const Setting& setting ,
                           TbBool suspendNewLine = TB_FALSE );

private:

    TbBitArray32                m_BitArray;                 //!< フラグ
    TbUint32                    m_RenderPassIndex;          //!< レンダリングパス番号
    TbTextureSampler            m_TextureSampler;           //!< テクスチャサンプラ
    TbFont*                     m_Font;                     //!< フォント
    TbDimensionS32              m_ScreenSize;               //!< 画面サイズ
    TbFloat32                   m_LineHeightScale;          //!< 行間スケール値
    TbUint8*                    m_Buffer;                   //!< バッファ
    TbUint32                    m_BufferSize;               //!< 
    TbUint32                    m_BufferUseSize;            //!< バッファ使用サイズ
    Setting                     m_Setting;                  //!< 設定
    SettingStackHeader*         m_SettingStackTail;         //!< 設定スタック
    CommandHeader*              m_CommandTop;               //!< コマンドルート
    CommandHeader*              m_CommandTail;              //!< コマンド末端

};

}

#endif
