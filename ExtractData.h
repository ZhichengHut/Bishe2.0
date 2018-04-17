#ifndef EXTRACTDATA_H
#define EXTRACTDATA_H

#include <iostream>
#include <fstream>  
#include <sstream>  
#include <string>  
#include <vector>
#include <opencv.hpp>
#include <opencv2/opencv.hpp>

#include <typeinfo>
#include <io.h>
#include <direct.h>
#include <windows.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;
using namespace cv;

vector<int> readCSV(string csvFile);
Mat preProcess(Mat img, float thresh);
vector<Point2i> getCenter(Mat img, int R);
void clearFold(string out_fold);
void saveTrainData(string img_file, string csv_file, string out_fold, string out_csv, int width, float thresh);
void saveTrainData(string img_file, string out_fold, int width, float thresh);
void getTrainingSet(string train_fold, string out_fold, int width, float thresh);
void extractData(string train_fold, string out_fold, int width, float train_thresh, bool get_train);

#endif//EXTRACTDATA_H