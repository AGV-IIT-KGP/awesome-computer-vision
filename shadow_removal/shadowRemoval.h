#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>

class ShadowRemoval {
public: 
	cv::Mat shadowRemoval();
	ShadowRemoval(cv::Mat image, int debug=0);

private:
	cv::Mat original_image;
	int debug_mode;
};