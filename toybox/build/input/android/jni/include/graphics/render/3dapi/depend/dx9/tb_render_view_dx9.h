/*!
 * @file
 * @brief レンダービューのクラス
 * @author Miyake Shunsuke
 * @since 2010.04.10
 */

#ifndef _INCLUDED_TB_RENDER_VIEW_STD_DX9_H_
#define _INCLUDED_TB_RENDER_VIEW_STD_DX9_H_

#include "./tb_lost_resource_dx9.h"

namespace toybox
{

class TbRenderDevice;
class TbVertexFormat;
class TbRenderView;

class TbRenderViewDepend : public TbLostResourceDx9
{
public:

    // コンストラクタ
    TbRenderViewDepend( const TbRenderViewParameter& param , TbSint32 index);

    // デストラクタ
    ~TbRenderViewDepend();

public: // 依存関数(アプリケーション層からは原則呼ばない)

    //!< スワップチェーン取得
    IDirect3DSwapChain9* GetSwapChain(){ return m_SwapChain; }

    //!< レンダーターゲット取得
    TbRenderTarget* GetRenderTarget( TbUint32 index ) {
        if( index < m_RenderTargetCount ){
            return m_RenderTargetList[ index ]; 
        }
        return nullptr;
    }

public: // 継承関数

    // 解放
    virtual void Release();

    // 再構築
    virtual void Recover();

private:

    enum {
        FLAG_CREATE_DEPTH_STENCIL , // 深度ステンシルを生成した
    };

private:

    // セットアップ
    void setup( TbRenderView* view );

private:
    
    TbRenderView*           m_Common;           //!< 所属View
    TbBitArray32            m_BitArray;         //!< ビット配列
    TbSint32                m_Index;            //!< 新規生成ではなくスワップチェーン取り出し方
    D3DPRESENT_PARAMETERS   m_Param;            //!< 生成パラメータ
    IDirect3DSwapChain9*    m_SwapChain;        //!< スワップチェーン
    TbRenderTarget**        m_RenderTargetList; //!< レンダーターゲットリスト
    TbUint32                m_RenderTargetCount;  //!< レンダーターゲット数

private:

    friend class TbRenderView;

};

}

#endif