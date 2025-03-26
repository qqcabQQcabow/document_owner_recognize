#ifndef RECOGNIZER
#define RECOGNIZER

#include <cppflow/cppflow.h>
#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
#include <vector>

class Recognizer
{
    private:
        cppflow::model model;
        std::string input_layer_name;
        std::string output_layer_name;
        int img_height;
        int img_width;
        std::vector<int64_t> inp_tensor_shape;
        std::vector<std::string> labels;
    public:
        Recognizer
            (
                const std::string& model_path,
                const std::string& inp,
                const std::string& out,
                const int img_height_,
                const int img_width_,
                const std::vector<int64_t>& inp_tensor_shape_,
                const std::vector<std::string>& labels_names
            );

        std::vector<float> getPredictList(cv::Mat image);


};

#endif //RECOGNIZER
