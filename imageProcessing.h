#pragma once
#include "ui_QtWidgetsApplication.h"

class imageProcessing {

public:
	void contouringSingle(QImage image);
	void contouring(QVector<QImage> images);
	void binarizeSingle(QImage &image);
	void binarize(QVector <QImage>& images);

	void getGrayscaleValue(QImage image);

	QVector<int> grayscaleValues;
};