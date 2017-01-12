#include <opencv2/opencv.hpp>
#include<iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include<Windows.h>

using namespace cv;
using namespace std;

void on_trackbar(int, void*);
void createTrackbars();

int THRESH_MIN = 0;
int THRESH_MAX = 255;

int kerode = 1;
int kdilate = 5;

int objArea = 118026;
int objLength = 1379.31;

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
	createTrackbars();
	on_trackbar(0, 0);

	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	while (1)
	{
		int largest_area = 0;
		int largest_contour_index = 0;
		Rect bounding_rect;

		Mat frame;
		cap.read(frame);

		Mat matGrayscale;
		Mat matBlurred;
		Mat matCanny;

		bool bSuccess = cap.read(frame);

		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

		cvtColor(frame, matGrayscale, CV_BGR2GRAY);

		threshold(matGrayscale, 
			matGrayscale, 
			THRESH_MIN, 
			THRESH_MAX, 
			THRESH_OTSU);

		GaussianBlur(matGrayscale,
			matBlurred,			
			Size(5, 5),			
			1.8);				

		GaussianBlur(matBlurred,
			matBlurred,			
			Size(5, 5),			
			1.8);				

		erode(matBlurred, matBlurred, getStructuringElement(MORPH_RECT, Size(kerode, kerode)));
		dilate(matBlurred, matBlurred, getStructuringElement(MORPH_RECT, Size(kdilate, kdilate)));

		Canny(matBlurred,
			matCanny,	
			50,			
			100);		

		findContours(matCanny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); 

		vector<vector<Point>> contours; 
		vector<Vec4i> hierarchy;
		vector<Point> approx;

		for (int i = 0; i< contours.size(); i++)
		{
			double a = contourArea(contours[i], false);
			if (a>largest_area)
			{
				largest_area = a;
				largest_contour_index = i;                
				bounding_rect = boundingRect(contours[i]);
			}

			approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

			if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
				continue;

			if (approx.size() == 3)
			{
				printf("/////////////////////////////////////////////\n");
				printf("Contour[%d]\n", i);
				printf("Shape = Triangle\n");

				double ptArea;
				double ptLength;
				double rtArea;
				double rxtLength;
				double rytLength;

				ptArea = contourArea(contours[i]);
				rtArea = 603.2246*(ptArea / objArea);
				ptLength = arcLength(contours[i], true);
				rxtLength = 27.94*(ptLength / objLength);
				rytLength = rtArea / (0.5*rxtLength);

				printf("Object Distance = 70 cm\n");
				printf("Pixel Area = %.2f\n", ptArea);
				printf("Real Area = %.2f cm^2\n", rtArea);
				printf("Pixel Length = %.2f\n", ptLength);
				printf("Real Length (x-axis) = %.2f cm\n", rxtLength);
				printf("Real Length (y-axis) = %.2f cm\n", rytLength);
				printf("/////////////////////////////////////////////\n");
			}
			else if (approx.size() >= 4 && approx.size() <= 6)
			{
				int vtc = approx.size();

				vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				sort(cos.begin(), cos.end());

				double mincos = cos.front();
				double maxcos = cos.back();

				if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
				{
					printf("/////////////////////////////////////////////\n");
					printf("Contour[%d]\n", i);
					printf("Shape = Rectangle\n");

					double prArea;
					double prLength;
					double rrArea;
					double rxrLength;
					double ryrLength;

					prArea = contourArea(contours[i]);
					rrArea = 603.2246*(prArea / objArea);
					prLength = arcLength(contours[i], true);
					rxrLength = 27.94*(prLength / objLength);
					ryrLength = rrArea / rxrLength;	//For rectangle

					printf("Object Distance = 70 cm\n");
					printf("Pixel Area = %.2f\n", prArea);
					printf("Real Area = %.2f cm^2\n", rrArea);
					printf("Pixel Length = %.2f\n", prLength);
					printf("Real Length (x-axis) = %.2f cm\n", rxrLength);
					printf("Real Length (y-axis) = %.2f cm\n", ryrLength);
					printf("/////////////////////////////////////////////\n");
				}
			}
			else
			{
				double area = contourArea(contours[i]);
				Rect r = boundingRect(contours[i]);
				int radius = r.width / 2;

				if (abs(1 - ((double)r.width / r.height)) <= 0.2 &&
					abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2)
				{
					printf("/////////////////////////////////////////////\n");
					printf("Contour[%d]\n", i);
					printf("Shape = Circle\n");

					double pcArea;
					double pcLength;
					double rcArea;
					double rxcLength;
					double rycLength;

					pcArea = contourArea(contours[i]);
					rcArea = 603.2246*(pcArea / objArea);
					pcLength = arcLength(contours[i], true);
					rycLength = sqrt(rcArea / 3.1416);	

					printf("Object Distance = 70 cm\n");
					printf("Pixel Area = %.2f\n", pcArea);
					printf("Real Area = %.2f cm^2\n", rcArea);
					printf("Pixel Length = %.2f\n", pcLength);
					printf("Real Length = %.2f cm\n", rycLength);
					printf("/////////////////////////////////////////////\n");
				}
				else
				{
					printf("/////////////////////////////////////////////\n");
					printf("Contour[%d]\n", i);
					printf("Shape = Irregular\n");

					double piArea;
					double piLength;
					double riArea;
					double rxiLength;
					double ryiLength;

					piArea = contourArea(contours[i]);
					riArea = 603.2246*(piArea / objArea);

					printf("Object Distance = 70 cm\n");
					printf("Pixel Area = %.2f\n", piArea);
					printf("Real Area = %.2f cm^2\n", riArea);
					printf("/////////////////////////////////////////////\n");
				}
			}


		}
		Scalar color(255, 255, 255);
		drawContours(matCanny, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy);
		rectangle(frame, bounding_rect, Scalar(0, 255, 0), 1, 8, 0);

		imshow("Video Feed", frame);
		imshow("Largest Contour", matCanny);
	}	

	system("pause");
	return(0);
}

void on_trackbar(int, void*)
{

}
void createTrackbars()
{
	String trackbarWindowName = "TrackBars";
	namedWindow(trackbarWindowName, WINDOW_NORMAL);
	createTrackbar("THRESH_MIN", trackbarWindowName, &THRESH_MIN, THRESH_MAX, on_trackbar);
	createTrackbar("THRESH_MAX", trackbarWindowName, &THRESH_MAX, THRESH_MAX, on_trackbar);
	createTrackbar("Erode", trackbarWindowName, &kerode, 31, on_trackbar);
}