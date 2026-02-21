/*
 * Интерфейс Native API 1С:Предприятие 8 (по документации 1c-dn.com).
 * При наличии официального ComponentBase.h из поставки 1С — замените этот файл.
 */
#ifndef COMPONENT_BASE_H
#define COMPONENT_BASE_H

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  #ifndef WCHAR_T
    #define WCHAR_T wchar_t
  #endif
  #ifndef STDCALL
    #define STDCALL __stdcall
  #endif
#else
  #ifndef WCHAR_T
    #define WCHAR_T unsigned short
  #endif
  #define STDCALL
#endif

// Типы вариантов (TV_VT)
#define VTYPE_EMPTY     0
#define VTYPE_I2        2
#define VTYPE_I4        3
#define VTYPE_R4        4
#define VTYPE_R8        5
#define VTYPE_BOOL      11
#define VTYPE_PSTR      0x0010
#define VTYPE_PWSTR     0x0020

typedef unsigned long AppCapabilities;
enum {
    eAppCapabilitiesInvalid = -1,
    eAppCapabilities1 = 1,
    eAppCapabilitiesLast = eAppCapabilities1
};

#pragma pack(push, 4)
struct tVariant {
    unsigned short vt;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned short reserved3;
    union {
        long    lVal;
        double  dblVal;
        short   boolVal;
        char*   pstrVal;
        WCHAR_T* pwstrVal;
    };
};
#pragma pack(pop)

// Менеджер памяти: возвращаемые строки выделять только через него
struct IMemoryManager {
    void* (STDCALL *AllocMemory)(void* pThis, unsigned long size);
    void  (STDCALL *FreeMemory)(void* pThis, void* pMem);
};

// Базовый интерфейс объекта компоненты (полный список методов по документации)
struct IComponentBase {
    // Инициализация
    virtual bool Init(void* pConnection) = 0;
    virtual bool setMemManager(void* pMemManager) = 0;
    virtual long GetInfo() = 0;
    virtual void Done() = 0;
    virtual bool RegisterExtensionAs(WCHAR_T** wsExtName) = 0;

    // Свойства (у нас 0)
    virtual long GetNProps() = 0;
    virtual long FindProp(const WCHAR_T* wsPropName) = 0;
    virtual const WCHAR_T* GetPropName(long lPropNum, long lPropAlias) = 0;
    virtual bool GetPropVal(long lPropNum, tVariant* pvarPropVal) = 0;
    virtual bool SetPropVal(long lPropNum, tVariant* pvarPropVal) = 0;
    virtual bool IsPropReadable(long lPropNum) = 0;
    virtual bool IsPropWritable(long lPropNum) = 0;

    // Методы
    virtual long GetNMethods() = 0;
    virtual long FindMethod(const WCHAR_T* wsMethodName) = 0;
    virtual const WCHAR_T* GetMethodName(long lMethodNum, long lMethodAlias) = 0;
    virtual long GetNParams(long lMethodNum) = 0;
    virtual bool GetParamDefValue(long lMethodNum, long lParamNum, tVariant* pvarParamDefValue) = 0;
    virtual bool HasRetVal(long lMethodNum) = 0;
    virtual bool CallAsProc(long lMethodNum, tVariant* paParams, long lSizeArray) = 0;
    virtual bool CallAsFunc(long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, long lSizeArray) = 0;

    // Локализация и режимы
    virtual void SetLocale(const WCHAR_T* locale) = 0;
    virtual bool IsModeSet(unsigned long mode) = 0;

    virtual ~IComponentBase() {}
};

#endif
