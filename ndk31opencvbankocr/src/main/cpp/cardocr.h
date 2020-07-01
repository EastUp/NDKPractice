//
// Created by 123 on 2020/6/30.
//

#ifndef NDKPRACTICE_CARDOCR_H
#define NDKPRACTICE_CARDOCR_H

#include "opencv2/opencv.hpp"

using namespace cv;

// 会针对不同的场景，做不同的事
namespace co1{

    /**
     * 找到银行卡区域
     * @param mat  图片的 mat
     * @param area 卡号区域
     * @return  是否成功 0 成功, 1 失败
     */
    int find_card_area(const Mat &mat,Rect &area);

    /**
     * 通过银行卡区域截取到卡号区域
     * @param mat  银行的 mat
     * @param area 存放截取的区域
     * @return
     */
    int find_card_number_area(const Mat &mat,Rect &area);

    /**
     *  找到所有的数字
     * @param mat 银行卡号区域
     * @param numbers 存放所有数字
     * @return 是否成功
     */
    int find_card_numbers(const Mat &mat,std::vector<Mat> numbers);

    /**
     * 找到所有的数字
     * @param mat 银行卡号区域
     * @return 粘连的那一列
     */
    int find_split_cols_pos(Mat mat);

}

/**
 * 备选方案
 */
namespace co2{

}

#endif //NDKPRACTICE_CARDOCR_H
