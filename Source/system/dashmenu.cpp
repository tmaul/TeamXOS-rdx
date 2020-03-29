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

Menu::Menu()
{
	Items.clear();
	Selected_Item=0;
	Parent=0;
	emulator_system=0;
	enabled =false;
	vis_start=0;
	vis_end=0;
	return;
}


MenuItem* Menu::AddXBE (char *filename) 
{
	DWORD baseaddr, certaddr, cert_id, region,rating;
	WCHAR cert_string[41];
	WCHAR alt_cert_string[41];

	char newstring[250];

	char *path = new char[MAX_PATH];

	HANDLE xbefd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if(xbefd!=INVALID_HANDLE_VALUE) {
		DWORD readin;

		SetFilePointer(xbefd, 0x104, NULL, FILE_BEGIN);
		ReadFile(xbefd, &baseaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, 0x118, NULL, FILE_BEGIN);
		ReadFile(xbefd, &certaddr, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0x8, NULL, FILE_BEGIN);
		ReadFile(xbefd, &cert_id, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xc, NULL, FILE_BEGIN);
		ReadFile(xbefd, &cert_string, 0x50, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0x5c, NULL, FILE_BEGIN);
		ReadFile(xbefd, &alt_cert_string, 0x50, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xa0, NULL, FILE_BEGIN);
		ReadFile(xbefd, &region, 0x4, &readin, NULL);

		SetFilePointer(xbefd, (certaddr-baseaddr)+0xa4, NULL, FILE_BEGIN);
		ReadFile(xbefd, &rating, 0x4, &readin, NULL);


		cert_string[0x50]=0x0000;

		CloseHandle(xbefd);

		MenuItem *newitem = new MenuItem;
		newitem->Action=ACTION_XBELAUNCH;
		if(newitem!=NULL) 
		{
			int added=0;
			newitem->TitleID=cert_id;
			strcpy(newitem->Filename,filename);
			if(!added) {
				int x=lstrlenW(cert_string);
				if(x==0) 
				{
					strcpy(newstring, FolderNameFromFilePathA(filename));
					strcpy(newitem->Title,newstring);
				}
				else if(x<44) 
				{
					sprintf(newstring, "%S", cert_string);
					strcpy(newitem->Title,newstring);
				} 
				else 
				{
					WCHAR tstr[33];
					lstrcpynW(tstr, cert_string, 32);
					sprintf(newstring, "%S...", tstr);
					strcpy(newitem->Title,newstring);
				}
			}

			{
				char * idtmp = strdup(filename);
				char * imtmp = strrchr(idtmp, '\\');
				imtmp[0]=0;

				newitem->DBlocks=XGetDisplayBlocks(idtmp);
				free(idtmp);
			}

			if (region) {
				newitem->Region=region;
			} else {
				newitem->Region = 0;
			}

			if (rating) {
				newitem->Rating = rating;
			} else {
				newitem->Rating = 0;
			}

		}
		return newitem;
	} 
	else 
	{
		OutputDebugString("Error adding ");
		OutputDebugString(filename);
		OutputDebugString("to a list.\n");
		return NULL;
	}
}

MenuItem* Menu::AddFile (char *filename) 
{
	char newstring[MAX_PATH];
	MenuItem *newitem = new MenuItem();
	if(newitem!=NULL) 
	{
		strcpy(newstring, filename);
		strcpy(newitem->Filename,newstring);
		if(char *c=strrchr(filename,'\\')) 
		{
			c++;
			strcpy(newstring, c);
			strcpy(newitem->Title,newstring);
		} 
		else
		{
			strcpy(newstring, filename);
			strcpy(newitem->Title,newstring);
		}

		HANDLE fd=CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
		if(fd!=INVALID_HANDLE_VALUE) 
		{
			newitem->DBlocks=GetFileSize(fd, NULL);
			CloseHandle(fd);
		}
		return newitem;
	}
	return NULL;
}

void Menu::AddFiles(const char *where, const char *filter, int norecursion) 
{
	int u,w,z=0;
	WIN32_FIND_DATA fd;
	HANDLE hFD;

	char *tmpc = new char[MAX_PATH];
	char *tmpb = new char[MAX_PATH];
	char **paths = new char*[128];
	for(z=0; z<128; z++) 
		paths[z] = new char[MAX_PATH];

	if(where==NULL)
	{
		return;
	}

	if(!norecursion) 
	{
		strcpy(tmpb, where);
		sprintf(tmpc, "%s\\*", where);
		if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE) {
			for(z=0;z<128;) {
				if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					sprintf(paths[z], "%s\\%s", tmpb, fd.cFileName);
					z++;
				}

				if(!FindNextFile(hFD, &fd)) {
					FindClose(hFD);
					break;
				}
			}
		} else {
			DWORD Err= GetLastError();
			return;
		}
	} else {
		strcpy(paths[0], where);
		z=1;
	}

	w=0;
	if(z>0) for(u=0; u<z; u++) {
		if(w==0) {
			if(filter!=NULL)
				sprintf(tmpc, "%s\\%s", paths[u], filter);
			else
				sprintf(tmpc, "%s\\default.xbe", paths[u]);
			if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE)
				w=1;
		}
		while(w==1) {
			sprintf(tmpb, "%s\\%s", paths[u], fd.cFileName);
			if(!filter || !strcmp(filter, "*.xbe"))
			{
				MenuItem *addme = AddXBE(tmpb);
				if (addme)
				{
					Items.push_back(addme);
				}
			}
			else
			{
				MenuItem *addme =AddFile(tmpb);
				if (addme)
				{
					Items.push_back(addme);
				}
			}
			if(!FindNextFile(hFD, &fd))
				w=2;
		}

		if(w==2) {
			FindClose(hFD);
			w=0;
		}
	}
}

int Menu::Count()
{
	return Items.size();
}


