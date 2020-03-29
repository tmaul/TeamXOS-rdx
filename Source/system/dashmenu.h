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

#pragma once
#include <xtl.h>
#include <iostream>
#include <vector>


using namespace std;

#define ACTION_XBELAUNCH 1

#define ACTION_SWITCHMENU 3
#define ACTION_SAVEMANAGER 4
#define ACTION_FILEMANAGER 5
#define ACTION_SYSTEMINFO 6
#define ACTION_POWERDOWN 7
#define ACTION_REBOOT 8
#define ACTION_INTERNAL_EMULATOR 9

class MenuItem
{
public:
	char Title[MAX_PATH];
	char Filename[MAX_PATH];
	char Cmdline[MAX_PATH];
	DWORD Action;
	DWORD TitleID;
	DWORD DBlocks;
	DWORD Region;
	DWORD Rating;

	void SetTitle(char* NewTitle)
	{
		strcpy(Title,NewTitle);
	}
	void SetFilename(char* NewFilename)
	{
		strcpy(Filename,NewFilename);
	}
	void SetCmdline(char* NewCmdline)
	{
		strcpy(Cmdline,NewCmdline);
	}

	MenuItem() {
		strcpy(Title,"Unknown Item");
		strcpy(Filename,"");
		strcpy(Cmdline,"");
		Action=0;
		TitleID=0;
		DBlocks=0;
		Region=0;
		Rating=0;
	}
};

class Menu
{
private:
	MenuItem* AddXBE (char *filename);
	MenuItem* AddFile (char *filename);
public:
	vector<MenuItem*> Items;
	int Selected_Item;
	char Name[MAX_PATH];
	DWORD Parent;
	DWORD emulator_system;
	bool enabled;
	int vis_start;
	int vis_end;

	void Rename(char* newname)
	{
		strcpy(Name,newname);
	}

	Menu();
    ~Menu()
	{
	}

	void AddFiles(const char *where, const char *filter, int norecursion); 
	int Count();

};
