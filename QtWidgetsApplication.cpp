#include "QtWidgetsApplication.h"

using namespace std;
QtWidgetsApplication::QtWidgetsApplication(QWidget *parent)
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
    // fileName is the path to a file being selected.
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File!!!!"),
        "",
        tr("All Files (*.*);;Text Files (*.txt)"));
    if (!fileName.isEmpty()) {  
        // When the fileName is not empty, that means the user has selected a file. 
        /* Here are things to check: 
        - first make sure the file's format/type is correct. 
        - handle the file. The file should contain data for a 3D image. 
        - process the file and return an 3D array I guess?*/
        ui.textEdit->append("Current selected file path: " + fileName);
        TIFF* tif = TIFFOpen(fileName.toLocal8Bit().constData(), "r");
        if (tif) {
            uint32 columns, rows; 
            size_t numPixels;
            uint32* raster;

            // The number of columns in the image, i.e., the number of pixels per row.
            TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &columns);
            // The number of rows of pixels in the image.
            TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &rows);

            numPixels = columns * rows;
            // allocate memory for storing the pixels on the image.
            raster = (uint32*)_TIFFmalloc(numPixels * sizeof(uint32));

            if (raster != nullptr) {
                if (TIFFReadRGBAImage(tif, columns, rows, raster, 0)) {
                    QImage image((uchar*)raster, columns, rows, QImage::Format_ARGB32);
                    ui.label->setPixmap(QPixmap::fromImage(image));
                }
                _TIFFfree(raster);
            }
        }
        TIFFClose(tif);
    }
    else {
        ui.textEdit->append("Could not open the TIFF file");
    }
}
