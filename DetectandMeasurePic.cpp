/**
* Simple shape detector program.
* It loads an image and tries to find simple shapes (rectangle, triangle, circle, etc) in it.
* This program is a modified version of `squares.cpp` found in the OpenCV sample dir.
*/
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

using namespace cv;
using namespace std;

/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/
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
	Mat src = imread("Circle.jpg"); //detect-simple-shapes-src-img.png
	if (src.empty())
		return -1;

	// Convert to grayscale
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	Mat bw;
	Canny(gray, bw, 0, 50, 5);

	// Find contours
	vector<vector<Point> > contours;
	findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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
			printf("Triangle\n");    // Triangles
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
				printf("Rectangle\n");
			else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
				printf("Pentagon\n");
			else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
				printf("Hexagon\n");
		}
		else if (approx.size() != (1>6))
		{
			printf("Irregular\n");
		}
		else
		{
			// Detect and label circles
			double area = contourArea(contours[i]);
			Rect r = boundingRect(contours[i]);
			int radius = r.width / 2;

			if (abs(1 - ((double)r.width / r.height)) <= 0.2 &&
				abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2)
				printf("Circle\n");
		}
	}

	imshow("src", src);
	waitKey(0);
	return 0;
}

