#include<iostream>
#include<opencv.hpp>
using namespace std;
using namespace cv;
int main()
{
	Mat sample = Mat::zeros(Size(100, 100), CV_8UC3);

	rectangle(sample, Rect(Point(30, 30), Point(70, 70)), Scalar(255, 255, 255), -1);
	imshow("test", sample);
	waitKey();

	return 0;
}