#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "Hacks/CArchive.h"
#include "Hacks/CMap.h"

class CNamespace
{
public:
    CNamespace();
    virtual ~CNamespace();

public:
    virtual void Serialize(CArchive& ar);

public:
    INT_PTR GetSize() const;
    std::string GetStringByIndex(INT_PTR nIndex) ;
    ULONG GetOffsetByIndex(INT_PTR nIndex) ;

    void Dump(CArchive& ar);

public:
    std::string operator [] (ULONG ulOffset) const;

private:
    // CMapDWordToString
    typedef CMap<DWORD, DWORD, std::string, LPCTSTR> CMapDWordToString;

    CMapDWordToString m_Map;
    CDWordArray m_Order;
};

#endif //NAMESPACE_H
