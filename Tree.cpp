#include "Tree.h"

Tree::Tree(vector<Mat> &SP, vector<int> &LB, int w_w, int maxD, int minL, float minInfo){
	window_width = w_w;

	//sample.assign(SP.begin(), SP.end());
	//label.assign(LB.begin(), LB.end());
	sample = SP;
	label = LB;
	num_1 = accumulate(label.begin(), label.end(),0);
	num_0 = label.size() - num_1;


	maxDepth = maxD;
	NodeNum = (long)pow(2.0,maxDepth)-1;
	minLeafSample = minL;
	minInfoGain = minInfo;
	node = new Node*[NodeNum];
	node[0] = new Node(sample, label, window_width);
	for(int i=1; i<NodeNum; i++)
		node[i] = NULL;
}

Tree::~Tree(){
	for(int i=0; i<NodeNum; i++){
		if(node[i] != NULL){
			delete node[i];
			node[i] = NULL;
		}
	}

	delete[] node;
	node = NULL;

	sample.clear();
	vector<Mat>().swap(sample);
	label.clear();
	vector<int>().swap(label);
}

void Tree::train(){
	for(int i=0; i<NodeNum; i++){
		//cout << "*****************************************" <<endl;
		//cout << "i= " << i << endl;
		int parentID = (i-1)/2;
		//cout << "parentID = " << parentID << endl;

		//if parent node is null
		if(node[parentID] == NULL && i != 0){
			//cout << 111 << endl;;
			continue;
		}
		//if parent node is leaf
		if(node[parentID]->isLeaf()){
			//cout << 222 << endl;
			continue;
		}
		//if the left child is out of range
		if(i*2+1>=NodeNum){
			node[i]->setLeaf();
			node[i]->judge();
			//cout << "i = " << i << " node set leaf" << endl;
			continue;
		}

		//cout << "Length = " << node[i]->getLength() << endl;
		//randomly choose the patch parameter
		node[i]->select_Para();
		//split the node according to the patch
		node[i]->split_Node();
		//cout << "left length: " << node[i]->get_Left().size() << " right length: " << node[i]->get_Right().size() << endl;
		//calculate the information gain
		//node[i]->calculate_infoGain();
		//cout << "infoGain = " << node[i]->get_infoGain() << endl;

		if(node[i]->get_infoGain() > minInfoGain){
			node[i*2+1] = new Node(node[i]->get_Left(), node[i]->get_Left_Label(), window_width);
			node[i*2+2] = new Node(node[i]->get_Right(), node[i]->get_Right_Label(), window_width);
			node[i]->release_Vector();
		}
		else{
			//cout << "i = " << i << " node set leaf" << endl;
			node[i]->setLeaf();
			node[i]->judge();
		}
	}
	sample.clear();
	label.clear();
	//cout << "Train completed "<< endl;
}

int Tree::predict(Mat test_img){
	int i=0;
	while(!node[i]->isLeaf())
		//cout << "current node = " << i << endl;
		i = 2*i+node[i]->predict(test_img);
	
	//cout << "use judge of " << i << " ndoe" << endl;
	return node[i]->get_vote();
}
