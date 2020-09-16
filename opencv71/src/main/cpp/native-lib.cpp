#include <jni.h>
#include <string>
#include <android/log.h>  //系统自带的用<>
#include <android/bitmap.h>
#include "opencv2/opencv.hpp"
#include "opencv2/face.hpp"


#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
//使用命名空间
using namespace cv;
using namespace face;
using namespace std;

// 静态注册，动态注册（6.0 以上 frameworker NDK 源码） Linux

// 加载人脸识别的级联分类器
CascadeClassifier cascadeClassifier;
// android native 源码 Ptr<BasicFaceRecognizer> 相当于 BasicFaceRecognizer*
Ptr<BasicFaceRecognizer> model = EigenFaceRecognizer::create();

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv71_FaceDetection_loadCascade(JNIEnv *env, jobject thiz, jstring file_path) {
    const char *filePath = env->GetStringUTFChars(file_path,0);

    cascadeClassifier.load(filePath);
    LOGE("人脸识别级联分类器加载成功");
    env->ReleaseStringUTFChars(file_path,filePath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv71_FaceDetection_faceDetection(JNIEnv *env, jobject thiz, jlong nativeObj) {
    Mat *src = reinterpret_cast<Mat*>(nativeObj);

    int width = src->cols;
    int height = src->rows;

    Mat gray;
    // 转灰度图，提升运算速度，灰度图所对应的 CV_8UC1 单颜色通道，信息量少 0-255 1u
    cvtColor(*src,gray,COLOR_BGR2GRAY);

    // 检测人脸
    vector<Rect> faces;
    // @param 1.1 会采用上采样、下采样、缩放比例
    // @param 3 会检测多少次才能判断是个人脸
    // @param Size(width/4,height/4) 最小的人脸大小
    // @param Size(width,height) 最大的人脸
    cascadeClassifier.detectMultiScale(gray,faces,1.1,3,0,Size(width/4,height/4),Size(width,height));
    LOGE("人脸size = %d",faces.size());

    if(faces.size() <= 0){
        return;
    }

    // 把脸框出来
    for (int i = 0; i < faces.size(); ++i) {
        rectangle(*src,faces[i],Scalar(255,0,0,255),4,LINE_AA);
        // 不断检测，录入 10 张，张张嘴巴，眨眨眼睛 ，保证准确率
        // 还需要注意一点，确保人脸大小一致，reSize(128,128) ,确保收集到的人脸眼睛尽量在一条线上
        // 与服务端进行比对，是不是我

        // 用一个计数器，这里我们做及时的
        Mat face = (*src)(faces[i]).clone();
        resize(face, face, Size(128, 128));
        cvtColor(face, face, COLOR_BGRA2GRAY);
        // 直方均衡，harr 检测人脸
        int label = model->predict(face);
        // 训练的时候存的是 11
        if (label == 11) {
            // 识别到了自己
            LOGE("识别到了自己");
            putText(*src, "Darren", Point(faces[i].x + 20, faces[i].y - 20),
                    HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0, 255),2,LINE_AA);
        } else {
            // 不是自己
            LOGE("不是自己");
            putText(*src, "UnKnow", Point(faces[i].x + 20, faces[i].y - 20),
                    HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0, 255),2,LINE_AA);
        }
        // 速度， 准确率, 人脸尽量正常
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv71_FaceDetection_trainingPattern(JNIEnv *env, jobject instance) {
    // 训练样本，这一步是在数据采集做的
    // train it
    vector<Mat> faces;
    vector<int> labels;

    // 样本比较少
    for (int i = 1; i <= 5; ++i) {
        for (int j = 1; j <= 5; ++j) {
            Mat face = imread(format("/storage/emulated/0/s%d/%d.pgm", i, j), 0);
            if (face.empty()) {
                LOGE("face mat is empty");
                continue;
            }
            // 确保大小一致
            resize(face, face, Size(128, 128));
            faces.push_back(face);
            labels.push_back(i);
        }
    }

    for (int i = 1; i <= 8; ++i) {
        Mat face = imread(format("/storage/emulated/0/face_%d.png", i), 0);
        if (face.empty()) {
            LOGE("face mat is empty");
            continue;
        }
        resize(face, face, Size(128, 128));
        faces.push_back(face);
        labels.push_back(11);
    }

    // 训练方法
    Ptr<BasicFaceRecognizer> model = EigenFaceRecognizer::create();
    // 采集了八张，同一个人 label 一样
    model->train(faces, labels);
    // 训练样本是 xml ，本地
    model->save("/storage/emulated/0/face_darren_pattern.xml");// 存的是处理的特征数据
    LOGE("樣本訓練成功");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_east_opencv71_FaceDetection_loadPattern(JNIEnv *env, jobject instance,
                                                    jstring patternPath_) {
    const char *patternPath = env->GetStringUTFChars(patternPath_, 0);
    // 加载样本数据，Error:(142, 12) error: no matching member function for call to 'load' 怎么搞？
    // model->load(patternPath);
    FileStorage fs(patternPath, FileStorage::READ);
    // 报错，没有提示
    // CV_Assert(fs.isOpened());
    // 抛个 java 异常
    FileNode fn = fs.getFirstTopLevelNode();
    model->read(fn);
    env->ReleaseStringUTFChars(patternPath_, patternPath);
    LOGE("训练样本加载成功");
}