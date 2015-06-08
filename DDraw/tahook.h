#ifndef tahookH
#define tahookH

#include <windows.h>
#include "oddraw.h"
#include "tamem.h"

#define ShareMacro 1
#define DTLine 2
#define ScrolledDTLine 3
#define DTRing 4

#define SCROLL 10000

struct QueMSG
  {
  UINT Message;
  WPARAM wParam;
  LPARAM lParam;
  };

class CTAHook
{
  private:
	TAdynmemStruct *TAdynmem;
    int VirtualKeyCode;
    char ShareText[1000];
    bool OptimizeRows;
    bool FullRingsEnabled;
    HWND TAhWnd;
    QueMSG MessageQueue[1000];
    int QueuePos;
    int QueueLength;
    void QueueMessage(UINT M, WPARAM W, LPARAM L);
    void SendQueued();
    void WriteDTLine();
    void CalculateLine();
    void OptimizeDTRows();
    void VisualizeRow();
    void WriteScrollDTLine();
    unsigned int SendMessage;
    int Delay;
    bool WriteLine;
    int StartX, StartY;
    int EndX, EndY;
    int FootPrintX;
	int FootPrintY;
	int Spacing;
    int QueueStatus;
    void UpdateSpacing();
    short XMatrix[1000];
    short YMatrix[1000];
	int MouseOverUnit;

    int MatrixLength;
    int Direction;
    LPDIRECTDRAWSURFACE lpRectSurf;
    bool ScrollEnabled;
    bool RingWrite;
    void CalculateRing();
	void CalculateRing(int posx, int posy, int footx, int footy);
	void FindConnectedSquare(int &x1, int &y1, int &x2, int &y2, char *unittested);
    void VisualizeRing(LPDIRECTDRAWSURFACE DestSurf);

	void ClickBuilding(int Xpos, int Ypos);
	short GetFootX();
	short GetFootY();
	void DrawBuildRect(int posx, int posy, int sizex, int sizey, int color);
	void EnableTABuildRect();
	void DisableTABuildRect();
	void PaintMinimapRect();

	void (__stdcall *ShowText)(PlayerStruct *Player, char *Text, int Unk1, int Unk2);
	void (__stdcall *InterpretCommand)(char *Command, int Access);
	void (__stdcall *TAMapClick)(void *msgstruct);
	void (__stdcall *TestBuildSpot)(void);
	void (__stdcall *TADrawRect)(tagRECT *unk, tagRECT *rect, int color);
	unsigned short (__stdcall *FindMouseUnit)(void);
	int (__stdcall *SendText)(char *Text, int Type);

    char *Add;
    char *Sub;
    char OldAdd;
    char OldSub;

    //int StartMapX;
    //int StartMapY;
    //int EndMapX;
    //int EndMapY;
    //int *MapX;
    //int *MapY;

	struct msgstruct{
		int xpos;
		int ypos;
		int shiftstatus; //should be 5 for shiftclick
	};

  public:
    CTAHook();
    ~CTAHook();
    bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    void Set(int KeyCodei, char *ChatMacroi, bool FullRingsi, bool VisualizeRowsi, int iDelay);
    void WriteShareMacro();
    void Blit(LPDIRECTDRAWSURFACE DestSurf);
	void TABlit();

    static int GetMaxScrollX();
    static int GetMaxScrollY();
};

#endif