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
    connect(ui.pushButton, &QPushButton::pressed, this, &QtWidgetsApplication::onActionProcessPressed);
    
}

QtWidgetsApplication::~QtWidgetsApplication()
{}

void QtWidgetsApplication::logPrint(QString str) {
    ui.textEdit->clear();
    ui.textEdit->append(str);
}

void QtWidgetsApplication::onActionProcessPressed() {
    qDebug() << "Button pressed";
    if (!images.isEmpty()) {
        caller.getGrayscaleValue(images[0]);
        //caller.binarizeSingle(images[0]);
        caller.binarize(images);
    }
    else {
        qDebug() << "There is no image loaded";
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

