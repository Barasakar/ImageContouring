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
	int counter = 0;
	if (!image.isNull()) {
		for (int y = 0; y < image.height(); ++y) {
			for (int x = 0; x < image.width(); ++x) {
				QColor newColor = (qRed(image.pixel(x, y)) > 100) ? Qt::white : Qt::black;
				image.setPixelColor(x, y, newColor);
				++counter;
			}
		}

	}
	counter = 0;
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
