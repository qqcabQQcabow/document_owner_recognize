#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#define DEFAULT_THRESH 200

namespace{
    cv::Mat from_file;
    cv::Mat source;
    cv::Mat source_gray;

    int thresh = DEFAULT_THRESH;

    const int max_thresh = 255;

    const char* source_window = "Source image";
    const char* corners_window = "Corners detected";
    void cornerHarris_demo(int, void*);
}

int main(int argc, char** argv)
{
    cv::CommandLineParser parser(argc, argv, "{@input | building.jpg | input image}");
    from_file = cv::imread(cv::samples::findFile(parser.get<cv::String>("@input")));
    if(from_file.empty()){
        std::cout << "Could not open or find the image!\n" << std::endl;
        std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
        return -1;
    }
    
    //cv::resize(from_file, source, cv::Size(), 0.3, 0.4);
    source = from_file;
    cvtColor(source, source_gray, cv::COLOR_BGR2GRAY);
    cv::namedWindow(source_window);

    cv::createTrackbar("Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo);
    cv::setTrackbarPos("Threshold: ", source_window, thresh);

    imshow(source_window, source);
    cornerHarris_demo(0, 0);
    char k = cv::waitKey();
    while(k != 'q'){
        k = cv::waitKey();
    }
    return 0;
}
namespace{
    void cornerHarris_demo(int ph, void* phh)
    {
        std::cout << "------------------------START-------------------------------" << std::endl;

        int blockSize = 5;
        int apertureSize = 3;
        double k = 0.04;

        cv::Mat dst_output = cv::Mat::zeros(source.size(), CV_32FC1);

        cornerHarris(source_gray, dst_output, blockSize, apertureSize, k);

        cv::Mat dst_norm;
        cv::Mat dst_norm_scaled;

        normalize(dst_output, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());

        convertScaleAbs(dst_norm, dst_norm_scaled);

        for(int i = 0; i < dst_norm.rows; i++){
            for(int j = 0; j < dst_norm.cols; j++){

                if((int)dst_norm.at<float>(i,j) > thresh){
                    cv::circle(dst_norm_scaled, cv::Point(j,i), 3, cv::Scalar(1000), 1, 8, 0);
                    std::cout << "Found point of corner " << j << "x" << i << std::endl; 
                }
            }
        }

        cv::namedWindow(corners_window);
        imshow(corners_window, dst_norm_scaled);
        std::cout << "------------------------END-------------------------------" << std::endl;
    }
}
