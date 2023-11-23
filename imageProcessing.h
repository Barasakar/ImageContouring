#pragma once
#include "ui_QtWidgetsApplication.h"
#include <utility>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class imageProcessing {

public:
	void contouringSingle(QImage image);
	void contouring(QVector<QImage> images);
	void binarizeSingle(QImage &image);
	void binarize(QVector <QImage>& images);
	void getGrayscaleValue(QImage image);
	


	// helper functions:
	void findLocalMaximaSingle(QImage image);
	void printMaxima();
	cv::Mat QtImageToCvMat(const QImage& inputImage, bool inCloneImageData);
	QImage CvMatToQtImage(const cv::Mat& inputImage, bool inCloneImageData);
	QImage applyOtsuThreshold(QImage inputImage);


	QVector<int> grayscaleValues;
	QVector<QRgb> allMaxima;
	QVector< std::pair<int, int> > allMaximaLocation; //{first, second} = {x, y}
};