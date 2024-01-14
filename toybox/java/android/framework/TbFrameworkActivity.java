/*!
 * toybox用デフォルトActivity
 * @author Miyake Shunsuke
 * @since 2012.07.06
 */

package toybox.framework;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.WindowManager;

public class TbFrameworkActivity extends Activity
{
	private final String TAG = "TbFrameworkActivity";

    // 生成時コールバック
    @Override
    protected void onCreate(Bundle bundle)
    {
		Log.d(TAG,"onCreate");

        super.onCreate(bundle);
        m_View = new TbFrameworkView(getApplication());
        setContentView(m_View);

        // ディスプレイ情報をNDKに渡す
        WindowManager windowManager = getWindowManager();
        Display display = windowManager.getDefaultDisplay();
        DisplayMetrics displayMetrics = new DisplayMetrics();
        display.getMetrics(displayMetrics);
        int width = displayMetrics.widthPixels;
        int height = displayMetrics.heightPixels;

        float rate = display.getRefreshRate();
        int refreshRate = (int)rate;
        int pixelFormat = display.getPixelFormat();
        m_Dpi = displayMetrics.densityDpi;
        /*
        Log.d(TAG,"density="+displayMetrics.density+" densityDpi="+displayMetrics.densityDpi);
        Log.d(TAG,"scaledDensity="+displayMetrics.scaledDensity);
        Log.d(TAG,"xDpi="+displayMetrics.xdpi);
        Log.d(TAG,"yDpi="+displayMetrics.ydpi);
        */
        setDisplayInfoNdk( width , height , pixelFormat , refreshRate );

        // アセット
        AssetManager mgr = getResources().getAssets();
        setAssetNdk(mgr);
    }

    // ポーズ
    @Override
    protected void onPause()  {
        super.onPause();
        m_View.onPause();
        onPauseNdk();
		Log.d(TAG,"onPause");
    }

    // 復帰
    @Override protected void onResume() {
        super.onResume();
        m_View.onResume();
		Log.d(TAG,"onResume");
    }

    @Override
	protected void onDestroy() {
		super.onDestroy();
		Log.d(TAG,"onDestroy");
	}


    @Override
	protected void onRestart() {
		super.onRestart();
		Log.d(TAG,"onRestart");
	}

    @Override
	protected void onStart() {
		super.onStart();
		Log.d(TAG,"onStart");
	}

	@Override
	protected void onStop() {
		super.onStop();
		Log.d(TAG,"onStop");
	}

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int eventAction = event.getActionMasked();
        int pointerIndex = event.getActionIndex();
        int count = event.getPointerCount();
        int pointerId = event.getPointerId(pointerIndex);

        switch (eventAction) {
        case MotionEvent.ACTION_DOWN:
        	touchPressNdk(pointerId);
        	//Log.v(TAG, "Touch Down" + " count=" + count + ", id=" + pointerId);
            break;

        case MotionEvent.ACTION_POINTER_DOWN:
        	touchPressNdk(pointerId);
        	//    	Log.v(TAG, "Touch PTR Down" + " count=" + count + ", id=" + pointerId);
            break;

        case MotionEvent.ACTION_POINTER_UP:
        	touchReleaseNdk(pointerId);
        	//    	Log.v(TAG, "Touch PTR Up" + " count=" + count + ", id=" + pointerId);
            break;

        case MotionEvent.ACTION_CANCEL:
        case MotionEvent.ACTION_UP:
        	touchReleaseNdk(pointerId);
        	//        	        	Log.v(TAG, "Touch Up" + " count=" + count + ", id=" + pointerId);
            break;

        case MotionEvent.ACTION_MOVE:
            break;
        }
        float radius = m_Dpi/10.0f;
        for(int i=0; i<count; i++) {
        	touchOnNdk(event.getPointerId(i),event.getX(i),event.getY(i),radius,radius);
//        	Log.v("Multi", " X=" + event.getX(i) + ", Y=" + event.getY(i) + "Size=" + radius +  ", id=" + event.getPointerId(i) );
        }
        return true;
    }

    // ディスプレイ情報をNDKに通知
    public native void setDisplayInfoNdk( int width , int height , int pixelFormat , int refleshRate );

    // ポーズ
    public native void onPauseNdk();

    // アセットをNDKに通知
    public native void setAssetNdk( AssetManager mgr );

    // タッチ開始通知
    public native void touchPressNdk( int id  );

    // タッチ通知
    public native void touchOnNdk( int id , float x , float y , float radiusX , float radiusY );

    // タッチ離した通知
    public native void touchReleaseNdk( int id );

    private TbFrameworkView m_View;
    private float m_Dpi;
}
