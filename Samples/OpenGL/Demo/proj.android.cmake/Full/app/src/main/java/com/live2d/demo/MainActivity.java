/**
 * Copyright(c) Live2D Inc. All rights reserved.
 * <p>
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

package com.live2d.demo;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.MediaPlayer;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends Activity {

    private GLSurfaceView _glSurfaceView;
    private GLRenderer _glRenderer;
    public int modelChooseRecyclerViewPosition = 0;
    String changeToBackground = "";
    String filterStr = "";
    static float humanVolume = 0.6f;
    static float backgroundVolume = 0.4f;
    static JSONObject textList = null;
    static MediaPlayer mediaPlayer1;
    StrokeTextView strokeTextView;
    TextView hintTextView;
    Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(@NonNull Message message) {
            switch (message.what){
                case 1:
                    Bundle b = message.getData();
                    String text = b.getString("text");
                    strokeTextView.setText(text);
                    hintTextView.setText(b.getString("hintText"));
                    break;
                case 2:
                    Toast.makeText(MainActivity.this,"获取失败",Toast.LENGTH_SHORT).show();
                    break;
                case 3:
                    Toast.makeText(MainActivity.this,"获取成功",Toast.LENGTH_SHORT).show();
                    break;
                default:
            }
            return true;
        }
    });
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == RESULT_OK) {
            switch(requestCode){
                case 1:
                    modelChooseRecyclerViewPosition = data.getIntExtra("position",0);
                    filterStr = data.getStringExtra("filteredText");
                    String return_data = data.getStringExtra("data");
                    JniBridgeJava.nativeSetCharacter(return_data);//"100100"
                    strokeTextView.setText("");
                    hintTextView.setText("");
                    break;
                case 2:
                    //Log.d("SGY","changeTo"+data.getStringExtra("data"));
                    //JniBridgeJava.nativeChangeBackground(data.getStringExtra("data"));
                    changeToBackground = data.getStringExtra("data");
                    break;
                case 3:
                    String ret_data = data.getStringExtra("data");
                    if(ret_data.equals("cancel")){
                        if((mediaPlayer1 != null) && (mediaPlayer1.isPlaying())){
                            mediaPlayer1.release();
                            mediaPlayer1 = null;
                        }
                    }else if(ret_data.equals("getSubtitle")){
                        try{
                            new Thread(runnable).start();
                        }catch (Exception e){
                            Toast.makeText(this,"获取失败",Toast.LENGTH_SHORT).show();
                            e.printStackTrace();
                        }
                    }else {
                        setNewBGM(ret_data);
                    }
                default:
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("SGY","activityStart");
        super.onCreate(savedInstanceState);
        JniBridgeJava.SetActivityInstance(this);
//        JniBridgeJava.SetContext(this);
        _glSurfaceView = new GLSurfaceView(this);
        _glSurfaceView.setEGLContextClientVersion(2);
        _glRenderer = new GLRenderer();
        _glSurfaceView.setRenderer(_glRenderer);
        _glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        setContentView(R.layout.activity_main);
        LinearLayout layout = findViewById(R.id.linearLayout);
        layout.addView(_glSurfaceView);
        getVoiceTextList();
        strokeTextView = findViewById(R.id.strokeTextView);
        hintTextView = findViewById(R.id.hintText);
        ImageButton b = findViewById(R.id.button);

        b.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //Log.d("SGY","changeModel");
                //JniBridgeJava.nativeSetCharacter("100500");
                Intent intent1 = new Intent(MainActivity.this, ModelChooseActivity.class);
                intent1.putExtra("position",modelChooseRecyclerViewPosition);
                intent1.putExtra("editText",filterStr);
                startActivityForResult(intent1,1);
            }
        });

        ImageButton b2 = findViewById(R.id.button2);
        b2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //JniBridgeJava.nativeChangeBackground("back_class_normal2.png");
                Intent intent1 = new Intent(MainActivity.this, BackgroundChooseActivity.class);
                startActivityForResult(intent1,2);
            }
        });

        ImageButton b3 = findViewById(R.id.button3);
        b3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //JniBridgeJava.nativeChangeBackground("back_class_normal2.png");
                Intent intent1 = new Intent(MainActivity.this, MusicChooseActivity.class);
                startActivityForResult(intent1,3);
            }
        });

        //getWindow().getDecorView().setSystemUiVisibility(
        //        View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        //                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
        //                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
        //                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
        //                | View.SYSTEM_UI_FLAG_FULLSCREEN
        //                | (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT
        //                ? View.SYSTEM_UI_FLAG_LOW_PROFILE
        //                : View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY)
        //);
        JniBridgeJava.nativeOnStart();
        Log.d("SGY","onCreate");
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.d("SGY","onStart");
        //JniBridgeJava.nativeOnStart();

    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if(!changeToBackground.equals("")){
            JniBridgeJava.nativeChangeBackground(changeToBackground);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d("SGY","onResume");
        //_glSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.d("SGY","onPause");
        _glSurfaceView.onPause();
        JniBridgeJava.nativeOnPause();
        JniBridgeJava.nativeOnStop();
        JniBridgeJava.nativeOnStart();
        _glSurfaceView.onResume();

        //JniBridgeJava.nativeOnPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.d("SGY","onStop");
        //JniBridgeJava.nativeOnStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if((mediaPlayer1 != null) && (mediaPlayer1.isPlaying())){
            mediaPlayer1.release();
            mediaPlayer1 = null;
        }
        Log.d("SGY","onDestroy");
        //JniBridgeJava.nativeOnDestroy();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float pointX = event.getX();
        float pointY = event.getY();
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                JniBridgeJava.nativeOnTouchesBegan(pointX, pointY);
                break;
            case MotionEvent.ACTION_UP:
                JniBridgeJava.nativeOnTouchesEnded(pointX, pointY);
                break;
            case MotionEvent.ACTION_MOVE:
                JniBridgeJava.nativeOnTouchesMoved(pointX, pointY);
                break;
        }
        return super.onTouchEvent(event);
    }

    public void setNewBGM(final String filename){
        if((mediaPlayer1 != null) && (mediaPlayer1.isPlaying())){
            mediaPlayer1.release();
            mediaPlayer1 = null;
        }
        mediaPlayer1 = MediaPlayer.create(MainActivity.this,getResource(filename));//用create方法会自动调用prepare不要再自己调用了
        mediaPlayer1.setVolume(backgroundVolume,backgroundVolume);
        mediaPlayer1.setLooping(true);
        mediaPlayer1.start();
        mediaPlayer1.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                setNewBGM(filename);
            }
        });
    }

    public int getResource(String idName){
        return getResources().getIdentifier(idName, "raw", getPackageName());
    }

    public void getVoiceTextList(){
        if(textList == null){
            //首先检查sharedPref里面有没有，如果没有再从文件中读取
            SharedPreferences pref = getSharedPreferences("VoiceData", MODE_PRIVATE);
            String voiceText = pref.getString("text","");
            if(voiceText.equals("")){
                InputStream stream = getResources().openRawResource(R.raw.char_text);
                BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
                StringBuffer sb = new StringBuffer();
                String line = "";
                try{
                    while((line = reader.readLine())!=null){
                        sb.append(line);
                    }
                }catch(IOException e){
                    e.printStackTrace();
                }
                try{
                    textList = new JSONObject(sb.toString());
                    SharedPreferences.Editor editor = getSharedPreferences("VoiceData",MODE_PRIVATE).edit();
                    editor.putString("text",sb.toString());
                    editor.apply();
                    //Character c = new Character(jsonObject.getString("name"),jsonObject.getString("id"));
                }catch(Exception e){
                    e.printStackTrace();
                }
            }else{
                try{
                    textList = new JSONObject(voiceText);
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
        }
    }

    private String makeHttpRequest(URL url) throws IOException {
        String jsonResponse = "";
        HttpURLConnection urlConnection = null;
        InputStream inputStream = null;

        try {
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestMethod("GET");
            urlConnection.setReadTimeout(10000 /* milliseconds */);
            urlConnection.setConnectTimeout(15000 /* milliseconds */);
            urlConnection.connect();
            inputStream = urlConnection.getInputStream();
            jsonResponse = readFromStream(inputStream);
        } catch (Exception e) {
            // TODO: Handle the exception
            Message m = new Message();
            m.what = 2;
            handler.sendMessage(m);
            e.printStackTrace();
        } finally {
            if (urlConnection != null) {
                urlConnection.disconnect();
            }
            if (inputStream != null) {
                // function must handle java.io.IOException here
                inputStream.close();
            }
        }
        return jsonResponse;
    }

    private String readFromStream(InputStream inputStream) throws Exception {
        StringBuilder sb = new StringBuilder();
        Reader reader = new InputStreamReader(inputStream, "UTF-8");
        BufferedReader bufferedReader = new BufferedReader(reader);
        String line;
        while((line = bufferedReader.readLine()) != null){
            sb.append(line);
        }
        bufferedReader.close();
        return sb.toString();
    }

    Runnable runnable = new Runnable() {
        @Override
        public void run() {
            String callback = "notGet";
            Message m = new Message();
            try{
                callback = makeHttpRequest(new URL("https://l3-prod-all-gs-mfsn2.bilibiligame.net/magica/resource/json_web/lobby_vo_char_text.json"));
                if(!callback.equals("")){
                    SharedPreferences.Editor editor = getSharedPreferences("VoiceData",MODE_PRIVATE).edit();
                    editor.putString("text",callback);
                    editor.apply();
                    textList = new JSONObject(callback);
                    m.what=3;
                }else{
                    m.what=2;
                }
            }catch(Exception e){
                m.what=2;
                e.printStackTrace();
            }
            handler.sendMessage(m);
        }
    };


}
