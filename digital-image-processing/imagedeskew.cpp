/* imagedeskew.cpp: Hough transform based image skew correctoin */
#include "imagedeskew.h"

ImageDeskew::ImageDeskew() {
	cAlphaStart = -20;	
	cAlphaStep = 0.2;
	cSteps = 40 * 5;
	cDStep = 1.0;
	cBinary = false;

	cSinA = new double[cSteps - 1];
	cCosA = new double[cSteps - 1];

	double angle;
	const double PI  =3.141592653589793238463;
	// pre-calculation of sin and cos
	for (int i = 0; i < cSteps - 1; i++) {
		angle = getAlpha(i) * PI / 180.0;
		cSinA[i] = sin(angle);
		cCosA[i] = cos(angle);
 	}
 
	// range of d, should be updated when set image
	cDMin = 0;
	cDCount = 0;
	cHMatrix = NULL;	
}

void ImageDeskew::printSinCos() {
	for (int i = 0; i < cSteps - 1; i++) {
		cout << cSinA[i] << "\t";
	}
	cout << endl;
	for (int i = 0; i < cSteps - 1; i++) {
		cout << cCosA[i] << "\t";
	}
}

ImageDeskew::~ImageDeskew() {
	delete[] cSinA;
	delete[] cCosA;
	if (!cHMatrix) {
		delete[] cHMatrix;
	}
}

void ImageDeskew::setImage(Mat image, bool binary) {
	cImage = image;
	cBinary = binary;
	
	// update the range of d;
	cDMin = -image.size().width;
	cDCount = (int) (2.0 * ((cImage.size().width + cImage.size().height)) / cDStep);
	if (!cHMatrix) {
		delete[] cHMatrix;
	}
	cHMatrix = new int[cDCount * cSteps];
	for (int i = 0; i < cDCount * cSteps; i++) {
		cHMatrix[i] = 0;
	}
}

double ImageDeskew::getAlpha(int index) {
	return cAlphaStart + index * cAlphaStep;
}

// calculate the skew angle of the image cImage
double ImageDeskew::getSkewAngle() {
	HoughLine *hl;
	double sum = 0.0;
	int count = 0;
	int topCount = 20;

	// perform Hough Transformation
	calc();
	// top 20 of the detected lines in the image
	hl = getTop(topCount);

	if (countOfValid(hl, topCount) >= 20) {
		// average angle of the lines
		for (int i = 0; i < 19; i++) {
			sum += hl[i].alpha;
			count++;
		}
		return (sum / count);
	} else {
		return 0.0;
	}
}

// calculate the count lines in the image with most points
HoughLine* ImageDeskew::getTop(int count) {

	HoughLine *hl = new HoughLine[count];

	HoughLine tmp;
	int length = cDCount * cSteps;
	for (int i = 0; i < (length - 1); i++) {
		if (cHMatrix[i] > hl[count - 1].count) {
			hl[count - 1].count = cHMatrix[i];
			hl[count - 1].index = i;
			int j = count - 1;
			while ((j > 0) && (hl[j].count > hl[j - 1].count)) {
				tmp = hl[j];
				hl[j] = hl[j - 1];
				hl[j - 1] = tmp;
				j--;
			}
		}
	}

	int alphaIndex;
	int dIndex;
	for (int i = 0; i < count; i++) {
		dIndex = hl[i].index / cSteps;					// integer division, no remainder
		alphaIndex = hl[i].index - dIndex * cSteps;
		hl[i].alpha = getAlpha(alphaIndex);
		hl[i].d = dIndex + cDMin;
	}

	return hl;
}

// Hough Transformation
void ImageDeskew::calc() {
	int hMin = (int) ((cImage.size().height) / 4.0);
	int hMax = (int) ((cImage.size().height) * 3.0 / 4.0);

	for (int y = hMin; y < hMax; y++) {
		for (int x = 1; x < (cImage.size().width - 2); x++) {
			// only lower edges are considered
			if (isBlack(x, y)) {
				if (!isBlack(x, y + 1)) {
					calc(x, y);
				}
			}
		}
	}
}

// calculate all lines through the point (x,y)
void ImageDeskew::calc(int x, int y) {
	double d;
	int dIndex;
	int index;

	int uBound = cDCount * cSteps;
	for (int alpha = 0; alpha < cSteps - 1; alpha++) {
		d = y * cCosA[alpha] - x * cSinA[alpha];
		dIndex = (int) (d - cDMin);
		index = dIndex * cSteps + alpha;
		if (index < uBound) {
			cHMatrix[index] += 1;
		} 
	}
}

bool ImageDeskew::isBlack(int x, int y) {
	// for black/white binary image, if pixel value equals 0, then black
	if (cBinary) {
		uint8_t* pixelPtr = (uint8_t*)cImage.data;
		int pixelValue = pixelPtr[y * cImage.cols * cImage.channels() + x * cImage.channels() + 0];
		if (pixelValue == 0) {
			return true;
		} else {
			return false;
		}
	}

	int luminanceValue = 140;
	return isBlack(x, y, luminanceValue);
}

bool ImageDeskew::isBlack(int x, int y, int luminanceCutOff) {
	// return white on areas outside of image boundaries
	if (x < 0 || y < 0 || x > cImage.cols - 1 || y > cImage.rows - 1) {
		return false;
	}

	double luminance = 0.0;
	uint8_t* pixelPtr = (uint8_t*)cImage.data;
	int channels = cImage.channels();
	if (channels == 3) {
		int b = pixelPtr[y * cImage.cols * channels + x * channels + 0];
		int g = pixelPtr[y * cImage.cols * channels + x * channels + 1];
		int r = pixelPtr[y * cImage.cols * channels + x * channels + 2];
		luminance = (r * 0.299) + (g * 0.587) + (b * 0.114);
	} else if (channels == 1) {
		luminance = pixelPtr[y * cImage.cols * channels + x * channels + 0];
	}

	return luminance < luminanceCutOff;
}

int ImageDeskew::countOfValid(HoughLine *hl, int size) {
	int count = 0;
	for (int i = 0; i < size; i++) {
		if (hl[i].count > 0) {
			count++;
		}
	}

	return count;
}