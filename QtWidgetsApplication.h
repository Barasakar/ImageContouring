/* QtWidgetsApplication.h */

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication.h"
#include "qfiledialog.h"
#include <iostream>
#include <qdebug.h>
#include "tiffio.h"

class QtWidgetsApplication : public QMainWindow
{
    Q_OBJECT

public:
    QVector<QImage> images;
    QtWidgetsApplication(QWidget *parent = nullptr);
    ~QtWidgetsApplication();
    void onActionFileTriggered();
    void logPrint(QString str);
    void onSliderValueChanged(int n);

private:
    Ui::QtWidgetsApplicationClass ui;
};
