#include "shadowRemoval.h"

ShadowRemoval::ShadowRemoval(cv::Mat image, int debug){
	original_image = image;
	debug_mode = debug;
}


/* Return the standard Deviation of Y channel of the image*/
int standardDeviation(cv::Mat &image){
	int mean = 0, sigma = 0;
	int number_of_pixels = image.rows * image.cols;
	for (int i=0;i<image.rows;i++){
		for (int j=0;j<image.cols;j++){
			mean += image.at<cv::Vec3b>(i,j)[0]; 
		}
	}
	mean = mean/(number_of_pixels);
	for (int i=0;i<image.rows;i++){
		for (int j=0;j<image.cols;j++){
			sigma += (mean - image.at<cv::Vec3b>(i,j)[0])*(mean - image.at<cv::Vec3b>(i,j)[0]); 
		}
	}
	sigma = sigma/(number_of_pixels);
	sigma = sqrt((float)sigma);
	return sigma;
}


/* Thresholding the Y channel of the image on the basis of the sigma obtained
	After this we are doing window based thresholding
 */
cv::Mat shadowDetection(cv::Mat &image){
	int sigma = standardDeviation(image);
	// First threshold on the basis of sigma
	cv::Mat binary(image.rows,image.cols,CV_8UC1,cvScalarAll(0));
	for (int i=0;i<image.rows;i++){
		for (int j=0;j<image.cols;j++){
			if (image.at<cv::Vec3b>(i,j)[0] > 1.5*sigma){
				binary.at<uchar>(i,j) = 255; 
			}
			else
				binary.at<uchar>(i,j) = 0; 
		}
	}

	// Window based thresholding
	
	int mean = 0, count = 0;
	cv::Mat binary_final(image.rows,image.cols,CV_8UC1,cvScalarAll(0));
	for (int i=1;i<image.rows-1;i++){
        for (int j=1;j<image.cols-1;j++){
            mean = sigma =0;
            if (binary.at<uchar>(i,j) == 255){
                for (int k = i-1;k<=i+1;k++){
                    for (int l= j-1;l<=j+1;l++){
                        mean += image.at<cv::Vec3b>(k,l)[0];
                    }
                }
                mean /=9;
                for (int k = i-1;k<=i+1;k++){
                    for (int l= j-1;l<=j+1;l++){
                        sigma += (mean - image.at<cv::Vec3b>(k,l)[0])*(mean - image.at<cv::Vec3b>(k,l)[0]);
                    }
                }
                sigma /= 9;
                sigma = sqrt((float)sigma);
                if (image.at<cv::Vec3b>(i,j)[0] > 1*sigma)
                    binary_final.at<uchar>(i,j)=255;
                else
                    binary_final.at<uchar>(i,j)=0;
            }
            else
                binary_final.at<uchar>(i,j) = 0;
        }
    }
    return binary_final;
}


/* Shadow Removal on the basis of Y correction and colour adjustment*/
cv::Mat ShadowRemoval::shadowRemoval(){
	int shadow_mean =0, non_shadow_mean =0 ,count_shadow=0,count_non_shadow=0, difference;
    cv::Mat binary(original_image.rows,original_image.cols,CV_8UC1,cvScalarAll(0));
    cv::Mat image_ycrcb(original_image.rows,original_image.cols,CV_8UC3,cvScalarAll(0));
    cv::Mat final(original_image.rows,original_image.cols,CV_8UC3,cvScalarAll(0));
    
    cvtColor(original_image,image_ycrcb,CV_BGR2YCrCb);
    binary = shadowDetection(image_ycrcb);
    if (debug_mode > 0){
	    cv::namedWindow("binary_image",1);
	    cv::imshow("binary_image",binary);
	    cv::waitKey(0);
	}
    cv::Mat element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 7,7),
                                       cv::Point( 3, 3) );
    dilate(binary,binary,element);
    element = cv::getStructuringElement( cv::MORPH_ELLIPSE,
                                       cv::Size( 5,5),
                                       cv::Point( 2, 2) );
    erode(binary,binary,element);
    
    if (debug_mode > 0){
	    cv::namedWindow("eroded_image",1);
	    cv::imshow("eroded_image",binary);
	    cv::waitKey(0);
	}
    
    for (int i=0;i<image_ycrcb.rows;i++){
        for (int j=0;j<image_ycrcb.cols;j++){
            if (binary.at<uchar>(i,j)==0){
                shadow_mean += image_ycrcb.at<cv::Vec3b>(i,j)[0];
                count_shadow++;
            }
            else{
                non_shadow_mean += image_ycrcb.at<cv::Vec3b>(i,j)[0];
                count_non_shadow++;
            }
        }
    }
    difference = non_shadow_mean/count_non_shadow - shadow_mean/count_shadow;
    for (int i=0;i<image_ycrcb.rows;i++){
        for (int j=0;j<image_ycrcb.cols;j++){
            if (binary.at<uchar>(i,j)==0){
              	image_ycrcb.at<cv::Vec3b>(i,j)[0] += difference/2; // Y adjustment
                image_ycrcb.at<cv::Vec3b>(i,j)[2] -= difference/6; // Colour adjustment
            }
        }
    }
  	cvtColor(image_ycrcb,final,CV_YCrCb2BGR);
  	return final;
}