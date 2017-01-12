// CannyWebcam.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
RNG rng(12345);
Mat matOriginal;

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int main()
{
	VideoCapture cap(0);// open the video camera no. 0

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

		cvtColor(frame, matGrayscale, CV_BGR2GRAY);		// convert to grayscale

		GaussianBlur(matGrayscale,			// input image
			matBlurred,							// output image
			Size(5, 5),						// smoothing window width and height in pixels
			1.8);								// sigma value, determines how much the image will be blurred

		Canny(matBlurred,			// input image
			matCanny,					// output image
			50,							// low threshold
			100);						// high threshold

		// declare windows
		namedWindow("Canny", CV_WINDOW_AUTOSIZE);
		imshow("Canny", matCanny);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		/// Find contours
		findContours(matCanny, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		/// Get the moments
		vector<Moments> mu(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
		}

		///  Get the mass centers:
		vector<Point2f> mc(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		}

		/// Draw contours
		Mat drawing = Mat::zeros(matCanny.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			circle(drawing, mc[i], 4, color, -1, 8, 0);
		}

		/// Show in a window
		namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		imshow("Contours", drawing);

		/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
		printf("\t Info: Area and Contour Length \n");

		vector<Point> approx;

		for (int i = 0; i < contours.size(); i++)
		{
			// Approximate contour with accuracy proportional
			// to the contour perimeter
			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			// Skip small or non-convex objects 
			if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
				continue;

			if (approx.size() == 3)
			{
				printf("/////////////////////////////////////////////\n");
				printf("Contour[%d]\n", i);
				printf("Shape = Triangle\n");
			}
			else if (approx.size() >= 4 && approx.size() <= 6)
			{
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				// Sort ascending the cosine values
				sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
					printf("/////////////////////////////////////////////\n");
				printf("Contour[%d]\n", i);
				printf("Shape = Rectangle\n");
			}
			else
			{
				// Detect and label circles
				double area = contourArea(contours[i]);
				Rect r = boundingRect(contours[i]);
				int radius = r.width / 2;

				if (abs(1 - ((double)r.width / r.height)) <= 0.2 &&
					abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2)
					printf("/////////////////////////////////////////////\n");
				printf("Contour[%d]\n", i);
				printf("Shape = Circle\n");
			}
		}
	}

	return(0);
}
