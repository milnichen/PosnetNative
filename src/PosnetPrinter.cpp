#include "PosnetPrinter.h"
#include "posnet_protocol.h"
#include <cstring>
#include <cwchar>
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
  static std::wstring WideFromCp1250(const std::string& s);
  static std::string Cp1250FromWide(const std::wstring& wide);
#endif

PosnetPrinter::PosnetPrinter()
    : m_pConnection(NULL)
    , m_pMemManager(NULL)
{}

PosnetPrinter::~PosnetPrinter() {}

// Вызов AllocMemory менеджера памяти. Платформа 1С передаёт объект с vtable (C++): первый указатель — таблица методов.
void* PosnetPrinter::AllocMemoryViaManager(unsigned long size) {
    if (!m_pMemManager || size == 0) return NULL;
    
    // Приводим void* к интерфейсу IMemoryManager
    IMemoryManager* pMM = static_cast<IMemoryManager*>(m_pMemManager);
    void* pRes = nullptr;
    
    // Вызываем через штатный метод интерфейса
    if (pMM->AllocMemory(&pRes, size)) {
        return pRes;
    }
    return nullptr;
}

bool PosnetPrinter::Init(void* pConnection) {
    m_pConnection = pConnection;
    return true;
}

bool PosnetPrinter::setMemManager(void* pMemManager) {
    m_pMemManager = pMemManager;
    return true;
}

long PosnetPrinter::GetInfo() {
    return 100; // версия 1.0.0 -> 100
}

void PosnetPrinter::Done() {
    m_pConnection = NULL;
    m_pMemManager = NULL;
}

bool PosnetPrinter::RegisterExtensionAs(WCHAR_T** wsExtName) {
    if (!wsExtName) return false;
    *wsExtName = NULL;
    size_t len = (wcslen(EXTENSION_NAME) + 1) * sizeof(WCHAR_T);
    if (len > 0x7FFFFFFFu) return false;
    WCHAR_T* p = (WCHAR_T*)AllocMemoryViaManager((unsigned long)len);
    if (!p) return false;
#ifdef _WIN32
    wcscpy_s(p, wcslen(EXTENSION_NAME) + 1, EXTENSION_NAME);
#else
    wcscpy(p, EXTENSION_NAME);
#endif
    *wsExtName = p;
    return true;
}

long PosnetPrinter::GetNProps() { return 0; }
long PosnetPrinter::FindProp(const WCHAR_T*) { return -1; }
const WCHAR_T* PosnetPrinter::GetPropName(long, long) { return NULL; }
bool PosnetPrinter::GetPropVal(long, tVariant*) { return false; }
bool PosnetPrinter::SetPropVal(long, tVariant*) { return false; }
bool PosnetPrinter::IsPropReadable(long) { return false; }
bool PosnetPrinter::IsPropWritable(long) { return false; }

long PosnetPrinter::GetNMethods() { return 1; }

long PosnetPrinter::FindMethod(const WCHAR_T* wsMethodName) {
    if (!wsMethodName) return -1;
#ifdef _WIN32
    if (_wcsicmp(wsMethodName, SENDCOMMAND_NAME) == 0)
#else
    if (wcscmp(wsMethodName, SENDCOMMAND_NAME) == 0)
#endif
        return METHOD_SEND_COMMAND;
    return -1;
}

const WCHAR_T* PosnetPrinter::GetMethodName(long lMethodNum, long lMethodAlias) {
    (void)lMethodAlias;
    if (lMethodNum != METHOD_SEND_COMMAND) return NULL;
    return AllocCopyWstring(SENDCOMMAND_NAME);
}

long PosnetPrinter::GetNParams(long lMethodNum) {
    if (lMethodNum != METHOD_SEND_COMMAND) return 0;
    return SENDCOMMAND_PARAMS;
}

bool PosnetPrinter::GetParamDefValue(long lMethodNum, long lParamNum, tVariant* pvarParamDefValue) {
    if (!pvarParamDefValue || lMethodNum != METHOD_SEND_COMMAND) return false;
    pvarParamDefValue->vt = VTYPE_EMPTY;
    return true;
}

bool PosnetPrinter::HasRetVal(long lMethodNum) {
    return (lMethodNum == METHOD_SEND_COMMAND);
}

bool PosnetPrinter::CallAsProc(long lMethodNum, tVariant* paParams, long lSizeArray) {
    (void)lMethodNum;
    (void)paParams;
    (void)lSizeArray;
    return false;
}

bool PosnetPrinter::CallAsFunc(long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, long lSizeArray) {
    if (lMethodNum != METHOD_SEND_COMMAND || !pvarRetValue) return false;
    pvarRetValue->vt = VTYPE_EMPTY;

    if (!paParams || lSizeArray < SENDCOMMAND_PARAMS) {
        WstringToVariant(L"ERR_PARAM", pvarRetValue);
        return true;
    }

    std::wstring ip, command;
    int port = 0;
    VariantToWstring(&paParams[0], ip);
    if (paParams[1].vt == VTYPE_I4 || paParams[1].vt == VTYPE_I2)
        port = (int)paParams[1].lVal;
    else if (paParams[1].vt == VTYPE_PWSTR && paParams[1].pwstrVal)
        port = (int)_wtoi(paParams[1].pwstrVal);
    VariantToWstring(&paParams[2], command);

    std::wstring result = SendCommandImpl(ip, port, command);
    return WstringToVariant(result, pvarRetValue);
}

void PosnetPrinter::SetLocale(const WCHAR_T* locale) {
    m_locale = locale ? locale : L"";
}

bool PosnetPrinter::IsModeSet(unsigned long mode) {
    (void)mode;
    return false;
}

std::wstring PosnetPrinter::SendCommandImpl(const std::wstring& ip, int port, const std::wstring& command) {
    if (ip.empty()) return L"ERR_PARAM_IP";
    if (port <= 0 || port > 65535) return L"ERR_PARAM_PORT";

#ifdef _WIN32
    std::string ipA = Cp1250FromWide(ip);
    std::string cmdA = Cp1250FromWide(command);
#else
    std::string ipA(ip.begin(), ip.end());
    std::string cmdA(command.begin(), command.end());
#endif
    std::string res = posnet::SendTcpCommand(ipA, port, cmdA);
#ifdef _WIN32
    return WideFromCp1250(res);
#else
    return std::wstring(res.begin(), res.end());
#endif
}

void PosnetPrinter::VariantToWstring(const tVariant* v, std::wstring& out) {
    out.clear();
    if (!v) return;
    if (v->vt == VTYPE_PWSTR && v->pwstrVal) {
        const WCHAR_T* p = v->pwstrVal;
        while (*p) { out += (wchar_t)*p; ++p; }
    } else if (v->vt == VTYPE_I4 || v->vt == VTYPE_I2) {
        wchar_t buf[32];
        swprintf(buf, 32, L"%ld", (long)v->lVal);
        out = buf;
    }
}

bool PosnetPrinter::WstringToVariant(const std::wstring& s, tVariant* v) {
    if (!v) return false;
    v->vt = VTYPE_PWSTR;
    if (s.empty()) {
        v->pwstrVal = (WCHAR_T*)L"";
        return true;
    }
    WCHAR_T* buf = AllocCopyWstring(s);
    if (!buf) {
        v->pwstrVal = (WCHAR_T*)L"";
        return true;
    }
    v->pwstrVal = buf;
    return true;
}

WCHAR_T* PosnetPrinter::AllocCopyWstring(const std::wstring& s) {
    size_t len = (s.size() + 1) * sizeof(WCHAR_T);
    if (len > 0x7FFFFFFFu) return NULL;
    WCHAR_T* p = (WCHAR_T*)AllocMemoryViaManager((unsigned long)len);
    if (!p) return NULL;
    for (size_t i = 0; i < s.size(); i++) p[i] = (WCHAR_T)s[i];
    p[s.size()] = 0;
    return p;
}

#ifdef _WIN32
static std::wstring WideFromCp1250(const std::string& s) {
    if (s.empty()) return L"";
    int wlen = MultiByteToWideChar(1250, 0, s.c_str(), (int)s.size(), NULL, 0);
    if (wlen <= 0) return std::wstring(s.begin(), s.end());
    std::wstring out((size_t)wlen, 0);
    MultiByteToWideChar(1250, 0, s.c_str(), (int)s.size(), &out[0], wlen);
    return out;
}
static std::string Cp1250FromWide(const std::wstring& wide) {
    if (wide.empty()) return "";
    int len = WideCharToMultiByte(1250, 0, wide.c_str(), (int)wide.size(), NULL, 0, NULL, NULL);
    if (len <= 0) return ""; /* ошибка конвертации — не сужаем wchar_t в char */
    std::string out((size_t)len, 0);
    WideCharToMultiByte(1250, 0, wide.c_str(), (int)wide.size(), &out[0], len, NULL, NULL);
    return out;
}
#endif
