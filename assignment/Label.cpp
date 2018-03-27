#include"Label.hpp"
Label::Label(int _p = 0, int _l = 0) :pixelNum(_p), labelNum(_l)
{
	corners[0].x = INT_MAX;
	corners[0].y = INT_MAX;

	corners[1].x = 0;
	corners[1].y = 0;

	corners[2].x = INT_MAX;
	corners[2].y = INT_MAX;

	corners[3].x = 0;
	corners[3].y = 0;
}
void Label::setCorners(int x, int y)
{
	if (x <= corners[0].x) {
		corners[0].x = x;
		corners[0].y = y;
	}
	if (x >= corners[1].x) {
		corners[1].x = x;
		corners[1].y = y;
	}
	if (y <= corners[2].y) {
		corners[2].x = x;
		corners[2].y = y;
	}
	if (y >= corners[3].y) {
		corners[3].x = x;
		corners[3].y = y;
	}
}

void Label::setCorner(int x, int y, int i)
{
	if (i >= 4)
		return;
	corners[i].x = x;
	corners[i].y = y;
}

cv::Point Label::getCorner(int i)
{
	return corners[i];
}

bool bigPixel(const Label& lA, const Label& lB)
{
	return lA.pixelNum > lB.pixelNum;
}
