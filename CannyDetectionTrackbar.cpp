#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<Windows.h>

using namespace cv;
using namespace std;

void on_trackbar(int, void*);
void createTrackbars();

int THRESH_MIN = 0;
int THRESH_MAX = 255;
int kerode = 1;
int kdilate = 5;

int main()
{
	createTrackbars();
	on_trackbar(0, 0);

	VideoCapture cap(0);// open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	while (1) // until the Esc key is pressed or webcam connection is lost
	{
		int largest_area = 0;
		int largest_contour_index = 0;
		Rect bounding_rect;

		Mat frame;
		cap.read(frame);

		Mat matGrayscale;		// grayscale of input image
		Mat matBlurred;			// intermediate blured image
		Mat matCanny;			// Canny edge image

		Mat grad_x, grad_y;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

		//		fastNlMeansDenoising(frame, frame, 3, 7, 21);

		Mat thr(frame.rows, frame.cols, CV_8UC1);
		Mat dst(frame.rows, frame.cols, CV_8UC1, Scalar::all(0));

		//		equalizeHist(frame,	frame );

		cvtColor(frame, matGrayscale, CV_BGR2GRAY);		// convert to grayscale

		Scharr(matGrayscale, grad_x, CV_32F, 1, 0);
		Scharr(matGrayscale, grad_y, CV_32F, 0, 1);

		// Calculate overall gradient

		pow(grad_x, 2, grad_x);
		pow(grad_y, 2, grad_y);

		threshold(matGrayscale, matGrayscale, THRESH_MIN, THRESH_MAX, THRESH_OTSU); //Threshold the gray

		GaussianBlur(matGrayscale,			// input image
			matBlurred,							// output image
			Size(5, 5),						// smoothing window width and height in pixels
			1.8);								// sigma value, determines how much the image will be blurred

		GaussianBlur(matBlurred,			// input image
			matBlurred,							// output image
			Size(5, 5),						// smoothing window width and height in pixels
			1.8);								// sigma value, determines how much the image will be blurred

		erode(matBlurred, matBlurred, getStructuringElement(MORPH_RECT, Size(kerode, kerode)));
		dilate(matBlurred, matBlurred, getStructuringElement(MORPH_RECT, Size(kdilate, kdilate)));

		Canny(matBlurred,			// input image
			matCanny,					// output image
			50,							// low threshold
			100);						// high threshold

		vector<vector<Point>> contours; // Vector for storing contour
		vector<Vec4i> hierarchy;

		findContours(matCanny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

		for (int i = 0; i< contours.size(); i++) // iterate through each contour. 
		{
			double a = contourArea(contours[i], false);  //  Find the area of contour
			if (a>largest_area){
				largest_area = a;
				largest_contour_index = i;                //Store the index of largest contour
				bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
			}

		}
		Scalar color(255, 255, 255);
		drawContours(matCanny, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy); // Draw the largest contour using previously stored index.
		rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

		imshow("Video Feed", frame);
		imshow("Largest Contour", matCanny);
	}	// end while

	return(0);
}

void on_trackbar(int, void*)
{//This function gets called whenever a
	// trackbar position is changed
}
void createTrackbars()
{
	String trackbarWindowName = "TrackBars";
	namedWindow(trackbarWindowName, WINDOW_NORMAL);
	createTrackbar("THRESH_MIN", trackbarWindowName, &THRESH_MIN, THRESH_MAX, on_trackbar);
	createTrackbar("THRESH_MAX", trackbarWindowName, &THRESH_MAX, THRESH_MAX, on_trackbar);
	createTrackbar("Erode", trackbarWindowName, &kerode, 31, on_trackbar);
//	createTrackbar("Dilate", trackbarWindowName, &kdilate, 31, on_trackbar);
}