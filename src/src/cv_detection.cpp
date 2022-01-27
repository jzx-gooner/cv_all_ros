//jzx
#include "cv_detection.hpp"

using namespace cv;
using namespace std;

static const char* cocolabels[] = {
    "person", "bicycle", "car", "motorcycle", "airplane",
    "bus", "train", "truck", "boat", "traffic light", "fire hydrant",
    "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse",
    "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
    "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis",
    "snowboard", "sports ball", "kite", "baseball bat", "baseball glove",
    "skateboard", "surfboard", "tennis racket", "bottle", "wine glass",
    "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich",
    "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake",
    "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv",
    "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave",
    "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase",
    "scissors", "teddy bear", "hair drier", "toothbrush"
};


struct AffineMatrix{
    float i2d[6];       // image to dst(network), 2x3 matrix
    float d2i[6];       // dst to image, 2x3 matrix

    void compute(const cv::Size& from, const cv::Size& to){
        float scale_x = to.width / (float)from.width;
        float scale_y = to.height / (float)from.height;
        float scale = std::min(scale_x, scale_y);
        /* 
                + scale * 0.5 - 0.5 的主要原因是使得中心更加对齐，下采样不明显，但是上采样时就比较明显
            参考：https://www.iteye.com/blog/handspeaker-1545126
        */
        i2d[0] = scale;  i2d[1] = 0;  i2d[2] = -scale * from.width  * 0.5  + to.width * 0.5 + scale * 0.5 - 0.5;
        i2d[3] = 0;  i2d[4] = scale;  i2d[5] = -scale * from.height * 0.5 + to.height * 0.5 + scale * 0.5 - 0.5;
        cv::Mat m2x3_i2d(2, 3, CV_32F, i2d);
        cv::Mat m2x3_d2i(2, 3, CV_32F, d2i);
        cv::invertAffineTransform(m2x3_i2d, m2x3_d2i);
    }

    cv::Mat i2d_mat(){
        return cv::Mat(2, 3, CV_32F, i2d);
    }
};



cv::Mat CvDetection::inference(cv::Mat &img)
{
     // 遍历检测到的目标，可视化，并记录到目标类别
    auto det_objs = engine_->commit(img).get();
    for (auto &obj : det_objs)
    {
        uint8_t b, g, r;
        tie(b, g, r) = random_color(obj.class_label);
        cv::rectangle(img, cv::Point(obj.left, obj.top), cv::Point(obj.right, obj.bottom), cv::Scalar(b, g, r), 5);
        auto name = cocolabels[obj.class_label];
        auto caption = cv::format("%s %.2f", name, obj.confidence);
        int width = cv::getTextSize(caption, 0, 1, 2, nullptr).width + 10;
        cv::rectangle(img, cv::Point(obj.left - 3, obj.top - 33), cv::Point(obj.left + width, obj.top), cv::Scalar(b, g, r), -1);
        cv::putText(img, caption, cv::Point(obj.left, obj.top - 5), 0, 1, cv::Scalar::all(0), 2, 16);
        cv::Rect boundingbox(int(obj.left), int(obj.top), int(obj.right - obj.left), int(obj.bottom - obj.top));
        auto center = cv::Point(int((obj.left + obj.right) / 2), int((obj.top + obj.bottom) / 2));
        cv::circle(img, center, 5, cv::Scalar(b, g, r), -1);
    }    
    return img;
    // INFO("Done, save to detect.out.jpg");
    // cv::imwrite("detect.out.jpg", image);
}


void CvDetection::getEngine(const string& model_file){
    int device_id = 0;
    auto type = SimpleYolo::Type::V5;
    auto mode = SimpleYolo::Mode::FP32;
    SimpleYolo::set_device(device_id);
    float confidence_threshold = 0.50f;
    float nms_threshold = 0.5f;
    engine_ = SimpleYolo::create_infer(model_file, type, device_id, confidence_threshold, nms_threshold);
}

