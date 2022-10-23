package com.live2d.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import android.content.Intent;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.List;

public class BackgroundChooseActivity extends AppCompatActivity {
    private List<Background> backgroundList = new ArrayList<>();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_background_choose);
        RecyclerView recyclerView = (RecyclerView)findViewById(R.id.recyclerView2);
        if(backgroundList.size() == 0){
            for(int i = 1; i <= 22; i++){
                String id = (""+(i+100)).substring(1,3);
                id = "background" + id + ".png";
                Background ba = new Background(id);
                backgroundList.add(ba);
            }
        }
        StaggeredGridLayoutManager m = new StaggeredGridLayoutManager(4,StaggeredGridLayoutManager.VERTICAL);
        recyclerView.setLayoutManager(m);
        recyclerView.setAdapter(new BackgroundAdapter(backgroundList,this));
    }

    public void retData(String name){
        Intent intent2 = new Intent();
        intent2.putExtra("data",name);
        setResult(RESULT_OK, intent2);
        finish();
    }
}