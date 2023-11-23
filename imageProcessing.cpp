#include "imageProcessing.h"

using namespace std;

void imageProcessing::contouringSingle(QImage image) {
	if (!image.isNull()) {
		qDebug() << image.isGrayscale();
		qDebug() << image.pixelColor(0, 0);
	}
	
}

void imageProcessing::contouring(QVector<QImage> images) {

}

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

		cv::Mat threshMat;
		double thresholdValue = cv::threshold(grayMat, threshMat, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
		qDebug() << "Otsu Threashold value: " << thresholdValue;

		return CvMatToQtImage(threshMat);
	}
	catch (const cv::Exception& e) {
		qDebug() << "OpenCV Exception: " << e.what();
		return QImage();
	}
}


