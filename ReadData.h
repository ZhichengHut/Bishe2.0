#ifndef READDATA_H
#define READDATA_H

#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>  

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <opencv.hpp>

using namespace std;
using namespace cv;

void readTrainData(string path, vector<Mat>& imgList, vector<int>& labelList);
//void readTestData(string path, vector<Mat>& imgList, vector<int>& X, vector<int>& Y);


#endif//READDATA_H