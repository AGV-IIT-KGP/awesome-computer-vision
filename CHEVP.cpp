#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/stitching/stitcher.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <sys/time.h>

using namespace std;
using namespace cv;

int main(){
	namedWindow("road",0);
	Mat orig=imread("road.jpg",0);
	Mat cannyEdge(orig.rows,orig.cols,CV_8UC1);
	Canny(orig,cannyEdge,60,180,3,false);
	imshow("road",cannyEdge);
	waitKey(0);
	Mat hough=cannyEdge.clone();
	for(int section=1;section<=5;section++){
		int s1=(((section+1)*section)/2)*orig.rows/15,s2=(((section-1)*section)/2)*orig.rows/15;
		Mat subcanny(s1-s2,orig.cols,CV_8UC1);
		for(int r=0;r<s1-s2;r++){
			for(int c=0;c<orig.cols;c++){
				subcanny.at<uchar>(r,c)=cannyEdge.at<uchar>(r+s2,c);
			}}
		vector<Vec4i> lines;
		HoughLinesP(subcanny,lines,1.0,0.01,30);
		for( size_t i = 0; i < lines.size(); i++ )
    {
        line( hough, Point(lines[i][0], s2+lines[i][1]),
            Point(lines[i][2], s2+lines[i][3]), Scalar(255), 3, 8 );
    }
		
	}	
	for(int section=1;section<=4;section++){
		line(hough,Point(0,(((section+1)*section)/2)*orig.rows/15),Point(orig.cols,(((section+1)*section)/2)*orig.rows/15),Scalar(100),1,8);
	}
	imshow("road",hough);
	
	waitKey(0);
}
