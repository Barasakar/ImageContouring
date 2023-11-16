#include "QtWidgetsApplication.h"
#include <QDebug>

using namespace std;

QtWidgetsApplication::QtWidgetsApplication(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.actionFile, &QAction::triggered, this, &QtWidgetsApplication::onActionFileTriggered);
}

QtWidgetsApplication::~QtWidgetsApplication()
{}

void QtWidgetsApplication::errorCheck() {

}

void QtWidgetsApplication::onActionFileTriggered() {
    qDebug() << "Opening File...";
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        tr("All Files (*.*);;Text Files (*.txt)"));
    qDebug() << "Got filename:" << fileName;
    if (!fileName.isEmpty()) {
        qDebug() << "Current selected file path:" << fileName;
        TIFF* tif = TIFFOpen(fileName.toLocal8Bit().constData(), "r");

        if (tif) {
            qDebug() << "Successfully opened TIFF file";
            uint32 columns, rows;
            size_t numPixels;
            uint32* raster;

            TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &columns);
            TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &rows);

            numPixels = columns * rows;
            qDebug() << "Image dimensions:" << columns << "x" << rows;

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
            qDebug() << "Finished processing TIFF file";
            TIFFClose(tif);
            qDebug() << "tif is closed with TIFFClose()";
            if (!images.isEmpty() && !images[0].isNull()) {
                if (!images[0].isNull()) {
                    qDebug() << "Image size:" << images[0].size();
                    qDebug() << "Image format:" << images[0].format();
                    qDebug() << "Pixel value at (0,0):" << images[0].pixel(0, 0);
                }
                else {
                    qDebug() << "Image is null";
                }
                QSize labelSize = ui.label->size();
                if (labelSize.width() > 0 && labelSize.height() > 0) {
                    
                    QPixmap pixmap = QPixmap::fromImage(images[0]);
                    qDebug() << "labelSize.width() > 0 && labelSize.height() > 0";
                    if (!pixmap.isNull()) {
                        ui.label->setPixmap(pixmap.scaled(ui.label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    }
                    else {
                        qDebug() << "Failed to create pixmap from image";
                    }
                }
                else {
                    qDebug() << "Label size is invalid:" << labelSize;
                }
            }
            else {
                qDebug() << "Image is invalid or no images loaded";
            }
        }
        else {
            qDebug() << "Could not open the TIFF file";
        }
    }
    else {
        qDebug() << "No file was selected";
    }
}
