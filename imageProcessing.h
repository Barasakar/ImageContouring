#pragma once
#include "ui_QtWidgetsApplication.h"
#include <utility>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unordered_set>



class imageProcessing {

public:
	void binarizeSingle(QImage &image);
	void binarize(QVector <QImage>& images);


	void getGrayscaleValue(QImage image);
	double distance(const QPointF& p1, const QPointF& p2);


	// helper functions:
	void findLocalMaximaSingle(QImage image);
	void printMaxima();
	cv::Mat QtImageToCvMat(const QImage& inputImage, bool inCloneImageData);
	QImage CvMatToQtImage(const cv::Mat& inputImage, bool inCloneImageData);
	QImage applyOtsuThreshold(QImage inputImage);


	QVector<int> grayscaleValues;
	QVector<QRgb> allMaxima;
	QVector< std::pair<int, int> > allMaximaLocation; //{first, second} = {x, y}

	class Contouring {
	public:
		Contouring(imageProcessing* parent, QImage& image, float iso) : parent(parent), image(image), iso(iso) {};

		struct PixelBlock {
			int x1, y1, x2, y2; // this is the coordinate of the corners in a 2x2 pixel block.
			bool topLeft, topRight, bottomLeft, bottomRight; // Edge cases flags;
			int values[4]; // an array to store pixel values in the pixel block.
		};

		struct LineSegment {
			QPointF start;
			QPointF end;
		};

		QVector<QPointF> intersectionPoints;
		QVector<QPointF> contourPoints; //processed points to form contours
		QVector<LineSegment> lineSegments;
		QSet<QPointF> connectedPoints;


		void generateContours();
		void printSuccess();
		bool isPointConnected(const QPointF& point);
		double calculateDistance(QPointF p1, QPointF p2);
		void connectPoints(QPointF p1, QPointF p2);
		int getEdgeCase(const PixelBlock& pixelblock) const ;
		void markPointAsConnected(QPointF point);
		QPointF calculateIntersection(const PixelBlock& block);
		QPointF linearInterpolation(int f1, int f2, int x1, int y1, int x2, int y2);
		QVector<QPointF> getIntersectionPointsSingle(QImage& image);
		
		
	private:
		QImage image;
		imageProcessing* parent;
		QVector<QImage> images;
		float iso;
	};
};
