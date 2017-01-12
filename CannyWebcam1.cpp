// CannyWebcam.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

using namespace cv;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1) // until the Esc key is pressed or webcam connection is lost

	{

		Mat frame;
		cap.read(frame);

		Mat matOriginal;		// input image
		Mat matGrayscale;		// grayscale of input image
		Mat matBlurred;			// intermediate blured image
		Mat matCanny;			// Canny edge image

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		imshow("MyVideo", frame); //show the frame in "MyVideo" window

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

//		Mat thr(frame.rows, frame.cols, CV_8UC1);
//		Mat dst(frame.rows, frame.cols, CV_8UC1, Scalar::all(0));

		cvtColor(frame, matGrayscale, CV_BGR2GRAY);		// convert to grayscale

		threshold(matGrayscale, matGrayscale, 25, 255, THRESH_BINARY); //Threshold the gray

		GaussianBlur(matGrayscale,			// input image
			matBlurred,							// output image
			Size(5, 5),						// smoothing window width and height in pixels
			1.8);								// sigma value, determines how much the image will be blurred

		GaussianBlur(matBlurred,			// input image
			matBlurred,							// output image
			Size(5, 5),						// smoothing window width and height in pixels
			1.8);								// sigma value, determines how much the image will be blurred

		Canny(matBlurred,			// input image
			matCanny,					// output image
			50,							// low threshold
			100);						// high threshold

		// declare windows
		namedWindow("Canny", CV_WINDOW_NORMAL);		// or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
		// CV_WINDOW_AUTOSIZE is the default
		imshow("Canny", matCanny);

	}	// end while

	return(0);
}
