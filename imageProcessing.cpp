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


QPointF imageProcessing::Contouring::linearInterpolation(int x1, int y1, int x2, int y2) {
	QPointF point;

	point.setX((x1 + x2) / 2.0);
	point.setY((y1 + y2) / 2.0);

	return point;
}


/*
true = 255 white
*/
int imageProcessing::Contouring::getEdgeCase(const PixelBlock& block) const {

	if (block.topLeft && block.topRight && block.bottomLeft && block.bottomRight) {
		return 0;
	}
	if (block.topLeft && block.topRight && !block.bottomLeft && block.bottomRight) {
		return 1;
	}
	if (block.topLeft && block.topRight && block.bottomLeft && !block.bottomRight) {
		return 2;
	}
	if (block.topLeft && block.topRight && !block.bottomLeft && !block.bottomRight) {
		return 3;
	}
	if (block.topLeft && !block.topRight && block.bottomLeft && block.bottomRight) {
		return 4;
	}
	if (block.topLeft && !block.topRight && !block.bottomLeft && block.bottomRight) {
		return 5;
	}
	if (block.topLeft && !block.topRight && block.bottomLeft && !block.bottomRight) {
		return 6;
	}
	if (block.topLeft && !block.topRight && !block.bottomLeft && !block.bottomRight) {
		return 7;
	}
	if (!block.topLeft && block.topRight && block.bottomLeft && block.bottomRight) {
		return 8;
	}
	if (!block.topLeft && block.topRight && !block.bottomLeft && block.bottomRight) {
		return 9;
	}
	if (!block.topLeft && block.topRight && block.bottomLeft && !block.bottomRight) {
		return 10;
	}
	if (!block.topLeft && block.topRight && !block.bottomLeft && !block.bottomRight) {
		return 11;
	}
	if (!block.topLeft && !block.topRight && block.bottomLeft && block.bottomRight) {
		return 12;
	}
	if (!block.topLeft && !block.topRight && !block.bottomLeft && block.bottomRight) {
		return 13;
	}
	if (!block.topLeft && !block.topRight && block.bottomLeft && !block.bottomRight) {
		return 14;
	}
	if (!block.topLeft && !block.topRight && !block.bottomLeft && !block.bottomRight) {
		return 15;
	}

}


/*
* values[0] = top-left corner
* values[1] = top-right corner
* values[2] = bottom-left corner
* values[3] = bottom-right corner
*/
//QPointF imageProcessing::Contouring::calculateIntersection(const PixelBlock& block) {
//	QPointF intersectionPoint;
//	int edgeCase = getEdgeCase(block);
//	int x1 = block.x1;
//	int x2 = block.x2;
//	int y1 = block.y1;
//	int y2 = block.y2;
//
//	switch (edgeCase) {
//	case 0:
//	case 7:
//	case 8:
//		return QPointF();
//	case 1:
//		intersectionPoint = linearInterpolation(block.values[0], block.values[1], x1, y1, x2, y1);
//		break;
//	case 2:
//		if (block.values[0] == 255) {
//			intersectionPoint = QPointF(x1, y1);
//		}
//		else {
//			intersectionPoint = linearInterpolation(block.values[0], block.values[2], x1, y1, x1, y2);
//		}
//		break;
//	case 3:
//		intersectionPoint = linearInterpolation(block.values[1], block.values[2], x1, y1, x2, y2);
//		break;
//	case 4:
//		if (block.values[1] == 255) {
//			intersectionPoint = QPointF(x2, y1);
//		}
//		else {
//			intersectionPoint = linearInterpolation(block.values[1], block.values[3], x2, y1, x2, y2);
//		}
//		break;
//	case 5:
//		intersectionPoint = linearInterpolation(block.values[2], block.values[3], x1, y1, x2, y2);
//		break;
//
//	case 9:
//		intersectionPoint = linearInterpolation(block.values[0], block.values[2], x1, y1, x2, y2);
//		break;
//	case 10:
//		if (block.values[2] == 255) {
//			intersectionPoint = QPointF(x1, y2);
//		}
//		else {
//			intersectionPoint = linearInterpolation(block.values[2], block.values[3], x1, y2, x2, y2);
//		}
//		break;
//	case 12:
//		intersectionPoint = linearInterpolation(block.values[0], block.values[1], x1, y1, x1, y2);
//		break;
//	case 13:
//		if (block.values[0] == 255) {
//			intersectionPoint = QPointF(x1, y1);
//		}
//		else {
//			intersectionPoint = linearInterpolation(block.values[0], block.values[2], x1, y1, x1, y2);
//		}
//		break;
//	case 14:
//		intersectionPoint = linearInterpolation(block.values[1], block.values[2], x1, y1, x2, y2);
//		break;
//	}
//
//	return intersectionPoint;
//}


void imageProcessing::Contouring::getLines(const PixelBlock& block) {
	int edgeCase = getEdgeCase(block);
	QPointF intersectionPoint_1;
	QPointF intersectionPoint_2;
	LineSegment line;
	switch (edgeCase) {
		case 0:
		case 15:
			return;
		case 1:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 2:
			intersectionPoint_1 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 3:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 4:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 5:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			intersectionPoint_1 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 6:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 7:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 8:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 9:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 10:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 11:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y4, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 12:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 13:
			intersectionPoint_1 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
		    intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
		case 14:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
	}
		
}

void imageProcessing::Contouring::march(QImage& image) {
	for (int y = 0; y < image.height() - 1; y++) {
		for (int x = 0; x < image.width() - 1; x++) {
			PixelBlock block;

			block.topLeft = (qRed(image.pixel(x, y)) == 255);
			block.topRight = (qRed(image.pixel(x + 1, y)) == 255);
			block.bottomLeft = (qRed(image.pixel(x, y + 1)) == 255);
			block.bottomRight = (qRed(image.pixel(x + 1, y + 1)) == 255);

			//top-left
			block.x1 = x;
			block.y1 = y;
			
			//top-right
			block.x2 = x + 1;
			block.y2 = y;
			
			//bootom-left
			block.x3 = x;
			block.y3 = y + 1;
			
			//bottom-right
			block.x4 = x + 1;
			block.y4 = y + 1;

			block.values[0] = qRed(image.pixel(x, y));
			block.values[1] = qRed(image.pixel(x + 1, y));
			block.values[2] = qRed(image.pixel(x, y + 1));
			block.values[3] = qRed(image.pixel(x + 1, y + 1));

			getLines(block);
		}
	}
	
}


bool imageProcessing::Contouring::isPointConnected(const QPointF& point) {
	return contourPoints.contains(point);
}


qreal imageProcessing::Contouring::calculateDistance(QPointF p1, QPointF p2) {
	qreal p1_x = p1.rx();
	qreal p1_y = p1.ry();
	qreal p2_x = p2.rx();
	qreal p2_y = p2.ry();

	return qSqrt(qPow((p1_x - p2_x), 2) + qPow((p1_y - p2_y), 2));
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


void imageProcessing::Contouring::generateContours(QVector<QImage>& images) {

	for (int i = 0; i < images.length(); i++) {
		march(images[i]);
		lineSegmentsAll.push_back(lineSegments);
		lineSegments.clear();
	}

	for (int i = 0; i < images.length(); i++) {
		QImage& currentImage = images[i]; 
		QPainter painter(&currentImage); 
		painter.setPen(QPen(Qt::red, 1));

		for (const auto& line : lineSegmentsAll[i]) {
			painter.drawLine(line.start, line.end);
		}

		painter.end();
	}
}



