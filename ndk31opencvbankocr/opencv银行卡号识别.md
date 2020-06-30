# <center>opencv银行卡识别<center>
@[TOC](opencv银行卡识别)

步骤：

## 1. bitmap 转 mat

```c++
int BitmapMatUtils::bitmap2mat(JNIEnv *env, jobject bitmap, Mat &mat) {
    // 锁定画布
    void* pixels;

    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    // 获取 bitmap 的信息
    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env,bitmap,&bitmapInfo);

    // 返回三通道 CV_8UC4-> argb_8888, CV_8UC2->rgb CV_8UC1->黑白
    Mat createMat(bitmapInfo.height,bitmapInfo.width,CV_8UC4);
    if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888){ // mat 里面的四颜色通道 CV_8UC4
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC4,pixels);
        temp.copyTo(createMat);
    }else if(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565){// mat 里面的三颜色通道 CV_8UC2
        Mat temp(bitmapInfo.height,bitmapInfo.width,CV_8UC2,pixels);
        cvtColor(temp,createMat,COLOR_BGR5652BGRA);
    }

    createMat.copyTo(mat);

    // 解锁画布
    AndroidBitmap_unlockPixels(env,bitmap);

    return 0;

}
```

## 2. 银行卡区域裁剪

注意：  
`imwrite("/storage/emulated/0/ocr/binary_n.jpg",grad);// 这个方法会创建文件，但是不会创建文件夹`

```c++
int co1::find_card_area(const Mat &mat, Rect &area) {

    // 首先降噪
    Mat blur;
    GaussianBlur(mat,blur,Size(5,5),BORDER_DEFAULT,BORDER_DEFAULT);

    // 边缘梯度增强（保存图片）x,y 增强
    Mat grad_x,grad_y;
    Scharr(blur,grad_x,CV_32F,1,0);
    Scharr(blur,grad_y,CV_32F,0,1);
    Mat grad_abs_x,grad_abs_y;
    convertScaleAbs(grad_x, grad_abs_x);
    convertScaleAbs(grad_y, grad_abs_y);
    Mat grad;
    addWeighted(grad_abs_x, 0.5, grad_abs_y, 0.5, 0, grad);

    // 写到内存卡
    imwrite("/storage/emulated/0/ocr/grad_n.jpg",grad);

    // 二值化，筛选轮廓
    Mat gray;
    cvtColor(grad,gray,COLOR_BGRA2GRAY);
    imwrite("/storage/emulated/0/ocr/gray_n.jpg",gray);
    Mat binary;
    threshold(gray,binary,40,255,THRESH_BINARY);

    imwrite("/storage/emulated/0/ocr/binary_n.jpg",grad);

    // 稍微知道这么个概念，card.io 很多图像过滤
    vector<vector<Point>> contours;
    findContours(binary,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i) {
        Rect rect = boundingRect(contours[i]);
        // 过滤轮廓
        if(rect.width > mat.cols/2 && rect.width != mat.cols && rect.height > mat.rows / 2){
            // 银行卡区域的宽高必须大于图片的一半
            area = rect;
            break;
        }
    }

    // release source
    blur.release();
    grad_x.release();
    grad_y.release();
    grad_abs_x.release();
    grad_abs_y.release();
    grad.release();
    gray.release();
    binary.release();
    return 0;
}
```
