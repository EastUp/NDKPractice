package com.east.ffmpegvideo.ui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;

import androidx.annotation.Nullable;

public class MusicProgressBar extends View {
    private int mMax = 0;
    private int mCurrent = 0;
    private Paint mPaint;

    public MusicProgressBar(Context context) {
        this(context, null);
    }

    public MusicProgressBar(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MusicProgressBar(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mPaint = new Paint();
        mPaint.setAntiAlias(true);
        mPaint.setDither(false);
    }


    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        // 画背景
        int progressBgHeight = getMeasuredHeight() / 6;
        int top = (getMeasuredHeight() - progressBgHeight) / 2;
        int bottom = top + progressBgHeight;
        mPaint.setColor(Color.parseColor("#ECECEC"));
        RectF rectF = new RectF(0, top, getMeasuredWidth(), bottom);
        canvas.drawRoundRect(rectF, getMeasuredHeight() / 2, getMeasuredHeight() / 2, mPaint);

        if (mMax <= 0) {
            return;
        }
        if (mCurrent <= 0) {
            return;
        }

        float padding = dip2px(5);
        String timeStr = getTimeStr(mCurrent) + "/" + getTimeStr(mMax);
        mPaint.setTextSize(dip2px(10));
        // 计算进度条
        float progressWidth = mPaint.measureText(timeStr) + 2 * padding;
        float progressCenter = (mCurrent * 1f / mMax) * getMeasuredWidth();
        float left = progressCenter - progressWidth / 2;
        if (left < 0) {
            left = 0;
        }
        float right = left + progressWidth;
        if (right > getMeasuredWidth()) {
            right = getMeasuredWidth();
            left = right - progressWidth;
        }

        // 画进度背景
        mPaint.setColor(Color.parseColor("#46a6f8"));
        rectF = new RectF(0, top, left + progressWidth / 2, bottom);
        canvas.drawRoundRect(rectF, getMeasuredHeight() / 2, getMeasuredHeight() / 2, mPaint);
        // 画进度
        rectF = new RectF(left, 0, right, getMeasuredHeight());
        canvas.drawRoundRect(rectF, getMeasuredHeight() / 2, getMeasuredHeight() / 2, mPaint);
        // 画文字（要用基线去算，这里我省略了）
        mPaint.setColor(Color.WHITE);
        canvas.drawText(timeStr, left + padding, getMeasuredHeight() * 3 / 4, mPaint);
    }

    private float dip2px(int dip) {
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dip, getResources().getDisplayMetrics());
    }

    private String getTimeStr(int seconds) {
        int minute = seconds / 60;
        seconds = seconds % 60;
        return String.format("%02d:%02d", minute, seconds);
    }

    public void setProgress(int current, int total) {
        this.mMax = total;
        this.mCurrent = current;
        postInvalidate();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mMax <= 0) {
            return false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_MOVE:
                float moveX = event.getX();
                mCurrent = (int) ((moveX / getMeasuredWidth()) * mMax);
                setProgress(mCurrent, mMax);
                break;

            case MotionEvent.ACTION_UP:
                float upX = event.getX();
                mCurrent = (int) ((upX / getMeasuredWidth()) * mMax);
                setProgress(mCurrent, mMax);
                if (mListener != null) {
                    mListener.onProgress(mCurrent);
                }
                break;
        }
        return true;
    }

    /**
     * 后退
     *
     * @param seconds 秒
     */
    public void last(int seconds) {
        mCurrent -= seconds;
        if (mCurrent < 0) {
            mCurrent = 0;
        }
        postInvalidate();
        if (mListener != null) {
            mListener.onProgress(mCurrent);
        }
    }

    /**
     * 快进
     *
     * @param seconds 秒
     */
    public void next(int seconds) {
        mCurrent += seconds;
        if (mCurrent > mMax) {
            mCurrent = mMax;
        }
        postInvalidate();
        if (mListener != null) {
            mListener.onProgress(mCurrent);
        }
    }

    // 进度监听
    public interface ProgressListener {
        void onProgress(int progress);
    }
    private ProgressListener mListener;
    public void setOnProgressListener(ProgressListener listener) {
        mListener = listener;
    }
}
