#include<iostream>
#include<algorithm>
#include<opencv.hpp>

#include"Label.hpp"
#include"LabelFactory.hpp"
using namespace std;
using namespace cv;

//#define DEBUG
void IMSHOW(string name, Mat& input)
{
#ifdef DEBUG
	imshow(name, input);
#endif
}
const double PI = 3.14159265;
double distance(const Point& A, const Point& B)
{
	return sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y));
}
Point rotatePoint(const Point& pt, const Point& center, double angle)
{
	Mat trans = Mat::zeros(Size(2, 2), CV_64F);
	trans.ptr<double>(0)[0] = cos(angle);
	trans.ptr<double>(0)[1] = -sin(angle);
	trans.ptr<double>(1)[0] = sin(angle);
	trans.ptr<double>(1)[1] = cos(angle);
	cout << trans << endl;
	cout << cos(angle) << " " << sin(angle) << endl;
	Mat rotate = Mat::zeros(Size(2, 1), CV_64F);
	rotate.ptr<double>(0)[0] = pt.x - center.x;
	rotate.ptr<double>(0)[1] = pt.y -center.y;
	cout << sin(30 * CV_PI / 180)<<endl;
	rotate *= trans;

	Point retV;
	retV.x = rotate.ptr<double>(0)[0] + center.x;
	retV.y = rotate.ptr<double>(0)[1] + center.y;
	return retV;
}
void rotateImg(const Mat& input, Mat& output, Label& labels, Mat* bar)
{
	Point lt = labels.getCorner(0);
	Point rb = labels.getCorner(1);
	Point rt = labels.getCorner(2);
	Point lb = labels.getCorner(3);
	
	Point center = Point((lt.x + rt.x + rb.x + lb.x)/4, (lt.y + rt.y + lb.y + rb.y)/4);	
	Point middle = Point((lt.x + rt.x) / 2, (lt.y + rt.y) / 2);
	double tilt = (double)(lt.y - rt.y) / (double)(lt.x - rt.x);
	Point cross = Point(center.x, tilt*center.x + lt.y - (tilt*lt.x));
	double distanceA = distance(center, middle);
	double distanceB = distance(center, cross);
	int angle = -(acos((double)distanceA/ (double)distanceB) * 180.0 / CV_PI);
	cout << angle << " "<<distanceA<<" "<<distanceB<<endl;

	if (angle != 0.0) {					// 90도가 아닌 경우
		Mat matRotation = getRotationMatrix2D(Point(input.cols / 2, input.rows / 2),angle, 1);
		warpAffine(input, output, matRotation, input.size());
		matRotation = getRotationMatrix2D(Point(bar->cols / 2, bar->rows / 2), angle, 1);
		warpAffine(*bar, *bar, matRotation, bar->size());
	}
	//waitkey();
	if (distance(lb, lt) < distance(rb, lb)) {		// 90도 인경우 종이의 너비가 높이보다 긴 경우 90도를 회전
		angle += 90;
		Mat matRotation = getRotationMatrix2D(Point(output.cols / 2, output.rows / 2), 90, 1);
		warpAffine(output, output, matRotation, output.size());
		matRotation = getRotationMatrix2D(Point(bar->cols / 2, bar->rows / 2), 90, 1);
		warpAffine(*bar, *bar, matRotation, bar->size());
	}

	IMSHOW("rotate", output);
	waitKey();

	/*
	 * 높이가 너비보다 긴 상황에서 종이가 뒤집혀있는지 확인
	 * 종이의 범위에서 절반을 기준으로 구멍이 많은 쪽이 위
	*/
	vector<Label> rolabels = LabelFactory::findLabel(*bar);
	sort(rolabels.begin(), rolabels.end(), bigPixel);
	int top = rolabels[0].bound[2], bottom = rolabels[0].bound[3];

	labels = rolabels[0];

	if (bar != NULL) {
		Mat& img = *bar;
		int count[2] = { 0,0 };
		for (int r = top; r < (top+bottom)/2; ++r) {
			auto imgBar = img.ptr<uchar>(r);
			for (int c = 0; c < img.cols; ++c) {
				if (imgBar[c] == 255)
					++count[0];
			}
		}
		for (int r = (top + bottom) / 2; r < bottom; ++r) {
			auto imgBar = img.ptr<uchar>(r);
			for (int c = 0; c < img.cols; ++c) {
				if (imgBar[c] == 255)
					++count[1];
			}
		}
		cout << count[0] << " " << count[1] << endl;
		if (count[0] < count[1]) {
			Mat matRotation = getRotationMatrix2D(Point(input.cols / 2, input.rows / 2), 180, 1);
			warpAffine(output, output, matRotation, input.size());
			matRotation = getRotationMatrix2D(Point(bar->cols / 2, bar->rows / 2), 180, 1);
			warpAffine(*bar, *bar, matRotation, bar->size());
		}
	}
}

int main()
{
	Mat input = imread("data\\lotto_gray.jpg", IMREAD_COLOR);
	Mat rotto;
	vector<Label> labels;
	cout << "A" << endl;
	// 이미지 사이즈 조정
	Mat resizeImg;
	Size rSize = Size(input.cols / 1, input.rows / 1);
	resize(input, input, rSize);
	rSize = Size(input.cols / 1, input.rows / 1);
	resize(input, resizeImg, rSize);
	cout << "B" << endl;
	
	// 이미지 이진화
	Mat thresholdImg;
	cvtColor(resizeImg, thresholdImg, CV_RGB2GRAY);
	threshold(thresholdImg, thresholdImg, 200, 255, CV_THRESH_BINARY);	
	cout << "C" << endl;

	// 로또 종이 영역 추출
	Mat cornerImg;
	time_t beg, end;
	beg = clock();
	labels = LabelFactory::findLabel(thresholdImg);
	sort(labels.begin(), labels.end(), bigPixel);
	end = clock();	
	cout << "D" << end - beg << endl;

	// 이미지 회전
	Mat rotatedImg;
	beg = clock();
	rotateImg(input, rotatedImg, labels[0], &thresholdImg);
	imshow("rotated", rotatedImg);
	cout << "D" << end - beg << endl;

	// 숫자 영역 추출
	// projection
	Mat projection;
	thresholdImg.copyTo(projection);
	vector<int> rowProjection(projection.cols);
	vector<int> colProjection(projection.rows);

	for (int r = 0; r < projection.rows; ++r) {
		auto projectionBar = projection.ptr<uchar>(r);
		for (int c = 0; c < projection.cols; ++c) {
			if (projectionBar[c] == 255) {
				++colProjection[r];
			}
		}
	}
	bool prevIsBlank = false;
	vector<pair<int, int>> colBound;
	for (int i = 0; i < projection.rows; ++i) {
		if (colProjection[i]>=(labels[0].bound[1]-labels[0].bound[0])*9/10.0){
			if (!prevIsBlank) {
				if (!colBound.empty()) {
					colBound.back().second = i;
					//line(projection, Point(0, i), Point(projection.cols, i), Scalar(0), 1);
				}
			}
			prevIsBlank = true;
		}
		else {
			if (prevIsBlank) {
				colBound.push_back(pair<int, int>(i, 0));
			}
			prevIsBlank = false;
		}
	}
	
	for (int r = colBound[6].first; r <  colBound[10].second; ++r) {
		auto projectionBar = projection.ptr<uchar>(r);
		for (int c = 0; c < projection.cols; ++c) {
			if (projectionBar[c] == 255) {
				++rowProjection[c];
			}
		}
	}
	prevIsBlank = false;
	vector<pair<int, int>> rowBound;
	for (int i = 0; i < projection.cols; ++i) {
		if (rowProjection[i] >= (colBound[10].second - colBound[6].first) / 6.0 * 5) {
			if (!prevIsBlank) {
				if (!rowBound.empty()) {
					rowBound.back().second = i;
				}
			}
			prevIsBlank = true;
		}
		else {
			if (prevIsBlank) {
				rowBound.push_back(pair<int, int>(i, 0));
			}
			prevIsBlank = false;
		}
	}
	
	for (int i = 6; i < 11; ++i) {
		line(projection, Point(0, colBound[i].first), Point(projection.cols - 1, colBound[i].first), Scalar(125), 1);
		line(projection, Point(0, colBound[i].second), Point(projection.cols - 1, colBound[i].second), Scalar(125), 1);
	}
	for (int i = 3; i < rowBound.size(); ++i) {
		line(projection, Point(rowBound[i].first, 0), Point(rowBound[i].first, projection.rows), Scalar(125), 1);
		line(projection, Point(rowBound[i].second, 0), Point(rowBound[i].second, projection.rows), Scalar(125), 1);
	}
	imshow("projection", projection);
	// 숫자 값 학습
 


	while (waitKey() != 27);
}