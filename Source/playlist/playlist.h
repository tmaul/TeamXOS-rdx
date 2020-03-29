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

class Music_Item
{
public:
	char Title[MAX_PATH];
	char Filename[MAX_PATH];

	void SetTitle(char* NewTitle)
	{
		strcpy(Title,NewTitle);
	}
	void SetFilename(char* NewFilename)
	{
		strcpy(Filename,NewFilename);
	}

	Music_Item() {
		strcpy(Title,"Unknown Item");
		strcpy(Filename,"");
	}
};

class Music_Playlist
{
private:
	Music_Item* AddFile (char *filename);
public:
	vector<Music_Item*> Items;
	int Selected_Item;
	char Name[MAX_PATH];
	int vis_start;
	int vis_end;

	void Rename(char* newname)
	{
		strcpy(Name,newname);
	}

	Music_Playlist();

    ~Music_Playlist()
	{
	}

	void AddFiles(const char *where, const char *filter, int norecursion); 
	int Count();

};

void Init_Playlist();
void Exit_Playlist();

void Clear_Playlist();
void Load_Playlist(char * filename);
void Save_Playlist(char * filename);

void Is_Music(char * filename);
void Is_Playlist(char * filename);
void Playlist_Add_File(char * filename);
void Playlist_Add_Dir(const char * filename);


void Playlist_Play();
void Playlist_Stop();
void Playlist_Toggle_Pause();
void Playlist_NextTrack();
void Playlist_PrevTrack();

void Playlist_Set_Vol(int newvolume);
void Playlist_Toggle_Mute();