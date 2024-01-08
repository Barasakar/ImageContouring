#pragma once
#include "ui_QtWidgetsApplication.h"
#include <utility>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unordered_set>
#include <QHashFunctions>
#include "qpainter.h"
#include <Eigen/Dense>
#include <cuda.h>


class imageProcessing {

public:
	void binarizeSingleDraw(QImage &image);
	void binarizeSingle(QImage& image);
	void binarizeDraw(QVector <QImage>& images);
	void binarize(QVector <QImage>& images);


	void getGrayscaleValue(QImage image);


	// helper functions:
	void findPeaks(QImage image, QVector<int>& peaksVal, QVector<int>& peaksLocation, int x); // local peaks
	void polyfit(const QVector<int>& x, const QVector<int>& y, Eigen::VectorXd& coefficients, int order);
	void polyVal(const Eigen::VectorXd& coefficients, const Eigen::VectorXd& x, Eigen::VectorXd& y);
	void formPolynomialSingleDraw(QImage& image);
	void formPolynomialDraw(QVector<QImage>& images);
	void formPolynomialSingleNoDraw(QImage& image);
	void formPolynomialNoDraw(QVector<QImage>& images);
	void printMaxima();
	void calculateMedian(QVector<int>& first_index, double& medianValue);
	void drawQuadratic(QImage& image);
	void removeArchSingle(QImage& image, int thickness, Eigen::VectorXd& coefficients);
	void removeArch(QVector<QImage>& images, int thickness);
	void drawMedian(QImage& image, double medianValue);

	cv::Mat QtImageToCvMat(const QImage& inputImage, bool inCloneImageData);
	QImage CvMatToQtImage(const cv::Mat& inputImage, bool inCloneImageData);
	QImage applyOtsuThreshold(QImage inputImage);
	

	QVector<QImage> thresholdImages;
	QVector<int> grayscaleValues;
	QVector<double> medianValues;
	QVector<QRgb> allMaxima;
	QVector< std::pair<int, int> > allMaximaLocation; //{first, second} = {x, y}
	Eigen::VectorXd coefficients;

	class Contouring {
	public:
		Contouring(imageProcessing* parent, QImage& image, float iso) : parent(parent), image(image), iso(iso) {};
		Contouring(imageProcessing* parent, QVector<QImage>& images, float iso) : parent(parent), image(image), iso(iso) {};
		
		struct PixelBlock {
			int x1, y1, x2, y2, x3, y3, x4, y4; // this is the coordinate of the corners in a 2x2 pixel block.
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
		QVector < QVector<LineSegment> > lineSegmentsAll;
		QSet<QPointF> connectedPoints;


		void generateContours(QVector<QImage> &images);
		void printSuccess();
		bool isPointConnected(const QPointF& point);
		qreal calculateDistance(QPointF p1, QPointF p2);
		void connectPoints(QPointF p1, QPointF p2);
		int getEdgeCase(const PixelBlock& pixelblock) const ;
		void markPointAsConnected(QPointF point);
		void getLines(const PixelBlock& block);
		QPointF linearInterpolation(int x1, int y1, int x2, int y2);
		void march(QImage& image);
		
		
	private:
		QImage image;
		imageProcessing* parent;
		QVector<QImage> images;
		float iso;
	};
};
namespace std {
	template <>
	struct hash<QPointF> {
		size_t operator()(const QPointF& point) const {
			// Calculate a hash value based on the x and y coordinates of the point
			size_t hashValue = std::hash<int>()(point.x()) ^ std::hash<int>()(point.y());
			return hashValue;
		}
	};
}