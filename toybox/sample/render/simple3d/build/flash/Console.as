/*!
 * カスタムコンソール
 * @author Miyake Shunsuke
 * @since 2013.05.25
 */
package com.adobe.flascc
{
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.display.DisplayObjectContainer;
    import flash.display.Sprite;
    import flash.display.Stage;
    import flash.display.StageScaleMode;
    import flash.events.Event;
    import flash.net.LocalConnection;
    import flash.net.URLRequest;
    import flash.text.TextField;
    import flash.utils.ByteArray;

    import com.adobe.flascc.vfs.ISpecialFile;
    import com.adobe.flascc.vfs.RootFSBackingStore;

    // カスタムコンソール
    public class Console extends Sprite implements ISpecialFile
    {
        // コンストラクタ
        public function Console(container:DisplayObjectContainer = null)
        {
            m_IsEnableConsole = false;
            m_ConsoleContainer = null;
            m_ConsoleTextField = null;
            
            CModule.rootSprite = container ? container.root : this
            
            // 既にワーカーが動いていたら何もしない
            if(CModule.runningAsWorker()) {
                return;
            }

            if(container) {
                container.addChild(this)
                init(null)
            } else {
                addEventListener(Event.ADDED_TO_STAGE, init)
            }
        }

        // 初期化処理
        protected function init(e:Event):void
        {            
            stage.frameRate = 60
            stage.scaleMode = StageScaleMode.NO_SCALE
            CModule.vfs.console = this
            CModule.vfs.addDirectory("/resource");
            CModule.vfs.addDirectory("/resource/tex");
            CModule.vfs.addBackingStore(new RootFSBackingStore(), "/resource/tex")
            CModule.startAsync(this)
            
            // デバッグ用(コンソール出力を画面表示)
            if( m_IsEnableConsole )
            {
                m_ConsoleContainer = new Sprite();
                addChild(m_ConsoleContainer);
                m_ConsoleTextField = new TextField
                m_ConsoleTextField.multiline = true
                m_ConsoleTextField.width = stage.stageWidth
                m_ConsoleTextField.height = stage.stageHeight 
                m_ConsoleContainer.addChild(m_ConsoleTextField)
            }
        }

        // プロセス終了時処理(posixのexit呼ばれた時）
        public function exit(code:int):Boolean
        {
            // default to unhandled
            return exitHook ? exitHook(code) : false;
        }

        // Cのwrite関数呼んだ時の処理
        public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
        {
            var str:String = CModule.readString(bufPtr, nbyte);
            consoleWrite(str);
            return nbyte;
        }

        // Cのread関数呼んだ時の処理
        public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
        {
            return 0;
        }

        // Cのfcntlを呼ばれた時の処理
        public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
        {
            return 0;
        }

        // Cのioctlを呼ばれた時の処理
        public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
        {
            return 0;
        }

        // コンソールに出力
        protected function consoleWrite(s:String):void
        {
            trace(s);
            if( m_IsEnableConsole )
            {
                m_ConsoleTextField.appendText(s);
                m_ConsoleTextField.scrollV = m_ConsoleTextField.maxScrollV;
            }
        }

        //! ENTER_FRMAEイベント(毎フレーム呼ばれます)
        protected function enterFrame(e:Event):void
        {
            CModule.serviceUIRequests()
        }

        // exitのフック用関数
        public var exitHook:Function;
        private var m_IsEnableConsole:Boolean;
        private var m_ConsoleTextField:TextField;
        private var m_ConsoleContainer:DisplayObjectContainer;
    }
}
