#ifndef EVALUATE_H
#define EVALUATE_H

#include "RandomForest.h"
#include "ExtractData.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include<math.h>

void classifier(RandomForest *RF, string test_fold, int width);

void get_predict_result(RandomForest *RF, string test_fold, int width, int sample_interval, float prob_threshold);

float get_F1_score(string test_fold);

#endif//EVALUATE_H