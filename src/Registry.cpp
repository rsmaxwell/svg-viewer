#include "Registry.h"
#include "Logger.h"

STDAPI CreateRegistryKey(REGKEY_SUBKEY_AND_VALUE* pKey)
{
    qCDebug(general) << "Enter: Create single registry key";
    size_t cbData;
    LPVOID pvData = NULL;
    HRESULT hr = S_OK;

    switch(pKey->dwType)
    {
    case REG_DWORD:
        pvData = (LPVOID)(LPDWORD)&pKey->dwData;
        cbData = sizeof(DWORD);
        break;

    case REG_SZ:
    case REG_EXPAND_SZ:
        hr = StringCbLength((LPCWSTR)pKey->dwData, STRSAFE_MAX_CCH, &cbData);
        if (SUCCEEDED(hr))
        {
            pvData = (LPVOID)(LPCWSTR)pKey->dwData;
            cbData += sizeof(WCHAR);
        }
        break;

    default:
        hr = E_INVALIDARG;
    }

    if (SUCCEEDED(hr))
    {
        QString keyName = QString::fromWCharArray(pKey->lpszSubKey);
        qCDebug(general) << "Setting key: " << keyName;
        LSTATUS status = SHSetValue(pKey->hKey, pKey->lpszSubKey, pKey->lpszValue, pKey->dwType, pvData, (DWORD)cbData);
        if (NOERROR != status)
        {
            hr = HRESULT_FROM_WIN32(status);
            qCDebug(general) << "Failed to set registry key.";
        }
    }

    qCDebug(general) << "Leave: Create single registry key";
    return hr;
}

STDAPI CreateRegistryKeys(REGKEY_SUBKEY_AND_VALUE* aKeys, ULONG cKeys)
{
    qCDebug(general) << "Enter: Create multiple registry keys";
    HRESULT hr = S_OK;

    for (ULONG iKey = 0; iKey < cKeys; iKey++)
    {
        HRESULT hrTemp = CreateRegistryKey(&aKeys[iKey]);
        if (FAILED(hrTemp))
        {
            hr = hrTemp;
        }
    }

    qCDebug(general) << "Leave: Create multiple registry keys";
    return hr;
}

STDAPI DeleteRegistryKeys(REGKEY_SUBKEY* aKeys, ULONG cKeys)
{
    qCDebug(general) << "Enter: Delete registry keys";
    HRESULT hr = S_OK;
    LSTATUS status;

    for (ULONG iKey = 0; iKey < cKeys; iKey++)
    {
        QString keyName = QString::fromWCharArray(aKeys[iKey].lpszSubKey);
        qCDebug(general) << "Deleting key: " << keyName;
        status = RegDeleteTree(aKeys[iKey].hKey, aKeys[iKey].lpszSubKey);
        if (NOERROR != status)
        {
            hr = HRESULT_FROM_WIN32(status);
            qCDebug(general) << "Failed to delete registry key";
        }
    }
    qCDebug(general) << "Leave: Delete registry keys";
    return hr;
}
