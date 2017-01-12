#include <opencv2/opencv.hpp>
#include<iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace cv;
using namespace std;

void on_trackbar(int, void*);
void createTrackbars();

int THRESH_MIN = 0;
int THRESH_MAX = 255;

int kerode = 1;
int kdilate = 3;

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

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		findContours(matCanny, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

		vector<Point> approx;

		for (int i = 0; i< contours.size(); i++)
		{
			double a = contourArea(contours[i], false);
			if (a>largest_area)
			{
				largest_area = a;
				largest_contour_index = i;
				bounding_rect = boundingRect(contours[i]);

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

					ptArea = contourArea(contours[i]);
					ptLength = arcLength(contours[i], true);

					printf("Pixel Area = %.2f\n", ptArea);
					printf("Pixel Length = %.2f\n", ptLength);
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

						prArea = contourArea(contours[i]);
						prLength = arcLength(contours[i], true);

						printf("Pixel Area = %.2f\n", prArea);
						printf("Pixel Length = %.2f\n", prLength);
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

						pcArea = contourArea(contours[i]);
						pcLength = arcLength(contours[i], true);

						printf("Pixel Area = %.2f\n", pcArea);
						printf("Pixel Length = %.2f\n", pcLength);
						printf("/////////////////////////////////////////////\n");
					}
					else
					{
						printf("/////////////////////////////////////////////\n");
						printf("Contour[%d]\n", i);
						printf("Shape = Irregular\n");

						double piArea;
						double piLength;

						piArea = contourArea(contours[i]);
						piLength = arcLength(contours[i], true);

						printf("Pixel Area = %.2f\n", piArea);
						printf("Pixel Length = %.2f\n", piLength);
						printf("/////////////////////////////////////////////\n");
					}
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
	namedWindow(trackbarWindowName, WINDOW_AUTOSIZE);
	createTrackbar("Thresh(Min)", trackbarWindowName, &THRESH_MIN, THRESH_MAX, on_trackbar);
	createTrackbar("Thresh(Max)", trackbarWindowName, &THRESH_MAX, THRESH_MAX, on_trackbar);
	createTrackbar("Erode", trackbarWindowName, &kerode, 31, on_trackbar);
}