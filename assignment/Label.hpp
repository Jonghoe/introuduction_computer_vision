#pragma once
#include<opencv.hpp>
class Label {
public:
	explicit Label(int _p , int _l);
	void setCorners(int x, int y);
	void setCorner(int x, int y, int i);
	void Label::setCorner(cv::Point pt, int i);

	cv::Point getCorner(int i);


private:
	cv::Point corners[4];
public:
	int bound[4];
	int pixelNum;
	int labelNum;
	friend bool bigPixel(const Label& lA, const Label& lB);
	Label& operator = (Label& input);
};