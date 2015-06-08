#include "cincome.h"
#include "iddrawsurface.h"
#include <stdio.h>
#include "font.h"

int X,Y;

CIncome::CIncome()
{
  LocalShare->Income = this;
  LPDIRECTDRAW TADD = (IDirectDraw*)LocalShare->TADirectDraw;

  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
  ddsd.dwWidth = PlayerWidth;
  ddsd.dwHeight = PlayerHight*10;

  TADD->CreateSurface(&ddsd, &lpIncomeSurf, NULL);

  BackgroundType = 0;
  ReadPos();
  First = true;
}

CIncome::~CIncome()
{
  if(lpIncomeSurf)
    lpIncomeSurf->Release();

  WritePos();
}

void CIncome::BlitIncome(LPDIRECTDRAWSURFACE DestSurf)
{
  if(lpIncomeSurf->IsLost() != DD_OK)
    {
    if(lpIncomeSurf->Restore() != DD_OK)
      return;
    }
  BlitState++;

  //ShowAllIncome();
  if(DataShare->TAProgress == TAInGame)
    {
    if(BlitState%30 == 1)
      {
      DDSURFACEDESC ddsd;
      DDRAW_INIT_STRUCT(ddsd);
      int PlayerDrawn;

      if(lpIncomeSurf->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)==DD_OK)
        {
        SurfaceMemory = ddsd.lpSurface;
        lPitch = ddsd.lPitch;

        if(DataShare->PlayingDemo)
          PlayerDrawn = ShowAllIncome();
        else
          PlayerDrawn = ShowAllyIncome();
          //PlayerDrawn = ShowAllIncome();
        }
      else
        SurfaceMemory = NULL;

      if(First == true && PlayerDrawn>0)
        {
        First = false;
        CorrectPos();
        }

      lpIncomeSurf->Unlock(NULL);
      }

    RECT Dest;
    Dest.left = posX;
    Dest.top = posY;
    Dest.right = posX + PlayerWidth;
    Dest.bottom = posY + LocalShare->Height;
    RECT Source;
    Source.left = 0;
    Source.top = 0;
    Source.right = PlayerWidth;
    Source.bottom = LocalShare->Height;

    DDBLTFX ddbltfx;
    DDRAW_INIT_STRUCT(ddbltfx);
    ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 1;
    ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 1;
    if(DestSurf->Blt(&Dest, lpIncomeSurf, &Source, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE , &ddbltfx)!=DD_OK)
      {
      DestSurf->Blt(&Dest, lpIncomeSurf, &Source, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE , &ddbltfx);
      }
    }
}

int CIncome::ShowAllIncome()
{
  if(BackgroundType==2)
    FillRect(0);
  else
    FillRect(1);

  DataShare->IsRunning = 15;

  LocalShare->Width = PlayerWidth;

  int j = 0;
  for(int i=1; i<10; i++)
    {
    if(strlen(DataShare->PlayerNames[i])>0)
      {
      DataShare->IsRunning = 50;
      ShowPlayerIncome(i, 0, j*PlayerHight);
      j++;
      }
    }
  LocalShare->Height = PlayerHight*j;
  return j;
}

int CIncome::ShowAllyIncome()
{
  if(BackgroundType==2)
    FillRect(0);
  else
    FillRect(1);

  DataShare->IsRunning = 15;

  LocalShare->Width = PlayerWidth;

  //int OffsetX = posX;
  //int OffsetY = posY;
  LocalShare->Height = 0;
  int j = 0;
  for(int i=1; i<10; i++)
    {
    if(DataShare->allies[i])
      {
      DataShare->IsRunning = 100;
      ShowPlayerIncome(i, 0, j*PlayerHight);
      j++;
      }
    }

  LocalShare->Height = PlayerHight*j;
  return j;
}

void CIncome::ShowPlayerIncome(int Player, int posx, int posy)
{
  float ValueF;
  char Value[100];

  char C = GetPlayerColor(Player);

  //DrawText(DataShare->PlayerNames[Player], posx+45, posy+1, 0);
  DrawPlayerRect(posx+36, posy+1, C);

  DrawText(DataShare->PlayerNames[Player], posx+44, posy, 7);

  DrawStorageText(DataShare->storedM[Player], posx, posy+10);
  DrawStorageText(DataShare->storedE[Player], posx, posy+20);

  PaintStoragebar(posx+44, posy+10, Player , MetalBar);
  ValueF = DataShare->incomeM[Player];
  sprintf(Value, "+%.1f", ValueF);
  DrawText(Value, posx+150, posy+11, 0);
  DrawText(Value, posx+149, posy+10, 6);

  PaintStoragebar(posx+44, posy+20, Player , EnergyBar);
  ValueF = DataShare->incomeE[Player];
  sprintf(Value, "+%.0f", ValueF);
  DrawText(Value, posx+150, posy+21, 0);
  DrawText(Value, posx+149, posy+20, 6);
}

int CIncome::DrawStorageText(float Storage, int posx, int posy)
{
  char Value[100];
  if(Storage<10000)
    {
    sprintf(Value, "%.0f", Storage);
    }
  else if(Storage<100000)
    {
    Storage = Storage/1000;
    sprintf(Value, "%.1fK", Storage);
    }
  else
    {
    Storage = Storage/1000;
    sprintf(Value, "%.0fK", Storage);
    }
  DrawText(Value, posx+1 + (5-strlen(Value))*8, posy+1, 0);
  DrawText(Value, posx + (5-strlen(Value))*8, posy, 6);

  return 0;
}

void CIncome::DrawText(char *String, int posx, int posy, char Color)
{
  if(SurfaceMemory == NULL)
    return;

  char *SurfMem = (char*)SurfaceMemory;

  if(BackgroundType == 1)
    {
    for(int i=0; i<9; i++)
      {
      memset(&SurfMem[posx+(posy+i)*lPitch], 0, strlen(String)*8+1);
      }
    }

  posx++;
  posy++;
  for(int i=0; i<strlen(String); i++)
    {
    for(int j=0; j<8; j++)
      {
      for(int k=0; k<8; k++)
        {
        bool b = ThinFont[String[i]*8+j] & (1 << k);
        if(b)
          SurfMem[(posx+(i*8)+(7-k))+(posy+j)*lPitch] = Color;
        }
      }
    }
}

void CIncome::PaintStoragebar(int posx, int posy, int Player, int Type)
{
  if(SurfaceMemory == NULL)
    return;

  char *SurfMem = (char*)SurfaceMemory;
  int StorageBarLength;
  char FillColor;
  for(int i=0; i<100; i++)
    {
    SurfMem[(posx+i)+posy*lPitch] = 0;
    SurfMem[(posx+i)+(posy+1)*lPitch] = 0;
    SurfMem[(posx+i)+(posy+2)*lPitch] = 0;
    }

  if(Type == EnergyBar)
    {
    if(DataShare->storageE[Player]!=0)
      StorageBarLength = (DataShare->storedE[Player] / DataShare->storageE[Player])*100;
    FillColor = 208;
    }
  if(Type == MetalBar)
    {
    if(DataShare->storageM[Player]!=0)
      StorageBarLength = (DataShare->storedM[Player] / DataShare->storageM[Player])*100;
    FillColor = 224;
    }

  for(i=0; i<StorageBarLength; i++)
    {
    SurfMem[(posx+i)+posy*lPitch] = FillColor;
    SurfMem[(posx+i)+(posy+1)*lPitch] = FillColor;
    SurfMem[(posx+i)+(posy+2)*lPitch] = FillColor;
    }
}

void CIncome::FillRect(char Color)
{
  if(SurfaceMemory == NULL)
    return;

  char *SurfMem = (char*)SurfaceMemory;

  for(int i=0; i<LocalShare->Height; i++)
    {
    memset(&SurfMem[i*lPitch], Color, PlayerWidth);
    }
}

void CIncome::Set(int BGType)
{
  BackgroundType = BGType;
}

void CIncome::ReadPos()
{
  HKEY hKey;
  DWORD dwDisposition;
  DWORD Size = sizeof(int);

  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

  if(RegQueryValueEx(hKey, "IncomePosX", NULL, NULL, (unsigned char*)&posX, &Size) == ERROR_SUCCESS)
    {
    }
  else
    {
    //default pos
    posX = LocalShare->ScreenWidth - 254;
    }

  if(RegQueryValueEx(hKey, "IncomePosY", NULL, NULL, (unsigned char*)&posY, &Size) == ERROR_SUCCESS)
    {
    }
  else
    {
    posY = 50;
    }

  RegCloseKey(hKey);
}

void CIncome::WritePos()
{
  HKEY hKey;
  DWORD dwDisposition;

  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

  RegSetValueEx(hKey, "IncomePosX", NULL, REG_DWORD, (unsigned char*)&posX, sizeof(int));
  RegSetValueEx(hKey, "IncomePosY", NULL, REG_DWORD, (unsigned char*)&posY, sizeof(int));

  RegCloseKey(hKey);
}

bool CIncome::Message(HWND WinProchWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch(Msg)
    {
    case WM_MOUSEMOVE:
      if((wParam&MK_LBUTTON)>0 && StartedInRect == true)
        {
        posX += LOWORD(lParam)-X;
        posY += HIWORD(lParam)-Y;
        X = LOWORD(lParam);
        Y = HIWORD(lParam);
        return true;
        }
      else
        StartedInRect = false;
      X = LOWORD(lParam);
      Y = HIWORD(lParam);
      break;
    case WM_LBUTTONDOWN:
      if(LOWORD(lParam)>posX && LOWORD(lParam)<(posX+LocalShare->Width) && HIWORD(lParam)>(posY) && HIWORD(lParam)<((posY)+LocalShare->Height))
        {
        StartedInRect = true;
        return true;
        }
      break;
    case WM_LBUTTONDBLCLK:
      if(LOWORD(lParam)>posX && LOWORD(lParam)<(posX+LocalShare->Width) && HIWORD(lParam)>(posY) && HIWORD(lParam)<((posY)+LocalShare->Height))
        {
        ((Dialog*)LocalShare->Dialog)->ShowDialog();
        return true;
        }
      break;
    }
  return false;
}

void CIncome::CorrectPos()
{
  if(posX<0)
    posX = 0;
  if(posX>(LocalShare->ScreenWidth-PlayerWidth))
    posX = LocalShare->ScreenWidth-PlayerWidth;

  if(posY<0)
    posY = 0;
  if(posY>(LocalShare->ScreenHeight-(PlayerHight*2))) //always two players inside screen
    posY = LocalShare->ScreenHeight-(PlayerHight*2);

}

char CIncome::GetPlayerColor(int Player)
{
  switch(DataShare->PlayerColors[Player])
    {
    case 0:
      return 227;
    case 1:
      return 212;
    case 2:
      return 80;
    case 3:
      return 235;
    case 4:
      return 108;
    case 5:
      return 219;
    case 6:
      return 208;
    case 7:
      return 93;
    case 8:
      return 130;
    case 9:
      return 67;
    }
}

void CIncome::DrawPlayerRect(int posx, int posy, char Color)
{
  if(SurfaceMemory == NULL)
    return;

  char *SurfMem = (char*)SurfaceMemory;

  for(int i=0; i<8; i++)
    {
    memset(&SurfMem[posx+(posy+i)*lPitch], Color, 8);
    }

}

