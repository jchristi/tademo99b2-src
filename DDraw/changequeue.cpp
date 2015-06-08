#include "changequeue.h"
#include "iddrawsurface.h"
#include <stdio.h>
#include "tamem.h"
extern void OutptTxt(char *string);

TAdynmemStruct *TAdynmem;

bool pressed = false;
HWND GWinProchWnd;

CChangeQueue::CChangeQueue()
{
	LocalShare->ChangeQueue = this;

   int *PTR = (int*)0x00511de8;
   TAdynmem = (TAdynmemStruct*)(*PTR);
}

CChangeQueue::~CChangeQueue()
{

}

void Write(HWND WinProchWnd)
{
	/*char Mtmp[100];
   sprintf(Mtmp, "IsUnit: %i", &TAdynmem->Units[0].IsUnit);
   OutptTxt(Mtmp);
   sprintf(Mtmp, "Weapon1: %i", &TAdynmem->Units[0].Weapon1);
   OutptTxt(Mtmp);
   sprintf(Mtmp, "Weapon2: %i", &TAdynmem->Units[0].Weapon2);
   OutptTxt(Mtmp);
   sprintf(Mtmp, "Weapon3: %i", &TAdynmem->Units[0].Weapon3);
   OutptTxt(Mtmp);

   sprintf(Mtmp, "Xpos: %i", &TAdynmem->Units[0].XPos);
   OutptTxt(Mtmp);

	sprintf(Mtmp, "Orders: %i", TAdynmem->Units[0].UnitOrders);
   OutptTxt(Mtmp);
	sprintf(Mtmp, "UnitSelected: %i", TAdynmem->Units[0].UnitSelected);
   OutptTxt(Mtmp);*/


   /*UnitOrdersStruct *LastOrder = TAdynmem->Units[0].UnitOrders;
   UnitOrdersStruct *NewLastOrder;
   while(LastOrder->NextOrder)
   {
     NewLastOrder = LastOrder;
     LastOrder = LastOrder->NextOrder;

   }

   LastOrder->NextOrder = TAdynmem->Units[0].UnitOrders;
   TAdynmem->Units[0].UnitOrders = LastOrder;

   NewLastOrder->NextOrder = NULL; */


}

void CChangeQueue::Blit(LPDIRECTDRAWSURFACE DestSurf)
{

}

bool CChangeQueue::Message(HWND WinProchWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  /*switch(Msg)
    {
    case WM_KEYDOWN:
      if(wParam == 67) //c
        {
        //TAdynmem->Units[0].UnitOrders->ThisStart.ThisStart = 1;
        //Write(WinProchWnd);
        *((int*)NULL) = 1;
        return true;
        }
      break;
    case WM_KEYUP:
      if(wParam == 67) //c
        {
        //TAdynmem->Units[0].UnitOrders->ThisStart.ThisStart = 0;
        return true;
        }
      break;
    }*/

  return false;
}

