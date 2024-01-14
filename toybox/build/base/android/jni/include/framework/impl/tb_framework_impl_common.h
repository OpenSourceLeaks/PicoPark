/*!
 * フレームワーク実装部分の共通
 * @author Miyake_Shunsuke
 * @since 2012.05.25
 */

#ifndef _INCLUDED_TB_FRAMEWORK_IMPL_COMMON_H_
#define _INCLUDED_TB_FRAMEWORK_IMPL_COMMON_H_

#include <base/task/tb_task_manager.h>
#include <base/io/tb_file_manager.h>

namespace toybox
{

class TbFrameworkImplCommon
{
public:
    typedef void (*MainLoopFunc)();
public:

    // コンストラクタ
    TbFrameworkImplCommon( TbUint32 taskLineMax )
        : m_TaskManager(taskLineMax)
        , m_FileManager()
        , m_MainLoopBeginFunc(NULL)
        , m_MainLoopFunc()
        , m_MainLoopEndFunc()
    {
    }

    // デストラクタ
    ~TbFrameworkImplCommon()
    {
    }

public:

    // メインループスタート
    void StartMainLoop( MainLoopFunc begin , MainLoopFunc main , MainLoopFunc end )
    {
        m_BitArray.SetBit( FLAG_ENABLE_MAINLOOP );
        m_MainLoopBeginFunc = begin;
        m_MainLoopFunc = main;
        m_MainLoopEndFunc = end;
    }

    // メインループ終了
    void FinishMainLoop()
    {
        m_BitArray.SetBit( FLAG_ENABLE_MAINLOOP , TB_FALSE );
    }

    // メインループアクティブか
    TbBool IsEnableMainLoop() const {
        return m_BitArray.Test( FLAG_ENABLE_MAINLOOP );
    }

    // メインループ中
    TbBool IsBusyMainLoop() const {
        return m_BitArray.Test(FLAG_BEGIN_MAIN_LOOP);
    }

protected:

    // タスク管理取得
    TbTaskManager& GetTask() { 
        return m_TaskManager;
    }

    // 先頭関数
    void CallMainLoopBegin()
    {
        if( m_MainLoopBeginFunc ) {
            m_MainLoopBeginFunc();
        }
        m_BitArray.SetBit(FLAG_BEGIN_MAIN_LOOP);
    }

    // メイン
    void CallMainLoop()
    {
        if( IsBusyMainLoop() ){
            if( m_MainLoopFunc ) {
                m_MainLoopFunc();
            }
        }
    }

    // 末端関数
    void CallMainLoopEnd()
    {
        if( IsBusyMainLoop() ){
            if( m_MainLoopEndFunc ) {
                m_MainLoopEndFunc();
            }
            m_BitArray.ResetBit(FLAG_BEGIN_MAIN_LOOP);
        }
    }

private:
        
    enum Flag
    {
        FLAG_ENABLE_MAINLOOP , 
        FLAG_BEGIN_MAIN_LOOP ,
        FLAG_MAX , 
    };

private:
    TbBitArray32        m_BitArray;
    TbTaskManager       m_TaskManager;          // タスク管理
    TbFileManager       m_FileManager;          // ファイル管理
    MainLoopFunc        m_MainLoopBeginFunc;    // メインループ開始関数
    MainLoopFunc        m_MainLoopFunc;         // メインループ関数
    MainLoopFunc        m_MainLoopEndFunc;      // メインループ終了関数
};

}

#endif
