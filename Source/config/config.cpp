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

void LoadConfig()
{
	int done_menus=0;
	char strValue[256]="";

	// init internal emulator menus
	NESMenu= new Menu();
	SNESMenu= new Menu();
	MDMenu= new Menu();
	SMSMenu= new Menu();
	Init_Playlist();

	TiXmlDocument document("D:\\config.xml");
	bool success = document.LoadFile();
	if(!success)
		return;

	MenusNew[0] = new Menu();
	resetpaths();
	int value = 0;
	const char *text = NULL;
	TiXmlElement *root = document.RootElement();
	TiXmlElement *element = root->FirstChildElement() ;
	if(element)
		element = element->FirstChildElement();

	while(element)
	{
		std::string element_name(element->Value());
		if(element_name == "menu")
		{

			done_menus++;
			MenusNew[done_menus] = new Menu();
			sprintf(strValue ,"%s", element->Attribute("name"));
			if(strValue)
			{
				MenusNew[done_menus]->Rename(strValue);
				MenuItem *newitem = new MenuItem;
				newitem->Action= ACTION_SWITCHMENU;
				newitem->SetTitle(strValue);
				newitem->SetFilename(strValue);
				newitem->TitleID=done_menus;
				MenusNew[0]->Items.push_back(newitem);
				TiXmlElement *path_element = element->FirstChildElement() ;
				while(path_element)
				{
					std::string pathelement_name(path_element->Value());
					if(pathelement_name == "autoadd")
					{
						TiXmlNode* text = path_element->FirstChild();
						if (text)
						{
							char strpathValue[256]="";
							sprintf(strpathValue ,"%s",text->Value());
							if(strpathValue)
							{
								MenusNew[done_menus]->AddFiles(strpathValue,NULL,0);
							}
						}
					}
					path_element = path_element->NextSiblingElement();
				}

			}
			// need to handle children here.
		}
		else if(element_name == "action")
		{
			sprintf(strValue ,"%s", element->Attribute("name"));
			if(strValue)
			{
				MenuItem *newitem = new MenuItem;
				TiXmlNode* text = element->FirstChild();
				if (text)
				{
					char txtValue[256]="";
					sprintf(txtValue ,"%s", text->Value());
					if(!strncmp(strValue,"fileman",sizeof(strValue)))
					{
						newitem->Action= ACTION_FILEMANAGER;
						newitem->SetTitle(txtValue);
						newitem->SetFilename(strValue);
						newitem->TitleID=done_menus;
						MenusNew[0]->Items.push_back(newitem);		
					}
					if(!strncmp(strValue,"system",sizeof(strValue)))
					{
						newitem->Action= ACTION_SYSTEMINFO;
						newitem->SetTitle(txtValue);
						newitem->SetFilename(strValue);
						newitem->TitleID=done_menus;
						MenusNew[0]->Items.push_back(newitem);		
					}
					if(!strncmp(strValue,"poweroff",sizeof(strValue)))
					{
						newitem->Action= ACTION_POWERDOWN;
						newitem->SetTitle(txtValue);
						newitem->SetFilename(strValue);
						newitem->TitleID=done_menus;
						MenusNew[0]->Items.push_back(newitem);		
					}
					if(!strncmp(strValue,"reboot",sizeof(strValue)))
					{
						newitem->Action= ACTION_REBOOT;
						newitem->SetTitle(txtValue);
						newitem->SetFilename(strValue);
						newitem->TitleID=done_menus;
						MenusNew[0]->Items.push_back(newitem);		
					}
				}
			}
		} 
		else if (element_name == "music")
		{
				TiXmlElement *path_element = element->FirstChildElement() ;
				while(path_element)
				{
					std::string pathelement_name(path_element->Value());
					if(pathelement_name == "autoadd")
					{
						TiXmlNode* text = path_element->FirstChild();
						if (text)
						{
							char strpathValue[256]="";
							sprintf(strpathValue ,"%s",text->Value());
							if(strpathValue)
							{
								Playlist_Add_Dir(strpathValue);
							}
						}
					}
				  path_element = path_element->NextSiblingElement();
				}
		}
		else if (element_name == "emulator")
		{
			MenuItem *newitem = new MenuItem;
			sprintf(strValue ,"%s", element->Attribute("name"));
			if(strValue)
			{
				if(!strncmp(strValue,"sms",sizeof(strValue)))
				{
					newitem->Action= ACTION_INTERNAL_EMULATOR;
					newitem->SetTitle("Sega Master System");
					newitem->SetFilename("");
					newitem->TitleID=SYSSMS;
					MenusNew[0]->Items.push_back(newitem);	

					TiXmlElement *path_element = element->FirstChildElement() ;
					while(path_element)
					{
						std::string pathelement_name(path_element->Value());
						if(pathelement_name == "autoadd")
						{
							TiXmlNode* text = path_element->FirstChild();
							if (text)
							{
								char strpathValue[256]="";
								sprintf(strpathValue ,"%s",text->Value());
								if(strpathValue)
								{
									SMSMenu->AddFiles(strpathValue,"*.*",1);
								}
							}
						}
						path_element = path_element->NextSiblingElement();
					}
				}

				if(!strncmp(strValue,"md",sizeof(strValue)))
				{
					newitem->Action= ACTION_INTERNAL_EMULATOR;
					newitem->SetTitle("Sega Megadrive");
					newitem->SetFilename("");
					newitem->TitleID=SYSMD;
					MenusNew[0]->Items.push_back(newitem);	
					TiXmlElement *path_element = element->FirstChildElement() ;
					while(path_element)
					{
						std::string pathelement_name(path_element->Value());
						if(pathelement_name == "autoadd")
						{
							TiXmlNode* text = path_element->FirstChild();
							if (text)
							{
								char strpathValue[256]="";
								sprintf(strpathValue ,"%s",text->Value());
								if(strpathValue)
								{
									MDMenu->AddFiles(strpathValue,"*.*",1);
								}

							}
						}
						path_element = path_element->NextSiblingElement();
					}
				}

				if(!strncmp(strValue,"nes",sizeof(strValue)))
				{
					newitem->Action= ACTION_INTERNAL_EMULATOR;
					newitem->SetTitle("Nintendo Entertainment System");
					newitem->SetFilename("");
					newitem->TitleID=SYSNES;
					MenusNew[0]->Items.push_back(newitem);	
					TiXmlElement *path_element = element->FirstChildElement() ;
					while(path_element)
					{
						std::string pathelement_name(path_element->Value());
						if(pathelement_name == "autoadd")
						{
							TiXmlNode* text = path_element->FirstChild();
							if (text)
							{
								char strpathValue[256]="";
								sprintf(strpathValue ,"%s",text->Value());
								if(strpathValue)
								{
									NESMenu->AddFiles(strpathValue,"*.*",1);
								}
							}
						}
						path_element = path_element->NextSiblingElement();
					}
				}

				if(!strncmp(strValue,"snes",sizeof(strValue)))
				{
					newitem->Action= ACTION_INTERNAL_EMULATOR;
					newitem->SetTitle("Super Nintendo Entertainment System");
					newitem->SetFilename("");
					newitem->TitleID=SYSSNES;
					MenusNew[0]->Items.push_back(newitem);
					TiXmlElement *path_element = element->FirstChildElement() ;
					while(path_element)
					{
						std::string pathelement_name(path_element->Value());
						if(pathelement_name == "autoadd")
						{
							TiXmlNode* text = path_element->FirstChild();
							if (text)
							{
								char strpathValue[256]="";
								sprintf(strpathValue ,"%s",text->Value());
								if(strpathValue)
								{
									SNESMenu->AddFiles(strpathValue,"*.*",1);
								}
							}
						}
						path_element = path_element->NextSiblingElement();
					}
				}
			}
		}
		element = element->NextSiblingElement();
	}
	Playlist_Play();
}
