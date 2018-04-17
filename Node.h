#ifndef NODE_H
#define NODE_H

#include<stdio.h>
#include<vector>
#include<list>
#include <opencv.hpp>

#include <time.h>
#include <math.h>
#include <cmath>
#include<numeric>

using namespace std;
using namespace cv;

class Node{
private:
	int x1;
	int x2;
	int y1;
	int y2;
	int d;
	float theta;
	int voting;
	int sample_num;
	//int threshold;
	bool LeafNode;
	float infoGain;
	float Entro;
	vector<Mat> imgList;
	vector<Mat> leftImg;
	vector<Mat> rightImg;
	vector<int> imgLabel;
	vector<int> leftLabel;
	vector<int> rightLabel;

public:
	Node(vector<Mat> sample, vector<int>label, int w_w = 1);
	~Node();
	inline vector<Mat> get_Left(){return leftImg;};
	inline vector<Mat> get_Right(){return rightImg;};
	inline vector<int> get_Left_Label(){return leftLabel;};
	inline vector<int> get_Right_Label(){return rightLabel;};
	inline void setLeaf(){LeafNode = true;};
	inline bool isLeaf(){return LeafNode;};
	void select_Para();
	//void calculate_infoGain();
	float calculate_entropy(vector<int> label);
	inline float get_infoGain(){return infoGain;};
	void split_Node();
	void release_Vector();
	int predict(Mat test_img);
	void judge(int num_1 = 550, int num_0 = 2616);
	int get_vote();
	inline int getLength(){return imgList.size();};
	//void split_new(vector<Mat> leftImg, vector<Mat>rightImg, vector<int>leftLabel, vector<int>rightLable);
};
#endif//NODE_H