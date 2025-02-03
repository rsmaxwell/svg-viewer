#pragma once

#include <QtCore/qglobal.h>


namespace QtWin
{
    enum PixelFormat 
    {
        HBitmapNoAlpha,
        HBitmapPremultipliedAlpha,
        HBitmapAlpha
    };


    HBITMAP toHBITMAP(const QPixmap &p, PixelFormat  format = PixelFormat::HBitmapNoAlpha);

}
