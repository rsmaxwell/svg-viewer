#define INITGUID
#include "Registry.h"

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "Logger.h"

#include <windows.h>
#include <iostream>


HINSTANCE g_hinstDll = NULL;
LONG g_cRef = 0;

QApplication * app;





void dump() {

    TCHAR username[256];
    DWORD username_len = _countof(username);

    if (GetUserName(username, &username_len)) {
        qCDebug(general) << "Username: " << QString::fromWCharArray(username);
    }
    else {
        qCDebug(general) << "Failed to get username. Error: " << GetLastError();
    }

    // Get the current working directory
    TCHAR currentDir[MAX_PATH];
    DWORD dir_len = GetCurrentDirectory(MAX_PATH, currentDir);

    if (dir_len > 0) {
        qCDebug(general) << "Current Directory: " << QString::fromWCharArray(currentDir);
    }
    else {
        qCDebug(general) << "Failed to get current directory. Error: " << GetLastError();
    }
}


Q_LOGGING_CATEGORY(general, "ExplorerSvgSupport.general")

void Initialize(HMODULE module) {
    qCDebug(general) << "Initialize";
    int c = 0;
    WCHAR path[2048];
    ZeroMemory(path, sizeof(path));
    auto length = GetModuleFileNameW(module, path, 2048);
    if (GetLastError() != ERROR_SUCCESS || length <= 0) {
        qCWarning(general) << "Failed to retrieve module name";
    }
    auto modulePath = QString::fromWCharArray(path, length);
    qCDebug(general) << "Module path is: " << modulePath;
    QFileInfo dll(modulePath);
    QDir libraryPath = dll.dir();
    QStringList libraryPaths = (QStringList() << libraryPath.absolutePath());
    QApplication::setLibraryPaths(libraryPaths);
    app = new QApplication(c, (char **)0, 0);
}


BOOL APIENTRY DllMain(HINSTANCE hinstDll,
                      DWORD dwReason,
                      LPVOID pvReserved)
{
    Q_UNUSED(pvReserved)
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        Logger::init();
        qCDebug(general) << "DllMain: DLL_PROCESS_ATTACH";

        dump();

        g_hinstDll = hinstDll;
        Initialize(hinstDll);
        break;

    case DLL_PROCESS_DETACH:
        qCDebug(general) << "DllMain: DLL_PROCESS_DETACH";
        Logger::clean();
        break;
    }

    return TRUE;
}
STDAPI_(HINSTANCE) DllInstance() {
    qCDebug(general) << "DllInstance";
    return g_hinstDll;
}

STDAPI DllCanUnloadNow() {
    HRESULT result = g_cRef ? S_FALSE : S_OK;
    qCDebug(general) << "DllCanUnloadNow --> " << result;
    return result;
}

STDAPI_(ULONG) DllAddRef() {
    LONG cRef = InterlockedIncrement(&g_cRef);
    return cRef;
}

STDAPI_(ULONG) DllRelease() {
    LONG cRef = InterlockedDecrement(&g_cRef);
    if (0 > cRef)
        cRef = 0;
    return cRef;
}



STDAPI DllRegisterServer() {
    qCDebug(general) << "DLLRegisterServer: entry";
    WCHAR szModule[MAX_PATH];

    ZeroMemory(szModule, sizeof(szModule));
    GetModuleFileName(g_hinstDll, szModule, ARRAYSIZE(szModule));

    REGKEY_SUBKEY_AND_VALUE keys[] = {
        {HKEY_CLASSES_ROOT, L"CLSID\\" szCLSID_ExplorerSvgSupport, NULL, REG_SZ, (DWORD_PTR)L"ExplorerSvgSupport"},
        {HKEY_CLASSES_ROOT, L"CLSID\\" szCLSID_ExplorerSvgSupport L"\\InprocServer32", NULL, REG_SZ, (DWORD_PTR)szModule},
        {HKEY_CLASSES_ROOT, L"CLSID\\" szCLSID_ExplorerSvgSupport L"\\InprocServer32", L"ThreadingModel", REG_SZ, (DWORD_PTR)L"Apartment"},
        {HKEY_CLASSES_ROOT, L".SVG\\shellex\\" szCLSID_IThumbnailProvider, NULL, REG_SZ, (DWORD_PTR)szCLSID_ExplorerSvgSupport},
        {HKEY_CLASSES_ROOT, L".SVGZ\\shellex\\" szCLSID_IThumbnailProvider, NULL, REG_SZ, (DWORD_PTR)szCLSID_ExplorerSvgSupport}
    };

    auto result = CreateRegistryKeys(keys, ARRAYSIZE(keys));

    qCDebug(general) << "DLLRegisterServer: exit";
    return result;
}

STDAPI DllUnregisterServer() {
    qCDebug(general) << "DllUnregisterServer";
    REGKEY_SUBKEY keys[] = {{HKEY_CLASSES_ROOT, L"CLSID\\" szCLSID_ExplorerSvgSupport}};
    auto result = DeleteRegistryKeys(keys, ARRAYSIZE(keys));
    return result;
}
