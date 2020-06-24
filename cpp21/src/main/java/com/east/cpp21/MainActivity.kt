package com.east.cpp21

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val persons = arrayOfNulls<Person>(10)
        for (i in persons.indices){
            persons[i] = Person("eastrise",25+i)
        }

        // java 层赋值
        val newPersons = arrayOfNulls<Person>(15)

       /* for(i in newPersons.indices){
            newPersons[i] = persons[i];
        }*/

        try {
            arraycopy(persons,0,newPersons,5,10)
        } catch (e: Exception) {
            e.printStackTrace()
        }

        for((i,value) in newPersons.withIndex()){
            if(value != null)
                Log.e("TAG","$i: ${value.toString()}")
            else
                Log.e("TAG","$i : null")
        }
    }

    external fun arraycopy(src:Any,srcPos:Int,
                           dest:Any,destPos:Int,
                           length:Int)

    companion object{
        init {
            System.loadLibrary("native-lib")
        }

    }
}
