package com.east.jni09;

import android.os.Parcel;
import android.os.Parcelable;

public class Student implements Parcelable {
    protected Student(Parcel in) {
        in.readInt(); // char*
        in.readString();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(12);
        dest.writeString("Eastrise");
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<Student> CREATOR = new Creator<Student>() {
        @Override
        public Student createFromParcel(Parcel in) {
            return new Student(in);
        }

        @Override
        public Student[] newArray(int size) {
            return new Student[size];
        }
    };
}
