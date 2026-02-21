#include "PosnetPrinter.h"
#include "posnet_protocol.h"
#include <cstring>
#include <cwchar>
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
  #include <objbase.h>  // for CoTaskMemAlloc
  static std::wstring WideFromCp1250(const std::string& s);
  static std::string Cp1250FromWide(const std::wstring& wide);
#endif

PosnetPrinter::PosnetPrinter()
    : m_pConnection(NULL)
    , m_iMemory(NULL)
{}

PosnetPrinter::~PosnetPrinter() {}

bool ADDIN_API PosnetPrinter::Init(void* pConnection) {
    OutputDebugStringW(L"[PosnetPrinter] Init() called\n");
    m_pConnection = pConnection;
    return true;
}

bool ADDIN_API PosnetPrinter::setMemManager(void* pMemManager) {
    OutputDebugStringW(L"[PosnetPrinter] setMemManager() called\n");
    m_iMemory = reinterpret_cast<IMemoryManager*>(pMemManager);
    return true;
}

long ADDIN_API PosnetPrinter::GetInfo() {
    OutputDebugStringW(L"[PosnetPrinter] GetInfo() called\n");
    return 2000; // interface v2
}

void ADDIN_API PosnetPrinter::Done() {
    OutputDebugStringW(L"[PosnetPrinter] Done() called\n");
    m_pConnection = NULL;
    m_iMemory = NULL;
}

bool ADDIN_API PosnetPrinter::RegisterExtensionAs(WCHAR_T** wsExtName) {
    OutputDebugStringW(L"[PosnetPrinter] RegisterExtensionAs() called\n");
    if (!wsExtName) return false;
    *wsExtName = NULL;

    if (!m_iMemory) {
        OutputDebugStringW(L"[PosnetPrinter] Error: MemoryManager is NULL in RegisterExtensionAs\n");
        return false;
    }

    const size_t cch = wcslen(EXTENSION_NAME);
    const unsigned long cb = (unsigned long)((cch + 1) * sizeof(WCHAR_T));
    WCHAR_T* p = NULL;

    if (!m_iMemory->AllocMemory((void**)&p, cb) || !p) return false;

#ifdef _WIN32
    wcscpy_s(p, cch + 1, EXTENSION_NAME);
#else
    wcscpy(p, EXTENSION_NAME);
#endif
    *wsExtName = p;
    return true;
}

long ADDIN_API PosnetPrinter::GetNProps() { return 0; }
long ADDIN_API PosnetPrinter::FindProp(const WCHAR_T*) { return -1; }
const WCHAR_T* ADDIN_API PosnetPrinter::GetPropName(long, long) { return NULL; }
bool ADDIN_API PosnetPrinter::GetPropVal(const long, tVariant*) { return false; }
bool ADDIN_API PosnetPrinter::SetPropVal(const long, tVariant*) { return false; }
bool ADDIN_API PosnetPrinter::IsPropReadable(const long) { return false; }
bool ADDIN_API PosnetPrinter::IsPropWritable(const long) { return false; }

long ADDIN_API PosnetPrinter::GetNMethods() { return 1; }

long ADDIN_API PosnetPrinter::FindMethod(const WCHAR_T* wsMethodName) {
    if (!wsMethodName) return -1;
#ifdef _WIN32
    if (_wcsicmp(wsMethodName, SENDCOMMAND_NAME) == 0)
#else
    if (wcscmp(wsMethodName, SENDCOMMAND_NAME) == 0)
#endif
        return METHOD_SEND_COMMAND;
    return -1;
}

const WCHAR_T* ADDIN_API PosnetPrinter::GetMethodName(const long lMethodNum, const long lMethodAlias) {
    (void)lMethodAlias;
    if (lMethodNum != METHOD_SEND_COMMAND) return NULL;
    
    uint32_t len = 0;
    return AllocCopyWstring(SENDCOMMAND_NAME, &len);
}

long ADDIN_API PosnetPrinter::GetNParams(const long lMethodNum) {
    if (lMethodNum != METHOD_SEND_COMMAND) return 0;
    return SENDCOMMAND_PARAMS;
}

bool ADDIN_API PosnetPrinter::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue) {
    if (!pvarParamDefValue || lMethodNum != METHOD_SEND_COMMAND) return false;
    (void)lParamNum;
    tVarInit(pvarParamDefValue);
    return true;
}

bool ADDIN_API PosnetPrinter::HasRetVal(const long lMethodNum) {
    return (lMethodNum == METHOD_SEND_COMMAND);
}

bool ADDIN_API PosnetPrinter::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) {
    (void)lMethodNum;
    (void)paParams;
    (void)lSizeArray;
    return false;
}

bool ADDIN_API PosnetPrinter::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) {
    if (lMethodNum != METHOD_SEND_COMMAND || !pvarRetValue) return false;
    tVarInit(pvarRetValue);

    if (!paParams || lSizeArray < SENDCOMMAND_PARAMS) {
        AllocWstringToVariant(L"ERR_PARAM", pvarRetValue);
        return true;
    }

    std::wstring ip, command;
    int port = 0;
    VariantToWstring(&paParams[0], ip);
    if (TV_VT(&paParams[1]) == VTYPE_I4)
        port = (int)TV_I4(&paParams[1]);
    else if (TV_VT(&paParams[1]) == VTYPE_I2)
        port = (int)TV_I2(&paParams[1]);
    else if (TV_VT(&paParams[1]) == VTYPE_PWSTR && TV_WSTR(&paParams[1]))
        port = (int)_wtoi(TV_WSTR(&paParams[1]));
    VariantToWstring(&paParams[2], command);

    std::wstring result = SendCommandImpl(ip, port, command);
    return AllocWstringToVariant(result, pvarRetValue);
}

void ADDIN_API PosnetPrinter::SetLocale(const WCHAR_T* locale) {
    m_locale = locale ? locale : L"";
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
    if (TV_VT(v) == VTYPE_PWSTR && TV_WSTR(v)) {
        const WCHAR_T* p = TV_WSTR(v);
        const uint32_t n = v->wstrLen;
        if (n > 0) {
            out.assign(p, p + n);
        } else {
            out.assign(p);
        }
    } else if (TV_VT(v) == VTYPE_I4) {
        wchar_t buf[32];
        swprintf(buf, 32, L"%ld", (long)TV_I4(v));
        out = buf;
    } else if (TV_VT(v) == VTYPE_I2) {
        wchar_t buf[32];
        swprintf(buf, 32, L"%hd", (short)TV_I2(v));
        out = buf;
    }
}

bool PosnetPrinter::WstringToVariant(const std::wstring& s, tVariant* v) {
    if (!v) return false;
    // Deprecated: kept for compatibility with older code paths
    return AllocWstringToVariant(s, v);
}

bool PosnetPrinter::AllocWstringToVariant(const std::wstring& s, tVariant* v) {
    if (!v) return false;
    tVarInit(v);
    TV_VT(v) = VTYPE_PWSTR;

    uint32_t wlen = 0;
    WCHAR_T* buf = AllocCopyWstring(s, &wlen);
    if (!buf) {
        // If allocation fails, return empty string
        v->pwstrVal = NULL;
        v->wstrLen = 0;
        return true;
    }
    v->pwstrVal = buf;
    v->wstrLen = wlen;
    return true;
}

WCHAR_T* PosnetPrinter::AllocCopyWstring(const std::wstring& s, uint32_t* outLen) {
    if (outLen) *outLen = 0;
    if (!m_iMemory) return NULL;

    const size_t cch = s.size();
    const unsigned long cb = (unsigned long)((cch + 1) * sizeof(WCHAR_T));
    WCHAR_T* p = NULL;
    if (!m_iMemory->AllocMemory((void**)&p, cb) || !p) return NULL;
    for (size_t i = 0; i < cch; i++) p[i] = (WCHAR_T)s[i];
    p[cch] = 0;
    if (outLen) *outLen = (uint32_t)cch;
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
