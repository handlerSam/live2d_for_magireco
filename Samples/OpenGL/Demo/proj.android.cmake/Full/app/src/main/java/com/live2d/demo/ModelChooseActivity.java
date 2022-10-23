package com.live2d.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class ModelChooseActivity extends AppCompatActivity {
    StaggeredGridLayoutManager m;
    int modelPosition;
    String filteredText = "";
    private List<Character> characterList = new ArrayList<>();
    public CharacterAdapter adapter;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_model_choose);
        getJsonList();
        RecyclerView recyclerView = (RecyclerView)findViewById(R.id.recyclerView);
        m = new StaggeredGridLayoutManager(4,StaggeredGridLayoutManager.VERTICAL);
        recyclerView.setLayoutManager(m);
        m.scrollToPositionWithOffset(getIntent().getIntExtra("position",0),0);
        modelPosition = getIntent().getIntExtra("position",0);
        adapter = new CharacterAdapter(characterList,this,m);
        recyclerView.setAdapter(adapter);
        recyclerView.addOnScrollListener(new RecyclerView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(@NonNull RecyclerView recyclerView, int newState) {
                super.onScrollStateChanged(recyclerView, newState);
                modelPosition = m.findFirstVisibleItemPositions(null)[0];
            }
        });

        EditText editText = findViewById(R.id.EditText);
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                filteredText = charSequence.toString();
                adapter.getFilter().filter(charSequence.toString());
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });
        filteredText = getIntent().getStringExtra("editText");
        editText.setText(filteredText);
        adapter.getFilter().filter(filteredText);
    }

    public void retData(String code){
        Intent intent2 = new Intent();
        intent2.putExtra("data",code);
        intent2.putExtra("position",modelPosition);
        intent2.putExtra("filteredText",filteredText);
        setResult(RESULT_OK, intent2);
        finish();
    }

    public void getJsonList(){
        if(characterList.size() == 0){
            InputStream stream = getResources().openRawResource(R.raw.name_dic);
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
                JSONArray jsonArray = new JSONArray(sb.toString());
                for(int i = 0; i < jsonArray.length();i++){
                    JSONObject jsonObject = (JSONObject)jsonArray.get(i);
                    Character c = new Character(jsonObject.getString("name"),jsonObject.getString("id"));
                    characterList.add(c);
                }
            }catch(Exception e){
                e.printStackTrace();
            }
            Collections.sort(characterList, new Comparator<Character>() {
                @Override
                public int compare(Character lhs, Character rhs) {
                    // -1 - less than, 1 - greater than, 0 - equal, all inversed for descending
                    int leftCode = Integer.parseInt(lhs.code);
                    int rightCode = Integer.parseInt(rhs.code);
                    return leftCode < rightCode? -1:(leftCode > rightCode? 1:0);
                }
            });
        }
    }
}