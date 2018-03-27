#include "LabelFactory.hpp"

void LabelFactory::MakeLabelNet(vector<set<int>>& equivalanceTable, set<int>& net, int j)
{
	net.insert(j);
	for (auto it = equivalanceTable[j].begin(); it != equivalanceTable[j].end(); ++it) {
		if (net.find(*it) == net.end())
		{
			MakeLabelNet(equivalanceTable, net, *it);
		}
	}
}

void LabelFactory::MakeLabelIdx(vector<int>& labelIdx, vector<set<int>>& equivalanceTable)
{
	vector<set<int>> net;
	for (int i = 0; i < equivalanceTable.size(); ++i) {
		set<int>::iterator it;
		int j;

		for (j = 0; j < net.size(); ++j) {
			it = find(net[j].begin(), net[j].end(), i);
			if (it != net[j].end())
				break;
		}
		if (j == net.size()) {
			net.push_back(set<int>());
			MakeLabelNet(equivalanceTable, net.back(), j);
		}
		labelIdx[i] = j;
	}
}

void LabelFactory::MakeLabel(const Mat& labelImg, vector<int> labelIdx, vector<Label>& labels)
{
	for (int r = 0; r < labelImg.rows; ++r) {
		auto bar = labelImg.ptr<int>(r);
		for (int c = 0; c < labelImg.cols; ++c) {
			if (bar[c] != 0) {
				int labelID = labelIdx[bar[c]];

				int i;
				for (i = 0; i < labels.size(); ++i)
					if (labels[i].labelNum == labelID)
						break;

				if (i == labels.size()) {
					labels.push_back(Label(0, labelID));
					i = labels.size() - 1;
				}

				labels[i].setCorners(c, r);
				++labels[i].pixelNum;
			}
		}
	}
}
int LabelFactory::MakeEquivalanceTable(const Mat& input, Mat& labelImg, vector<set<int>>& equivalanceTable)
{
	int labelNum = 1;
	for (int r = 1; r < input.rows - 1; ++r) {
		auto imgBar = input.ptr<uchar>(r);
		auto labelBar = labelImg.ptr<int>(r);
		for (int c = 1; c < input.cols - 1; ++c) {
			if (imgBar[c] == 255) {
				int upVal = labelImg.ptr<int>(r - 1)[c];
				int leftVal = labelImg.ptr<int>(r)[c - 1];
				if (upVal != 0 && leftVal != 0) {
					labelBar[c] = upVal;
					if (upVal != leftVal) {
						equivalanceTable[upVal].insert(leftVal);
						equivalanceTable[leftVal].insert(upVal);
					}
				}
				else if (upVal != 0) {
					labelBar[c] = upVal;
				}
				else if (leftVal != 0) {
					labelBar[c] = leftVal;
				}
				else {
					if (equivalanceTable.size() <= labelNum) {
						equivalanceTable.push_back(set<int>());
					}
					labelBar[c] = labelNum++;
				}
			}
		}
	}
	return labelNum;
}


vector<Label> LabelFactory::findLabel(const Mat& input)
{
	Mat labelImg = Mat::zeros(input.size(), CV_32S);
	vector<set<int>> equivalanceTable(2);

	//equivalance table 생성 및 레이블링
	int labelNum = LabelFactory::MakeEquivalanceTable(input, labelImg, equivalanceTable);

	vector<int> labelIdx(labelNum);

	LabelFactory::MakeLabelIdx(labelIdx, equivalanceTable);

	vector<Label> labels;
	LabelFactory::MakeLabel(labelImg, labelIdx, labels);

	return labels;
}