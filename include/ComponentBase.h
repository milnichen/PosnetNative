/*
 *  1C:Enterprise Native API headers (adapted from common public templates).
 *  Keep method signatures and calling conventions intact.
 */

#ifndef __COMPONENT_BASE_H__
#define __COMPONENT_BASE_H__

#include "types.h"

// Macro for DLL export/import
#ifndef POSNET_EXPORT
#if defined(_WIN32) || defined(_WIN64)
#define POSNET_EXPORT __declspec(dllexport)
#else
#define POSNET_EXPORT __attribute__((visibility("default")))
#endif
#endif

// Exported entry points calling convention.
// Some 1C builds on Win32 expect cdecl; keep stdcall by default.
#ifdef POSNET_CDECL
#define POSNET_EXPORT_API
#else
#define POSNET_EXPORT_API __stdcall
#endif

/// Interface of component initialization.
class IInitDoneBase
{
public:
    virtual ~IInitDoneBase() {}
    virtual bool ADDIN_API Init(void* disp) = 0;
    virtual bool ADDIN_API setMemManager(void* mem) = 0;
    virtual long ADDIN_API GetInfo() = 0; // 2000 - interface v2
    virtual void ADDIN_API Done() = 0;
};

/// Interface describing extension of language.
class ILanguageExtenderBase
{
public:
    virtual ~ILanguageExtenderBase() {}
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsExtensionName) = 0;

    virtual long ADDIN_API GetNProps() = 0;
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName) = 0;
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias) = 0;
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal) = 0;
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal) = 0;
    virtual bool ADDIN_API IsPropReadable(const long lPropNum) = 0;
    virtual bool ADDIN_API IsPropWritable(const long lPropNum) = 0;

    virtual long ADDIN_API GetNMethods() = 0;
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName) = 0;
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias) = 0;
    virtual long ADDIN_API GetNParams(const long lMethodNum) = 0;
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue) = 0;
    virtual bool ADDIN_API HasRetVal(const long lMethodNum) = 0;
    virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) = 0;
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) = 0;
};

/// Base interface for component localization.
class LocaleBase
{
public:
    virtual ~LocaleBase() {}
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc) = 0;
};

/// Base interface describing object as a set of properties and methods.
class IComponentBase : public IInitDoneBase, public ILanguageExtenderBase, public LocaleBase
{
public:
    virtual ~IComponentBase() {}
};

extern "C" long POSNET_EXPORT_API GetClassObject(const WCHAR_T*, IComponentBase** pIntf);
extern "C" long POSNET_EXPORT_API DestroyObject(IComponentBase** pIntf);
extern "C" const WCHAR_T* POSNET_EXPORT_API GetClassNames();

// Optional: attach type export (some loaders expect it)
extern "C" long POSNET_EXPORT_API GetAttachType();

#endif //__COMPONENT_BASE_H__
