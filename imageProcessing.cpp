#include "imageProcessing.h"

using namespace std;
using namespace Eigen;

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

// find local peaks; based on greyscale binarized images.
void imageProcessing::findPeaks(QImage image, QVector<int> & peaksVal, QVector<int> &peaksLocation, int x) {
	if (!image.isNull()) {
		for (int y = 1; y < image.height() - 1; ++y) {
			int pixelValue = qGray(image.pixel(x, y));
			if (pixelValue > qGray(image.pixel(x, y - 1))
				&& pixelValue > qGray(image.pixel(x, y + 1))) {
				peaksVal.push_back(pixelValue);
				peaksLocation.push_back(y);
			}
		}
	}
}

void imageProcessing::polyfit(const QVector<int>& x, const QVector<int>& y, Eigen::VectorXd& coefficients, int order) {
	int numPoints = x.size();
	MatrixXd X(numPoints, order + 1);
	VectorXd yVec(numPoints);

	for (int i = 0; i < numPoints; ++i) {
		int value = 1;
		for (int j = 0; j <= order; ++j) {
			X(i, j) = value;
			value *= x[i];
		}
		yVec(i) = y[i];
	}

	coefficients = (X.transpose() * X).ldlt().solve(X.transpose() * yVec);
}

void imageProcessing::polyVal(const Eigen::VectorXd& coefficients, const Eigen::VectorXd& x, Eigen::VectorXd& y) {
	int numPoints = x.size();
	y.resize(numPoints);

	for (int i = 0; i < numPoints; ++i) {
		int value = 1;
		y[i] = 0;
		for (int j = 0; j < coefficients.size(); ++j) {
			y[i] += coefficients[j] * value;
			value *= x[i];
		}
	}
}


void imageProcessing::calculateMedian(QVector<int>& first_index, double& medianValue) {
	if (first_index.isEmpty()) {
		medianValue = 0;
		return;
	}
	size_t n = first_index.size() / 2;
	auto nth = first_index.begin() + n;
	nth_element(first_index.begin(), nth, first_index.end());

	if (first_index.size() % 2 == 1) {
		medianValue = *nth;
	}
	else {
		auto nthPrevious = max_element(first_index.begin(), nth);
		medianValue = (*nth + *nthPrevious) / 2.0;
	}
}

void imageProcessing::formPolynomialSingleDraw(QImage& image) {
	QVector<int> first_index;
	QVector<int> frame;

	for (int x = 0; x < image.width(); ++x) {
		QVector<int> peaksVal;
		QVector<int> peaksLocation;
		findPeaks(image, peaksVal, peaksLocation, x);
		// if there are multiple peaks, we don't need it
		if (peaksVal.size() == 1 && peaksLocation[0] >= 10) {
			first_index.push_back(peaksLocation[0]);
			frame.push_back(x);
		}
		//qDebug() << "peaksVal: " << peaksVal.size();
		//qDebug() << "peaksLocation: " << peaksLocation.size();
	}
	QVector<int> temp_value(first_index);
	double medianValue;
	calculateMedian(temp_value, medianValue);
	medianValues.push_back(medianValue);
	//qDebug() << "medianValue: " << medianValue;
	QVector<int> final_index;
	QVector<int> final_frame;

	//qDebug() << "First index size: " << first_index.size();
	
	for (int i = 0; i < first_index.size(); ++i) {
		//150 < first_index[i] && first_index[i] < medianValue - 160
		if (first_index[i] < medianValue + 30 && first_index[i] > medianValue - 30) {
			final_index.push_back(first_index[i]);
			final_frame.push_back(frame[i]);
			// The following code is only for drawing.
			QPixmap pixmap = QPixmap::fromImage(image);
			QPainter painter(&pixmap);
			QPen pen(Qt::red);
			pen.setWidth(3);
			painter.setPen(pen);
			painter.setBrush(Qt::NoBrush);
			int radius = 4;
			painter.drawEllipse(QPointF(frame[i], first_index[i]), radius, radius);
			painter.end();
			image = pixmap.toImage();

		}
	}
	polyfit(final_frame, final_index, coefficients, 2);

	VectorXd X(image.height());
	for (int i = 0; i < image.height(); ++i) {
		X(i) = i + 1;
	}

	VectorXd CurvePoint;
	polyVal(coefficients, X, CurvePoint);
	double a = coefficients(2);
	double b = coefficients(1);
	double c = coefficients(0);
	double peak_frame = -b / (2 * a); // x-coordinate
	double peak = (4 * a * c - b * b) / (4 * a); // y-coordinate aka the peak.

	//qDebug() << "New Peak[x, y]: " << "[" << round(peak_frame) << ", " << round(peak) << "]";

	//QPixmap pixmap = QPixmap::fromImage(image);
	//QPainter painter(&pixmap);
	//QPen pen(Qt::red);
	//pen.setWidth(3);
	//painter.setPen(pen);
	//painter.setBrush(Qt::NoBrush);
	//int radius = 4;
	//painter.drawEllipse(QPointF(peak_frame, peak), radius, radius);
	//painter.end();
	//image = pixmap.toImage();
	 // drawQuadratic(image, coefficients);
	 //removeArch(image, 15, coefficients);
}


void imageProcessing::drawQuadratic(QImage& image) {
	formPolynomialSingleDraw(image);
	QPixmap pixmap = QPixmap::fromImage(image);
	QPainter painter(&pixmap);
	QPen pen(Qt::red, 2);
	painter.setPen(pen);

	double a = coefficients(2);
	double b = coefficients(1);
	double c = coefficients(0);

	QPoint prevPoint;
	bool firstPoint = true;

	for (int x = 0; x < image.width(); x++) {
		double y = a * x * x + b * x + c; //hard to read? essentially ax^2 + bx + c
		QPoint currentPoint(x, y);
		if (!firstPoint) {
			painter.drawLine(prevPoint, currentPoint);
		}
		prevPoint = currentPoint;
		firstPoint = false;
	}
	painter.end();
	image = pixmap.toImage();

}

void imageProcessing::drawMedian(QImage& image, double medianValue) {
	QPixmap pixmap = QPixmap::fromImage(image);
	QPainter painter(&pixmap);
	QPen pen(Qt::green, 2);
	painter.setPen(pen);
	QPoint startPoint(0, medianValue);
	QPoint endPoint(image.width() - 1, medianValue);
	painter.drawLine(startPoint, endPoint);
	painter.end();
	image = pixmap.toImage();
}

void imageProcessing::formPolynomialSingleNoDraw(QImage& image) {
	QVector<int> first_index;
	QVector<int> frame;

	for (int x = 0; x < image.width(); ++x) {
		QVector<int> peaksVal;
		QVector<int> peaksLocation;
		findPeaks(image, peaksVal, peaksLocation, x);
		// if there are multiple peaks, we don't need it
		if (peaksVal.size() == 1 && peaksLocation[0] >= 10) {
			first_index.push_back(peaksLocation[0]);
			frame.push_back(x);
		}
		/*qDebug() << "peaksVal: " << peaksVal.size();
		qDebug() << "peaksLocation: " << peaksLocation.size();*/
	}
	QVector<int> temp_value(first_index);
	double medianValue;
	calculateMedian(temp_value, medianValue);
	qDebug() << "medianValue: " << medianValue;
	QVector<int> final_index;
	QVector<int> final_frame;

	//qDebug() << "First index size: " << first_index.size();

	for (int i = 0; i < first_index.size(); ++i) {
		//if (150 < first_index[i] && first_index[i] < medianValue - 160) {
			final_index.push_back(first_index[i]);
			final_frame.push_back(frame[i]);


		//}
	}
	polyfit(final_frame, final_index, coefficients, 2);

	VectorXd X(image.height());
	for (int i = 0; i < image.height(); ++i) {
		X(i) = i + 1;
	}

	VectorXd CurvePoint;
	polyVal(coefficients, X, CurvePoint);
	double a = coefficients(2);
	double b = coefficients(1);
	double c = coefficients(0);
	double peak_frame = -b / (2 * a); // x-coordinate
	double peak = (4 * a * c - b * b) / (4 * a); // y-coordinate aka the peak.

	qDebug() << "New Peak[x, y]: " << "[" << round(peak_frame) << ", " << round(peak) << "]";
}
void imageProcessing::formPolynomialNoDraw(QVector<QImage>& images) {
	for (int i = 0; i < images.size(); ++i) {
		formPolynomialSingleNoDraw(images[i]);
	}
}

void imageProcessing::formPolynomialDraw(QVector<QImage>& images) {
	for (int i = 0; i < images.size(); ++i) {
		formPolynomialSingleDraw(images[i]);
	}
}

void imageProcessing::removeArch(QVector<QImage>& images, int thickness) {
	for (int i = 0; i < images.size(); ++i) {
		formPolynomialSingleNoDraw(images[i]);
		removeArchSingle(images[i], thickness, this->coefficients);
	}
}

void imageProcessing::removeArchSingle(QImage& image, int thickness, VectorXd& coefficients) {
	qDebug() << "remove Arch called";
	if (image.format() != QImage::Format_RGB32) {
		image = image.convertToFormat(QImage::Format_RGB32);
	}

	thickness = thickness;
	double a = coefficients(2);
	double b = coefficients(1);
	double c = coefficients(0);
	for (int x = 0; x < image.width(); ++x) {
		double y = a * x * x + b * x + c;
		for (int dy = -thickness; dy <= thickness; ++dy) {
			int newY = static_cast<int>(y) + dy;
			if (newY >= 0 && newY < image.height()) {
				image.setPixel(x, newY, qRgb(0, 0, 0));
				//qDebug() << "Modifying pixel at (" << x << ", " << newY << ")";
			}
		}
	}
	


	//QPixmap pixmap = QPixmap::fromImage(image);
	//QPainter painter(&pixmap);
	//painter.setCompositionMode(QPainter::CompositionMode_Source);
	
	//QPen pen(Qt::black, thickness);
	//pen.setCapStyle(Qt::FlatCap);
	//painter.setPen(pen);

	//double a = coefficients(2);
	//double b = coefficients(1);
	//double c = coefficients(0);

	//QPoint prevPoint;
	//bool firstPoint = true;

	//for (int x = 0; x < image.width(); ++x) {
	//	double y = a * x * x + b * x + c;
	//	QPoint currentPoint(x, static_cast<int>(y));

	//	if (!firstPoint) {
	//		painter.drawLine(prevPoint, currentPoint);
	//	}

	//	prevPoint = currentPoint;
	//	firstPoint = false;
	//}

	//painter.end();
	//image = pixmap.toImage();
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

		// Gaussian Blur 
		cv::GaussianBlur(grayMat, preprocessedMat, cv::Size(5, 5), 0);

		

		// Apply Otsu threshold
		cv::Mat threshMat;
		cv::threshold(preprocessedMat, threshMat, 0, 255, cv::THRESH_TOZERO | cv::THRESH_OTSU);
		// otsuValues.push_back(cv::threshold(preprocessedMat, threshMat, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU));

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
			break;
		case 2:
			intersectionPoint_1 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 3:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 4:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
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
			break;
		case 6:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 7:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 8:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 9:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
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
			break;
		case 11:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x2, block.y2);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y4, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 12:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 13:
			intersectionPoint_1 = linearInterpolation(block.x2, block.y2, block.x4, block.y4);
		    intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		case 14:
			intersectionPoint_1 = linearInterpolation(block.x1, block.y1, block.x3, block.y3);
			intersectionPoint_2 = linearInterpolation(block.x3, block.y3, block.x4, block.y4);
			line.start = intersectionPoint_1;
			line.end = intersectionPoint_2;
			lineSegments.push_back(line);
			break;
		default:
			break;
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
		QPixmap pixmap = QPixmap::fromImage(images[i]);
		QPainter painter(&pixmap);
		QPen pen(Qt::red);
		painter.setPen(pen);

		for (const auto& line : lineSegmentsAll[i]) {
			painter.drawLine(line.start, line.end);
		}

		painter.end();
		images[i] = pixmap.toImage();
	}
}



