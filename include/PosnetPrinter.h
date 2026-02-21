#ifndef POSNET_PRINTER_H
#define POSNET_PRINTER_H

#include "ComponentBase.h"
#include "IMemoryManager.h"
#include <string>

#define CLASS_NAME_POSNET_PRINTER L"PosnetPrinter"
#define EXTENSION_NAME L"PosnetPrinter"
#define METHOD_SEND_COMMAND 0
#define SENDCOMMAND_NAME L"SendCommand"
#define SENDCOMMAND_PARAMS 3

class PosnetPrinter : public IComponentBase {
public:
    PosnetPrinter();

    bool ADDIN_API Init(void* pConnection) override;
    bool ADDIN_API setMemManager(void* pMemManager) override;
    long ADDIN_API GetInfo() override;
    void ADDIN_API Done() override;
    bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsExtName) override;

    long ADDIN_API GetNProps() override;
    long ADDIN_API FindProp(const WCHAR_T* wsPropName) override;
    const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias) override;
    bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal) override;
    bool ADDIN_API SetPropVal(const long lPropNum, tVariant* pvarPropVal) override;
    bool ADDIN_API IsPropReadable(const long lPropNum) override;
    bool ADDIN_API IsPropWritable(const long lPropNum) override;

    long ADDIN_API GetNMethods() override;
    long ADDIN_API FindMethod(const WCHAR_T* wsMethodName) override;
    const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias) override;
    long ADDIN_API GetNParams(const long lMethodNum) override;
    bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue) override;
    bool ADDIN_API HasRetVal(const long lMethodNum) override;
    bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) override;
    bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) override;

    void ADDIN_API SetLocale(const WCHAR_T* locale) override;
    ~PosnetPrinter();

private:
    void* m_pConnection;
    IMemoryManager* m_iMemory;
    std::wstring m_locale;

    std::wstring SendCommandImpl(const std::wstring& ip, int port, const std::wstring& command);
    void VariantToWstring(const tVariant* v, std::wstring& out);
    bool WstringToVariant(const std::wstring& s, tVariant* v);
    bool AllocWstringToVariant(const std::wstring& s, tVariant* v);
    WCHAR_T* AllocCopyWstring(const std::wstring& s, uint32_t* outLen /*symbols*/);
};

#endif
