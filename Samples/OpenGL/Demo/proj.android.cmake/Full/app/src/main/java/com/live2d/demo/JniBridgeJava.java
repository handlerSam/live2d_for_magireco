/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

package com.live2d.demo;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioFormat;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;


public class JniBridgeJava {

    private static final String LIBRARY_NAME = "Demo";
    private static Activity _activityInstance;
//    private static Context _context;
    static MediaPlayer mediaPlayer1;

    static {
        System.loadLibrary(LIBRARY_NAME);
    }

    // Native -----------------------------------------------------------------

    public static native void nativeOnStart();

    public static native void nativeOnPause();

    public static native void nativeOnStop();

    public static native void nativeOnDestroy();

    public static native void nativeOnSurfaceCreated();

    public static native void nativeOnSurfaceChanged(int width, int height);

    public static native void nativeOnDrawFrame();

    public static native void nativeOnTouchesBegan(float pointX, float pointY);

    public static native void nativeOnTouchesEnded(float pointX, float pointY);

    public static native void nativeOnTouchesMoved(float pointX, float pointY);

    public static native void nativeSetCharacter(String index);

    public static native void nativeChangeBackground(String name);
    // Java -----------------------------------------------------------------

//    public static void SetContext(Context context) {
//
//        _context = context;
//    }

    public static void SetActivityInstance(Activity activity) {
        if((_activityInstance != null) && (!_activityInstance.isFinishing())){
            _activityInstance.finish();
            _activityInstance = null;
        }
        _activityInstance = activity;
    }

    public static byte[] LoadFile(String filePath, boolean isSound) {
        if(filePath.equals("-1")){
            if((mediaPlayer1 != null) && (mediaPlayer1.isPlaying())){
                clearCharText();
                mediaPlayer1.release();
                mediaPlayer1 = null;
            }
            return null;
        }
        InputStream fileData = null;
        Log.d("SGY","loadFileFrom:"+filePath);
        try {
            fileData = ((Context)_activityInstance).getAssets().open(filePath);
            int fileSize = fileData.available();
            byte[] fileBuffer = new byte[fileSize];
            fileData.read(fileBuffer, 0, fileSize);
            return fileBuffer;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        } finally {
            try {
                if (fileData != null) {
                    fileData.close();
                }
                if(isSound){
                    //filePath: sounds/vo_char_1001_00_36_hca [1].wav
                    //vo_char_1001_00_07_hca
                    PlaySound(filePath);
                    setCharText(filePath.substring(7,29));
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void MoveTaskToBack() {
        _activityInstance.moveTaskToBack(true);
    }

    public static void PlaySound(String filePath){
        Log.d("SGY",filePath);
        if(filePath.equals("-1")){
            if(mediaPlayer1 != null){
                clearCharText();
                mediaPlayer1.release();
                mediaPlayer1 = null;
            }
            return;
        }
        try{
            AssetFileDescriptor afd = ((Context)_activityInstance).getAssets().openFd(filePath);
            if((mediaPlayer1 != null) && (mediaPlayer1.isPlaying())){
                clearCharText();
                mediaPlayer1.release();
                mediaPlayer1 = null;
            }
            if(!filePath.equals("")){
                mediaPlayer1 = new MediaPlayer();
                mediaPlayer1.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(),afd.getLength());
                mediaPlayer1.setVolume(MainActivity.humanVolume,MainActivity.humanVolume);
                mediaPlayer1.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                    @Override
                    public void onCompletion(MediaPlayer mp) {
                        clearCharText();
                        mediaPlayer1.release();
                        mediaPlayer1 = null;
                    }
                });
                mediaPlayer1.prepare();
                mediaPlayer1.start();
            }
            afd.close();
        } catch (IOException e){
            e.printStackTrace();

        }
    }

    public static void setCharText(String path){
        //path:vo_char_1001_00_01_hca
        //try{
        //    //Log.d("SGY","text:" + MainActivity.textList.getString(path));
        //    ((MainActivity)(_context)).strokeTextView.setText(MainActivity.textList.getString(path));
        //}catch(Exception e){
        //    e.printStackTrace();
        //}
        try{
            Message msg = Message.obtain();
            msg.what = 1;
            //创建Bundle
            Bundle bundle = new Bundle();
            bundle.putString("text",MainActivity.textList.getString(path));
            bundle.putString("hintText","——「"+ getHintText(Integer.parseInt(path.substring(16,18))) +"」");
            //为Message设置Bundle数据
            msg.setData(bundle);
            //发送消息
            ((MainActivity)_activityInstance).handler.sendMessage(msg);
        }catch(Exception e){
            e.printStackTrace();
        }
    }

    public static void clearCharText(){
        Message msg = Message.obtain();
        msg.what = 1;
        //创建Bundle
        Bundle bundle = new Bundle();
        bundle.putString("text","");
        bundle.putString("hintText","");
        //为Message设置Bundle数据
        msg.setData(bundle);
        //发送消息
        ((MainActivity)(_activityInstance)).handler.sendMessage(msg);
    }

    public static String getHintText(int id){
        switch(id){
            case 1: return "自我介绍";
            case 2: return "初次见面";
            case 24: return "初次登陆";
            case 25: return "问候·早晨";
            case 26: return "问候·中午";
            case 27: return "问候·晚上";
            case 28: return "问候·深夜";
            case 29: return "问候·其他";
            case 30: return "问候·AP最大时";
            case 31: return "问候·BP最大时";
            case 32: return "问候·未使用";
            case 33: return "闲聊·其一";
            case 34: return "闲聊·其二";
            case 35: return "闲聊·其三";
            case 36: return "闲聊·其四";
            case 37: return "闲聊·其五";
            case 38: return "闲聊·其六";
            case 39: return "闲聊·其七";
            case 40: return "闲聊·其八";
            case 41: return "闲聊·隐藏";
            default: return "";
        }
    }

}
