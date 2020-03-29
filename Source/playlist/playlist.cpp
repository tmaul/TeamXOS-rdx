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

#include "playlist.h"

Music_Playlist *PlaylistMenu;

int current_track=0;
Music_Item* Music_Playlist::AddFile (char *filename) 
{
	char newstring[MAX_PATH];
	Music_Item *newitem = new Music_Item();
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
		return newitem;
	}
	return NULL;
}

void Music_Playlist::AddFiles(const char *where, const char *filter, int norecursion) 
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
			sprintf(tmpc, "%s\\%s", paths[u], filter);
			if((hFD=FindFirstFile(tmpc, &fd))!=INVALID_HANDLE_VALUE)
				w=1;
		}
		while(w==1) {
			sprintf(tmpb, "%s\\%s", paths[u], fd.cFileName);
			{
				Music_Item *addme =AddFile(tmpb);
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

int Music_Playlist::Count()
{
	return Items.size();
}


Music_Playlist::Music_Playlist()
{
	Items.clear();
	Selected_Item=0;
	vis_start=0;
	vis_end=0;
	return;
}


// music done callback - used to play next item in the playlist
void music_done() {
	if (current_track <   PlaylistMenu->Count())
	{
		current_track++;
		Playlist_Play();
	}
}

void Init_Playlist()
{
	PlaylistMenu = new Music_Playlist();
}


void Exit_Playlist()
{
	music_done();
	delete PlaylistMenu;
}

void Clear_Playlist()
{

}

void Load_Playlist(char * filename)
{
}

void Save_Playlist(char * filename)
{
}


void Is_Music(char * filename)
{
}

void Is_Playlist(char * filename)
{
}

void Playlist_Add_File(char * filename)
{
}

void Playlist_Add_Dir(const char * path)
{
	PlaylistMenu->AddFiles(path,"*.mp3",0);
	PlaylistMenu->AddFiles(path,"*.ogg",0);
}

BOOL  PlFileExists( LPCTSTR szFilename )
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
DWORD PlFileLength( LPCTSTR szFilename )
{
	DWORD dwReturn = 0;

	if ( PlFileExists(szFilename) )
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


void Playlist_Play()
{
	if (current_track < PlaylistMenu->Count())
	{
		if (PlFileExists(PlaylistMenu->Items[current_track]->Filename))
		{
			// play currently selected track
		}
	}
}

void Playlist_Stop()
{
}

void Playlist_Toggle_Pause()
{
}

void Playlist_NextTrack()
{
	if (current_track <   PlaylistMenu->Count())  // if there is a next track
	{
		Playlist_Stop();
		current_track++;
		Playlist_Play();
	}
}

void Playlist_PrevTrack()
{
	if (current_track > 0)  // if there is a next track
	{
		if (current_track <   PlaylistMenu->Count())
		{
			Playlist_Stop();
			current_track--;
			Playlist_Play();
		}
	}
}

void Playlist_Set_Vol(int newvolume)
{
}

void Playlist_Toggle_Mute()
{
}
