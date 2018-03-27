#pragma once
#include"opencv.hpp"
#include"Label.hpp"
using namespace std;
using namespace cv;
class LabelFactory {
private:
	static void MakeLabelNet(vector<set<int>>& equivalanceTable, set<int>& net, int j);
	static void MakeLabelIdx(vector<int>& labelIdx, vector<set<int>>& equivalanceTable);
	static void MakeLabel(const Mat& labelImg, vector<int> labelIdx, vector<Label>& labels);
	static int MakeEquivalanceTable(const Mat& input, Mat& labelImg, vector<set<int>>& equivalanceTable);
public:
	static vector<Label> findLabel(const Mat& input);

};