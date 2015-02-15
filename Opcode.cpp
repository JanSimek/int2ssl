/**
 *
 * Copyright (c) 2005-2009 Anchorite (TeamX), <anchorite2001@yandex.ru>
 * Copyright (c) 2014-2015 Nirran, phobos2077
 * Copyright (c) 2015 alexeevdv <mail@alexeevdv.ru>
 * Distributed under the GNU GPL v3. For full terms see the file license.txt
 *
 */

// C++ standard includes
#include <iostream>

// int2ssl includes
#include "Opcode.h"
#include "Utility.h"

// Third party includes

extern int g_nFalloutVersion;

COpcode::COpcode() :
    m_wOperator(O_NOOP),
    m_ulArgument(0)
{
}

COpcode::COpcode(const COpcode& opcode) :
    m_wOperator(opcode.m_wOperator),
    m_ulArgument(opcode.m_ulArgument)
{
}


COpcode::~COpcode()
{
}

COpcode& COpcode::operator = (const COpcode& opcode)
{
    m_wOperator  = opcode.m_wOperator;
    m_ulArgument = opcode.m_ulArgument;

    return (*this);
}

void COpcode::Serialize(CArchive& ar)
{
    if (ReadMSBWord(ar, m_wOperator) != OPERATOR_SIZE)
    {
        printf("Error: Unable read opcode\n");
        throw std::exception();
    }

    if ((m_wOperator < O_OPERATOR) ||
        ((m_wOperator >= O_END_OP) &&
         (m_wOperator != O_STRINGOP) &&
         (m_wOperator != O_FLOATOP)&&
         (m_wOperator != O_INTOP)))
    {
        ar.Flush();
        printf("Error: Invalid opcode at 0x%08x\n", ar.GetFile()->GetPosition() - 2);
        throw std::exception();
    }

    if ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP))
    {
        if (ReadMSBULong(ar, m_ulArgument) != ARGUMENT_SIZE)
        {
            printf("Error: Unable read opcode argument\n");
            throw std::exception();
        }
    }
}

void COpcode::Expect(CArchive& ar, uint16_t wOperator, bool bArgumentFound, uint32_t ulArgument)
{
    Serialize(ar);

    if (m_wOperator != wOperator)
    {
        printf("Error: Unexpected opcode (0x%04X expected, but 0x%04X found)\n", wOperator, m_wOperator);
        throw std::exception();
    }

    if (bArgumentFound && ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP)))
    {
        if (m_ulArgument != ulArgument)
        {
            printf("Error: Unexpected argument of opcode. (0x%08X expected, but 0x%08X found)\n", ulArgument, m_ulArgument);
            throw std::exception();
        }
    }
}

void COpcode::Expect(CArchive& ar, int nCount, uint16_t pwOperators[])
{
    Serialize(ar);
    bool bFound = false;

    for(int i = 0; i < nCount; i++)
    {
        if (m_wOperator == pwOperators[i])
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        printf("Error: Unexpected opcode (");

        for(int i = 0; i < nCount; i++)
        {
            if (i != 0)
            {
                printf(" or ");
            }

            printf("0x%04X", pwOperators[i]);
        }

        printf(" expected, but 0x%04X found)\n", m_wOperator);
        throw std::exception();
    }
}

bool COpcode::HasArgument() const
{
    return ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP));
}

int COpcode::GetSize() const
{
    if ((m_wOperator == O_STRINGOP) || (m_wOperator == O_FLOATOP) || (m_wOperator == O_INTOP))
    {
        return OPERATOR_SIZE + ARGUMENT_SIZE;
    }
    else
    {
        return OPERATOR_SIZE;
    }
}

uint16_t COpcode::GetOperator() const
{
    return m_wOperator;
}

void COpcode::SetOperator(uint16_t op)
{
    m_wOperator = op;
}

uint32_t COpcode::GetArgument() const
{
    return m_ulArgument;
}

const COpcode::COpcodeAttributes COpcode::GetAttributes() const
{
    static CF1OpcodeAttributesMap f1OpcodeAttributes;
    static CF2OpcodeAttributesMap f2OpcodeAttributes;
    
    COpcodeAttributes result;

    if (g_nFalloutVersion == 1)
    {
        if (f1OpcodeAttributes.Lookup(m_wOperator, result))
        {
            return result;
        }
    }

    if (!f2OpcodeAttributes.Lookup(m_wOperator, result))
    {
        printf("Error: Attempt to obtain attributes of unknown opcode\n");
        throw std::exception();
    }

    return result;
}
