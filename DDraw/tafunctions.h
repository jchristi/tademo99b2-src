#ifndef tafunctionsH
#define tafunctionsH

struct msgstruct{
	int xpos;
	int ypos;
	int shiftstatus; //should be 5 for shiftclick
};

struct posstruct{
	int x;
	int y;
};

//////////////////////////////////////////////////////////////////////////////////////////
/// Working.
//////////////////////////////////////////////////////////////////////////////////////////
unsigned short (__stdcall *FindMouseUnit)(void); //find unit under mousepointer
FindMouseUnit = (unsigned short (__stdcall *)(void))0x48CD80;

//fill TAdynmem->MouseMapPosX & TAdynmem->MouseMapPosY first
void (__stdcall *TAMapClick)(void *msgstruct);
TAMapClick = (void (__stdcall *)(void *msgstruct))0x498F70;
void (__stdcall *TestBuildSpot)(void);
TestBuildSpot = (void (__stdcall *)(void))0x4197D0;

//Type - 0 = chat, 1 = popup
int (__stdcall *SendText)(char *Text, int Type);
SendText = (int (__stdcall *)(char*, int))0x46bc70;

void (__stdcall *ShowText)(PlayerStruct *Player, char *Text, int Unk1, int Unk2);
ShowText = (void (__stdcall *)(PlayerStruct *Player, char *Text, int Unk1, int Unk2))0x463E50;

void (__stdcall *TADrawRect)(char *Context, tagRECT *rect, int color);
TADrawRect = (void (__stdcall *)(char *Context, tagRECT *rect, int color))0x4BF8C0;

void (__cdecl *TADrawLine)(char *Context, int x1,int y1,int x2,int y2,int color);
TADrawLine = (void (__cdecl *)(char *Context, int x1,int y1,int x2,int y2,int color))0x4CC7AB;				

int (__stdcall *GetContext)(char *ptr);
GetContext = (int (__stdcall *)(char *ptr))0x4C5E70;

//CirclePointer = CirclePointer in tadynmemstruct
void (__stdcall *TADrawCircle)(char *context, void *CirclePointer, posstruct *pos, int radius, int color, char *text, int unk);
TADrawCircle = (void (__stdcall *)(char *context , void *CirclePointer, posstruct *pos, int radius, int color, char *text, int unk))0x438EA0;				

//////////////////////////////////////////////////////////////////////////////////////////
/// Not working.
//////////////////////////////////////////////////////////////////////////////////////////
void (__stdcall *TestGridSpot)(UnitStruct *BuildUnit, int pos, int unk, PlayerStruct *Player); //unk=zero
TestGridSpot = (void (__stdcall *)(UnitStruct *BuildUnit, int pos, int unk, PlayerStruct *Player))0x47D2E0;

#endif