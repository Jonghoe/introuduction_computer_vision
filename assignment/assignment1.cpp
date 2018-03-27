#include<iostream>
#include<algorithm>
#include<opencv.hpp>

#include"Label.hpp"
#include"LabelFactory.hpp"
using namespace std;
using namespace cv;


void rotateImg(const Mat& input, Mat& output, Label& labels, Mat* bar)
{
	Point lt = labels.getCorner(0);
	Point rt = labels.getCorner(1);
	Point lb = labels.getCorner(2);
	Point rb = labels.getCorner(3);
	double angle = atan2((rb.y - lb.y), (rb.x - lb.x)) * 57.2957951;
	if (angle != 0.0) {
		Mat matRotation = getRotationMatrix2D(Point(input.cols / 2, input.rows / 2), 90 - angle, 1);
		warpAffine(input, output, matRotation, input.size());
		matRotation = getRotationMatrix2D(Point(bar->cols / 2, bar->rows / 2), 90 - angle, 1);
		warpAffine(*bar, *bar, matRotation, bar->size());

	}
	else if (lb.y - lt.y < rt.x - lt.x) {
		Mat matRotation = getRotationMatrix2D(Point(input.cols / 2, input.rows / 2), 90, 1);
		warpAffine(input, output, matRotation, input.size());
		matRotation = getRotationMatrix2D(Point(bar->cols / 2, bar->rows / 2), 90, 1);
		warpAffine(*bar, *bar, matRotation, bar->size());
	}

	if (bar != NULL) {
		Mat& img = *bar;
		int count[2] = { 0,0 };
		for (int r = 0; r < img.rows / 2; ++r) {
			auto imgBar = img.ptr<uchar>(r);
			for (int c = 0; c < img.cols; ++c) {
				if (imgBar[c] == 255)
					++count[0];
			}
		}
		for (int r = img.rows / 2; r < img.rows; ++r) {
			auto imgBar = img.ptr<uchar>(r);
			for (int c = 0; c < img.cols; ++c) {
				if (imgBar[c] == 255)
					++count[1];
			}
		}
		if (count[0] < count[1]) {
			Mat matRotation = getRotationMatrix2D(Point(input.cols / 2, input.rows / 2), 180, 1);
			warpAffine(output, output, matRotation, input.size());
		}
	}
}

int main()
{
	Mat input = imread("lotto_gray.jpg", IMREAD_COLOR);
	Mat rotto;
	vector<Label> labels;

	// 이미지 사이즈 조정
	Mat resizeImg;
	Size rSize = Size(input.cols / 2, input.rows / 2);
	resize(input, input, rSize);
	rSize = Size(input.cols / 4, input.rows / 4);
	resize(input, resizeImg, rSize);

	// 이미지 이진화
	Mat thresholdImg;
	cvtColor(resizeImg, thresholdImg, CV_RGB2GRAY);
	threshold(thresholdImg, thresholdImg, 200, 255, CV_THRESH_BINARY);
	imshow("threshold", thresholdImg);

	// 로또 종이 영역 추출
	Mat cornerImg;
	labels = LabelFactory::findLabel(thresholdImg);
	sort(labels.begin(), labels.end(), bigPixel);

	thresholdImg.copyTo(cornerImg);
	for (int i = 0; i < 4; ++i) {
		Point pt = labels[0].getCorner(i);
		circle(cornerImg, pt, 5, Scalar(200), 2);
	}
	imshow("corner", cornerImg);

	// 이미지 회전
	Mat rotatedImg;
	rotateImg(input, rotatedImg, labels[0], &thresholdImg);
	imshow("rotated", rotatedImg);

	// projection
	Mat projection;
	threshold(input, projection, 200, 255, CV_THRESH_BINARY);
	vector<int> rowProjection(projection.cols);
	vector<int> colProjection(projection.rows);
	for (int r = 0; r < projection.rows; ++r) {
		auto projectionBar = projection.ptr<uchar>(r);
		for (int c = 0; c < projection.cols; ++c) {
			if (projectionBar[c] == 255) {
				++colProjection[r];
				++rowProjection[c];
			}
		}
	}
	// 숫자 영역 추출


	// 숫자 값 학습

	while (waitKey() != 27);
}