#include<opencv2/opencv.hpp>

#include <iostream>

 

using namespace std;

using namespace cv;

 

int main(int argc , char** argv) {

    Mat image = imread(argv[1]);

    // convert the image to grayscale format
    Mat img_gray;
    cvtColor(image, img_gray, COLOR_BGR2GRAY);


    Mat thresh;
    threshold(img_gray, thresh, 150, 255, THRESH_BINARY);
    imshow("Binary mage", thresh);
    waitKey(0);
    imwrite("image_thres1.jpg", thresh);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(thresh, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    // draw contours on the original image
    Mat image_copy = image.clone();
    drawContours(image_copy, contours, -1, Scalar(0, 255, 0), 2);

    imshow("None approximation", image_copy);
    waitKey(0);
    imwrite("contours_none_image1.jpg", image_copy);

    return 0;
}



