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


public class BackgroundAdapter extends RecyclerView.Adapter<BackgroundAdapter.ViewHolder> {
    //直接把ViewHolder与xml文件适配
    private List<Background> mBackground;//用于存储传入的List<Person>
    private BackgroundChooseActivity instance;
    //初始化ViewHolder，创建时直接绑定好两个布局
    static class ViewHolder extends RecyclerView.ViewHolder{
        ImageView imageView;
        LinearLayout background;
        public ViewHolder(View view){
            super(view);
            imageView = view.findViewById(R.id.imageView);
            background = view.findViewById(R.id.itemBackground);
        }
    }

    //构造方法
    public BackgroundAdapter(List<Background> backgroundList, BackgroundChooseActivity instance){
        mBackground = backgroundList;
        this.instance = instance;
    }

    //以下为三个必须重写的方法：
    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.background_list_item, parent,false);
        ViewHolder holder = new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {//注意有个同名方法，不要覆写错了
        //该方法在该View出现在屏幕上时被调用，次序晚于onCreate
        //获得position对应的类，并设置holder的内容
        final Background background = mBackground.get(position);
        holder.imageView.setImageResource(getResource(background.name.substring(0,12)));
        holder.background.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                instance.retData(background.name);
            }
        });
    }

    //该方法决定了ListView内控件的数量
    @Override
    public int getItemCount() {
        return mBackground.size();
    }


    public int getResource(String idName){
        int resId = instance.getResources().getIdentifier(idName, "drawable", instance.getPackageName());
        return resId;
    }
}

class Background{
    String name;
    Background(String name){
        this.name = name;
    }
}