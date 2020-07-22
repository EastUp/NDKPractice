package com.east.datastructure43graph;

import android.util.Log;

import java.util.ArrayDeque;

/**
 * |---------------------------------------------------------------------------------------------------------------|
 *
 * @description: 数据结构--图
 * @author: jamin
 * @date: 2020/7/22
 * |---------------------------------------------------------------------------------------------------------------|
 */
public class Graph {
    public final static int MAX_WEIGHT = 65535;
    // 顶点的个数
    int vertexSize;

    // 顶点的集合
    int[] vertexs;

    //边的集合
    int[][] matrix;

    public Graph(int vertexSize, int[] vertexs, int[][] matrix) {
        this.vertexSize = vertexSize;
        this.vertexs = vertexs;
        this.matrix = matrix;
    }


    // 广度遍历
    public void breadthFirstSearch() {
        // 访问过的顶点角标
        boolean[] visited = new boolean[vertexSize];

        // 把第一个顶点加入
        ArrayDeque<Integer> vertexQ = new ArrayDeque<>();
        vertexQ.push(vertexs[3]);

        while (!vertexQ.isEmpty()) {
            // 当前顶点，但是这里又是角标
            int currentVertex = vertexQ.pop();
            int index = currentVertex;

            if (!visited[index]) {
                Log.e("TAG", "访问到了顶点：" + currentVertex);
                visited[index] = true;

                for (int i = 0; i < vertexSize; i++) {
                    if (matrix[index][i] == 1 && !visited[i]) { // 代表 与 index 相连
                        // 画蛇添足的做法：写一个判断如果队列中不包含再往里面新增，这个时候的复杂度就是 O(n)了
//                        if(!vertexQ.contains(i)){  // 0(N)
                        vertexQ.addLast(i);
//                        }
                    }
                }
            }
        }
    }

    // 最小生成树（普里姆算法）
    public void prim(){
        // 定义一个数组内存，当前修好村庄，lowcost = 0 代表已经修了
        int[] lowcost = new int[vertexSize];

        // 第一行的数据先放到 lowcost
        for (int i = 0; i < vertexSize; i++) {
            lowcost[i] = matrix[0][i];        // [0,1,5,max,max,max,max,max,max]
        }

        int sum = 0;

        for (int i = 1; i < vertexSize; i++) {
            int min = MAX_WEIGHT;
            int minId = 0;

            // 找最小的，之间是否连通
            for (int j = 1; j < vertexSize; j++) {
                if(lowcost[j] < min && lowcost[j] != 0){
                    min = lowcost[j];
                    minId = j;
                }
            }

            // min = 1, minId = 1;

            Log.e("TAG","找到村庄："+vertexs[minId] + ", 修路距离：" + min);
            lowcost[minId] = 0;
            sum += min;

            for (int k = 0; k < vertexSize; k++) {
                // 边考虑边淘汰
                if(matrix[minId][k] < lowcost[k] && lowcost[k] > 0){
                    lowcost[k] = matrix[minId][k];
                }
            }

//            for (int o = 0; o< vertexSize; o++){
//                Log.e("TAG","i = "+o+",  number = " + lowcost[o]); //[0,0,3,7,5,max,max,max,max]
//            }
//            Log.e("TAG","==================================== ");

        }

        Log.e("TAG","最短路径是：" + sum);

    }

    // 生成最短路径（迪杰斯特拉算法）
    public void dijstra() {
        // 有没找到最短路径
        boolean[] isPath = new boolean[vertexSize];
        // 存放每个村庄的最短路径
        int[] shortPath = new int[vertexSize];

        for (int i = 0; i <vertexSize; i++) {
            shortPath[i] = matrix[0][i];
        }

        shortPath[0] = 0;
        isPath[0] = true;

        for (int i = 1; i < vertexSize; i++) { // 外循环每次找到下一个村庄最短的路径
            int minId = 0;
            int min = MAX_WEIGHT;
            for (int j = 1; j < vertexSize; j++) {
                if(shortPath[j] < min && !isPath[j]){
                    min = shortPath[j];   // 1 4
                    minId = j;            // 1 2
                }
            }

            isPath[minId] = true; // 已经找到了最短的路径

            for (int k = 0; k < vertexSize; k++) {
                if(!isPath[k] && (matrix[minId][k] + min) < shortPath[k]){
                    shortPath[k] = matrix[minId][k] + min;
                }
            }

            for (int k = 0; k < vertexSize; k++) {
                Log.e("TAG", shortPath[k]+" ");
            }
            Log.e("TAG", "===================");
        }

        for (int i = 0; i < vertexSize; i++) {
            Log.e("TAG", "顶点 0 到顶点 " + i + " 的最短距离为：" + shortPath[i]);
        }

    }
}
