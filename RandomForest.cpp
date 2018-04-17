#include "RandomForest.h"


RandomForest::RandomForest(vector<Mat> &img, vector<int> &label, int w_w, int t_n, int s_n, int maxD, int minL, float minInfo){
	window_width = w_w;

	//imgData.assign(img.begin(), img.end());
	//LabelData.assign(label.begin(), label.end());
	imgData = img;
	LabelData = label;
	//cout << "sum: = " << accumulate(LabelData.begin(),LabelData.end(), 0) << endl;

	tree_num = t_n;
	maxDepth = maxD;
	minLeafSample = minL;
	minInfoGain = minInfo;
	tree = new Tree*[tree_num];

	if(s_n > imgData.size()){
		cout << "Sample size out of range, " << imgData.size() << " sample will be used" << endl;
		sample_num = imgData.size();
	}
	else
		sample_num = s_n;
}

RandomForest::~RandomForest(){
	for(int i=0; i<tree_num; i++){
		if(tree[i] != NULL){
			delete tree[i];
			tree[i] = NULL;
		}
	}

	delete[] tree;
	tree = NULL;

	imgData.clear();
	LabelData.clear();
}

void RandomForest::train(){
	srand(unsigned(time(NULL)));

	for(int i=0; i<tree_num; i++){
		cout << "Start to train the " << i << "th tree" << endl;
		
		float sp = 1.0*sample_num/imgData.size();

		vector<Mat> img;
		vector<int> lab;

		for(int i=0; i<imgData.size(); i++){
			if((rand()%10001)/10000.0<=sp){
				img.push_back(imgData[i]);
				lab.push_back(LabelData[i]);
			}
		}

		tree[i] = new Tree(img, lab, window_width, maxDepth, minLeafSample, minInfoGain);
		img.clear();
		lab.clear();
		vector<Mat>().swap(img);
		vector<int>().swap(lab);
		tree[i]->train();
	}
}


vector<int> RandomForest::predict(vector<Mat> test_img){
	//cout << "Start to predict" << endl;
	//cout << "test size = " << test_img.size() << endl;
	vector<int> predict_result;
	for(int i=0; i<test_img.size(); i++){
		int vote = 0;
		for(int j=0; j<tree_num; j++)
			vote += tree[j]->predict(test_img[i]);

		if(vote>0.5*tree_num)
			predict_result.push_back(1);
		else
			predict_result.push_back(0);
	}

	//cout << "predict size = " << predict_result.size() << endl;

	return predict_result;
}

float RandomForest::predict(Mat test_img){
	int vote = 0;
	for(int j=0; j<tree_num; j++)
		vote += tree[j]->predict(test_img);
	
	return 1.0*vote/tree_num;
}