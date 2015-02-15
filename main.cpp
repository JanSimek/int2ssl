#include "stdafx.h"
#include "FalloutScript.h"
#include "XGetopt.h"
#include "Utility.h"
#include <iostream>

using namespace std;

// Globals
BOOL g_bDump = FALSE;
int g_nFalloutVersion = 2;
std::string  g_strIndentFill("\t");
BOOL g_bIgnoreWrongNumOfArgs = FALSE;
BOOL g_bInsOmittedArgsBackward = FALSE;
BOOL g_bStopOnError = FALSE;

std::string g_inputFileName;
std::string g_outputFileName;


// Functions
void PrintUsage(std::string filename);
BOOL ProcessCommandLine(int argc, char* argv[]);

int main(int argc, char* argv[])
{ 
    std::cout << "Fallout script decompiler, version 8.3.0 (sfall edition)" << std::endl
              << "Copyright (C) Anchorite (TeamX), 2005-2009" << std::endl
              << "anchorite2001@yandex.ru" << std::endl
              << "Continued by Nirran, phobos2077 (2014-2015)" << std::endl
              << "Crossplatformed by alexeevdv (2015)" << std::endl;

    if (argc < 2 || !ProcessCommandLine(argc, argv))
    {
        PrintUsage(argv[0]);
        cin.get();
        return 1;
    }

    CFile fileInput;
    CFile fileOutput;

    if (!fileInput.Open(g_inputFileName, CFile::modeRead | CFile::shareDenyWrite))
    {
        printf("Error: Unable open input file %s.\n", g_inputFileName.c_str());
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return 1;
    }

    if (!fileOutput.Open(g_outputFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyWrite))
    {
        std::cout << format("Error: Unable open output file %s", g_outputFileName) << std::endl;
        if (g_bStopOnError == TRUE)
        {
            std::cout << "Press ENTER to continue." << std::endl;
            cin.get();
        }
        return 1;
    }

    try
    {
        CArchive arInput(&fileInput, CArchive::load);
        CFalloutScript Script;

        std::cout << format("Loading file %s...", g_inputFileName) << std::endl;
        Script.Serialize(arInput);

        std::cout << format("File %s loaded successfuly", g_inputFileName) << std::endl
                  << std::endl;

        CArchive arOutput(&fileOutput, CArchive::store);

        if (g_bDump)
        {
            std::cout << format("Dumping %s...", g_inputFileName) << std::endl;
            Script.Dump(arOutput);

            std::cout << format("File %s dumped successfuly", g_inputFileName) << std::endl;
        }
        else
        {
            std::cout << "Decompiling..." << std::endl;

            printf("  Init definitions\n");
            Script.InitDefinitions();

            printf("  Processing code\n");
            Script.ProcessCode();

            printf("  Storing sources\n");
            Script.StoreSource(arOutput);
            std::cout << format("File %s decompiled successfuly\n", g_inputFileName) << std::endl;
        }
    }

    catch (const UserException& e)
    {
        // Error message already displayed
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return 1;
    }

    catch(...)
    {
        return 1;
        
//        const int c_nErrMsgLen = 1024;
//        TCHAR lpszErrMsg[c_nErrMsgLen];
//        e->GetErrorMessage(lpszErrMsg, c_nErrMsgLen);
//        printf("Error: %s.\n", lpszErrMsg);
//        if (g_bStopOnError == TRUE)
//        {
//            printf("Press ENTER to continue.\n");
//            cin.get();
//        }
//        return 1;
    }

    return 0;
}

void PrintUsage(std::string filename)
{
    std::cout << "Usage: " << filename <<  " [options] [-s value] file.int [file.ssl]" << std::endl
              << "Example: " << filename << " -d-1-a-b-e-s3 random.int" << std::endl
              << std::endl
              << "Options" << std::endl
              << "  -d: dump file" << std::endl
              << "  -1: input file is Fallout 1 script" << std::endl
              << "  -a: ignore wrong number of arguments" << std::endl
              << "  -b: insert omitted arguments backward" << std::endl
              << "  -s: use Space instead of tab to indent" << std::endl
              << "  -e: stop decompiling on error" << std::endl
              << "  --: end of options" << std::endl;
}

BOOL ProcessCommandLine(int argc, char* argv[])
{
    int c;
    int nIndentWidth;

    while((c = getopt(argc, argv, "d1abes:")) != EOF)
    {
        switch (c)
        {
            case 'd':
                g_bDump = TRUE;
                printf("dump file is on.\n");
                break;

            case '1':
                g_nFalloutVersion = 1;
                printf("int2ssl is using Fallout 1 code.\n");
                break;

            case 'a':
                g_bIgnoreWrongNumOfArgs = TRUE;
                printf("ignore wrong number of arguments is on.\n");
                break;

            case 'b':
                g_bInsOmittedArgsBackward = TRUE;
                printf("insert omitted arguments backward is on.\n");
                break;

            case 'e':
                g_bStopOnError = TRUE;
                printf("stop decompiling on error is on.\n");
                break;

            case 's':
                nIndentWidth = atoi(optarg);

                if (nIndentWidth <= 0)
                {
                    printf("Warning: Invalid indent width. Indent set 3\n");
                    nIndentWidth = 3;
                }

                g_strIndentFill = "";

                for(; nIndentWidth > 0; nIndentWidth--)
                {
                    g_strIndentFill += " ";
                }
                break;
        }
    }

    if (optind == argc)
    {
        printf("Error: Input file name omitted\n");
        if (g_bStopOnError == TRUE)
        {
            printf("Press ENTER to continue.\n");
            cin.get();
        }
        return FALSE;
    }

    g_inputFileName = argv[optind];
    optind++;

    if (optind < argc)
    {
        g_outputFileName = argv[optind];
    }
    else
    {
        g_outputFileName = g_inputFileName;

        std::string extension = g_outputFileName.substr(g_outputFileName.length() - 4);

        if (extension == ".int")
        {
            g_outputFileName = g_outputFileName.substr(0, g_outputFileName.length() - 4);
        }

        if (g_bDump)
        {
            g_outputFileName += ".dump";
        }
        else
        {
            g_outputFileName += ".ssl";
        }
    }
    return TRUE;
}
