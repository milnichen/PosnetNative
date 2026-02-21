#include "ComponentBase.h"
#include "PosnetPrinter.h"
#include <cstring>
#include <cwchar>

// Жестко задаем экспорт для Windows
#if defined(_WIN32) || defined(_WIN64)
  #define POSNET_EXPORT __declspec(dllexport)
  #define POSNET_CALL __stdcall
#else
  #define POSNET_EXPORT __attribute__((visibility("default")))
  #define POSNET_CALL
#endif

// Имена классов в формате 1С: список через перевод строки (\n)
static const WCHAR_T CLASS_NAMES[] = L"PosnetPrinter\n";

static bool IsOurClass(const WCHAR_T* clsName) {
    return wcscmp(clsName, L"PosnetPrinter") == 0;
}

extern "C" {

POSNET_EXPORT const WCHAR_T* POSNET_CALL GetClassNames() {
    return (const WCHAR_T*)CLASS_NAMES;
}

POSNET_EXPORT long POSNET_CALL GetClassObject(const WCHAR_T* clsName, IComponentBase** pIntf) {
    if (!pIntf || !clsName) return 0;
    *pIntf = nullptr;

    if (IsOurClass(clsName)) {
        *pIntf = new PosnetPrinter();
        return 1;
    }
    return 0;
}

POSNET_EXPORT long POSNET_CALL DestroyObject(IComponentBase** pIntf) {
    if (!pIntf || !*pIntf) return 0;
    delete *pIntf;
    *pIntf = nullptr;
    return 1;
}

POSNET_EXPORT AppCapabilities POSNET_CALL SetPlatformCapabilities(const AppCapabilities capabilities) {
    return eAppCapabilitiesLast;
}

}