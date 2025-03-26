#include "Recognizer.h"
#include <vector>

Recognizer::Recognizer
(
    const std::string& model_path,
    const std::string& inp,
    const std::string& out,
    const int img_height_,
    const int img_width_,
    const std::vector<int64_t>& inp_tensor_shape_;
    const std::vector<std::string>& labels_names;
)
:
    input_layer_name(inp),
    output_layer_name(out),
    img_height(img_height_),
    img_width(img_width_),
    inp_tensor_shape(inp_tensor_shape_),
    model(model_path),
    labels(labels_name)
{
}

std::vector<float> Recognizer::getPredictList(cv::Mat image)
{
    cv::resize(image, image, cv::Size(img_height, img_width));
    image.convertTo(image, CV_32FC3, 1.0 / 255.0);

    std::vector<float> input_data;
    input_data.assign((float*)image.datastart, (float*)image.dataend);

    cppflow::tensor input_tensor(input_data, inp_tensor_shape);
    std::vector<cppflow::tensor> output = model({{input_layer_name, input_tensor}},{outpu_layer_name});
    
    return output.at(0).get_data<float>();
}
