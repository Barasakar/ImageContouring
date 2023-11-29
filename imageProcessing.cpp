#include "imageProcessing.h"

using namespace std;


void imageProcessing::binarizeSingle(QImage &image) {
	/*int counter = 0;
	if (!image.isNull()) {
		for (int y = 0; y < image.height(); ++y) {
			for (int x = 0; x < image.width(); ++x) {
				QColor newColor = (qRed(image.pixel(x, y)) > 100) ? Qt::white : Qt::black;
				image.setPixelColor(x, y, newColor);
				++counter;
			}
		}

	}
	counter = 0;*/
	image = applyOtsuThreshold(image);
}

void imageProcessing::binarize(QVector<QImage>& images) {
	for (int i = 0; i < images.size(); i++) {
		binarizeSingle(images[i]);
	}
	
}

void imageProcessing::getGrayscaleValue(QImage image) {
	// A currently useless function.
	if (!image.isNull()) {
		for (int y = 0; y < image.height(); y++) {
			for (int x = 0; x < image.width(); x++) {
				QRgb pixelValue = image.pixel(x, y);
				grayscaleValues.push_back(qRed(pixelValue));
			}
		}

	}
}

void imageProcessing::findLocalMaximaSingle(QImage image) {

	if (!image.isNull()) {
		for (int x = 0; x < image.width(); x++) {
			QRgb currentMaxValue = image.pixel(x, 0);
			pair<int, int> currentXY(x, 0);

			for (int y = 0; y < image.height(); y++) {
				QRgb pixelValue = image.pixel(x, y);

				if (qRed(pixelValue) >= qRed(currentMaxValue)) { // Assuming grayscale
					currentMaxValue = pixelValue;
					currentXY.second = y;
				}
			}
			allMaxima.push_back(currentMaxValue); // Storing the QRgb value
			allMaximaLocation.push_back(currentXY); // Storing the location
		}
	}
	qDebug() << "Number of maxima: " << allMaxima.size();
	printMaxima();
}

void imageProcessing::printMaxima() {
	for (QVector<QRgb>::Iterator it = allMaxima.begin(); it != allMaxima.end(); ++it) {
		qDebug() << "value: " << *it;
	}
	for (auto it : allMaximaLocation) {
		qDebug() << "(" << it.first << ", " << it.second << ")";
	}
}


//cv::Mat imageProcessing::QtImageToCvMat(const QImage& inputImage, bool inCloneImageData = true) {
//	cv::Mat mat;
//	switch (inputImage.format()) {
//	case QImage::Format_ARGB32:
//	case QImage::Format_ARGB32_Premultiplied:
//		mat = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4,
//			const_cast<uchar*>(inputImage.bits()),
//			static_cast<size_t>(inputImage.bytesPerLine()));
//		break;
//	default:
//		qDebug() << "Unsupported QImage format.";
//		return cv::Mat();
//	}
//
//	return inCloneImageData ? mat.clone() : mat;
//}
cv::Mat imageProcessing::QtImageToCvMat(const QImage& inputImage, bool inCloneImageData=true) {
	cv::Mat mat;
	switch (inputImage.format()) {
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC4,
			const_cast<uchar*>(inputImage.bits()),
			static_cast<size_t>(inputImage.bytesPerLine()));
		break;
	case QImage::Format_Grayscale8:
		mat = cv::Mat(inputImage.height(), inputImage.width(), CV_8UC1,
			const_cast<uchar*>(inputImage.bits()),
			static_cast<size_t>(inputImage.bytesPerLine()));
		break;
	default:
		qDebug() << "Unsupported QImage format.";
		return cv::Mat();
	}

	return inCloneImageData ? mat.clone() : mat;
}




QImage imageProcessing::CvMatToQtImage(const cv::Mat& inputMat, bool inCloneImageData = true) {
	switch (inputMat.type()) {
	case CV_8UC1: {
		QImage image(inputMat.data, inputMat.cols, inputMat.rows, inputMat.step, QImage::Format_Grayscale8);
		return inCloneImageData ? image.copy() : image;
	}
	case CV_8UC4: {
		QImage image(inputMat.data, inputMat.cols, inputMat.rows, inputMat.step, QImage::Format_ARGB32);
		return inCloneImageData ? image.copy() : image;
	}
	default:
		qDebug() << "Unsupported cv::Mat format.";
		return QImage();
	}
}


QImage imageProcessing::applyOtsuThreshold(QImage inputImage) {
	try {
		cv::Mat mat = QtImageToCvMat(inputImage);

		cv::Mat grayMat;
		if (mat.channels() == 4) {
			cv::cvtColor(mat, grayMat, cv::COLOR_BGRA2GRAY);
		}
		else if (mat.channels() == 3) {
			cv::cvtColor(mat, grayMat, cv::COLOR_BGR2GRAY);
		}
		else {
			grayMat = mat;
		}
		cv::Mat preprocessedMat;
		cv::GaussianBlur(grayMat, preprocessedMat, cv::Size(5, 5), 0);

		

		// Apply Otsu threshold
		cv::Mat threshMat;
		double otsuThreshold = cv::threshold(preprocessedMat, threshMat, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

		qDebug() << "Otsu Threshold value:" << otsuThreshold;

		return CvMatToQtImage(threshMat);
	}
	catch (const cv::Exception& e) {
		qDebug() << "OpenCV Exception: " << e.what();
		return QImage();
	}
}


void imageProcessing::Contouring::printSuccess() {
	qDebug() << "called successfully.";
}


QPointF imageProcessing::Contouring::linearInterpolation(int f1, int f2, int x1, int y1, int x2, int y2) {
	QPointF point;

	if (f1 == 0 && f2 == 0) {
		point.setX((x1 + x2) / 2.0);
		point.setY((y1 + y2) / 2.0);
	}
	else if (f1 == 0) {
		point.setX(x1);
		point.setY(y1);
	}
	else if (f2 == 0) {
		point.setX(x2);
		point.setY(y2);
	}
	else {
		int numerator = f1;
		int denominator = f1 - f2;

		if (denominator != 0) {
			double t = static_cast<double>(numerator) / denominator;
			point.setX(x1 + t * (x2 - x1));
			point.setY(y1 + t * (y2 - y1));
		}
	}

	return point;
}


/*
Using a bit mask here to make things easier. 0001, 0011, 0111, 1111.
*/
int imageProcessing::Contouring::getEdgeCase(const PixelBlock& block) const {
		int edgeCase = 0;
		if (block.topLeft) edgeCase |= 1;
		if (block.topRight) edgeCase |= 2;
		if (block.bottomLeft) edgeCase |= 4;
		if (block.bottomRight) edgeCase |= 8;
}

/*
* values[0] = top-left corner
* values[1] = top-right corner
* values[2] = bottom-left corner
* values[3] = bottom-right corner
*/
QPointF imageProcessing::Contouring::calculateIntersection(const PixelBlock& block) {
	QPointF intersectionPoint;
	int edgeCase = getEdgeCase(block);
	int x1 = block.x1;
	int x2 = block.x2;
	int y1 = block.y1;
	int y2 = block.y2;

	switch (edgeCase) {
	case 0:
	case 7:
	case 8:
		return QPointF();
	case 1:
		intersectionPoint = linearInterpolation(block.values[0], block.values[1], x1, y1, x2, y1);
		break;
	case 2:
		if (block.values[0] == 1) {
			intersectionPoint = QPointF(x1, y1);
		}
		else {
			intersectionPoint = linearInterpolation(block.values[0], block.values[2], x1, y1, x1, y2);
		}
		break;
	case 3:
		intersectionPoint = linearInterpolation(block.values[1], block.values[2], x1, y1, x2, y2);
		break;
	case 4:
		if (block.values[1] == 1) {
			intersectionPoint = QPointF(x2, y1);
		}
		else {
			intersectionPoint = linearInterpolation(block.values[1], block.values[3], x2, y1, x2, y2);
		}
		break;
	case 5:
		intersectionPoint = linearInterpolation(block.values[2], block.values[3], x1, y1, x2, y2);
		break;

	case 9:
		intersectionPoint = linearInterpolation(block.values[0], block.values[2], x1, y1, x2, y2);
		break;
	case 10:
		if (block.values[2] == 1) {
			intersectionPoint = QPointF(x1, y2);
		}
		else {
			intersectionPoint = linearInterpolation(block.values[2], block.values[3], x1, y2, x2, y2);
		}
		break;
	case 12:
		intersectionPoint = linearInterpolation(block.values[0], block.values[1], x1, y1, x1, y2);
		break;
	case 13:
		if (block.values[0] == 1) {
			intersectionPoint = QPointF(x1, y1);
		}
		else {
			intersectionPoint = linearInterpolation(block.values[0], block.values[2], x1, y1, x1, y2);
		}
		break;
	case 14:
		intersectionPoint = linearInterpolation(block.values[1], block.values[2], x1, y1, x2, y2);
		break;
	}

	return intersectionPoint;
}

QVector<QPointF> imageProcessing::Contouring::getIntersectionPointsSingle(QImage& image) {
	for (int y = 0; y < image.height() - 1; y++) {
		for (int x = 0; x < image.width() - 1; x++) {
			PixelBlock block;

			block.x1 = x;
			block.y1 = y;
			block.x2 = x + 1;
			block.y2 = y + 1;

			block.values[0] = qRed(image.pixel(x, y));
			block.values[1] = qRed(image.pixel(x + 1, y));
			block.values[2] = qRed(image.pixel(x, y + 1));
			block.values[3] = qRed(image.pixel(x + 1, y + 1));

			QPointF intersectionPoint = calculateIntersection(block);

			intersectionPoints.push_back(intersectionPoint);
		}
	}

	return intersectionPoints;
}


bool imageProcessing::Contouring::isPointConnected(const QPointF& point) {
	return contourPoints.contains(point);
}


double imageProcessing::Contouring::calculateDistance(QPointF p1, QPointF p2) {
	double p1_x = p1.rx();
	double p1_y = p1.ry();
	double p2_x = p2.rx();
	double p2_y = p2.ry();

	return sqrt(pow((p1_x - p2_x), 2) + pow((p1_y - p2_y), 2));
}


void imageProcessing::Contouring::connectPoints(QPointF p1, QPointF p2) {
	LineSegment segment;
	segment.start = p1;
	segment.end = p2;
	lineSegments.push_back(segment);
}

void imageProcessing::Contouring::markPointAsConnected(QPointF p) {
	connectedPoints.insert(p);
}


void imageProcessing::Contouring::generateContours() {
	// Step 1: Identify the Starting Point
	QPointF startingPoint = intersectionPoints[0];

	// Step 2: Find the Nearest Unconnected Point
	QPointF currentPoint = startingPoint;
	QPointF nearestPoint;
	bool contourClosed = false;

	while (!contourClosed) {
		double minDistance = std::numeric_limits<double>::max(); // Initialize with a large value

		// Iterate through unconnected points and find the nearest one
		for (const QPointF& unconnectedPoint : intersectionPoints) {
			if (!isPointConnected(unconnectedPoint)) { // Implement this function
				double distance = calculateDistance(currentPoint, unconnectedPoint); // Implement this function
				if (distance < minDistance) {
					minDistance = distance;
					nearestPoint = unconnectedPoint;
				}
			}
		}

		// Step 3: Connect the Points
		connectPoints(currentPoint, nearestPoint); // Implement this function
		contourPoints.push_back(currentPoint);

		// Mark the nearest point as connected
		markPointAsConnected(nearestPoint); // Implement this function

		// Update the current point for the next iteration
		currentPoint = nearestPoint;

		// Step 4: Check if the contour is closed (return to the starting point)
		if (currentPoint == startingPoint) {
			contourClosed = true;
		}
	}
}
