#include "ComponentBase.h"
#include "PosnetPrinter.h"
#include <cstring>
#include <cwchar>
#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
#endif

#ifndef CLASS_NAME_POSNET_PRINTER
  #define CLASS_NAME_POSNET_PRINTER L"PosnetPrinter"
#endif

// Используем статический массив для имен классов
static const WCHAR_T* const CLASS_NAMES = (const WCHAR_T*)L"PosnetPrinter";

static bool IsOurClass(const WCHAR_T* clsName) {
    return wcscmp((const wchar_t*)clsName, (const wchar_t*)CLASS_NAME_POSNET_PRINTER) == 0;
}

extern "C" {

const WCHAR_T* POSNET_EXPORT_API GetClassNames() {
    OutputDebugStringW(L"[PosnetPrinter] GetClassNames() called\n");
    return CLASS_NAMES;
}

long POSNET_EXPORT_API GetClassObject(const WCHAR_T* clsName, IComponentBase** pIntf) {
    OutputDebugStringW(L"[PosnetPrinter] GetClassObject() called\n");
    if (!pIntf || !clsName) return 0;
    *pIntf = nullptr;

    if (IsOurClass(clsName)) {
        *pIntf = new PosnetPrinter();
        OutputDebugStringW(L"[PosnetPrinter] Created PosnetPrinter instance\n");
        return 1;
    }
    OutputDebugStringW(L"[PosnetPrinter] Class name does not match\n");
    return 0;
}

long POSNET_EXPORT_API DestroyObject(IComponentBase** pIntf) {
    OutputDebugStringW(L"[PosnetPrinter] DestroyObject() called\n");
    if (!pIntf || !*pIntf) return 0;
    delete static_cast<PosnetPrinter*>(*pIntf);
    *pIntf = nullptr;
    return 1;
}

// Some 1C builds require returning 1 here, otherwise initialization may fail.
long POSNET_EXPORT_API SetPlatformCapabilities(const long capabilities) {
    OutputDebugStringW(L"[PosnetPrinter] SetPlatformCapabilities() called\n");
    (void)capabilities;
    return 1;
}

// Attach type: 1 — Native add-in.
// Ensure this symbol is exported for x64 builds (some loaders expect GetAttachType present).
long POSNET_EXPORT_API GetAttachType() {
    OutputDebugStringW(L"[PosnetPrinter] GetAttachType() called\n");
    return 1L;
}

}