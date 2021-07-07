#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void his(const cv::Mat &src , cv::Mat &dst ){
	Mat sec_gray ;
    cvtColor( src, sec_gray, COLOR_BGR2GRAY );
    
    equalizeHist( sec_gray, dst );

	imwrite( "his.jpg", dst );
}

void edge_detection(const cv::Mat &src , cv::Mat &threshold_dst ){
	/// Remove noise by blurring with a Gaussian filter
	Mat src_smooth , sec_gray , dst , abs_dst , src_gray ;
	//GaussianBlur(src, src_smooth, Size(9, 9), 0, 0, BORDER_DEFAULT);
	medianBlur ( src, src_smooth, 5 );
	// Convert the image to grayscale
	cvtColor(src_smooth, sec_gray, COLOR_RGB2GRAY);

	//his(src , src_smooth);
	//medianBlur ( src_smooth, sec_gray, 13 );
	/// Apply Laplace function
	Laplacian(sec_gray, dst, CV_16S, 1, 1, 0, BORDER_DEFAULT);

	convertScaleAbs(dst, abs_dst, 1, 0);

	cvtColor(abs_dst, threshold_dst, COLOR_BGR2RGB);
}
// tool -> print img type
void type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch ( depth ) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans+'0');

	cout << (" %s \n", r.c_str()) << endl;
}

void extract_by_mask(const cv::Mat &image, const cv::Mat &edge , cv::Mat &img1 , cv::Mat &img2){
	cv::Mat markers ;
	cv::Mat markers_img = edge.clone();

	Mat img_gray;
    cvtColor(markers_img, img_gray, COLOR_BGR2GRAY);
    Mat thresh;
    threshold(img_gray, thresh, 10, 255, THRESH_BINARY);

	vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(thresh, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
    // draw contours on the original image
    Mat image_copy = image.clone();
    drawContours(image_copy, contours, -1, Scalar(0, 0, 0), -1);
	imshow("drawContours", image_copy);

	// obtain 2 mask
	Mat extract_mask , extract_mask_not;
	cvtColor(image_copy, img_gray, COLOR_BGR2GRAY);
	threshold(img_gray, extract_mask, 10, 255, THRESH_BINARY);
	bitwise_not(extract_mask , extract_mask_not);
	cvtColor(extract_mask, extract_mask, COLOR_GRAY2RGB);
	cvtColor(extract_mask_not, extract_mask_not, COLOR_GRAY2RGB);

	imwrite( "mask.jpg", extract_mask );
    imwrite( "mask_not.jpg" , extract_mask_not );
	
	// separate image
	bitwise_and(image , extract_mask , img1);
	bitwise_and(image , extract_mask_not , img2);

	imshow("img1", img1);
	imshow("img2", img2);
}

void Dilation(const cv::Mat &image, cv::Mat &dilation_dst){

	int dilation_elem = 2;
	int dilation_size = 2;

	int dilation_type = 0;
	if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
	Mat element = getStructuringElement( dilation_type,
						Size( 2*dilation_size + 1, 2*dilation_size+1 ),
						Point( dilation_size, dilation_size ) );
	dilate( image, dilation_dst, element );
	imwrite( "Dilation.jpg", dilation_dst );
}

void extract_by_Point(const cv::Mat &image, const cv::Mat &edge , cv::Mat &img1 , cv::Mat &img2){
	cv::Mat markers ;
	cv::Mat markers_img = edge.clone();

	Mat img_gray;
    cvtColor(markers_img, img_gray, COLOR_BGR2GRAY);
    Mat thresh , dilation;
    threshold(img_gray, thresh, 8, 255, THRESH_BINARY);
	Dilation(thresh ,dilation );
	imwrite( "contour.jpg ", thresh);
	vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(dilation, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	int largest_area=0;
    int largest_contour_index=0;
	Mat image_copy = image.clone();
	int roi_count = 0;
	for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.
    {
        double area = contourArea(contours[i]);  //  Find the area of contour

		if(area < image.size().width * image.size().height / 50)
			continue;

		Rect bounding_rect = boundingRect(contours[i]);
		Mat rez = image_copy(bounding_rect);
		char name[100] = {'\0'};
		sprintf( name , "ROI_%d.jpg" , roi_count++);
		imwrite( name, rez);
    }
}

int main(int argc, char** argv)
{
	cv::Mat src, edge , dst1 , dst2;

	char filename_lap[100], filename_1[100], filename_2[100], filename_output[100];
	string filename;

	/// Load an image
	src = imread(argv[1]);

	filename = argv[1];
	unsigned dot = filename.find_last_of(".");
	sprintf(filename_lap, "%s_edge.jpg", filename.substr(0, dot).c_str());
	sprintf(filename_1, "%s_subimg1.jpg", filename.substr(0, dot).c_str());
	sprintf(filename_2, "%s_subimg2.jpg", filename.substr(0, dot).c_str());

	if (!src.data)
	{
		return -1;
	}

	edge_detection(src , edge);

	extract_by_Point(src , edge , dst1 , dst2);

	/*imwrite( filename_lap, edge );
	imwrite( filename_1, dst1 );
    imwrite( filename_2, dst2 );*/
    

	waitKey(0);

	return 0;
}