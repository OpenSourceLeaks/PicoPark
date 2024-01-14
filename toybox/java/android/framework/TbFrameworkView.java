/*!
 * toybox用デフォルトView
 * @author Miyake Shunsuke
 * @since 2012.07.06
 */

package toybox.framework;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class TbFrameworkView extends GLSurfaceView implements GLSurfaceView.Renderer
{
    // コンストラクタ
    public TbFrameworkView(Context context) {
        super(context);
        this.setEGLContextClientVersion(2);
        setRenderer(this);
    }

    // ポーズ
    @Override
    public void onPause() {
        super.onPause();
    }

    // 復帰
    @Override
    public void onResume() {
        super.onResume();
    }

    // サーフェイス作成
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        onSurfaceCreatedNdk();
    }

    // サーフェイスサイズ変更
    public void onSurfaceChanged(GL10 gl, int w, int h) {
    	onSurfaceChangedNdk(w, h);
    }

    // フレーム処理
    public void onDrawFrame(GL10 gl) {
        onDrawFrameNdk();
    }

    // NDK サーフェイス作成
    public native void onSurfaceCreatedNdk();

    // NDK サーフェイスサイズ変更
    public native void onSurfaceChangedNdk(int w,int h);

    // NDK フレーム処理
    public native void onDrawFrameNdk();
}
