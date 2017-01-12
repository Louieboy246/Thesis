#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(){
	Mat img_rgb = imread("Multiple Shapes.png");
	Mat img_gry, img_cny, img_pyr, img_pyr2;	// Include img_pyr, img_pyr2
	cvtColor(img_rgb, img_gry, CV_BGR2GRAY);
	pyrDown(img_gry, img_pyr);	// Cuts the size by double
	pyrDown(img_pyr, img_pyr2);	//
	Canny(img_pyr2, img_cny, 10, 100, 3, true);
	namedWindow("Example Gray", WINDOW_AUTOSIZE);
	namedWindow("Example Canny", WINDOW_AUTOSIZE);
	imshow("Example Gray", img_gry);
	//Canny(img_gry, img_cny, 10, 100, 3, true);
	imshow("Example Canny", img_cny);

	int x = 16, y = 32;
	Vec3b intensity = img_rgb.at<Vec3b>(y, x);
	uchar blue = intensity.val[0];
	uchar green = intensity.val[1];
	uchar red = intensity.val[2];
	cout << "At (x,y) = (" << x << "," << y << "): (blue, green, red) = (" << (unsigned int)blue << ". " << (unsigned int)green << "," << (unsigned int)red << ")" << endl;
	cout << "Gray pixel there is: " << (unsigned int)img_gry.at<uchar>(x, y) << endl;

	x /= 4; y /= 4;
	cout << "Pyramid2 pixel there is: " << (unsigned int)img_pyr2.at<uchar>(x, y) << endl;
	img_cny.at<uchar>(x, y) = 128;

	waitKey(0);
}