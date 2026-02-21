#ifndef POSNET_PRINTER_H
#define POSNET_PRINTER_H

#include "ComponentBase.h"
#include <string>

#define CLASS_NAME_POSNET_PRINTER L"PosnetPrinter"
#define EXTENSION_NAME L"Posnet"
#define METHOD_SEND_COMMAND 0
#define SENDCOMMAND_NAME L"SendCommand"
#define SENDCOMMAND_PARAMS 3

class PosnetPrinter : public IComponentBase {
public:
    PosnetPrinter();
    virtual ~PosnetPrinter();

    bool Init(void* pConnection) override;
    bool setMemManager(void* pMemManager) override;
    long GetInfo() override;
    void Done() override;
    bool RegisterExtensionAs(WCHAR_T** wsExtName) override;

    long GetNProps() override;
    long FindProp(const WCHAR_T* wsPropName) override;
    const WCHAR_T* GetPropName(long lPropNum, long lPropAlias) override;
    bool GetPropVal(long lPropNum, tVariant* pvarPropVal) override;
    bool SetPropVal(long lPropNum, tVariant* pvarPropVal) override;
    bool IsPropReadable(long lPropNum) override;
    bool IsPropWritable(long lPropNum) override;

    long GetNMethods() override;
    long FindMethod(const WCHAR_T* wsMethodName) override;
    const WCHAR_T* GetMethodName(long lMethodNum, long lMethodAlias) override;
    long GetNParams(long lMethodNum) override;
    bool GetParamDefValue(long lMethodNum, long lParamNum, tVariant* pvarParamDefValue) override;
    bool HasRetVal(long lMethodNum) override;
    bool CallAsProc(long lMethodNum, tVariant* paParams, long lSizeArray) override;
    bool CallAsFunc(long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, long lSizeArray) override;

    void SetLocale(const WCHAR_T* locale) override;
    bool IsModeSet(unsigned long mode) override;

private:
    void* m_pConnection;
    void* m_pMemManager;
    std::wstring m_locale;

    std::wstring SendCommandImpl(const std::wstring& ip, int port, const std::wstring& command);
    void VariantToWstring(const tVariant* v, std::wstring& out);
    bool WstringToVariant(const std::wstring& s, tVariant* v);
    WCHAR_T* AllocCopyWstring(const std::wstring& s);
    void* AllocMemoryViaManager(unsigned long size);
};

#endif
