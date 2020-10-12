//
// Created by 123 on 2020/10/12.
//

#include "Media.h"

void Media::analysisStream(ThreadMode threadMode, AVFormatContext *pFormatContext) {
    publicAnalysisStream(threadMode,pFormatContext);
    privateAnalysisStream(threadMode,pFormatContext);
}

Media::~Media() {

}

