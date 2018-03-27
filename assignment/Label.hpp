#pragma once
#include<opencv.hpp>
class Label {
public:
	explicit Label(int _p = 0, int _l = 0);
	void setCorners(int x, int y);
	void setCorner(int x, int y, int i);
	cv::Point getCorner(int i);

private:
	cv::Point corners[4];
	int bound[4];
public:
	int pixelNum;
	int labelNum;
	friend bool bigPixel(const Label& lA, const Label& lB);
};