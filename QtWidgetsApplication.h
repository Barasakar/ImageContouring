/* QtWidgetsApplication.h */

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication.h"
#include "qfiledialog.h"
#include <iostream>
#include <qdebug.h>
#include "tiffio.h"
#include <QtOpenGLWidgets/qopenglwidget.h>
#include "imageProcessing.h"
#include "qpainter.h"




class QtWidgetsApplication : public QMainWindow
{
    Q_OBJECT

public:
    QVector<QImage> images;
    size_t numPixels;
    uint32 columns;
    uint32 rows;

    QtWidgetsApplication(QWidget *parent = nullptr);
    ~QtWidgetsApplication();
    void onActionFileTriggered();
    void logPrint(QString str);
    void onSliderValueChanged(int n);
    void onActionBinarizePressed();
    void onActionFindMaxPressed();
    void onActionPlotPressed();

    void plot();

private:
    Ui::QtWidgetsApplicationClass ui;
    imageProcessing caller;
};
