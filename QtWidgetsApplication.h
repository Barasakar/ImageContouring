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
    QVector<QImage> copiedImages;
    size_t numPixels;
    uint32 columns;
    uint32 rows;

    QtWidgetsApplication(QWidget *parent = nullptr);
    ~QtWidgetsApplication();
    void onActionFileTriggered(); // controls the open.. menu
    void logPrint(QString str);
    void onSliderValueChanged(int n);// controls the slider
    void onActionBinarizePressed(); // binarize button
    void onActionFindMaxPressed(); // find maxima button
    void onActionPlotPressed();    // plot button, but not being used.
    void onActionContourPressed(); // contour button
    void onActionClearPressed();    // clear button
    void onActionRemoveArchPressed();  // remove arches button
    void onActionSpinBoxValChanged(int value);
    void saveTiff();


    void plot();

private:
    Ui::QtWidgetsApplicationClass ui;
    imageProcessing caller;
};
