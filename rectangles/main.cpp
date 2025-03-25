#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <cinttypes>
#include <iostream>
using namespace cv;
using namespace std;
Mat src_gray;
int thresh = 100;
RNG rng(12345);
void thresh_callback(int, void* );
int main( int argc, char** argv )
{
    CommandLineParser parser( argc, argv, "{@input | HappyFish.jpg | input image}" );
    Mat src = imread( samples::findFile( parser.get<String>( "@input" ) ) );
    if( src.empty() )
    {
      cout << "Could not open or find the image!\n" << endl;
      cout << "Usage: " << argv[0] << " <Input image>" << endl;
      return -1;
    }

    const char* source_window = "Source";
    namedWindow( source_window );
    imshow( source_window, src );
    const int max_thresh = 255;
    createTrackbar( "Canny thresh:", source_window, &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );
    char k = waitKey();
    while(k != 'q'){
        k = waitKey();
    }
    return 0;
}
void thresh_callback(int, void* )
{
    Mat canny_output;
    Canny( src_gray, canny_output, thresh, thresh*2 );

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );

    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        vector<Point> aperoxRes;
        approxPolyDP(contours.at(i), aperoxRes, 0.01*arcLength(contours.at(i), true), true);
        if(aperoxRes.size() == 4){
            std::cout << "Find top of rectangle:" << std::endl;
            for(const auto& item : aperoxRes){
                std::cout << "      " << item.x << "x" << item.y << std::endl;
            }
        }
        else{
            std::cout << "This countur is not a rectange" << std::endl;
        }
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }
    imshow( "Contours", drawing );
}
namespace{
    std::vector<cv::Point> getSignatureCoords(const cv::Mat& source)
    {
        std::vector<cv::Point> res;

        cv::cvtColor(source, src_gray, COLOR_BGR2GRAY );
        cv::blur(src_gray, src_gray, Size(3,3) );

        cv::Mat canny_output;
        cv::Canny(src_gray, canny_output, thresh, thresh*2);

        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;

        findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );

        for(size_t i = 0; i< contours.size(); i++)
        {
            std::vector<cv::Point> aperoxRes;
            cv::approxPolyDP(contours.at(i), aperoxRes, 0.01*arcLength(contours.at(i), true), true);

            if(aperoxRes.size() == 4){
                std::cout << "Find top of rectangle:" << std::endl;
                for(const auto& item : aperoxRes){
                    std::cout << "      " << item.x << "x" << item.y << std::endl;
                }
            }
            else{
                std::cout << "This countur is not a rectange" << std::endl;
            }
        }
    }
}
