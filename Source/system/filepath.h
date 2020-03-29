#pragma once
#include <xtl.h>


char* GamePathFromFindFilesC (char* gamepath);
char* RemanderpathFromFindFilesC(char* gamepath);
const char * GamePathFromFindFiles (const char* gamepath);
const char* RemanderpathFromFindFiles(const char* gamepath);
char* FileFromFilePathA(char* filepath);
char* FolderNameFromFilePathA (char* filepath);
WCHAR* FileFromFilePathW(WCHAR* filepath);
char* PathFromFilePathA(char* filepath);
WCHAR* PathFromFilePathW(WCHAR* filepath);
BOOL  FileExists( LPCTSTR szFilename );
DWORD FileLength( LPCTSTR szFilename );
WCHAR* lstrdupW(WCHAR* src);
WCHAR* lstrdupWfromA(char* src);
inline char* lstrdupAfromW(const WCHAR* src);
DWORD ReadAFile(char *fn, unsigned char **out);
DWORD fread(void *dst, int cnta, int cntb, HANDLE src);
inline int matchext( char *file, char *ext );
float frand();
HRESULT LaunchMyXBE(char * XBEFile, PLAUNCH_DATA LDATA);