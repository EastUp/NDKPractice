package com.east.datastructure43graph

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.east.datastructure43graph.Graph.MAX_WEIGHT
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        tv.text = stringFromJNI()

        var vertexSize = 9
        val vertexes = IntArray(9)

        for (i in 0 until vertexSize){
            vertexes[i] = i
        }

        val matrix =
            Array(vertexSize) { IntArray(vertexSize) }

/*        // v0
        matrix[0][0] = 0
        matrix[0][1] = 1
        matrix[0][2] = 1
        matrix[0][3] = 0
        matrix[0][4] = 0
        matrix[0][5] = 0
        matrix[0][6] = 0
        matrix[0][7] = 0
        matrix[0][8] = 0

        // v1
        matrix[1][0] = 1
        matrix[1][1] = 0
        matrix[1][2] = 1
        matrix[1][3] = 1
        matrix[1][4] = 1
        matrix[1][5] = 0
        matrix[1][6] = 0
        matrix[1][7] = 0
        matrix[1][8] = 0

        // v2
        matrix[2][0] = 1
        matrix[2][1] = 1
        matrix[2][2] = 0
        matrix[2][3] = 0
        matrix[2][4] = 1
        matrix[2][5] = 1
        matrix[2][6] = 0
        matrix[2][7] = 0
        matrix[2][8] = 0

        // v3
        matrix[3][0] = 0
        matrix[3][1] = 1
        matrix[3][2] = 0
        matrix[3][3] = 0
        matrix[3][4] = 1
        matrix[3][5] = 0
        matrix[3][6] = 1
        matrix[3][7] = 0
        matrix[3][8] = 0

        // v4
        matrix[4][0] = 0
        matrix[4][1] = 1
        matrix[4][2] = 1
        matrix[4][3] = 1
        matrix[4][4] = 0
        matrix[4][5] = 1
        matrix[4][6] = 1
        matrix[4][7] = 1
        matrix[4][8] = 0

        // v5
        matrix[5][0] = 0
        matrix[5][1] = 0
        matrix[5][2] = 1
        matrix[5][3] = 0
        matrix[5][4] = 1
        matrix[5][5] = 0
        matrix[5][6] = 0
        matrix[5][7] = 1
        matrix[5][8] = 0

        // v6
        matrix[6][0] = 0
        matrix[6][1] = 0
        matrix[6][2] = 0
        matrix[6][3] = 1
        matrix[6][4] = 1
        matrix[6][5] = 0
        matrix[6][6] = 0
        matrix[6][7] = 1
        matrix[6][8] = 1

        // v7
        matrix[7][0] = 0
        matrix[7][1] = 0
        matrix[7][2] = 0
        matrix[7][3] = 0
        matrix[7][4] = 1
        matrix[7][5] = 1
        matrix[7][6] = 1
        matrix[7][7] = 0
        matrix[7][8] = 1

        // v8
        matrix[8][0] = 0
        matrix[8][1] = 0
        matrix[8][2] = 0
        matrix[8][3] = 0
        matrix[8][4] = 0
        matrix[8][5] = 0
        matrix[8][6] = 1
        matrix[8][7] = 1
        matrix[8][8] = 0*/

        // v0

        // v0
        /* matrix[0][0] = 0;
        matrix[0][1] = 1;
        matrix[0][2] = 1;
        matrix[0][3] = 0;
        matrix[0][4] = 0;
        matrix[0][5] = 0;
        matrix[0][6] = 0;
        matrix[0][7] = 0;
        matrix[0][8] = 0;

        // v1
        matrix[1][0] = 1;
        matrix[1][1] = 0;
        matrix[1][2] = 1;
        matrix[1][3] = 1;
        matrix[1][4] = 1;
        matrix[1][5] = 0;
        matrix[1][6] = 0;
        matrix[1][7] = 0;
        matrix[1][8] = 0;

        // v2
        matrix[2][0] = 1;
        matrix[2][1] = 1;
        matrix[2][2] = 0;
        matrix[2][3] = 0;
        matrix[2][4] = 1;
        matrix[2][5] = 1;
        matrix[2][6] = 0;
        matrix[2][7] = 0;
        matrix[2][8] = 0;

        // v3
        matrix[3][0] = 0;
        matrix[3][1] = 1;
        matrix[3][2] = 0;
        matrix[3][3] = 0;
        matrix[3][4] = 1;
        matrix[3][5] = 0;
        matrix[3][6] = 1;
        matrix[3][7] = 0;
        matrix[3][8] = 0;

        // v4
        matrix[4][0] = 0;
        matrix[4][1] = 1;
        matrix[4][2] = 1;
        matrix[4][3] = 1;
        matrix[4][4] = 0;
        matrix[4][5] = 1;
        matrix[4][6] = 1;
        matrix[4][7] = 1;
        matrix[4][8] = 0;

        // v5
        matrix[5][0] = 0;
        matrix[5][1] = 0;
        matrix[5][2] = 1;
        matrix[5][3] = 0;
        matrix[5][4] = 1;
        matrix[5][5] = 0;
        matrix[5][6] = 0;
        matrix[5][7] = 1;
        matrix[5][8] = 0;

        // v6
        matrix[6][0] = 0;
        matrix[6][1] = 0;
        matrix[6][2] = 0;
        matrix[6][3] = 1;
        matrix[6][4] = 1;
        matrix[6][5] = 0;
        matrix[6][6] = 0;
        matrix[6][7] = 1;
        matrix[6][8] = 1;

        // v7
        matrix[7][0] = 0;
        matrix[7][1] = 0;
        matrix[7][2] = 0;
        matrix[7][3] = 0;
        matrix[7][4] = 1;
        matrix[7][5] = 1;
        matrix[7][6] = 1;
        matrix[7][7] = 0;
        matrix[7][8] = 1;

        // v8
        matrix[8][0] = 0;
        matrix[8][1] = 0;
        matrix[8][2] = 0;
        matrix[8][3] = 0;
        matrix[8][4] = 0;
        matrix[8][5] = 0;
        matrix[8][6] = 1;
        matrix[8][7] = 1;
        matrix[8][8] = 0;*/

        // v0
        matrix[0][0] = 0
        matrix[0][1] = 1
        matrix[0][2] = 5
        matrix[0][3] = MAX_WEIGHT
        matrix[0][4] = MAX_WEIGHT
        matrix[0][5] = MAX_WEIGHT
        matrix[0][6] = MAX_WEIGHT
        matrix[0][7] = MAX_WEIGHT
        matrix[0][8] = MAX_WEIGHT

        // v1
        matrix[1][0] = 1
        matrix[1][1] = 0
        matrix[1][2] = 3
        matrix[1][3] = 7
        matrix[1][4] = 5
        matrix[1][5] = MAX_WEIGHT
        matrix[1][6] = MAX_WEIGHT
        matrix[1][7] = MAX_WEIGHT
        matrix[1][8] = MAX_WEIGHT

        // v2
        matrix[2][0] = 5
        matrix[2][1] = 3
        matrix[2][2] = 0
        matrix[2][3] = MAX_WEIGHT
        matrix[2][4] = 1
        matrix[2][5] = 7
        matrix[2][6] = MAX_WEIGHT
        matrix[2][7] = MAX_WEIGHT
        matrix[2][8] = MAX_WEIGHT

        // v3
        matrix[3][0] = MAX_WEIGHT
        matrix[3][1] = 7
        matrix[3][2] = MAX_WEIGHT
        matrix[3][3] = 0
        matrix[3][4] = 2
        matrix[3][5] = MAX_WEIGHT
        matrix[3][6] = 1
        matrix[3][7] = MAX_WEIGHT
        matrix[3][8] = MAX_WEIGHT

        // v4
        matrix[4][0] = MAX_WEIGHT
        matrix[4][1] = 5
        matrix[4][2] = 1
        matrix[4][3] = 2
        matrix[4][4] = 0
        matrix[4][5] = 3
        matrix[4][6] = 6
        matrix[4][7] = 9
        matrix[4][8] = MAX_WEIGHT

        // v5
        matrix[5][0] = MAX_WEIGHT
        matrix[5][1] = MAX_WEIGHT
        matrix[5][2] = 7
        matrix[5][3] = MAX_WEIGHT
        matrix[5][4] = 3
        matrix[5][5] = 0
        matrix[5][6] = MAX_WEIGHT
        matrix[5][7] = 5
        matrix[5][8] = MAX_WEIGHT

        // v6
        matrix[6][0] = MAX_WEIGHT
        matrix[6][1] = MAX_WEIGHT
        matrix[6][2] = MAX_WEIGHT
        matrix[6][3] = 3
        matrix[6][4] = 6
        matrix[6][5] = MAX_WEIGHT
        matrix[6][6] = 0
        matrix[6][7] = 2
        matrix[6][8] = 7

        // v7
        matrix[7][0] = MAX_WEIGHT
        matrix[7][1] = MAX_WEIGHT
        matrix[7][2] = MAX_WEIGHT
        matrix[7][3] = MAX_WEIGHT
        matrix[7][4] = 9
        matrix[7][5] = 5
        matrix[7][6] = 2
        matrix[7][7] = 0
        matrix[7][8] = 4

        // v8
        matrix[8][0] = MAX_WEIGHT
        matrix[8][1] = MAX_WEIGHT
        matrix[8][2] = MAX_WEIGHT
        matrix[8][3] = MAX_WEIGHT
        matrix[8][4] = MAX_WEIGHT
        matrix[8][5] = MAX_WEIGHT
        matrix[8][6] = 7
        matrix[8][7] = 4
        matrix[8][8] = 0

        var graph = Graph(vertexSize,vertexes,matrix)


        // 广度遍历
//        graph.breadthFirstSearch()

        // 最小生成树
        graph.prim()

    }

    external fun stringFromJNI():String

    companion object{
        init {
            System.loadLibrary("native-lib")
        }
    }
}
