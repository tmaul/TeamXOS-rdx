/////
//
//	This code is part of the rdx project. (c) Team Xos
//
//  Team xos grant you the right to use this code in any non commerical way,
//  under the condidtion that you release your source code with any compiled binaries
//
//  If you make any useful changes to the code you are encouraged to email to us
//  for inclusion in a future version. Full credit will be given.
//
//  rdxdev@gmail.com
//

#include "..\dash.h"

char* GamePathFromFindFilesC (char* gamepath)
{
	char * path = strrchr (gamepath,';');
	if (path) {
		return path+1;
	}
	return gamepath;
}

char* RemanderpathFromFindFilesC(char* gamepath) {
	char * path = strrchr(gamepath, ';');
	if(path)
		path[0]=0;
	return gamepath;
}

const char * GamePathFromFindFiles (const char* gamepath)
{
	char * path = strrchr (gamepath,';');
	if (path) {
		return path+1;
	}
	return gamepath;
}

const char* RemanderpathFromFindFiles(const char* gamepath) 
{
	char * path = strrchr(gamepath, ';');
	if(path)
		path[0]=0;
	return gamepath;
}

char* FileFromFilePathA(char* filepath) 
{
    char * filename = strrchr(filepath, '\\');
	if(filename)
		return filename+1;
	return filepath;
}

char* FolderNameFromFilePathA (char* filepath) 
{
	char *folder = PathFromFilePathA(filepath);
	return FileFromFilePathA(folder);
}

WCHAR* FileFromFilePathW(WCHAR* filepath) 
{
	WCHAR * filename = wcsrchr(filepath, '\\');
	if(filename)
		return filename+1;
	return filepath;
}

char* PathFromFilePathA(char* filepath) 
{
	char * filename = strrchr(filepath, '\\');
	if(filename)
		filename[0]=0;
	return filepath;
}

WCHAR* PathFromFilePathW(WCHAR* filepath) 
{
	WCHAR * filename = wcsrchr(filepath, '\\');
	if(filename)
		filename[0]=0;
	return filepath;
}

//Borrowed from MXM
BOOL  FileExists( LPCTSTR szFilename )
{
	BOOL bReturn = FALSE;
	

	DWORD dwAttributes;

	if ( szFilename  )
	{

		dwAttributes = GetFileAttributes( szFilename );
		if ( dwAttributes != (DWORD)-1 )
		{
			bReturn = TRUE;
		}
	}

	return bReturn;
}

DWORD FileLength( LPCTSTR szFilename )
{
	DWORD dwReturn = 0;

	if ( FileExists(szFilename) )
	{
		HANDLE hTemp;
		hTemp = CreateFile( szFilename, 
					   GENERIC_READ, 
					   FILE_SHARE_READ, 
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL,
					   NULL );
		if ( hTemp != INVALID_HANDLE_VALUE )
		{
			dwReturn = GetFileSize( hTemp, NULL );
		}
		CloseHandle( hTemp );
	}
	return dwReturn;
}

inline WCHAR* lstrdupW(WCHAR* src) 
{
	int i=(lstrlenW(src)+1)*sizeof(WCHAR);
	WCHAR* dst=(WCHAR *)malloc(i);
	if(dst==NULL) return NULL;
	lstrcpyW(dst, src);
	return dst;
}

inline WCHAR* lstrdupWfromA(char* src) 
{
	int i=(lstrlen(src)+1)*sizeof(WCHAR);
	WCHAR* dst=(WCHAR *)malloc(i);
	if(dst==NULL) return NULL;
	wsprintfW(dst, L"%S", src);
	return dst;
}

inline char* lstrdupAfromW(const WCHAR* src) 
{
	int i=(lstrlenW(src)+1)*sizeof(char);
	char* dst=(char *)malloc(i);
	if(dst==NULL) return NULL;
	sprintf(dst, "%S", src);
	return dst;
}

DWORD ReadAFile(char *fn, unsigned char **out) 
{
	DWORD filesize;
	HANDLE infile = CreateFileA( fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if(infile == INVALID_HANDLE_VALUE) {
		OutputDebugStringA("Could not load ");
		OutputDebugStringA(fn);
		OutputDebugStringA("\n");
		DWORD eno=GetLastError();
		*out=NULL;
		return 0;
	}

	filesize=GetFileSize(infile, NULL);
	*out=(unsigned char*)malloc(filesize+1);
	memset(*out, 0, filesize+1);
	DWORD readin;
	ReadFile( infile, *out, filesize, &readin, NULL );
	CloseHandle( infile );
	return readin;
}


inline DWORD fread(void *dst, int cnta, int cntb, HANDLE src) 
{
	DWORD readin=0;
	ReadFile( src, dst, cnta*cntb, &readin, NULL );
	return readin;
}

inline int matchext( char *file, char *ext ) 
{
	char *ref = strrchr(file, '.');
	ref++;
	return (!strcmp(ref,ext));
}

float frand() 
{
	double rn=(double)rand();
	rn/=RAND_MAX*0.5f;
	rn-=1.0f;
	return(float)rn;
}

// Launch XBE
HRESULT LaunchMyXBE(char * XBEFile, PLAUNCH_DATA LDATA)
{
	HRESULT r;
	char umFilename[5000];
	char mFilename[5000];
	char mDrivePath[5000];
	char mDriveLetter[5000];
	char mDevicePath[5000];
	char mFullPath[5000];
	char asspath[5000];
	g_pd3dDevice->PersistDisplay();
	strcpy(umFilename, XBEFile);
	strcpy(mFilename,FileFromFilePathA(umFilename));
	strcpy(mDrivePath,PathFromFilePathA(umFilename));
	int tm=0;
	while(mDrivePath[tm]!=':' && mDrivePath[tm]!=0) 
		tm++;
	lstrcpyn(mDriveLetter,mDrivePath, tm+2);
	mDriveLetter[tm+3]=0;
	strcpy(mDevicePath,getpath(mDriveLetter));
	sprintf(mFullPath, "%s%s", mDevicePath, (char *)mDrivePath+2);
	if(mFullPath[(strlen(mFullPath)-1)]=='\\')
		mFullPath[(strlen(mFullPath)-1)]=0;
	OutputDebugStringA("Mapping ");
	OutputDebugStringA(mFullPath);
	OutputDebugStringA(" to D:\n");
	mappath(mFullPath, "\\??\\D:");
	OutputDebugStringA("Launching ");
	OutputDebugStringA(mFullPath);
	OutputDebugStringA("\\");
	OutputDebugStringA(mFilename);
	OutputDebugStringA("\n");
	sprintf(asspath, "D:\\%s", mFilename);
	r=XLaunchNewImage(asspath, NULL);

	return r;
}

