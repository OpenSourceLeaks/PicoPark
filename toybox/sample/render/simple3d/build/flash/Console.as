/*!
 * �J�X�^���R���\�[��
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

    // �J�X�^���R���\�[��
    public class Console extends Sprite implements ISpecialFile
    {
        // �R���X�g���N�^
        public function Console(container:DisplayObjectContainer = null)
        {
            m_IsEnableConsole = false;
            m_ConsoleContainer = null;
            m_ConsoleTextField = null;
            
            CModule.rootSprite = container ? container.root : this
            
            // ���Ƀ��[�J�[�������Ă����牽�����Ȃ�
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

        // ����������
        protected function init(e:Event):void
        {            
            stage.frameRate = 60
            stage.scaleMode = StageScaleMode.NO_SCALE
            CModule.vfs.console = this
            CModule.vfs.addDirectory("/resource");
            CModule.vfs.addDirectory("/resource/tex");
            CModule.vfs.addBackingStore(new RootFSBackingStore(), "/resource/tex")
            CModule.startAsync(this)
            
            // �f�o�b�O�p(�R���\�[���o�͂���ʕ\��)
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

        // �v���Z�X�I��������(posix��exit�Ă΂ꂽ���j
        public function exit(code:int):Boolean
        {
            // default to unhandled
            return exitHook ? exitHook(code) : false;
        }

        // C��write�֐��Ă񂾎��̏���
        public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
        {
            var str:String = CModule.readString(bufPtr, nbyte);
            consoleWrite(str);
            return nbyte;
        }

        // C��read�֐��Ă񂾎��̏���
        public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
        {
            return 0;
        }

        // C��fcntl���Ă΂ꂽ���̏���
        public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
        {
            return 0;
        }

        // C��ioctl���Ă΂ꂽ���̏���
        public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
        {
            return 0;
        }

        // �R���\�[���ɏo��
        protected function consoleWrite(s:String):void
        {
            trace(s);
            if( m_IsEnableConsole )
            {
                m_ConsoleTextField.appendText(s);
                m_ConsoleTextField.scrollV = m_ConsoleTextField.maxScrollV;
            }
        }

        //! ENTER_FRMAE�C�x���g(���t���[���Ă΂�܂�)
        protected function enterFrame(e:Event):void
        {
            CModule.serviceUIRequests()
        }

        // exit�̃t�b�N�p�֐�
        public var exitHook:Function;
        private var m_IsEnableConsole:Boolean;
        private var m_ConsoleTextField:TextField;
        private var m_ConsoleContainer:DisplayObjectContainer;
    }
}
