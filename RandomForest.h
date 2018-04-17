#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include <fstream>

#include "Tree.h"

class RandomForest{
private:
	int window_width;
	int tree_num;
	int sample_num;
	int maxDepth;
	int minLeafSample;
	float minInfoGain;

	vector<Mat> imgData;
	vector<int> LabelData;

	Tree **tree;

public:
	RandomForest(vector<Mat> &img, vector<int> &label, int w_w = 1, int t_n = 1, int s_n = 3000, int maxD=10, int minL=1, float minInfo=0);
	~RandomForest();
	void train();
	vector<int> predict(vector<Mat> test_img);
	float predict(Mat test_img);
};

#endif//RANDOMFOREST_H