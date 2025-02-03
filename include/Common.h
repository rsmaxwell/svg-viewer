#ifndef COMMON_H
#define COMMON_H

#include <QtCore/qglobal.h>

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <thumbcache.h>
#include <strsafe.h>

STDAPI_(ULONG) DllAddRef();
STDAPI_(ULONG) DllRelease();
STDAPI_(HINSTANCE) DllInstance();

// {68BF0019-79AB-4e7f-9500-AA3B5227DFE6}
//#define szCLSID_SampleThumbnailProvider L"{68BF0019-79AB-4e7f-9500-AA3B5227DFE6}"
//DEFINE_GUID(CLSID_SampleThumbnailProvider, 0x68bf0019, 0x79ab, 0x4e7f, 0x95, 0x0, 0xaa, 0x3b, 0x52, 0x27, 0xdf, 0xe6);

// {4CA20D9A-98AC-4DD6-9C16-7449F29AC08A} - used by "SvgSee"
// #define szCLSID_ExplorerSvgSupport L"{4CA20D9A-98AC-4DD6-9C16-7449F29AC08A}"
// DEFINE_GUID(CLSID_ExplorerSvgSupport, 0x4ca20d9a, 0x98ac, 0x4dd6, 0x9c, 0x16, 0x74, 0x49, 0xf2, 0x9a, 0xc0, 0x8a);

// {B018B71A-7A1D-4D0B-A5E9-D6EB519A4820}
#define szCLSID_ExplorerSvgSupport L"{B018B71A-7A1D-4D0B-A5E9-D6EB519A4820}"
DEFINE_GUID(CLSID_ExplorerSvgSupport,   0xB018B71A, 0x7A1D, 0x4D0B, 0xA5, 0xE9, 0xD6, 0xEB, 0x51, 0x9A, 0x48, 0x20);

// The class identifier for IThumbnailProvider implementations.
#define szCLSID_IThumbnailProvider L"{E357FCCD-A995-4576-B01F-234630154E96}"
 

#include <QtWidgets/QApplication>
#include <QtSvg/QSvgRenderer>

#endif // COMMON_H
