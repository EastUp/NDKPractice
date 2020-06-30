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
}

/**
 * 备选方案
 */
namespace co2{

}

#endif //NDKPRACTICE_CARDOCR_H
