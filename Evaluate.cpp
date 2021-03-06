#include "Evaluate.h"


void get_predict_result(RandomForest *RF, string test_fold){
    char curDir[100];

    for(int c=10; c<=12; c++){
		list<Mat> imgList;
		sprintf(curDir, "%s%02i", test_fold.c_str(), c);
		cout << curDir << endl;

		DIR* pDIR;
		struct dirent *entry;
		struct stat s;

		stat(curDir,&s);

		// if path is a directory
		if ( (s.st_mode & S_IFMT ) == S_IFDIR ){
			if(pDIR=opendir(curDir)){
				//for all entries in directory
				while(entry = readdir(pDIR)){
					stat((curDir + string("/") + string(entry->d_name)).c_str(),&s);
					if (((s.st_mode & S_IFMT ) != S_IFDIR ) && ((s.st_mode & S_IFMT) == S_IFREG )){
						if(string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) == "tif"){
							//find the corresponding fold according to the image name
							string sub_curDIR = string(curDir) + "/" + string(entry->d_name).substr(0,2);
							
							vector<Mat> imgTest;
							vector<int> X,Y;

							DIR* subDIR;
							struct dirent *sub_entry;
							struct stat sub_s;

							stat(sub_curDIR.c_str(), &sub_s);

							/////////extract the test image and their position
							if((sub_s.st_mode & S_IFMT) == S_IFDIR ){
								if(subDIR=opendir(sub_curDIR.c_str())){
									while(sub_entry = readdir(subDIR)){
										stat((sub_curDIR + string("/") + string(sub_entry->d_name)).c_str(),&sub_s);
										if (((sub_s.st_mode & S_IFMT ) != S_IFDIR ) && ((sub_s.st_mode & S_IFMT) == S_IFREG )){
											if(string(sub_entry->d_name).substr(string(sub_entry->d_name).find_last_of('.') + 1) == "png"){
												Mat img_tmp = imread(sub_curDIR + string("/") + string(sub_entry->d_name), 1);
												int x = atoi(string(sub_entry->d_name).substr(0,4).c_str());
												int y = atoi(string(sub_entry->d_name).substr(5,4).c_str());
												imgTest.push_back(img_tmp);
												X.push_back(x);
												Y.push_back(y);
											}
										}
									}
								}
							}
							vector<int> result = RF->predict(imgTest);
							string csv_name = string(curDir) + "/" + string(entry->d_name).substr(0,2) + "_predict.csv";
							ofstream fout(csv_name);
							for(int i=0; i<result.size(); i++){
								if(result[i] == 1)
									fout << Y[i] << "," << X[i] << endl;
							}
							fout.close();
						}
					}
				}
			}
		}
	}
}

void get_predict_result(RandomForest *RF, string test_fold, int width, int sample_interval, float prob_threshold){
    char curDir[100];

    for(int c=10; c<=12; c++){
		sprintf(curDir, "%s%02i", test_fold.c_str(), c);
		cout << curDir << endl;

		DIR* pDIR;
		struct dirent *entry;
		struct stat s;

		stat(curDir,&s);

		// if path is a directory
		if ( (s.st_mode & S_IFMT ) == S_IFDIR ){
			if(pDIR=opendir(curDir)){
				//for all entries in directory
				while(entry = readdir(pDIR)){
					stat((curDir + string("/") + string(entry->d_name)).c_str(),&s);
					if (((s.st_mode & S_IFMT ) != S_IFDIR ) && ((s.st_mode & S_IFMT) == S_IFREG )){
						if(string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) == "tif"){
							//find the corresponding fold according to the image name
							string cur_img = string(curDir) + "/" + string(entry->d_name);
							cout << "current img: " << cur_img << endl;
							
							Mat imgTest = imread(cur_img,0);

							vector<float> result;

							for(int x=0; x<=imgTest.cols-2*width; x+=sample_interval){
								for(int y=0; y<=imgTest.rows-2*width; y+=sample_interval){
									result.push_back(RF->predict(imgTest(Rect(x,y,2*width,2*width))));
								}
							}

							int m = sqrt(result.size()*1.0);
							//cout << "test size: " << result.size() << endl;
							//cout << "m: " << m << endl;

							Mat heat_map_tmp = Mat::zeros(m,m,CV_32FC1);
							for(int j=0; j<m; j++){
								for(int i=0; i<m; i++){
									heat_map_tmp.at<float>(i,j) = result[j*m+i];
								}
							}

							//imshow("heat_map_orginal", heat_map_tmp);

							Mat heat_map_tmp2 = Mat::zeros(imgTest.cols-2*width,imgTest.rows-2*width,CV_32FC1);
							resize(heat_map_tmp,heat_map_tmp2,Size(imgTest.cols-2*width,imgTest.rows-2*width),0,0,INTER_LINEAR);

							Mat heat_map = Mat::zeros(2000,2000,CV_32FC1);
							heat_map_tmp2.copyTo(heat_map(Rect(width,width,heat_map_tmp2.cols,heat_map_tmp2.rows)));

							threshold(heat_map, heat_map, prob_threshold,255 ,THRESH_BINARY);

							//Mat kernel = Mat::ones(3,3,CV_32FC1);
							//int iteration = 3;
							
							/// Apply the close operation
							//morphologyEx(heat_map,heat_map, MORPH_CLOSE, kernel, Point(-1,-1), iteration);

							imwrite(curDir + string("/heat.png"),heat_map);
							//imshow("heat map", heat_map);
							//waitKey(0);
							heat_map = imread(curDir + string("/heat.png"),0);
							remove((curDir + string("/heat.png")).c_str());

							int R = 10;
							vector<Point2i> center = getCenter(heat_map, R);
							int cell_num = center.size();
							cout << "mitosis number: " << cell_num << endl;

							string csv_name = string(curDir) + "/" + string(entry->d_name).substr(0,2) + "_predict.csv";
							ofstream fout(csv_name);
							for(int i=0; i< cell_num; i++){
								int x = center[i].x;
								int y = center[i].y;

								/*circle(heat_map,cvPoint(x,y),10,CV_RGB(255,255,255),2,8,0);
								circle(imgTest,cvPoint(x,y),10,CV_RGB(0,255,255),2,8,0);*/
								
								fout << y << "," <<  x << endl;
								y = y>=1970?1969:y;
								y = y<=30?30:y;
								x = x>=1970?1969:x;
								x = x<=30?30:x;

								//imshow("extracted",imgTest(Rect(x-width,y-width,2*width,2*width)));
								//waitKey(0);
							}

							/*imshow("heat", heat_map);
							imshow("test", imgTest);
							waitKey(0);

							imwrite(curDir + string("/heat.png"),heat_map);
							imwrite(curDir + string("/test.png"),imgTest);
							cout << "To see the detected area" << endl;
							cin.get();
							remove((curDir + string("/heat.png")).c_str());
							remove((curDir + string("/test.png")).c_str());*/

							center.clear();
							vector<Point2i>().swap(center);
							result.clear();
							vector<float>().swap(result);
							fout.close();
						}
					}
				}
			}
		}
	}
}

float get_F1_score(string test_fold){
	int TP = 0;
	int FP = 0;
	int FN = 0;

	float F1_score = 0.0;

	char curDir[100];

    for(int c=10; c<=12; c++){
		list<Mat> imgList;
		sprintf(curDir, "%s%02i", test_fold.c_str(), c);

		DIR* pDIR;
		struct dirent *entry;
		struct stat s;

		stat(curDir,&s);

		// if path is a directory
		if ( (s.st_mode & S_IFMT ) == S_IFDIR ){
			if(pDIR=opendir(curDir)){
				//for all entries in directory
				while(entry = readdir(pDIR)){
					stat((curDir + string("/") + string(entry->d_name)).c_str(),&s);
					if (((s.st_mode & S_IFMT ) != S_IFDIR ) && ((s.st_mode & S_IFMT) == S_IFREG )){
						if(string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) == "tif"){
							vector<int> ground_truth;
							vector<int> prediction;

							string csv_name = string(curDir) + "/" + string(entry->d_name).substr(0,2) + ".csv";
							ifstream fin1(csv_name);
							if(fin1){
								//cout << csv_name << endl;
								ground_truth = readCSV(csv_name);
							}
							fin1.close();

							csv_name = string(curDir) + "/" + string(entry->d_name).substr(0,2) + "_detected.csv";
							ifstream fin2(csv_name);
							if(fin2){
								//cout << csv_name << endl;
								prediction = readCSV(csv_name);
							}
							fin2.close();

							int num1 = ground_truth.size() / 2;
							int num2 = prediction.size() / 2;
							FN += num1;
							FP += num2;

							for(vector<int>::iterator i=ground_truth.begin(); i!=ground_truth.end();){
								bool detected_flag = false;
								for(vector<int>::iterator j=prediction.begin(); j!=prediction.end();){
									 float distance = sqrt(pow(*i-*j,2.0)+pow(*(i+1)-*(j+1),2.0));
									 if(distance <= 30){
										TP++;
										FP--;
										FN--;
										i = ground_truth.erase(i);
										i = ground_truth.erase(i);
										j = prediction.erase(j);
										j = prediction.erase(j);
										detected_flag = true;
										break;
									 }
									 else
										 j+=2;
								}
								if(!detected_flag)
									i+=2;
							}
						}
					}
				}
			}
		}
	}

	float Pr = 1.0*TP/(TP+FP);
	float Re = 1.0*TP/(TP+FN);
	F1_score = 2*Pr*Re/(Pr+Re);
	cout << "Pr = " << Pr << ", Re = " << Re << ", F1 score = " << F1_score << endl;
	cout << "TP = " << TP << ", FP = " << FP << ", FN = " << FN << endl;

	ofstream fin("e:\\45 Thesis\\result\\result.csv",ios::app);
		if(!fin){
			cout << "open file error" <<endl; 
			cin.get();
			return 0;
		}

		fin << "TP," << TP << ",FP," << FP << ",FN," << FN << ",Pr," << Pr << ",Re," << Re << ",F1 score," << F1_score;
		fin.close();

	return F1_score;
}