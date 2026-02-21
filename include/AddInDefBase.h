
#ifndef __ADAPTER_DEF_H__
#define __ADAPTER_DEF_H__

#include "types.h"

/// Base interface for 1C:Enterprise platform communication.
class IAddInDefBase
{
public:
    virtual ~IAddInDefBase() {}

    virtual bool ADDIN_API AddError(
        unsigned short wcode,
        const WCHAR_T* source,
        const WCHAR_T* descr,
        long scode) = 0;

    virtual bool ADDIN_API Read(
        WCHAR_T* wszPropName,
        tVariant* pVal,
        long* pErrCode,
        WCHAR_T** errDescriptor) = 0;

    virtual bool ADDIN_API Write(
        WCHAR_T* wszPropName,
        tVariant* pVar) = 0;

    virtual bool ADDIN_API RegisterProfileAs(WCHAR_T* wszProfileName) = 0;

    virtual bool ADDIN_API SetEventBufferDepth(long lDepth) = 0;
    virtual long ADDIN_API GetEventBufferDepth() = 0;

    virtual bool ADDIN_API ExternalEvent(
        WCHAR_T* wszSource,
        WCHAR_T* wszMessage,
        WCHAR_T* wszData) = 0;

    virtual void ADDIN_API CleanEventBuffer() = 0;

    virtual bool ADDIN_API SetStatusLine(WCHAR_T* wszStatusLine) = 0;
    virtual void ADDIN_API ResetStatusLine() = 0;
};

#endif //__ADAPTER_DEF_H__
