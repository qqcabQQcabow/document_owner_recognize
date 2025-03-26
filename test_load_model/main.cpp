#include <iostream>
#include <cppflow/cppflow.h>
#include <opencv4/opencv2/opencv.hpp>

const int img_width = 128;
const int img_height = 128;


int main(int args, char* argv[])
{
    std::string model_path = "best_model.tf";
    std::string image_path = "image.jpg";
    int indx = 0;
    while(indx < args){

        if(std::string(argv[indx]) == "--model-path"){
            if((indx + 1) < args){
                model_path = std::string(argv[indx+1]);
                indx++;
            }
        }
        else if(std::string(argv[indx]) == "--image-path"){
            if((indx + 1) < args){
                image_path = std::string(argv[indx+1]);
                indx++;
            }
        }

        indx++;
    }

    
    // Load the model
    cppflow::model model(model_path);
    
    std::cout << "Start process image" << std::endl;

    cv::Mat image = cv::imread(image_path);
    cv::resize(image, image, cv::Size(img_height, img_width));
    image.convertTo(image, CV_32FC3, 1.0 / 255.0);

    std::cout << "Success process image" << std::endl;

    std::vector<float> input_data;
    input_data.assign((float*)image.datastart, (float*)image.dataend);

    std::cout << "Start create input data" << std::endl;

    cppflow::tensor input_tensor(input_data, {1, 128, 128, 3});

    std::cout << "Success create input data" << std::endl;

    std::cout << "Run model" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    auto output = model({{"serving_default_xception_input:0", input_tensor}},{"StatefulPartitionedCall:0"});

    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Exe model time is " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() << "ns\n";

    auto& output_tensor = output.at(0);
    auto output_data = output_tensor.get_data<float>();

    for(float val : output_data){
        std::cout << val << " " ;
    }
    std::cout << std::endl;
    std::cout << "Use model: " << model_path << std::endl;
    std::cout << "Use image: " << image_path << std::endl;
    return 0;

}

