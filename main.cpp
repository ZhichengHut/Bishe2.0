#include "ExtractData.h"
#include "ReadData.h"
#include "Node.h"
#include "Tree.h"
#include "Data.h"
#include "RandomForest.h"
#include "Evaluate.h"

#include <time.h>


int main(){
	string train_fold = "C:/45 Thesis/data/train/";
	string test_fold = "C:/45 Thesis/data/test/";
	string out_fold = "C:/45 Thesis/data/train/extracted/";
	string out_csv = "C:/45 Thesis/data/train/out_csv.csv";

	float train_thresh = 0.35;
	float test_thresh = 0.35;

	bool get_train = true;
	bool get_test = false;


	cout << "*****************Start to extract sub-image*****************" << endl;
	extractData(train_fold, test_fold, out_fold, out_csv, train_thresh, test_thresh, get_train, get_test);
	cout << "*****************Extraction completed*****************" << endl << endl;

	vector<Mat> imgTrain;
	vector<int> labelTrain;
	//vector<Mat> imgTest;
	//vector<int> labelTest;
	//vector<Mat> imgList_tmp;
	//vector<int> labelList_tmp;

	//readData(imgList, labelList);
	cout << "*****************Start to read training data*****************" << endl;
	readTrainData(out_fold, imgTrain, labelTrain);

	cout << "Sample number = " << imgTrain.size() << endl;
	cout << "*****************Reading completed*****************" << endl << endl;

	Node *nn = new Node(imgTrain, labelTrain, 1);
	cout << "size = " << sizeof(nn) << endl;
	cin.get();

	double start,end,cost;

	for(float i=1; i<=1; i++){
		int window_width = i;

		int tree_num = 30;
		int sample_num = 10000;
		int maxDepth = 20;
		int minLeafSample = 1;
		float minInfo = 0;

		cout << "*****************Start to train the model*****************" << endl;
		start=clock();
		RandomForest *RF = new RandomForest(imgTrain, labelTrain, window_width, tree_num, sample_num, maxDepth, minLeafSample, minInfo);
		RF->train();
		end = clock();
		double train_t = (end - start) / CLOCKS_PER_SEC ;
		cout << "*****************Training completed*****************" << endl << endl;

		cout << "*****************Start to evaluate the performance*****************" << endl;
		start=clock();
		get_predict_result(RF, test_fold);
		end=clock();
		double test_t = (end - start) / CLOCKS_PER_SEC ;
		cout << "*****************Evaluation completed*****************" << endl << endl;

		cout << "*****************Start to calculate F1 score*****************" << endl;
		float F1_score = get_F1_score(test_fold);
		cout << "*****************Calculation completed*****************" << endl << endl;

		ofstream fin("e:\\45 Thesis\\result\\result.csv",ios::app);
		if(!fin){
			cout << "open file error" <<endl; 
			cin.get();
			return 0;
		}

		fin <<",tree num," <<  tree_num << ",sumple num," << sample_num << ",maxDepth," << maxDepth << ",minLeafSample," << minLeafSample << ",minInfo," << minInfo <<",train time," << train_t << ",test time," << test_t <<",window width," << window_width << endl;;
		fin.close();
	}

	cout << "*****************Benchmark completed*****************" << endl;
	cin.get();
}