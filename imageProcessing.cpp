#include "imageProcessing.h"


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
				QColor newColor = (qRed(image.pixel(x, y)) > 55) ? Qt::white : Qt::black;
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
	if (!image.isNull()) {
		for (int y = 0; y < image.height(); y++) {
			for (int x = 0; x < image.width(); x++) {
				QRgb pixelValue = image.pixel(x, y);
				grayscaleValues.push_back(qRed(pixelValue));
			}
		}

	}
}