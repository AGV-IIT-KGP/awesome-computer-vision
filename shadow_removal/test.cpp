#include "shadowRemoval.h"

int main(int argc, char* argv[]){
  
  cv::Mat original = cv::imread("hqdefault.jpg",1);

  cv::Mat final_image(original.rows,original.cols,CV_8UC3,cvScalarAll(0));
  if (argc ==2){
  	ShadowRemoval Shadow(original,atoi(argv[1]));
  	final_image = Shadow.shadowRemoval();
  }
  else{
  	ShadowRemoval Shadow(original,0);
  	final_image = Shadow.shadowRemoval();
  }
  cv::namedWindow("output",1);
  cv::imshow("output",final_image);
  cv::imwrite("final_output.jpg",final_image);
  cv::waitKey(0);

}