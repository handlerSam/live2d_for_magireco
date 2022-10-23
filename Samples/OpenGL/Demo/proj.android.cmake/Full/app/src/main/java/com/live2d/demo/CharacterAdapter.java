package com.live2d.demo;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Filter;
import android.widget.Filterable;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.StaggeredGridLayoutManager;

import java.util.ArrayList;
import java.util.List;


public class CharacterAdapter extends RecyclerView.Adapter<CharacterAdapter.ViewHolder> implements Filterable {
    //直接把ViewHolder与xml文件适配
    private List<Character> mCharacter;//用于存储传入的List<Person>
    private List<Character> mFilterList;//用于存储传入的List<Person>
    private ModelChooseActivity instance;
    StaggeredGridLayoutManager manager;

    @Override
    public Filter getFilter() {
        return  new Filter() {
            //执行过滤操作
            @Override
            protected FilterResults performFiltering(CharSequence charSequence) {
                String charString = charSequence.toString();
                if (charString.isEmpty()) {
                    //没有过滤的内容，则使用源数据
                    mFilterList = mCharacter;
                } else {
                    List<Character> filteredList = new ArrayList<>();
                    for (Character c : mCharacter) {
                        //这里根据需求，添加匹配规则
                        if (c.name.contains(charString)) {
                            filteredList.add(c);
                        }
                    }

                    mFilterList = filteredList;
                }

                FilterResults filterResults = new FilterResults();
                filterResults.values = mFilterList;
                return filterResults;
            }
            //把过滤后的值返回出来
            @Override
            protected void publishResults(CharSequence charSequence, FilterResults filterResults) {
                mFilterList = (ArrayList<Character>) filterResults.values;
                notifyDataSetChanged();
            }
        };
    }

    //初始化ViewHolder，创建时直接绑定好两个布局
    static class ViewHolder extends RecyclerView.ViewHolder{
        TextView textView;
        TextView codeTextView;
        LinearLayout background;
        public ViewHolder(View view){
            super(view);
            textView = (TextView) view.findViewById(R.id.characterNameTextView);
            codeTextView = view.findViewById(R.id.characterCodeTextView);
            background = view.findViewById(R.id.itemBackground);
        }
    }

    //构造方法
    public CharacterAdapter(List<Character> characterList, ModelChooseActivity instance, StaggeredGridLayoutManager manager){
        mCharacter = characterList;
        mFilterList = characterList;
        this.instance = instance;
        this.manager = manager;
    }

    //以下为三个必须重写的方法：
    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.character_list_item, parent,false);
        ViewHolder holder = new ViewHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {//注意有个同名方法，不要覆写错了
        //该方法在该View出现在屏幕上时被调用，次序晚于onCreate
        //获得position对应的类，并设置holder的内容
        final Character character = mFilterList.get(position);
        holder.textView.setText(character.name);
        holder.codeTextView.setText(character.code);
        holder.background.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.d("SGY",character.code);
                instance.retData(character.code);
            }
        });
    }

    //该方法决定了ListView内控件的数量
    @Override
    public int getItemCount() {
        return mFilterList.size();
    }
}

class Character{
    String name;
    String code;
    Character(String name, String code){
        this.name = name;
        this.code = code;
    }
}