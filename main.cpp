#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace cv;
using namespace std;

//read homography from file
//warp the camera frame
//calculate the NCC between warpped camera frame and the reference frame

void warpFrame(Mat& homography, Mat& frame) {
	warpPerspective(frame, frame, homography, frame.size());
}

float getNCC(Mat& reference, Mat& current) {
	int result_cols =  1;
	int result_rows = 1;

    Mat result;
    result.create( result_cols, result_rows, CV_32FC1 );
	matchTemplate(reference, current, result, CV_TM_CCOEFF_NORMED);
	
	return result.at<float>(0,0);
	
}

int main(int argc, char** argv) {
	//open file and read all the line
	ifstream myfile;
	ofstream outfile;
	string name = argv[1];
	myfile.open((string("./dataset/") + argv[1]).c_str());
	outfile.open((string("./dataset/") + argv[1] +string(".csv")).c_str());
	vector<string> list;
	while(!myfile.eof())
	{
		string line;
		getline( myfile, line );
		list.push_back(line);
	}
	
	
	//open video
	VideoCapture capture((string("./dataset/") + argv[1] + string(".mp4")).c_str()); 
	Mat currentFrame; 	
	char* window="result";
	namedWindow(window, CV_WINDOW_AUTOSIZE);
	
	//open referen image
	Mat refeImage;
	refeImage = imread((string("./dataset/") + argv[1] + string(".jpg")).c_str());
	cvtColor(refeImage, refeImage, CV_RGB2GRAY);
	
	//process video
	int line_counter =  -1;
	float h1, h2, h3, h4, h5, h6, h7 ,h8, h9;
	bool shouldQuit = false;
	do
	{  
		line_counter++;
		
		//read frame
		capture >> currentFrame; 	
		if(currentFrame.empty())
		{
			shouldQuit = true;
			continue; 
		}
		cvtColor(currentFrame, currentFrame, CV_RGB2GRAY);

		//read line
		istringstream iss(list[line_counter]);
		if (!( iss >> h1 >> h2 >> h3 >> h4 >> h5 >> h6 >> h7 >> h8 >> h9)) {
			outfile<<"0.0"<<endl;
			continue;
		}
		else {
			//create mat
			Mat homo = (Mat_<float>(3,3) << h1, h2, h3, h4, h5, h6, h7, h8, h9);
			Mat invHomo = homo.inv();
			//warp current frame
			warpFrame(invHomo, currentFrame);
			
			//get ncc
			outfile<<getNCC(refeImage, currentFrame)<<endl;

		}
		
		imshow(window, currentFrame);
		waitKey(27);
		
	}while(!shouldQuit);
	
	myfile.close();
	outfile.close();
	
	return 0;
	
}
