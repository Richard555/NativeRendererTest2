package com.example.richardchen.nativerenderertest2;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.annotation.TargetApi;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.via.utility.NativeRender;

public class MainActivity extends AppCompatActivity {

    SurfaceView surfaceView = null;
    Surface surface = null;
    Handler handler = null;
    HandlerThread handlerThread = null;

    LinearLayout linearLayout = null;

    enum WINDOW_FORMAT {
        WINDOW_FORMAT_RGBA_8888(1),
        WINDOW_FORMAT_RGBX_8888(2),
        WINDOW_FORMAT_RGB_565(4),
        WINDOW_FORMAT_NV21(0x11),
        WINDOW_FORMAT_YV12(0x32315659),
        WINDOW_FORMAT_YV21(0x13);

        int value = 0;
        WINDOW_FORMAT(int v) {
            value = v;
        }

        public int getValue() {
            return value;
        }
    }

    Object lock = new Object();
    boolean bTestMode = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surfaceView = (SurfaceView)findViewById(R.id.testView);

        linearLayout = (LinearLayout) findViewById(R.id.resultLayout);

        handlerThread = new HandlerThread("DrawSurfaceView");
        handlerThread.start();
        handler = new Handler(handlerThread.getLooper());

        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                surface = holder.getSurface();

                if(bTestMode) {
                    startTestMode();
                } else {
                    colorFormat = WINDOW_FORMAT.WINDOW_FORMAT_NV21.getValue();
                    handler.post(drawSurfaceTask);
                }

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    void startTestMode() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                boolean first = true;

                for (WINDOW_FORMAT format : WINDOW_FORMAT.values()) {

                    if (NativeRender.drawSomething(surface, 1280, 720, 0, format.getValue()) >= 0) {


                        NativeRender.drawSomething(surface, 1280, 720, 0, format.getValue());
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }

                        NativeRender.drawSomething(surface, 1280, 720, 255, format.getValue());
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }


                        NativeRender.drawSomething(surface, 1280, 720, 0, format.getValue());
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }

                        NativeRender.drawSomething(surface, 1280, 720, 255, format.getValue());
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }


                        showDialogToGetFeedback(format);

                        synchronized (lock) {
                            try {
                                lock.wait();
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }

                    } else {
                        addTextViewToLayout(linearLayout, format.toString() + ":FAIL");
                    }
                }
            }
        }).start();
    }

    void addTextViewToLayout(final LinearLayout linearLayout, final String mesg) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TextView textView = new TextView(MainActivity.this);
                linearLayout.addView(textView);
                textView.setText(mesg);
            }
        });
    }

    public void showDialogToGetFeedback(WINDOW_FORMAT format) {
        final WINDOW_FORMAT window_format = format;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder b = new AlertDialog.Builder(MainActivity.this);
                b.setTitle("測試項目 " + window_format.toString());
                switch (window_format) {
                    case WINDOW_FORMAT_RGBX_8888:
                    case WINDOW_FORMAT_RGB_565:
                    case WINDOW_FORMAT_RGBA_8888:
                        b.setMessage("請問畫面有閃 黑~白~黑~白 嗎?");
                        break;
                    case WINDOW_FORMAT_NV21:
                    case WINDOW_FORMAT_YV12:
                    case WINDOW_FORMAT_YV21:
                        b.setMessage("請問畫面有閃 綠~紫~綠~紫 嗎?");
                        break;
                }
                b.setNegativeButton("有", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        addTextViewToLayout(linearLayout, window_format.toString() + ":SUCCESS");
                        synchronized (lock) {
                            lock.notifyAll();
                        }
                    }
                });
                b.setPositiveButton("沒有", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        addTextViewToLayout(linearLayout, window_format.toString() + ":FAIL");
                        synchronized (lock) {
                            lock.notifyAll();
                        }
                    }
                });
                b.show();
            }
        });
    }

    @TargetApi(18)
    @Override
    protected void onPause() {
        super.onPause();
        handler.removeCallbacks(drawSurfaceTask);
        try {
            handlerThread.quitSafely();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    int drawValue = 0;


    int colorFormat = 0;

    Runnable drawSurfaceTask = new Runnable() {
        @Override
        public void run() {

            NativeRender.drawSomething(surface, 1280, 720, drawValue++, colorFormat);
            if(drawValue==255) {
                drawValue = 0;
            }

            handler.postDelayed(this, 30);
        }
    };

}
