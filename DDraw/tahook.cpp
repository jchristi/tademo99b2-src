#include "tahook.h"
#include "iddrawsurface.h"
//#include "font.h"              
#include <stdio.h>
#include "pcxread.h"
#include "rings.h"

#define WM_MOUSEWHEEL 522
#define MAX_SPACING 10

CTAHook *TAHook;

char NumAdd = 100;
char NumSub = -100;

CTAHook::CTAHook()
{
  lpRectSurf = NULL;
  
  if(DataShare->ehaOff == 1)
    return;
    
  HKEY hKey;
  DWORD dwDisposition;
  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\TAHook", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
  //write tahook ver string for the .hookreport function in recorder
  char VerString [] = "Swedish Eye ver 0.8";
  RegSetValueEx(hKey, "Ver", NULL, REG_SZ, (unsigned char*)VerString, strlen(VerString));
  RegCloseKey(hKey);

  LocalShare->TAHook = this;
  TAHook = this;

  HWND TopWindow;
  TopWindow = GetForegroundWindow();
  char Text[255];
  GetClassName(TopWindow,Text,254);
  if(strcmp(Text,"Total Annihilation Class")==0)
    TAhWnd = TopWindow;

	QueuePos = 0;
	Delay = 5;
	WriteLine = false;
	FootPrintX = FootPrintY = 2;
	ScrollEnabled = LocalShare->CompatibleVersion;
	RingWrite = false;
	Spacing = 0;


  lpRectSurf = CreateSurfPCXResource(50, true);

  Add = (char*)0x41ac14;
  Sub = (char*)0x41ac18;

  OldAdd = *Add;
  OldSub = *Sub;

  	ShowText = (void (__stdcall *)(PlayerStruct *Player, char *Text, int Unk1, int Unk2))0x463E50;
	InterpretCommand = (void (__stdcall *)(char *Command, int Unk1))0x417B50;
	TAMapClick = (void (__stdcall *)(void *msgstruct))0x498F70;
	TestBuildSpot = (void (__stdcall *)(void))0x4197D0;
	TADrawRect = (void (__stdcall *)(tagRECT *unk, tagRECT *rect, int color))0x4BF8C0;
	FindMouseUnit = (unsigned short (__stdcall *)(void))0x48CD80;
	SendText = (int (__stdcall *)(char*, int))0x46bc70;
	ShowText = (void (__stdcall *)(PlayerStruct *Player, char *Text, int Unk1, int Unk2))0x463E50;

//	void (__stdcall *sShowText)(PlayerStruct *Player, char *text, int access, int type) = 0x463E50;

	int *PTR = (int*)0x00511de8;
	TAdynmem = (TAdynmemStruct*)(*PTR);
}

CTAHook::~CTAHook()
{
  if(lpRectSurf)
    lpRectSurf->Release();

  if(DataShare->ehaOff == 1)
    return;

  WriteProcessMemory(GetCurrentProcess(), (void*)Add, &OldAdd, 1, NULL);
  WriteProcessMemory(GetCurrentProcess(), (void*)Sub, &OldSub, 1, NULL);
}

bool CTAHook::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if(DataShare->ehaOff == 2)
    return false;

  switch(Msg)
    {
    case WM_KEYDOWN:
      switch((int)wParam)
        {
        case 122: //f11
          WriteShareMacro();
          return true;

        case 33:  //pageup
			if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
			{
				Spacing++;
				if(Spacing>MAX_SPACING)
					Spacing = MAX_SPACING;
				UpdateSpacing();
				return true;
			}

        case 34:  //pagedown
			if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
			{
				Spacing--;
				if(Spacing<0)
					Spacing = 0;
				UpdateSpacing();
				return true;
			}
        case 17: //ctrl
          //TAhWnd = FindWindow("Total Annihilation Class", "Total Annihilation");
          //GetWindowThreadProcessId(TAhWnd, &PID);
          //TAProc = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
          WriteProcessMemory(GetCurrentProcess(), (void*)Add, &NumAdd, 1, NULL);
          WriteProcessMemory(GetCurrentProcess(), (void*)Sub, &NumSub, 1, NULL);
          //CloseHandle(TAProc);
          break;
        }
      break;

    case WM_KEYUP:
      if(wParam == VirtualKeyCode)
        {
        WriteLine = false;
        RingWrite = false;
		EnableTABuildRect();
        }
      else
        {
        switch((int)wParam)
          {
          case 17:
            WriteProcessMemory(GetCurrentProcess(), (void*)Add, &OldAdd, 1, NULL);
            WriteProcessMemory(GetCurrentProcess(), (void*)Sub, &OldSub, 1, NULL);
            break;
          }
        }
      break;


    case WM_CHAR:
      if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0 && WriteLine==true)
        return true;
      break;

    case WM_LBUTTONDOWN:
      if(RingWrite)
        {
        WriteDTLine();
        return true;
        }
      else if(WriteLine==true && (GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
        {
			FootPrintX = GetFootX();
			if(FootPrintX == 0)
				FootPrintX = 1;
			FootPrintY = GetFootY();
			if(FootPrintY == 0)
				FootPrintY = 1;

        EndX = (LOWORD(lParam)-128) + TAdynmem->MapX;
        EndY = (HIWORD(lParam)) + TAdynmem->MapY - 32 + TAdynmem->Height/2;
        /*if(ScrollEnabled)
          WriteScrollDTLine();
        else
          WriteDTLine();*/
		WriteDTLine();

        StartX = (LOWORD(lParam)-128) + TAdynmem->MapX;
        StartY = (HIWORD(lParam)) + TAdynmem->MapY - 32 + TAdynmem->Height/2;
        /*if(ScrollEnabled)
          {
          StartMapX = *MapX;
          StartMapY = *MapY;
          }*/
        XMatrix[0]=-1;
        YMatrix[0]=-1;
		return true;
        }
      else if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0 && LOWORD(lParam)>127)
        {
			FootPrintX = GetFootX();
			if(FootPrintX == 0)
				FootPrintX = 1;
			FootPrintY = GetFootY();
			if(FootPrintY == 0)
				FootPrintY = 1;

        WriteLine = true;

		DisableTABuildRect();

        StartX = (LOWORD(lParam)-128) + TAdynmem->MapX;
        StartY = (HIWORD(lParam)) + TAdynmem->MapY - 32 + TAdynmem->Height/2;
        EndX = (LOWORD(lParam)-128) + TAdynmem->MapX;
        EndY = (HIWORD(lParam)) + TAdynmem->MapY - 32 + TAdynmem->Height/2;
        XMatrix[0]=-1;
        YMatrix[0]=-1;

		CalculateLine();
        /*if(ScrollEnabled)
          {
          StartMapX = *MapX;
          StartMapY = *MapY;
          }*/
		return true;
        }
      break;

    case WM_LBUTTONUP:
      if(RingWrite)
        {
        return true;
        }
      if((GetAsyncKeyState(VirtualKeyCode)&0x8000)==0)
	  {
        WriteLine = false;
		EnableTABuildRect();
	  }
      break;

    case WM_RBUTTONDOWN:
      if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
        {
        //RingWrite = true;
        return true;
        }
      break;

    case WM_RBUTTONUP:
      RingWrite = false;
      break;

    case WM_MOUSEMOVE:
      //MouseX = LOWORD(lParam);
      //MouseY = HIWORD(lParam);
      if(WriteLine)
      {
        //EndX = LOWORD(lParam);
        //EndY = HIWORD(lParam);
        //if(VisualizeDTRows)
        EndX = (LOWORD(lParam)-128) + TAdynmem->MapX;
        EndY = (HIWORD(lParam)) + TAdynmem->MapY - 32 + TAdynmem->Height/2;
		CalculateLine();
      }
	  else if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
	  {
		  MouseOverUnit = FindMouseUnit();
		  if(MouseOverUnit)
		  {
			  CalculateRing();
			  RingWrite = true;
		  }
		  else
		  {
			  RingWrite = false;
			  if(!WriteLine)
				  EnableTABuildRect();
		  }
	  }
	  else
	  {
		  RingWrite = false;
	  }
      break;
    case WM_MOUSEWHEEL:
      /*FootPrint += ((short)HIWORD(wParam))/120;
      if(FootPrint>8)
        FootPrint = 8;
      if(FootPrint<2)
        FootPrint = 2;
      OutputFootPrint();
      return true;*/

      if(HIWORD(wParam)>120)
        {
			if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
			{
				Spacing--;
				if(Spacing<0)
					Spacing = 0;
				UpdateSpacing();
				return true;
			}
        }
      else if(HIWORD(wParam)<=120)
        {

			if((GetAsyncKeyState(VirtualKeyCode)&0x8000)>0)
			{
				Spacing++;
				if(Spacing>MAX_SPACING)
					Spacing = MAX_SPACING;
				UpdateSpacing();
				return true;
			}
        }   
      break;
    }

  return false;
}

void CTAHook::Set(int KeyCodei, char *ChatMacroi, bool OptimizeRowsi, bool FullRingsi, int iDelay)
{
  VirtualKeyCode = KeyCodei;
  strcpy(ShareText, ChatMacroi);
  OptimizeRows = OptimizeRowsi;
  FullRingsEnabled = FullRingsi;
  Delay = iDelay;
}

void CTAHook::WriteShareMacro()
{

	char buf[1000];

	int lasti = 0;

	for(int i=0; i<=strlen(ShareText); i++)
	{
		if(ShareText[i] == 13 || ShareText[i] == 0)
		{
			for(int j=0; j<i-lasti; j++)
			{
				buf[j] = ShareText[lasti+j];
			}
			buf[j] = 0;
			ShowText(&TAdynmem->Players[0], buf, 4, 0);
		
			if(buf[0] == '+')
				InterpretCommand(buf+1, 1);

			lasti=i+1;
		}
	}

/*  if(QueuePos>0)
    return;

  QueueStatus = ShareMacro;

  QueueLength = 0;
  QueuePos = 0;
  QueueMessage(WM_CHAR, 13, 0);
  QueueMessage(WM_CHAR, 13, 0);
  for(int i=0; i<strlen(ShareText); i++)
    {
    QueueMessage(WM_CHAR, ShareText[i], 0);
    if(ShareText[i] == 13)
      {
      QueueMessage(WM_CHAR, ShareText[i], 0);
      QueueMessage(WM_CHAR, ShareText[i], 0);
      }
    }
  QueueMessage(WM_CHAR, 13, 0);*/
}

void CTAHook::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
  /*if(QueueStatus == ScrolledDTLine)
    {
    int slask = Delay/5;
    if(slask<1)
      slask = 1;
    if(QueuePos>0 && (SendMessage%slask)==0)
      SendQueued();
    }
  else if(QueuePos>0 && (SendMessage%Delay)==0)
    SendQueued();*/

/*  if(lpRectSurf->IsLost() != DD_OK)
    {
    lpRectSurf->Restore();
    RestoreFromPCX(50, lpRectSurf);
    }

  SendMessage++;

  if(WriteLine && ScrollEnabled)
    {
    bool Calc = false;
    if(StartMapX!=*MapX || StartMapY!=*MapY)
      Calc = true;
    StartX -= *MapX - StartMapX;
    StartY -= *MapY - StartMapY;
    StartMapX = *MapX;
    StartMapY = *MapY;
    if(Calc)
      CalculateLine();
    }

  if(VisualizeDTRows && WriteLine)
    VisualizeRow(DestSurf);

  if(VisualizeDTRows && RingWrite)
    VisualizeRing(DestSurf);*/
}

void CTAHook::TABlit()
{
	if(WriteLine || RingWrite)
	{
		EnableTABuildRect();
		PaintMinimapRect();
		VisualizeRow();
		DisableTABuildRect();
	}
}

void CTAHook::QueueMessage(UINT M, WPARAM W, LPARAM L)
{
  if(QueuePos==999)
    return;
  MessageQueue[QueuePos].Message = M;
  MessageQueue[QueuePos].wParam = W;
  MessageQueue[QueuePos].lParam = L;
  QueuePos++;
  QueueLength++;
}

void CTAHook::SendQueued()
{
  int i;

  switch(QueueStatus)
    {
    case ShareMacro:
    //post 20 messages each frame
    for(i=0; i<20; i++)
      {
      if(QueuePos==0)
        return;
      if(MessageQueue[QueueLength-QueuePos].Message==WM_CHAR && MessageQueue[QueueLength-QueuePos].wParam==13 && i!=0)
        return;
      LocalShare->TAWndProc(TAhWnd, MessageQueue[QueueLength-QueuePos].Message, MessageQueue[QueueLength-QueuePos].wParam, MessageQueue[QueueLength-QueuePos].lParam);
      QueuePos--;
      }
      break;

    case DTLine:
    case DTRing:
      for(i=0; i<5; i++)
        {
        if(QueuePos==0)
          return;
        LocalShare->TAWndProc(TAhWnd, MessageQueue[QueueLength-QueuePos].Message, MessageQueue[QueueLength-QueuePos].wParam, MessageQueue[QueueLength-QueuePos].lParam);
	
        QueuePos--;
        }
      break;

    case ScrolledDTLine:
      if(MessageQueue[QueueLength-QueuePos].Message == SCROLL)
        ((AlliesWhiteboard*)LocalShare->Whiteboard)->InstantScrollTo(MessageQueue[QueueLength-QueuePos].wParam, MessageQueue[QueueLength-QueuePos].lParam);
      else
        LocalShare->TAWndProc(TAhWnd, MessageQueue[QueueLength-QueuePos].Message, MessageQueue[QueueLength-QueuePos].wParam, MessageQueue[QueueLength-QueuePos].lParam);
      QueuePos--;
      break;
    }

}

void CTAHook::WriteDTLine()
{

	if(OptimizeRows && FootPrintX==2 && FootPrintY==2)
		OptimizeDTRows();

	int i=0;
	while(XMatrix[i] != -1 && YMatrix[i]!=-1)
    {
		ClickBuilding(XMatrix[i], YMatrix[i]);

		i++;	
	}
}

void CTAHook::CalculateLine()
{
  int dx = (EndX - StartX)/16;
  int dy = (EndY - StartY)/16;
  int x_inc = 16;
  int y_inc = 16;

  int error,index;

  int x = StartX;
  int y = StartY;

  int footprintx = Spacing + FootPrintX;
  int footprinty = Spacing + FootPrintY;

  if(dx<0)
    {
    x_inc = -x_inc;
    dx = -dx;
    }

  if(dy<0)
    {
    y_inc = -y_inc;
    dy = -dy;
    }

  if(dx > dy)
    {
    dx = dx/footprintx;
    x_inc *= footprintx;

    if(dx>999)
      return; //argh..

    YMatrix[dx+1]=-1;
    XMatrix[dx+1]=-1;

    MatrixLength = dx;
    Direction = 1;

    error = (dy + dy) - dx;

    for(index=0; index <= dx; index++)
      {
      XMatrix[index]=(short)x;
      YMatrix[index]=(short)y;

      while((error >= 0) && (dx != 0))
        {
        error -= (dx + dx);
        y += y_inc;
        }

      error += (dy + dy);
      x += x_inc;

      } // end for

    } // end if dx > dy
    else
      {
      dy = dy/footprinty;
      y_inc *= footprinty;

      if(dy>999)
        return;

      YMatrix[dy+1]=-1;
      XMatrix[dy+1]=-1;

      MatrixLength = dy;
      Direction = 2;

      error = (dx + dx) - dy;

      for(index=0; index <= dy; index++)
        {
        XMatrix[index]=(short)x;
        YMatrix[index]=(short)y;

        while((error >= 0) && (dy != 0))
          {
          error -= (dy + dy);
          x += x_inc;
          }

        error += (dx + dx);
        y += y_inc;
        } // end for
      } // end else if dx > dy
}

void CTAHook::UpdateSpacing()
{
  /*char CFootPrint[20];
  sprintf(CFootPrint, "%i", Spacing);

  char OutString[80];
  strcpy(OutString, "Spacing set to ");
  strcat(OutString, CFootPrint);

  SendText(OutString, 0);*/

	if(WriteLine)
		CalculateLine();

	if(RingWrite)
		CalculateRing();
}

void CTAHook::OptimizeDTRows()
{
  short tx,ty;

  switch(Direction)
    {
    case 1:
      if(MatrixLength>2)
        {
        for(int index=1; index <= MatrixLength-2; index++)
          {
          if(YMatrix[index]==YMatrix[index+2])
            {
            tx = XMatrix[index+2];
            ty = YMatrix[index+2];
            XMatrix[index+2] = XMatrix[index];
            YMatrix[index+2] = YMatrix[index];
            XMatrix[index] = tx;
            YMatrix[index] = ty;
            index+=2;
            }
          else if(YMatrix[index]==YMatrix[index+1])
            {
            tx = XMatrix[index+1];
            ty = YMatrix[index+1];
            XMatrix[index+1] = XMatrix[index];
            YMatrix[index+1] = YMatrix[index];
            XMatrix[index] = tx;
            YMatrix[index] = ty;
            index++;
            }
          }//end for
        }//end if
		break;

    case 2:
      if(MatrixLength>2)
        {
        for(int index=1; index <= MatrixLength-2; index++)
          {
          if(XMatrix[index]==XMatrix[index+2])
            {
            tx = XMatrix[index+2];
            ty = YMatrix[index+2];
            XMatrix[index+2] = XMatrix[index];
            YMatrix[index+2] = YMatrix[index];
            XMatrix[index] = tx;
            YMatrix[index] = ty;
            index+=2;
            }
          else if(XMatrix[index]==XMatrix[index+1])
            {
            tx = XMatrix[index+1];
            ty = YMatrix[index+1];
            XMatrix[index+1] = XMatrix[index];
            YMatrix[index+1] = YMatrix[index];
            XMatrix[index] = tx;
            YMatrix[index] = ty;
            index+=1;
            }
          }//end for
        }//end if
      break;
    }
}

void CTAHook::VisualizeRow()
{
	int i=0;

	while(XMatrix[i] != -1 && YMatrix[i]!=-1)
	{
		TAdynmem->MouseMapPosX = XMatrix[i];
		TAdynmem->MouseMapPosY = YMatrix[i];

		TAdynmem->BuildSpotState=70;

		TestBuildSpot();
		int color = TAdynmem->BuildSpotState==70 ? 234 : 214; 
		DrawBuildRect( (TAdynmem->BuildPosRealX - TAdynmem->MapX) + 128,
			(TAdynmem->BuildPosRealY - TAdynmem->MapY) + 32 - (TAdynmem->Height/2),
			GetFootX()*16,
			GetFootY()*16,
			color);

		i++;
	}

/*  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;

  int XCenter;
  int YCenter;
  int iMapX, iMapY;
  if(ScrollEnabled)
    {
    iMapX = ((AlliesWhiteboard*)LocalShare->Whiteboard)->GetMapX();
    iMapY = ((AlliesWhiteboard*)LocalShare->Whiteboard)->GetMapY();
    }
  else
    {
    iMapX = 0;
    iMapY = 0;
    }
  int i=0;
  while(XMatrix[i] != -1 && YMatrix[i]!=-1)
    {
    XCenter = (XMatrix[i])+8-((XMatrix[i])+iMapX+8 + ((FootPrintX%2)*8))%16;
    YCenter = (YMatrix[i])+8-((YMatrix[i])+iMapY+2 + ((FootPrintY%2)*8))%16;
    RECT Dest;
    Dest.left = XCenter - FootPrintX*8;
    Dest.top = YCenter - FootPrintY*8;
    Dest.right = XCenter + FootPrintX*8;
    Dest.bottom = YCenter + FootPrintY*8;
    if(DestSurf->Blt(&Dest, lpRectSurf, NULL, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
      {
      DestSurf->Blt(&Dest, lpRectSurf, NULL, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
      }

    i++;
    }*/

}

void CTAHook::WriteScrollDTLine()
{
/*  if(QueuePos>0)
    return;

  QueueLength = 0;
  QueuePos = 0;


  CalculateLine();
  if(OptimizeRows && FootPrintX==2 && FootPrintY==2)
    OptimizeDTRows();

  if(XMatrix[0]>128 && XMatrix[0]<LocalShare->ScreenWidth && YMatrix[0]>32 && YMatrix[0]<LocalShare->ScreenHeight-32
    && XMatrix[MatrixLength]>128 && XMatrix[MatrixLength]<LocalShare->ScreenWidth && YMatrix[MatrixLength]>32 && YMatrix[MatrixLength]<LocalShare->ScreenHeight-32)
      {
      //all DTs inside screen standar clicking
      QueueStatus = DTLine;
	   //delete the clicked DT
      long Pos;
      Pos = EndY << 16;
      Pos += EndX;
      QueueMessage(WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, Pos);
      QueueMessage(WM_LBUTTONUP, MK_LBUTTON | MK_SHIFT, Pos);
      Pos = StartY << 16;
      Pos += StartX;
      QueueMessage(WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, Pos);
      QueueMessage(WM_LBUTTONUP, MK_LBUTTON | MK_SHIFT, Pos);
 	   int i=0;
  		while(XMatrix[i] != -1 && YMatrix[i]!=-1)
        {
        short x = XMatrix[i];
        short y = YMatrix[i];
        long Pos;
        Pos = y << 16;
        Pos += x;
        QueueMessage(WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, Pos);
        QueueMessage(WM_LBUTTONUP, MK_LBUTTON | MK_SHIFT, Pos);
        i++;
        }
      }
  else
    {
    QueueStatus = ScrolledDTLine;
    WriteLine = false;
    int OldMapX = *MapX;
    int OldMapY = *MapY;

    int ScrolledToX = *MapX;
    int ScrolledToY = *MapY;
    int i=0;
    int Foot8 = FootPrintX*8;

    long Pos;

    ScrolledToX = (StartMapX + StartX)-(LocalShare->ScreenWidth-128)/2;
    if(ScrolledToX<0)
      ScrolledToX = 0;
    if(ScrolledToX>GetMaxScrollX())
      ScrolledToX = GetMaxScrollX();
    ScrolledToY = (StartMapY + StartY)-(LocalShare->ScreenHeight-128)/2;
    if(ScrolledToY<0)
      ScrolledToY = 0;
    if(ScrolledToY>GetMaxScrollY())
      ScrolledToY = GetMaxScrollY();
    QueueMessage(SCROLL, ScrolledToX, ScrolledToY);
    Pos = ((StartMapY + StartY) - ScrolledToY) << 16;
    Pos += (StartMapX + StartX) - ScrolledToX;
    QueueMessage(WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, Pos);
    QueueMessage(WM_LBUTTONUP, MK_LBUTTON | MK_SHIFT, Pos);

    ScrolledToX = (StartMapX + EndX)-(LocalShare->ScreenWidth-128)/2;
    if(ScrolledToX<0)
      ScrolledToX = 0;
    if(ScrolledToX>GetMaxScrollX())
      ScrolledToX = GetMaxScrollX();
    ScrolledToY = (StartMapY + EndY)-(LocalShare->ScreenHeight-128)/2;
    if(ScrolledToY<0)
      ScrolledToY = 0;
    if(ScrolledToY>GetMaxScrollY())
      ScrolledToY = GetMaxScrollY();
    QueueMessage(SCROLL, ScrolledToX, ScrolledToY);
    Pos = ((StartMapY + EndY) - ScrolledToY) << 16;
    Pos += (StartMapX + EndX) - ScrolledToX;
    QueueMessage(WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, Pos);
    QueueMessage(WM_LBUTTONUP, MK_LBUTTON | MK_SHIFT, Pos);


    while(XMatrix[i] != -1 && YMatrix[i]!=-1)
      {
      int RealX = StartMapX + XMatrix[i];
      int RealY = StartMapY + YMatrix[i];

        //scroll so DT is in middle of screen
        ScrolledToX = RealX-(LocalShare->ScreenWidth-128)/2;
        if(ScrolledToX<0)
          ScrolledToX = 0;
  		  if(ScrolledToX>GetMaxScrollX())
     		 ScrolledToX = GetMaxScrollX();
        ScrolledToY = RealY-(LocalShare->ScreenHeight-32)/2;
        if(ScrolledToY<0)
          ScrolledToY = 0;
   	  if(ScrolledToY>GetMaxScrollY())
     	    ScrolledToY = GetMaxScrollY();
        QueueMessage(SCROLL, ScrolledToX, ScrolledToY);

        short x = RealX - ScrolledToX;
        short y = RealY - ScrolledToY;
        long Pos;
        Pos = y << 16;
        Pos += x;
        QueueMessage(WM_LBUTTONDOWN, MK_LBUTTON | MK_SHIFT, Pos);
        QueueMessage(WM_LBUTTONUP, MK_LBUTTON | MK_SHIFT, Pos);

      i++;
      }

    QueueMessage(SCROLL, OldMapX, OldMapY);
    }

  QueueLength = QueuePos;
  */
}

int CTAHook::GetMaxScrollX()
{
  int *PTR = (int*)0x00511de8;
  int *MapSizeX = (int*)(*PTR + 0x1422b);
  return *MapSizeX - (LocalShare->ScreenWidth-128);
}

int CTAHook::GetMaxScrollY()
{
  int *PTR = (int*)0x00511de8;
  int *MapSizeY = (int*)(*PTR + 0x1422f);
  return *MapSizeY - (LocalShare->ScreenHeight-64);
}

void CTAHook::CalculateRing()
{
	//char *unittested = new char [0xffff];
	//memset(unittested, 0, 0xffff);

	int footx = TAdynmem->Units[MouseOverUnit].UnitType->FootX + Spacing*2;
	int footy = TAdynmem->Units[MouseOverUnit].UnitType->FootY + Spacing*2;
	int posx = TAdynmem->Units[MouseOverUnit].XGridPos*16 - Spacing*16;
	int posy =  TAdynmem->Units[MouseOverUnit].YGridPos*16 - Spacing*16;

	//int x1 = posx;
	//int y1 = posy;
	//int x2 = posx + footx*16;
	//int y2 = posy + footy*16;

	//unittested[MouseOverUnit-1] = 1;

	//FindConnectedSquare(x1, y1, x2, y2, unittested);

	CalculateRing(posx, posy, footx, footy);
}

void CTAHook::FindConnectedSquare(int &x1, int &y1, int &x2, int &y2, char *unittested)
{
	CalculateRing(x1, y1, (x2-x1)/16, (y2-y1)/16);

	int i=0;
	while(XMatrix[i]!=-1 && YMatrix[i]!=-1)
	{
		TAdynmem->MouseMapPosX = XMatrix[i];
		TAdynmem->MouseMapPosY = YMatrix[i];

		int building = FindMouseUnit();
		if(unittested[building-1]==0)
		{
			int newx1 = TAdynmem->Units[building].XGridPos*16;
			int newy1 = TAdynmem->Units[building].YGridPos*16;
			int newx2 = newx1 + TAdynmem->Units[building].UnitType->FootX*16;
			int newy2 = newy1 + TAdynmem->Units[building].UnitType->FootY*16;

			if(newx1<x1)
				x1 = newx1;
			if(newy1<y1)
				y1 = newy1;
			if(newx2>x2)
				x2 = newx2;
			if(newy2>y2)
				y2 = newy2;

			unittested[building-1] = 1;

			FindConnectedSquare(x1, y1, x2, y2, unittested);
		}
		i++;
	}
}

void CTAHook::CalculateRing(int posx, int posy, int footx, int footy)
{
	int pos = 0;

	int add = 0;
	add = (footx%GetFootX()&&FullRingsEnabled&&GetFootX()<3&&GetFootY()<3) ? 1 : 0;
	int linelengthx = (footx/GetFootX()) + add + 1;
	add = (footy%GetFootY()&&FullRingsEnabled&&GetFootX()<3&&GetFootY()<3) ? 1 : 0;
	int linelengthy = (footy/GetFootY()) + add + 1;

	for(int i=0; i<linelengthx; i++)
	{
		XMatrix[pos] = posx + (GetFootX()*16)/2 + i*GetFootX()*16;
		YMatrix[pos] = posy - (GetFootY()*16)/2;
		pos++;
	}
	for(i=0; i<linelengthy; i++)
	{
		XMatrix[pos] = posx + footx*16 + (GetFootX()*16)/2;
		YMatrix[pos] = posy + (GetFootY()*16)/2 + i*GetFootY()*16;
		pos++;
	}
	for(i=0; i<linelengthx; i++)
	{
		XMatrix[pos] = posx + footx*16 - (GetFootX()*16)/2 - GetFootX()*16*i;
		YMatrix[pos] = posy + footy*16 + (GetFootY()*16)/2;
		pos++;
	}
	for(i=0; i<linelengthy; i++)
	{
		XMatrix[pos] = posx - (GetFootX()*16)/2;
		YMatrix[pos] = posy + footy*16 - (GetFootY()*16)/2 - GetFootY()*16*i;
		pos++;
	}

	XMatrix[pos] = -1;
	YMatrix[pos] = -1;
}

void CTAHook::VisualizeRing(LPDIRECTDRAWSURFACE DestSurf)
{
  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;

  int iMapX, iMapY;
  if(ScrollEnabled)
    {
    iMapX = ((AlliesWhiteboard*)LocalShare->Whiteboard)->GetMapX();
    iMapY = ((AlliesWhiteboard*)LocalShare->Whiteboard)->GetMapY();
    }

  for(int i=0; i<Length[FootPrintX-2]; i++)
    {
    int XCenter = ((EndX) + Rings[FootPrintX-2][i].x)+8-(((EndX) + Rings[FootPrintX-2][i].x)+iMapX+8)%16;
    int YCenter = ((EndY) + Rings[FootPrintY-2][i].y)+8-(((EndY) + Rings[FootPrintY-2][i].y)+iMapY+2)%16;
    RECT Dest;
    Dest.left = XCenter - 16;
    Dest.top = YCenter - 16;
    Dest.right = XCenter + 16;
    Dest.bottom = YCenter + 16;
    if(DestSurf->Blt(&Dest, lpRectSurf, NULL, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
      {
      DestSurf->Blt(&Dest, lpRectSurf, NULL, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
      }
    }

}

void CTAHook::ClickBuilding(int Xpos, int Ypos)
{
	msgstruct mu;
	mu.shiftstatus = 5;

	TAdynmem->MouseMapPosX = (short)Xpos;
	TAdynmem->MouseMapPosY = (short)Ypos;

	TestBuildSpot();

	TAMapClick(&mu);
}

short CTAHook::GetFootX()  //get footprint of selected unit to build
{
	return TAdynmem->UnitDef[TAdynmem->BuildNum].FootX;
}

short CTAHook::GetFootY()  //get footprint of selected unit to build
{
	return TAdynmem->UnitDef[TAdynmem->BuildNum].FootY;
}

void CTAHook::DrawBuildRect(int posx, int posy, int sizex, int sizey, int color)
{
	tagRECT rect = {posx, posy, posx+sizex, posy+sizey};

	if(rect.top<32)
		rect.top = 32;
	if(rect.top>LocalShare->ScreenHeight-33)
		rect.top = LocalShare->ScreenHeight-33;
	if(rect.bottom<32)
		rect.bottom = 32;
	if(rect.bottom>LocalShare->ScreenHeight-33)
		rect.bottom = LocalShare->ScreenHeight-33;

	if(rect.left<128)
		rect.left = 128;
	if(rect.left>LocalShare->ScreenWidth)
		rect.left = LocalShare->ScreenWidth;
	if(rect.right<128)
		rect.right = 128;
	if(rect.right>LocalShare->ScreenWidth)
		rect.right = LocalShare->ScreenWidth;

	TADrawRect(NULL, &rect, color);
	
	rect.top++;
	rect.left++;
	rect.bottom++;
	rect.right++;

	if(rect.top<32)
		rect.top = 32;
	if(rect.top>LocalShare->ScreenHeight-33)
		rect.top = LocalShare->ScreenHeight-33;
	if(rect.bottom<32)
		rect.bottom = 32;
	if(rect.bottom>LocalShare->ScreenHeight-33)
		rect.bottom = LocalShare->ScreenHeight-33;

	if(rect.left<128)
		rect.left = 128;
	if(rect.left>LocalShare->ScreenWidth)
		rect.left = LocalShare->ScreenWidth;
	if(rect.right<128)
		rect.right = 128;
	if(rect.right>LocalShare->ScreenWidth)
		rect.right = LocalShare->ScreenWidth;

	TADrawRect(NULL, &rect, color);
}

void CTAHook::EnableTABuildRect()
{
	/*char ops = 0x53;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EB6, &ops, 1, NULL);

	ops = 0x52;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EBB, &ops, 1, NULL);
	ops = 0x50;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EBC, &ops, 1, NULL);

	ops = 0xe8;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC5, &ops, 1, NULL);
	ops = 0xf6;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC6, &ops, 1, NULL);
	ops = 0x59;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC7, &ops, 1, NULL);
	ops = 0x05;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC8, &ops, 1, NULL);
	ops = 0x00;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC9, &ops, 1, NULL);*/

	int ops = 0x5368EC83;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x4BF8C0, &ops, 4, NULL);//enable TA buildrectangel

}

void CTAHook::DisableTABuildRect()
{
	/*int ops = 0x90909090;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EB6, &ops, 1, NULL);

	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EBB, &ops, 2, NULL);

	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC5, &ops, 4, NULL);
	WriteProcessMemory(GetCurrentProcess(), (void*)0x469EC9, &ops, 1, NULL);*/
	
	int ops = 0xcc2;
	WriteProcessMemory(GetCurrentProcess(), (void*)0x4BF8C0, &ops, 4, NULL);//disable TA buildrectangel

}

void CTAHook::PaintMinimapRect()
{
	TADrawRect(NULL, &TAdynmem->MinimapRect, 0xC2);	
}