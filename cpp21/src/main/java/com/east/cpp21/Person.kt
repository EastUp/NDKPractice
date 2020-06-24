package com.east.cpp21

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *  @description:
 *  @author: jamin
 *  @date: 2020/6/24
 * |---------------------------------------------------------------------------------------------------------------|
 */
data class Person(var name:String,var age:Int){
    override fun toString(): String {
        return "Person{name = '$name',age = $age}"
    }
}