
#ifndef __IMEMORY_MANAGER_H__
#define __IMEMORY_MANAGER_H__

/// Interface representing 1C Native API memory manager.
class IMemoryManager
{
public:
    virtual ~IMemoryManager() {}

    virtual bool ADDIN_API AllocMemory(void** pMemory, unsigned long ulCountByte) = 0;
    virtual void ADDIN_API FreeMemory(void** pMemory) = 0;
};

#endif //__IMEMORY_MANAGER_H__
