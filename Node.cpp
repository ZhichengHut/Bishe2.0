#include "Node.h"

Node::Node(vector<Mat> sample, vector<int> label, int w_w){	
	imgList.assign(sample.begin(), sample.end());
	imgLabel.assign(label.begin(), label.end());
	sample_num = imgList.size();
	LeafNode = false;
	infoGain = 0;
	Entro = calculate_entropy(imgLabel);
	theta = 0;
	d = w_w;

	voting = 2;
	//cout << "Entro = " << Entro << endl;
}

Node::~Node(){
	imgList.clear();
	imgLabel.clear();
	leftImg.clear();
	rightImg.clear();
	leftLabel.clear();
	rightLabel.clear();
	vector<Mat>().swap(imgList);
	vector<Mat>().swap(leftImg);
	vector<Mat>().swap(rightImg);
	vector<int>().swap(imgLabel);
	vector<int>().swap(leftLabel);
	vector<int>().swap(rightLabel);
}


void Node::select_Para(){
	srand (time(NULL));
	d = rand() % (min(imgList[0].cols, imgList[0].rows)) + 1;
	//d = 1;
	x1 = rand() % (imgList[0].cols-d+1);
	x2 = rand() % (imgList[0].cols-d+1);
	y1 = rand() % (imgList[0].rows-d+1);
	y2 = rand() % (imgList[0].rows-d+1);

	theta = 0.0;
	//cout << "location: " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << d << endl;

	//threshold = rand() % 256;
	//threshold = 140;
}

void Node::split_Node(){
	//cout << "element # = " << imgList.size() << endl;
	int pppkkk = accumulate(imgLabel.begin() , imgLabel.end() , 0);
	//cout << "#1 = " << pppkkk << endl;
	if(pppkkk == 0 || pppkkk==imgLabel.size())
		return;

	srand (time(NULL));

	int r = imgList[0].rows;
	int c = imgList[0].cols;

	for(int i=0; i<100; i++){
		int d_tmp = rand() % (min(imgList[0].cols, imgList[0].rows)) + 1;
		//int d_tmp = d;
		int x1_tmp = rand() % (c-d_tmp+1);
		int y1_tmp = rand() % (r-d_tmp+1);
		int x2_tmp = rand() % (c-d_tmp+1);
		int y2_tmp = rand() % (r-d_tmp+1);

		//cout << "tmp : " << x1_tmp << " " << y1_tmp << " "  << x2_tmp << " "  << y2_tmp << " "  << " " << d_tmp << endl;

		//vector<Mat> leftImg_tmp, rightImg_tmp;
		vector<int> leftLabel_tmp, rightLabel_tmp;
		//leftLabel_tmp.resize(imgLabel.size());
		//rightLabel_tmp.resize(imgLabel.size());
		leftLabel_tmp.reserve(imgLabel.capacity());
		rightLabel_tmp.reserve(imgLabel.capacity());
		//cout << "tmp size: " << leftLabel_tmp.size() << " " << rightLabel_tmp.size() << endl;
		//cin.get();

		int ss_index1 = rand() % imgLabel.size();
		float theta_tmp = mean(imgList[ss_index1](Rect(x1_tmp,y1_tmp,d_tmp,d_tmp)))[0] - mean(imgList[ss_index1](Rect(x2_tmp,y2_tmp,d_tmp,d_tmp)))[0];
		while(true){
			int ss_index2 = rand() % imgLabel.size();
			if(imgLabel[ss_index1] + imgLabel[ss_index2] == 1){
				theta_tmp += (mean(imgList[ss_index2](Rect(x1_tmp,y1_tmp,d_tmp,d_tmp)))[0] - mean(imgList[ss_index2](Rect(x2_tmp,y2_tmp,d_tmp,d_tmp)))[0]);
				theta_tmp /= 2.0;
				break;
			}
		}

		for(int p=0; p<imgList.size(); p++){
			//cout << "img size " << imgList[p].cols <<" " << imgList[p].rows << endl;

			float mean1 = mean(imgList[p](Rect(x1_tmp,y1_tmp,d_tmp,d_tmp)))[0];
			//cout << 11 << endl;
			float mean2 = mean(imgList[p](Rect(x2_tmp,y2_tmp,d_tmp,d_tmp)))[0];
			//cout << 22 << endl;

			if(mean1-mean2>theta_tmp){
				//leftImg_tmp.push_back(imgList[p]);
				leftLabel_tmp.push_back(imgLabel[p]);
			}
			else{
				//rightImg_tmp.push_back(imgList[p]);
				rightLabel_tmp.push_back(imgLabel[p]);
			}
		}

		//cout << "entro = " << Entro <<", length = " << imgLabel.size() << ", 1= " << accumulate(imgLabel.begin(), imgLabel.end(),0) << endl;
		//cout << "left E = " << calculate_entropy(leftLabel_tmp) << ", left length = " << leftLabel_tmp.size() << ", 1 = " <<accumulate(leftLabel_tmp.begin(), leftLabel_tmp.end(),0) << endl;
		//cout << "right E = " << calculate_entropy(rightLabel_tmp) << ", right length = " << rightLabel_tmp.size() << ", 1 = " <<accumulate(rightLabel_tmp.begin(), rightLabel_tmp.end(),0) << endl;
			

		float infoGain_new = Entro - (leftLabel_tmp.size()*calculate_entropy(leftLabel_tmp) + rightLabel_tmp.size()*calculate_entropy(rightLabel_tmp))/imgList.size();
		
		//cout << "new gain = " << infoGain_new << endl;
		//cin.get();

		if(infoGain_new > infoGain){	
			//cout << "tmp : " << x1_tmp << " " << y1_tmp << " "  << x2_tmp << " "  << y2_tmp << " "  << " " << d << endl;
			infoGain = infoGain_new;
			x1 = x1_tmp;
			x2 = x2_tmp;
			y1 = y1_tmp;
			y2 = y2_tmp;
			d = d_tmp;
			theta = theta_tmp;
			
			//cout << "new gain = " << infoGain << endl;
			
		}
		//cout << "tmp size: " << leftLabel_tmp.size() << " " << rightLabel_tmp.size() << endl;
	}

	//cout << "new gain = " << infoGain << endl;

	/*vector<Mat>().swap(leftImg);
	vector<Mat>().swap(rightImg);
	vector<int>().swap(leftLabel);
	vector<int>().swap(rightLabel);*/

	leftImg.clear();
	rightImg.clear();
	leftLabel.clear();
	rightLabel.clear();

	//cout << "split node " << endl;
	for(int p=0; p<imgList.size(); p++){
		//cout << "location: " << x1 << " " << y1 << " " << x2 << " " << y2 << " " << d << endl;
		float mean1 = mean(imgList[p](Rect(x1,y1,d,d)))[0];
		//cout << 33 << endl;
		float mean2 = mean(imgList[p](Rect(x2,y2,d,d)))[0];
		//cout << 44 << endl;
		if(mean1-mean2>theta){
			leftImg.push_back(imgList[p]);
			leftLabel.push_back(imgLabel[p]);
		}
		else{
			rightImg.push_back(imgList[p]);
			rightLabel.push_back(imgLabel[p]);
		}
	}
}

void Node::release_Vector(){
	imgList.clear();
	imgLabel.clear();
	leftImg.clear();
	imgLabel.clear();
	leftLabel.clear();
	rightLabel.clear();
}

int Node::predict(Mat test_img){
	//cout << "node predict" << endl; 
	//cout << "x1=" << x1 << "y1=" << y1 << "x2=" << x2 << "y2=" << y2 << "d=" << d << endl;
	float mean1 = mean(test_img(Rect(x1,y1,d,d)))[0];
	float mean2 = mean(test_img(Rect(x2,y2,d,d)))[0];
	if(mean1-mean2>theta){
		//cout << "left node" << endl;
		return 1;
	}
	else{
		//cout << "right node" << endl;
		return 2;
	}
}

void Node::judge(int num_1, int num_0){
	//float p_1 = 1.0 * accumulate(imgLabel.begin(), imgLabel.end(),0) / num_1;
	//float p_2 = 1.0 * (imgLabel.size()-accumulate(imgLabel.begin(), imgLabel.end(),0)) / num_0;

	int p_1 = 1.0 * accumulate(imgLabel.begin(), imgLabel.end(),0);
	int p_2 = imgLabel.size() - p_1;

	if(p_2 > p_1)
		voting = 0;
	else
		voting = 1;

	release_Vector();
}

int Node::get_vote(){
	if(voting != 0 && voting != 1){
		cout << "vote error" << endl;
		cin.get();
	}
	else
		return voting;
}

float Node::calculate_entropy(vector<int> label){
	float entropy = 0;

	if(label.size() != 0){
		int class_1 = accumulate(label.begin(), label.end(),0);
		float pp = 1.0 * class_1 / label.size();						//positive%
		float np = 1.0 * (label.size() - class_1) / label.size();		//negtive%

		if(pp!=0 && np !=0)
			entropy = -1.0*pp*log(1.0*pp)/log(2.0) - 1.0*np*log(1.0*np)/log(2.0);
	}
	return entropy;
}