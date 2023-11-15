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
    QImage image;
    QtWidgetsApplication(QWidget *parent = nullptr);
    ~QtWidgetsApplication();
    void onActionFileTriggered();
    void errorCheck();
private:
    Ui::QtWidgetsApplicationClass ui;
};
