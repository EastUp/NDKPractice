package com.east.ffmpegmusic.ui;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.east.ffmpegmusic.R;

import java.util.List;

public class MusicAdapter extends BaseAdapter {
    private List<MusicItem> mMusicItems;
    private Context mContext;
    private MusicItem mCurrentItem;

    public MusicAdapter(List<MusicItem> musicItems, Context context) {
        this.mMusicItems = musicItems;
        this.mContext = context;
    }

    @Override
    public int getCount() {
        return mMusicItems.size();
    }

    @Override
    public Object getItem(int position) {
        return mMusicItems.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        convertView = LayoutInflater.from(mContext).inflate(R.layout.item_music_lv, parent, false);
        TextView musicNameTv = convertView.findViewById(R.id.music_name_tv);
        final MusicItem musicItem = mMusicItems.get(position);
        musicNameTv.setText(musicItem.fileName);

        if (musicItem.equals(mCurrentItem)) {
            musicNameTv.setTextColor(Color.parseColor("#46a6f8"));
        } else {
            musicNameTv.setTextColor(Color.parseColor("#333333"));
        }

        convertView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!musicItem.equals(mCurrentItem)) {
                    mCurrentItem = musicItem;
                    if (mListener != null) {
                        mListener.onItem(mCurrentItem);
                    }

                    notifyDataSetChanged();
                }
            }
        });

        return convertView;
    }

    public interface ItemClickListener {
        void onItem(MusicItem item);
    }

    public ItemClickListener mListener;

    public void setOnItemClickListener(ItemClickListener listener) {
        mListener = listener;
    }

    public void showCurrent(MusicItem musicItem) {
        mCurrentItem = musicItem;
        notifyDataSetChanged();
    }
}
