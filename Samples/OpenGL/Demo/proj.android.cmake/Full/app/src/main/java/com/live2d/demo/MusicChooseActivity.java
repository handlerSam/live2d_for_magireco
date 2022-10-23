package com.live2d.demo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import android.content.Intent;
import android.os.Bundle;
import android.widget.SeekBar;

import java.util.ArrayList;
import java.util.List;

public class MusicChooseActivity extends AppCompatActivity {
    private List<Music> musicList = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music_choose);
        RecyclerView recyclerView = (RecyclerView)findViewById(R.id.recyclerView3);

        if(musicList.size() == 0){
            createMusicList();
        }
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.setAdapter(new MusicAdapter(musicList,this));

        SeekBar humanBar = findViewById(R.id.seekBar_humanVoice);
        SeekBar backgroundBar = findViewById(R.id.seekBar_backgroundVoice);
        humanBar.setProgress((int)(MainActivity.humanVolume*100));
        backgroundBar.setProgress((int)(MainActivity.backgroundVolume*100));
        humanBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                MainActivity.humanVolume = 1.0f*i/100;
                if((JniBridgeJava.mediaPlayer1 != null) && (JniBridgeJava.mediaPlayer1.isPlaying())){
                    JniBridgeJava.mediaPlayer1.setVolume(1.0f*i/100,1.0f*i/100);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        backgroundBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                MainActivity.backgroundVolume = 1.0f*i/100;
                if((MainActivity.mediaPlayer1 != null) && (MainActivity.mediaPlayer1.isPlaying())){
                    MainActivity.mediaPlayer1.setVolume(1.0f*i/100,1.0f*i/100);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void retData(String name){
        Intent intent2 = new Intent();
        intent2.putExtra("data",name);
        setResult(RESULT_OK, intent2);
        finish();
    }

    public void createMusicList(){
        Music m = new Music("bgm01 - Scaena Felix","bgm01");
        musicList.add(m);
        m = new Music("bgm02 - Desiderium","bgm02");
        musicList.add(m);
        m = new Music("bgm03 - Inevitabilis","bgm03");
        musicList.add(m);
        m = new Music("bgm04 - Memories","bgm04");
        musicList.add(m);
        m = new Music("bgm05 - Repeat Days","bgm05");
        musicList.add(m);
        m = new Music("bgm06 - Remain","bgm06");
        musicList.add(m);
        m = new Music("bgm07 - Desiderium ver1","bgm07");
        musicList.add(m);
        m = new Music("bgm08 - Desiderium ver2","bgm08");
        musicList.add(m);
        m = new Music("bgm09 - Painful Memories","bgm09");
        musicList.add(m);
        m = new Music("bgm10 - Country Side","bgm10");
        musicList.add(m);
        m = new Music("bgm11 - Taenia memoriae","bgm11");
        musicList.add(m);
        m = new Music("bgm12 - her wings","bgm12");
        musicList.add(m);
        m = new Music("bgm13 - This Morning","bgm13");
        musicList.add(m);
        m = new Music("bgm14 - Double Christmas","bgm14");
        musicList.add(m);
        m = new Music("bgm15 - Tokime Tribe","bgm15");
        musicList.add(m);
        m = new Music("停止播放","cancel");
        musicList.add(m);
        m = new Music("重新获取字幕文件","getSubtitle");
        musicList.add(m);
    }
}