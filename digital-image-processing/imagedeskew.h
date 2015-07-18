#ifndef IMAGEDESKEW_H__
#define IMAGEDESKEW_H__

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

typedef unsigned char uint8_t;

class HoughLine {
public:
	int count;					// count of points in the line
	int index;					// index in matrix
	double alpha;				// the line is represented as all x, y that solve y * cos(alpha) - x * sin(alpha) = d
	double d;
	HoughLine() {
		count = 0;
		index = 0;
	}
};

class ImageDeskew {
private:
	Mat cImage;					// the source image
	bool cBinary;
	double cAlphaStart;	
	double cAlphaStep;
	int cSteps;
	double *cSinA;				// pre-calculation of sin
	double *cCosA;				// pre-calculation of cos
	double cDMin;				// range of d
	double cDStep;
	int cDCount;
	int *cHMatrix;				// count of points that fit in a line
	
	// calculate the count lines in the image with most points
	HoughLine* getTop(int count);
	// Hough Transformation
	void calc();
	// calculate all lines through the point(x, y)
	void calc(int x, int y);
	// get the count of valid hough lines
	int countOfValid(HoughLine *hl, int size);
	bool isBlack(int x, int y);
	bool isBlack(int x, int y, int luminaceCutOff);

public:
	ImageDeskew();
	~ImageDeskew();
	void setImage(Mat image, bool binary = false);
	double getSkewAngle();
	double getAlpha(int index);

	// for debug
	void printSinCos();
};

#endif