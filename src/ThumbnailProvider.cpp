//#include "Common.h"
#include "ThumbnailProvider.h"

//#include <objbase.h>
#include <gdiplus.h>
//#include <assert.h>

// #include <QtCore/QDateTime>
//#ifndef NDEBUG
//#include <QtCore/QString>
//#include <QtCore/QDir>
//#include <QtCore/QFile>
//#endif

//#include <QtGui/QImage>
#include <QtGui/QPainter>
//#include <QtGui/QPixmap>

#include "Extra.h"
#include "Logger.h"


using namespace Gdiplus;

CThumbnailProvider::CThumbnailProvider()
{
    DllAddRef();
    m_cRef = 1;
    m_pSite = NULL;
    loaded = false;
}

CThumbnailProvider::~CThumbnailProvider()
{
    if (m_pSite)
    {
        m_pSite->Release();
        m_pSite = NULL;
    }
    DllRelease();
}

/*
 * ===============
 * IUnkown methods
 * ===============
 */
HRESULT CThumbnailProvider::QueryInterfaceFactory(REFIID riid, void** ppvObject) {
    qCDebug(general) << "entry";
    *ppvObject = NULL;

    CThumbnailProvider * provider = new CThumbnailProvider();
    if (provider == nullptr) {
        return E_OUTOFMEMORY;
    }

    auto result = provider->QueryInterface(riid, ppvObject);

    provider->Release();


    qCDebug(general) << "exit";
    return result;
}

QString ToString(char *buffer, size_t buffer_size, const GUID& guid)
{        
    int n = snprintf(buffer, buffer_size, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return QString(buffer);
}

STDMETHODIMP CThumbnailProvider::QueryInterface(REFIID riid, void** ppvObject) {
    char buffer[256];
    qCDebug(general) << "entry riid" << ToString(buffer, sizeof(buffer), riid);

    static const QITAB qit[] =
    {
        QITABENT(CThumbnailProvider, IInitializeWithFile),
        QITABENT(CThumbnailProvider, IInitializeWithStream),
        QITABENT(CThumbnailProvider, IThumbnailProvider),
        QITABENT(CThumbnailProvider, IObjectWithSite),
        {0},
    };
    HRESULT result = QISearch(this, qit, riid, ppvObject);
    qCDebug(general) << "exit";
    return result;
}

STDMETHODIMP_(ULONG) CThumbnailProvider::AddRef()
{
    LONG cRef = InterlockedIncrement(&m_cRef);
    return (ULONG)cRef;
}

STDMETHODIMP_(ULONG) CThumbnailProvider::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
        delete this;
    return (ULONG)cRef;
}

/*
 * ===============
 * End IUnkown methods
 * ===============
 */



 // Initialize with file (provides file path)
STDMETHODIMP CThumbnailProvider::Initialize(LPCWSTR pszFilePath, DWORD grfMode) {
    qCDebug(general) << "Initialize with file";
    return S_OK;
}

/*
 * ============================
 * IInitializeWithSteam methods
 * ============================
 */

STDMETHODIMP CThumbnailProvider::Initialize(IStream *pstm, DWORD grfMode) {
    qCDebug(general) << "Initialize with stream";

    ULONG len;
    STATSTG stat;
    Q_UNUSED(grfMode)

    if(loaded) {
        return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
    }

    if(pstm->Stat(&stat, STATFLAG_DEFAULT) != S_OK){
        return S_FALSE;
    }

    char * data = new char[stat.cbSize.QuadPart];

    if(pstm->Read(data, stat.cbSize.QuadPart, &len) != S_OK){
        return S_FALSE;
    }

    QByteArray bytes = QByteArray(data, stat.cbSize.QuadPart);

    loaded = renderer.load(bytes);

    return S_OK;
}

/*
 * ============================
 * End IInitializeWith... methods
 * ============================
 */

/*
 * ============================
 * IThumbnailProvider methods
 * ============================
 */

STDMETHODIMP CThumbnailProvider::GetThumbnail(UINT cx, 
                                              HBITMAP *phbmp,
                                              WTS_ALPHATYPE *pdwAlpha)
{
    qCDebug(general) << "GetThumbnail";

    *phbmp = NULL;
    *pdwAlpha = WTSAT_ARGB;

    if(!loaded) {
        qCDebug(general) << "Not loaded";
        return S_FALSE;
    }

    // Fit the render into a (cx * cx) square while maintaining the aspect ratio.
    QSize size = renderer.defaultSize();
    size.scale(cx, cx, Qt::AspectRatioMode::KeepAspectRatio);

    qCDebug(general) << "Size: " << cx;

    QImage * device = new QImage(size, QImage::Format_ARGB32);
    device->fill(Qt::transparent);
    QPainter painter(device);

    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform |
                           QPainter::TextAntialiasing);

    assert(device->paintingActive() && painter.isActive());
    if(loaded){
        renderer.render(&painter);
    } else {
        QFont font;
        QColor color_font = QColor(255, 0, 0);
        int font_size = cx / 10;

        font.setStyleHint(QFont::Monospace);
        font.setPixelSize(font_size);

        painter.setPen(color_font);
        painter.setFont(font);
        painter.drawText(font_size, (cx - font_size) / 2, "Invalid SVG file.");
    }
    painter.end();

    assert(!device->isNull());

    *phbmp = QtWin::toHBITMAP(QPixmap::fromImage(*device), QtWin::HBitmapAlpha);

    assert(*phbmp != NULL);

    delete device;

    if( *phbmp != NULL )
        return S_OK;
    return S_FALSE;
}

/*
 * ============================
 * End IThumbnailProvider methods
 * ============================
 */

/*
 * ============================
 * IObjectWithSite methods
 * ============================
 */

STDMETHODIMP CThumbnailProvider::GetSite(REFIID riid, 
                                         void** ppvSite)
{
    if (m_pSite)
    {
        return m_pSite->QueryInterface(riid, ppvSite);
    }
    return E_NOINTERFACE;
}

STDMETHODIMP CThumbnailProvider::SetSite(IUnknown* pUnkSite)
{
    if (m_pSite)
    {
        m_pSite->Release();
        m_pSite = NULL;
    }

    m_pSite = pUnkSite;
    if (m_pSite)
    {
        m_pSite->AddRef();
    }
    return S_OK;
}

/*
 * ============================
 * End IObjectWithSite methods
 * ============================
 */
