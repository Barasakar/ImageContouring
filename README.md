# ImageContouring Dev Log
## Potential Issue: QImage and Memory Management
When you create a QImage using the constructor QImage((uchar*)raster, columns, rows, QImage::Format_ARGB32), it doesn't copy the raster data. Instead, it uses the existing memory buffer (raster in your case). This is fine for a single image because you immediately use this QImage to set a pixmap on the label, but when you store the QImage in a vector for later use, you're running into a problem because the raster data is being freed with _TIFFfree(raster).

When you later access this QImage from the vector, the underlying data buffer is no longer valid, leading to undefined behavior.
## Before:
``` c++
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
    } else {
        ui.textEdit->append("Could not open the TIFF file");
    }
```


## After:
```c++
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

```