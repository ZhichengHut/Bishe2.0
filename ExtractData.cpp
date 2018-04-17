#include "ExtractData.h"

int index = 0;
int R = 10;
int ran_point = 50;

vector<int> readCSV(string csvFile){
	ifstream fin(csvFile);
    string line;   
	vector<int> csvData;

	while (getline(fin, line))
    {
		stringstream line_tmp(line);
		string num;
		while(getline(line_tmp,num,',')){
			stringstream num_tmp(num);
			int position;
			num_tmp >> position;
			csvData.push_back(position);
		}
    }
	return csvData;
}

Mat preProcess(Mat img, float thresh){
	Mat out[3];
    split(img, out);

	Mat r = out[2];
	Mat g = out[1];
	Mat b = out[0];

	r.convertTo(r,CV_32FC1);
	g.convertTo(g,CV_32FC1);
	b.convertTo(b,CV_32FC1);

	Mat blue_ratio = 100*b/(1+r+g)*256/(1+r+g+b);

	blue_ratio.convertTo(blue_ratio,CV_8UC1);
	//cout << "blue ratio" << endl;
	//cout << blue_ratio(Rect(0,0,10,10)) << endl;

	GaussianBlur( blue_ratio, blue_ratio, Size(3,3), 0, 0, BORDER_DEFAULT );
	//cout << "br after gaussian" << endl;
	//cout << blue_ratio(Rect(0,0,10,10)) << endl;

	int kernel_size = 3;
	int scale = 1;
	int delta = 0.5;
	int ddepth = CV_16U;

	Laplacian( blue_ratio, blue_ratio, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
	convertScaleAbs(blue_ratio, blue_ratio);

	//cout << "after laplace" << endl;
	//cout << blue_ratio(Rect(0,0,10,10)) << endl;

	threshold(blue_ratio, blue_ratio, thresh*255,255 ,THRESH_BINARY);
	//cout << "after threshold" << endl;
	//cout << blue_ratio(Rect(0,0,10,10)) << endl;
	//cin.get();

	//imshow("brgl" , blue_ratio);
	//waitKey();

	Mat kernel = Mat::ones(3,3,CV_32FC1);
	int iteration = 3;
	
	/// Apply the close operation
	morphologyEx(blue_ratio,blue_ratio, MORPH_CLOSE, kernel, Point(-1,-1), iteration);

	return blue_ratio;
}

vector<Point2i> getCenter(Mat img, int R){
	vector<vector<Point>> contours;
	vector<Vec4i> heirarchy;
	vector<Point2i> center;
	vector<int> radius;

	//find contours
	findContours(img.clone(), contours, heirarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0,0));

	size_t count = contours.size();
	
	for(int i=0; i<count; i++){
		Point2f c;
		float r;
		minEnclosingCircle(contours[i], c, r);

		if(r>R){
			center.push_back(c);
			radius.push_back(r);
		}
	}
	return center;
}

void clearFold(string out_fold){
	if(!access(out_fold.c_str(), 0)){
		char curDir[100];
		sprintf(curDir,out_fold.c_str());
		
		DIR* pDIR;
		struct dirent *entry;
		struct stat s;
		stat(curDir,&s);
		
		pDIR=opendir(curDir);
		int i = 0;
		
		while(entry = readdir(pDIR)){
			stat((curDir + string("/") + string(entry->d_name)).c_str(),&s);
			if (((s.st_mode & S_IFMT) != S_IFDIR) && ((s.st_mode & S_IFMT) == S_IFREG)){
				remove((curDir + string(entry->d_name)).c_str());	
				/*cout << string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) << endl;
				cout << (string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) == "png") << endl;
				cout << (string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) == "csv") << endl;
				cin.get();*/
			}
		}
	}
	else
		mkdir(out_fold.c_str());

	//cout << "dir complete" << endl;
}


void saveTrainData(string img_file, string csv_file, string out_fold, int width, float thresh){
	Mat img = imread(img_file,1);
	Mat blue_ratio = preProcess(img, thresh);

	int R = 10;
	vector<Point2i> center = getCenter(blue_ratio, R);
	//vector<Point2i> center;
	for(int i=0; i<ran_point; i++){
		int x = rand() % (img.cols-width);
		int y = rand() % (img.rows-width);
		//cout << x << " " << y << endl; 
		center.push_back(Point2i(x,y));
	}

	int cell_num = center.size();
	vector<int> csvData = readCSV(csv_file);
	int mitosis_num = csvData.size()/2;

	//int width = 30;

	for(int i=0; i< cell_num; i++){
		int x = center[i].x;
		int y = center[i].y;

		//whether the position is out of bound
		if(x < width + 1)
			x = width + 1;
		else if(x > img.cols - width)
			x = img.cols - width;

		if(y < width + 1)
            y = width + 1;
        else if(y > img.rows - width)
            y = img.rows - width;

		bool label = false;
		for(int j=0; j<mitosis_num; j++){
			//if(((x-width)<csvData[2*j+1]+10) && ((x+width)>csvData[2*j+1]-10) && ((y-width)<csvData[2*j]+10) && ((y+width)>csvData[2*j]-10)){
			if(((x-width)<csvData[2*j+1]) && ((x+width)>csvData[2*j+1]) && ((y-width)<csvData[2*j]) && ((y+width)>csvData[2*j])){
                    label = true;
                    break;
			}
		}

		if(label)
			break;
		else{
			char img_name[100];
			sprintf(img_name, "%s%04i_0.png", out_fold.c_str(), index);
			imwrite(img_name, img(Rect(x-width,y-width,2*width,2*width)));
			index ++;
		}
	}

	for(int i=0; i<mitosis_num; i++){
		bool x_label = true;
		bool y_label = true;
		for(int p=0; p<1; p++){
			//int x = csvData[2*i+1]+5*(p-2);
			int x = csvData[2*i+1];
			if(x < width + 1 && x_label){
				x = width + 1;
				x_label = false;
			}
			else if(x < width + 1 && !x_label)
				continue;

			if(x > img.cols - width && x_label){
				x = img.cols - width;
				x_label = false;
			}
			else if(x > img.cols - width && !x_label)
				continue;

			for(int q=0; q<1; q++){
				//int y = csvData[2*i]+5*(q-2);
				int y = csvData[2*i];
				if(y < width + 1 && y_label){
					y = width + 1;
					y_label = false;
				}
				else if(y < width + 1 && !y_label)
					continue;
				
				if(y > img.rows - width && y_label){
					y = img.rows - width;
					y_label = false;
				}
				else if(y > img.rows - width && !y_label)
					continue;
				
				char img_name[100];
				sprintf(img_name, "%s%04i_1.png", out_fold.c_str(), index);
				imwrite(img_name, img(Rect(x-width,y-width,2*width,2*width)));
				index ++;
			}
		}
	}
}

void saveTrainData(string img_file, string out_fold, int width, float thresh){
	Mat img = imread(img_file,1);
	Mat blue_ratio = preProcess(img, thresh);

	int R = 10;
	vector<Point2i> center = getCenter(blue_ratio, R);
	//vector<Point2i> center;
	for(int i=0; i<ran_point; i++){
		int x = rand() % (img.cols-width);
		int y = rand() % (img.rows-width);
		//cout << x << " " << y << endl; 
		center.push_back(Point2i(x,y));
	}
	int cell_num = center.size();

	//int width = 30;

	for(int i=0; i< cell_num; i++){
		int x = center[i].x;
		int y = center[i].y;

		if(x < width + 1)
			x = width + 1;
		else if(x > img.cols - width)
			x = img.cols - width;

		if(y < width + 1)
            y = width + 1;
        else if(y > img.rows - width)
            y = img.rows - width;
		
		char img_name[100];
		sprintf(img_name, "%s%04i_0.png", out_fold.c_str(), index);
		imwrite(img_name, img(Rect(x-width,y-width,2*width,2*width)));
		index ++;
	}
}

void getTrainingSet(string train_fold, string out_fold, int width, float thresh){
	vector<string> tif_set, csv_set;
	clearFold(out_fold);

	char delim = '/';

    char curDir[100];
    
    for(int c=0; c<=9; c++){
		list<Mat> imgList;
		//sprintf(curDir, "%s%c%i%c", dataPath.c_str(), delim, c, delim);
		sprintf(curDir, "%s%02i", train_fold.c_str(), c);
		//cout << curDir << endl;

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
						//cout << string(entry->d_name) << endl;
						if(string(entry->d_name).substr(string(entry->d_name).find_last_of('.') + 1) == "csv")
							csv_set.push_back(curDir + string("/") + string(entry->d_name));
						else
							tif_set.push_back(curDir + string("/") + string(entry->d_name));
					}
				}
			}
		}
	}

	int j = 0;
	for(int i=0; i<tif_set.size(); i++){
		if(j<csv_set.size() && tif_set[i].substr(train_fold.length(),5) == csv_set[j].substr(train_fold.length(),5)){
			cout << tif_set[i] << endl;
			//cout << csv_set[j] << endl;
			saveTrainData(tif_set[i], csv_set[j++], out_fold, width, thresh);
		}
		else{
			cout << tif_set[i] << endl;
			saveTrainData(tif_set[i], out_fold, width, thresh);
		}
	}
}

void extractData(string train_fold, string out_fold, int width, float train_thresh, bool get_train){
	if(get_train){
		getTrainingSet(train_fold, out_fold, width, train_thresh);
		cout << "Extracted training set completed" << endl;
	}
};