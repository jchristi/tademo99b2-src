#include "idlevillager.h"
#include "iddrawsurface.h"
#include "tahook.h"
#include <stdio.h>

CIdleUnits::CIdleUnits()
{
  LocalShare->IdleUnits = this;
}

CIdleUnits::~CIdleUnits()
{

}

void CIdleUnits::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
/*  int *BasePTR = (int*)0x511de8;
  int *UnitPTR = (int*)((*BasePTR)+0x1b8e+0x3c);

  for(int i=0; i<30; i++)
    {
    int Data;
    char *BytePTR = (char*)((*UnitPTR)+i + (0x118*2));
    Data = *BytePTR;

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 140, 35 + i*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }*/


  int UnitOffset = 0x118;

  //find TA windows
  HWND TAhWnd = FindWindow("Total Annihilation Class", "Total Annihilation");
  //get PID
  DWORD PID;
  GetWindowThreadProcessId(TAhWnd, &PID);
  //enable all acess for ta
  HANDLE TAProc = OpenProcess(PROCESS_ALL_ACCESS, false, PID);

  int Adress = 0x511de8;
  int BasePTR;

  ReadProcessMemory(TAProc, (void*)Adress, (void*)&BasePTR, sizeof(int), NULL);

  int UnitPTR;
  ReadProcessMemory(TAProc, (void*)(BasePTR+0x1b8e+0x3c), (void*)&UnitPTR, sizeof(int), NULL);


/*  int StartAt = 272;
  for(int i=0; i<32; i++)
    {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + StartAt), (void*)&Data, 4, NULL);

    char Outstr[20];

    int Bit = (Data & (1<<i))>>i;
    sprintf(Outstr, "%1i : %1i", i, Bit);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 140, 35 + (i)*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }    */

  /*int StartAt = 70;       //printing the build queue
  for(int i=StartAt; i<StartAt+40; i++)
    {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + 92), (void*)&Data, 4, NULL);
    int Data2 = 0;
    ReadProcessMemory(TAProc, (void*)(Data + i), (void*)&Data2, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data2);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 140, 35 + (i-StartAt)*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }       */

  int StartAt = 0;
  for(int i=StartAt; i<StartAt+40; i++)
    {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 200, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }
  StartAt += 40;
  for(i=StartAt; i<StartAt+40; i++)
  {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 280, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }
  StartAt += 40;
  for(i=StartAt; i<StartAt+40; i++)
  {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 340, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }
  StartAt += 40;
  for(i=StartAt; i<StartAt+40; i++)
  {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 420, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }
  StartAt += 40;
  for(i=StartAt; i<StartAt+40; i++)
  {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 500, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }
  StartAt += 40;
  for(i=StartAt; i<StartAt+40; i++)
  {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 580, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }
  StartAt += 40;
  for(i=StartAt; i<StartAt+40; i++)
  {
    int Data = 0;
    ReadProcessMemory(TAProc, (void*)(UnitPTR + (2*UnitOffset) + i), (void*)&Data, 1, NULL);

    char Outstr[20];
    sprintf(Outstr, "%1i : %1i", i, Data);

    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 660, 35 + (i-(StartAt))*15, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);
    }

    char Outstr[20];
    sprintf(Outstr, "%1i", UnitPTR + (2*UnitOffset));
    HDC hdc;
    DestSurf->GetDC(&hdc);

    TextOut(hdc, 490, 35, Outstr, strlen(Outstr));

    DestSurf->ReleaseDC(hdc);

  /*int *PTR1 = (int*)0x511de8;
  int *Commander = (int*)((*PTR1)+0x1b8e+0x3c);
  unsigned short *XPos = (unsigned short*)((*Commander) + 0x6c);
  unsigned short *YPos = (unsigned short*)(*Commander + 0x74); */

  CloseHandle(TAProc);
}

void FixAck()
{
/*  HANDLE TAProc = GetCurrentProcess();

  int Adress = 0x511de8;
  int BasePTR = 0;
  int BasePTR2;
  int UnitOffset = 0x249;

  ReadProcessMemory(TAProc, (void*)Adress, (void*)&BasePTR, sizeof(int), NULL);

  //Edit1->Text = BasePTR + 0x1b8e+0x3c;

  //int UnitPTR;
  ReadProcessMemory(TAProc, (void*)(BasePTR+0x1439b), (void*)&BasePTR2, sizeof(int), NULL);

  int UnitPTR;
  int UnitArrayOffset = 0x118;
  ReadProcessMemory(TAProc, (void*)(BasePTR+0x1b8e+0x3c), (void*)&UnitPTR, sizeof(int), NULL);

  char Buff[30];
  for(int i=0; i<300; i++)
    {
    ReadProcessMemory(TAProc, (void*)(BasePTR2 + UnitOffset*i), (void*)Buff, 30, NULL);

    if(strcmp(Buff, "Adv. Construction Kbot") == 0)
      {
      static float BreakRate = 0.001f;
      //WriteProcessMemory(TAProc, (void*)(BasePTR2 + UnitOffset*i + 458), (void*)&BreakRate, sizeof(float), NULL);
      float *BPTR = (float*)(BasePTR2 + UnitOffset*i + 458);
      *BPTR = 0.01;
      BreakRate += 0.4;
      }
    }  */
}

void CIdleUnits::FindIdleConst()
{
  //FixAck();

  int UnitOffset = 0x118;

  int *PTR1 = (int*)0x511de8;
  int *UnitPTR = (int*)((*PTR1)+0x1b8e+0x3c);

  short NumUnits = *((int*)(*PTR1+0x1ca7));

  int i=0;
  int Unit = 0;

  static int LastNum = 0;

  while(Unit<NumUnits)
    {
    //short *Unit = (short*)(*UnitPTR + 2 + i*UnitOffset);
    char *UnitDead = (char*)(*UnitPTR + 247 + i*UnitOffset);
    char *Builder = (char*)(*UnitPTR + 31 + i*UnitOffset);  //16 == no weapon
    short *XPos = (short*)(*UnitPTR + 0x6c + i*UnitOffset);
    short *YPos = (short*)(*UnitPTR + 0x74 + i*UnitOffset);
    int *IsUnit = (int*)(*UnitPTR + 0 + i*UnitOffset);
    char *UnitSelected = (char*)(*UnitPTR + 272 + i*UnitOffset);
    //char *Working = (char*)(*UnitPTR + 186 + i*UnitOffset);
    //int *Moving = (int*)(*UnitPTR + 208 + i*UnitOffset);
    int *UnitOrderPTR = (int*)(*UnitPTR + 92 + i*UnitOffset);

    if(*UnitDead!=0 && *UnitDead!=1)
      {
      Unit++;

      if(*IsUnit)
        {
        //check if workertime > 0

        int *DefiPTR = (int*)(*UnitPTR + 146 + i*UnitOffset);
        unsigned short *WorkerTime = (unsigned short*)(*DefiPTR + 510);
        if(*WorkerTime>0)
          {
          char *UnitState = (char*)(*UnitOrderPTR + 4);
          if((*UnitState==41 || *UnitState==64) && i>LastNum) //idle
            {

            ScrollToCenter(*XPos, *YPos);
            LastNum = i;
            //*UnitSelected = (*UnitSelected) | 0x50;
            return;
            }
          }
        }
      }
    i++;
    if(DataShare->MaxUnits == 0)
      {
      if(i==500)
        {
        LastNum = 0;
        return;
        }
      }
   else
     {
     if(i==DataShare->MaxUnits)
      {
      LastNum = 0;
      return;
      }
     }
    }

  if(LastNum == 0) //no units found and all units searched
    return;

  //search from the beginning
  LastNum = 0;
  FindIdleConst();
}

void CIdleUnits::ScrollToCenter(int x, int y)
{
  int *PTR = (int*)0x00511de8;
  int *XPointer = (int*)(*PTR + 0x1431f);
  int *YPointer = (int*)(*PTR + 0x14323);

  x -= (LocalShare->ScreenWidth-128)/2;
  y -= (LocalShare->ScreenHeight-64)/2;

  if(x<0)
    x = 0;
  if(y<0)
    y = 0;
  if(x>CTAHook::GetMaxScrollX())
    x = CTAHook::GetMaxScrollX();
  if(y>CTAHook::GetMaxScrollY())
    y = CTAHook::GetMaxScrollY();

  //*XPointer = x;
  *(XPointer + 2) = x;
  //*YPointer = y;
  *(YPointer + 2)= y;
}

bool CIdleUnits::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  //shift == 16
  //ctrl == 17
  switch(Msg)
    {
    case WM_KEYDOWN:
      if(wParam == 66 && (GetAsyncKeyState(17)&0x8000)>0 && (GetAsyncKeyState(16)&0x8000)==0) // ctrl + b
        {
        FindIdleConst();
        return true;
        }
      break;
    }

  return false;
}

void CIdleUnits::DeselectUnits()
{
  int UnitOffset = 0x118;

  int *PTR1 = (int*)0x511de8;
  int *UnitPTR = (int*)((*PTR1)+0x1b8e+0x3c);

  int i=0;
  int Unit = 0;
  short NumUnits = *((int*)(*PTR1+0x1ca7));

  //deselects all units
  while(Unit<NumUnits)
    {
    char *UnitDead = (char*)(*UnitPTR + 247 + i*UnitOffset);
    char *UnitSelected = (char*)(*UnitPTR + 272 + i*UnitOffset);

    if(*UnitDead!=0 && *UnitDead!=1)
      {
      *UnitSelected = (*UnitSelected) & 0xAF;
      }
    if(*UnitDead!=0 && *UnitDead!=1)
      {
      Unit++;
      }
    i++;
    }
}
