#include "QtWidgetsApplication.h"
#include <QDebug>

using namespace std;

QtWidgetsApplication::QtWidgetsApplication(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setMouseTracking(false);

    connect(ui.actionFile, &QAction::triggered, this, &QtWidgetsApplication::onActionFileTriggered);
    connect(ui.horizontalSlider, &QSlider::valueChanged, this, &QtWidgetsApplication::onSliderValueChanged);
    connect(ui.binarizeButton, &QPushButton::pressed, this, &QtWidgetsApplication::onActionBinarizePressed);
    connect(ui.findMaximaButton, &QPushButton::pressed, this, &QtWidgetsApplication::onActionFindMaxPressed);
    connect(ui.plotButton, &QPushButton::pressed, this, &QtWidgetsApplication::onActionPlotPressed);
    connect(ui.contourButton, &QPushButton::pressed, this, &QtWidgetsApplication::onActionContourPressed);
    connect(ui.clearButton, &QPushButton::pressed, this, &QtWidgetsApplication::onActionClearPressed);
    connect(ui.removeArch, &QPushButton::pressed, this, &QtWidgetsApplication::onActionRemoveArchPressed);
    connect(ui.sliderNum, &QSpinBox::valueChanged, this, &QtWidgetsApplication::onActionSpinBoxValChanged);
    
}

QtWidgetsApplication::~QtWidgetsApplication()
{}

void QtWidgetsApplication::logPrint(QString str) {
    ui.textEdit->clear();
    ui.textEdit->append(str);
}

void QtWidgetsApplication::onActionBinarizePressed() {
    qDebug() << "Binarize Button pressed";
    if (!images.isEmpty()) {
        caller.getGrayscaleValue(images[0]);
        //caller.binarizeSingle(images[0]);
        caller.binarize(images);
    }
    else {
        qDebug() << "There is no image loaded";
    }
}

void QtWidgetsApplication::onActionFindMaxPressed() {
    qDebug() << "Find Maxima Button pressed";
    if (!images.isEmpty()) {
        for (int i = 0; i < images.size(); i++) {
            caller.drawQuadratic(images[i]);
            caller.drawMedian(images[i], caller.medianValues[i]);
        }
       
    }
    else {
        qDebug() << "There is no image loaded";
    }
}

void QtWidgetsApplication::onActionPlotPressed() {
    qDebug() << "Plot Button pressed";
    if (!images.isEmpty()) {
        plot();
    }
    else {
        qDebug() << "There is no image loaded";
    }
}

void QtWidgetsApplication::onActionContourPressed() {
    if (!images.isEmpty()) {
        imageProcessing::Contouring contouringInstance(&caller, images, 0.0);
        contouringInstance.generateContours(images);
    }
}

void QtWidgetsApplication::onActionRemoveArchPressed() {
    qDebug() << "remove Arch Button pressed";
    if (!images.isEmpty()) {
        caller.removeArch(images, 15);
    }

}


void QtWidgetsApplication::onActionSpinBoxValChanged(int value) {
    onSliderValueChanged(value);
    
}

void QtWidgetsApplication::onActionClearPressed() {
    for (int i = 0; i < copiedImages.size(); i++) {
        images[i] = copiedImages[i].copy();
    }
}

void QtWidgetsApplication::onActionFileTriggered() {
    logPrint("Open file..");
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        tr("All Files (*.*);;Text Files (*.txt)"));
    logPrint("Got file" + fileName);
    if (!fileName.isEmpty()) {
        qDebug() << "Current selected file path:" << fileName;
        TIFF* tif = TIFFOpen(fileName.toLocal8Bit().constData(), "r");

        if (tif) {
            qDebug() << "Successfully opened TIFF file";
            images.clear(); // flush the container before starting a new file.
            uint32 columns_temp, rows_temp;
            size_t numPixels;
            uint32* raster;

            TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &columns_temp);
            TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &rows_temp);

            numPixels = columns_temp * rows_temp;
            columns = columns_temp;
            rows = rows_temp;
            //qDebug() << "Image dimensions:" << columns << "x" << rows;
            ui.textEdit->append("Image dimensions: " + QString::number(rows) + "x" + QString::number(columns));

            do {
                raster = (uint32*)_TIFFmalloc(numPixels * sizeof(uint32));
                if (raster != nullptr) {
                    if (TIFFReadRGBAImage(tif, columns, rows, raster, 0)) {
                        QImage tempImage((uchar*)raster, columns, rows, QImage::Format_ARGB32);
                        QImage copiedImage = tempImage.copy(); // Deep copy of the image
                        images.push_back(copiedImage);
                        copiedImages.push_back(copiedImage);
                    }
                    _TIFFfree(raster);
                }
                else {
                    qDebug() << "Failed to allocate memory for raster";
                }
            } while (TIFFReadDirectory(tif));
            TIFFClose(tif);
            ui.textEdit->append("Number of slides: " + QString::number(images.size()));
            if (!images.isEmpty() && !images[0].isNull()) {

                // Set Qt slider's range
                ui.horizontalSlider->setMinimum(0);
                ui.horizontalSlider->setMaximum(images.size() - 1); 

                ui.sliderNum->setMinimum(0);
                ui.sliderNum->setMaximum(images.size() - 1);
                QSize labelSize = ui.label->size();
                if (labelSize.width() > 0 && labelSize.height() > 0) {
                    
                    QPixmap pixmap = QPixmap::fromImage(images[0]);
                    if (!pixmap.isNull()) {
                        ui.label->setPixmap(pixmap.scaled(ui.label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                    else {
                        logPrint("Failed to create pixmap from image");
                        //qDebug() << "Failed to create pixmap from image";
                    }
                }
                else {
                    qDebug() << "Label size is invalid:" << labelSize;
                }
            }
            else {
                logPrint("Image is invalid or no images loaded");
            }
        }  

    }
    else {
        qDebug() << "No file was selected";
    }
}
void QtWidgetsApplication::onSliderValueChanged(int n) {
    qDebug() << "value changed: " << n ;
    QPixmap pixmap = QPixmap::fromImage(images[n]);
    ui.label->setPixmap(pixmap.scaled(ui.label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void QtWidgetsApplication::plot() {
    if (images.isEmpty()) return;

    QImage currentImage = images[0];
    QPixmap pixmap = QPixmap::fromImage(currentImage);

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::red, 1)); // Increased width for visibility

    for (int i = 0; i < caller.allMaximaLocation.size() - 1; i++) {
        QPoint point1(caller.allMaximaLocation.at(i).first, caller.allMaximaLocation.at(i).second);
        QPoint point2(caller.allMaximaLocation.at(i + 1).first, caller.allMaximaLocation.at(i + 1).second);
        painter.drawLine(point1, point2);
    }

    painter.end();

    if (!painter.isActive()) {
        qDebug() << "Painter is not active. There might be an issue.";
    }

    ui.label->setPixmap(pixmap);
    ui.label->update(); // Ensure the label is updated
}