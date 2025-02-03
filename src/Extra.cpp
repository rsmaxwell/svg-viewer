#if defined(NTDDI_VERSION) && NTDDI_VERSION < 0x06010000 // NTDDI_WIN7
#  undef NTDDI_VERSION
#endif
#if !defined(NTDDI_VERSION)
#  define NTDDI_VERSION 0x06010000 // Enable functions for MinGW
#endif

// #include "qwinfunctions.h"
// #include "qwinfunctions_p.h"
// #include "qwineventfilter_p.h"
// #include "windowsguidsdefs_p.h"

#include <QtGui/qguiapplication.h>
#include <QtGui/qwindow.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qbitmap.h>
#include <QtGui/qimage.h>
#include <QtGui/qcolor.h>
#include <QtGui/qregion.h>
#include <QtCore/qmargins.h>
#include <QtCore/qsettings.h>

#include <comdef.h>
// #include "winshobjidl_p.h"

#include <Extra.h>



QT_BEGIN_NAMESPACE


HBITMAP qt_pixmapToWinHBITMAP(const QPixmap &pixmap, QtWin::PixelFormat format) {
    // Determine the QImage format based on the specified PixelFormat
    QImage::Format imageFormat;
    switch (format) {
        case QtWin::PixelFormat::HBitmapNoAlpha:
            imageFormat = QImage::Format_RGB32;
            break;
        case QtWin::PixelFormat::HBitmapPremultipliedAlpha:
            imageFormat = QImage::Format_ARGB32_Premultiplied;
            break;
        case QtWin::PixelFormat::HBitmapAlpha:
            imageFormat = QImage::Format_ARGB32;
            break;
        default:
            return nullptr; // Unsupported format
    }

    // Convert QPixmap to QImage in the specified format
    QImage image = pixmap.toImage().convertToFormat(imageFormat);

    // Get image dimensions
    int width = image.width();
    int height = image.height();

    // Define the BITMAPINFO structure
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative height for top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits per pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    // Create a DIB section
    void *bits = nullptr;
    HBITMAP hbitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

    if (!hbitmap) {
        return nullptr; // Failed to create HBITMAP
    }

    // Copy pixel data from QImage to HBITMAP
    uchar *srcBits = image.bits();
    int bytesPerLine = image.bytesPerLine();
    uchar *destBits = static_cast<uchar *>(bits);

    for (int y = 0; y < height; ++y) {
        memcpy(destBits + y * width * 4, srcBits + y * bytesPerLine, width * 4);
    }

    return hbitmap;
}



HBITMAP QtWin::toHBITMAP(const QPixmap &p, QtWin::PixelFormat format)
{
    return qt_pixmapToWinHBITMAP(p, format);
}

