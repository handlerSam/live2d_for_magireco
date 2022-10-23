package com.live2d.demo;

import android.content.Context;
import android.content.res.Resources;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import java.util.List;


public class MusicAdapter extends RecyclerView.Adapter<MusicAdapter.ViewHolder> {
    //直接把ViewHolder与xml文件适配
    private List<Music> mMusic;//用于存储传入的List<Person>
    private MusicChooseActivity instance;
    //初始化ViewHolder，创建时直接绑定好两个布局
    static class ViewHolder extends RecyclerView.ViewHolder{
        TextView textView;
        LinearLayout background;
        public ViewHolder(View view){
            super(view);
            textView = view.findViewById(R.id.musicTextView);
            background = view.findViewById(R.id.itemBackground);
        }
    }

    //构造方法
    public MusicAdapter(List<Music> musicList, MusicChooseActivity instance){
        mMusic = musicList;
        this.instance = instance;
    }

    //以下为三个必须重写的方法：
    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.music_list_item, parent,false);
        ViewHolder holder = new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {//注意有个同名方法，不要覆写错了
        //该方法在该View出现在屏幕上时被调用，次序晚于onCreate
        //获得position对应的类，并设置holder的内容
        final Music music = mMusic.get(position);
        holder.textView.setText(music.name);
        holder.background.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                instance.retData(music.fileName);
            }
        });
    }

    //该方法决定了ListView内控件的数量
    @Override
    public int getItemCount() {
        return mMusic.size();
    }

}

class Music{
    String name;
    String fileName;//bgm0X
    Music(String name, String fileName){
        this.name = name;
        this.fileName = fileName;
    }
}