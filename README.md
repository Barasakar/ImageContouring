# ImageContouring Dev Log


## Loading .tiff file in 3D to Qt UI:
Displaying a TIFF file in 3D within a Qt UI involves several steps and considerations. TIFF (Tagged Image File Format) files are generally 2D images, so to display them in 3D, you would need to apply some form of 3D visualization technique. One common approach is to use the TIFF image as a texture in a 3D environment. Here's a high-level overview of how you might approach this:

Reading the TIFF File:

First, you need to read the TIFF file. Qt does not natively support TIFF for 3D textures, so you might need to use a library like libtiff to read the TIFF file and convert it into a format that Qt can use, like a QImage.
Converting to QImage:

Once you have the TIFF data, convert it into a QImage. This might involve copying the pixel data into a QImage object. Ensure the format of the QImage matches the pixel format of the TIFF data.
Loading the Image as a Texture:

In your 3D OpenGL context (within a QOpenGLWidget), you'll use this QImage as a texture. You can create a texture object (e.g., using QOpenGLTexture) and load the QImage into it.
Creating a 3D Object:

To display the texture, you need a 3D object to apply it to. A simple approach is to use a rectangular plane or a box in 3D space. You'll need to define the vertices for this object and the texture coordinates that map the TIFF image onto the object.
Setting Up the OpenGL Environment:

In your QOpenGLWidget, set up the environment to render the 3D object. This includes initializing shaders (as previously described), setting up the view and projection matrices, and configuring the lighting if needed.
Rendering the 3D Object:

In the paintGL() function of your QOpenGLWidget, you'll render the 3D object with the TIFF texture applied. Bind the texture, set up the necessary transformations to position the object in the 3D space, and then draw the object.
Interactivity:

To enhance the 3D visualization, you might allow the user to interact with the 3D object, like rotating, zooming, or panning the view. This would involve capturing input events in the QOpenGLWidget and updating the view accordingly.



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
