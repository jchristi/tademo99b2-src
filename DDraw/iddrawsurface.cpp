//---------------------------------------------------------------------------
#pragma hdrstop

#include "iddrawsurface.h"
#include <stdio.h>
#include "font.h"
#include <time.h>
//#include <conio.h>
#include "pcx.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

extern HINSTANCE HInstance;
short MouseX,MouseY;
bool StartedInRect;

int Draw;

IDDrawSurface::IDDrawSurface(LPDIRECTDRAWSURFACE lpTASurf, bool iWindowed, int iScreenWidth, int iScreenHeight)
{
  lpFront = lpTASurf;
  OutptTxt("IDDrawSurface Created");
  Windowed = iWindowed;
  ScreenWidth = iScreenWidth;
  ScreenHeight = iScreenHeight;
  LocalShare->ScreenWidth = iScreenWidth;
  LocalShare->ScreenHeight = iScreenHeight;
  //LocalShare->XPos = ScreenWidth - 254;
  //LocalShare->YPos = 50;
  LocalShare->DDrawSurfClass = this;
  LocalShare->TADirectDrawFrontSurface = lpTASurf;

  DataShare->IsRunning = 10;

  if(LocalShare->TAWndProc == NULL)
    ReplaceTAProc();
  VerticalSync = true;

  //check if version is 3.1 standar
  if((*((unsigned char*)0x4ad494))==0x00 && (*((unsigned char*)0x4ad495))==0x55 && (*((unsigned char*)0x4ad496))==0xe8)
    LocalShare->CompatibleVersion = true;
  else
    {
    LocalShare->CompatibleVersion = false;
    DataShare->ehaOff = 1; //set the ehaofvariable
    }

  PlayingMovie = false;
  DisableDeInterlace = false;

  HKEY hKey;
  DWORD dwDisposition;
  DWORD Size;
  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
  Size = sizeof(bool);
  if(RegQueryValueEx(hKey, "DisableDeInterlaceMovie", NULL, NULL, (unsigned char*)&DisableDeInterlace, &Size) != ERROR_SUCCESS)
    {
    //value does not exist.. create it
    DisableDeInterlace = true;
    RegSetValueEx(hKey, "DisableDeInterlaceMovie", NULL, REG_BINARY, (unsigned char*)&DisableDeInterlace, sizeof(bool));
    }
  RegCloseKey(hKey);

  //DisableDeInterlace = true;
}

HRESULT __stdcall IDDrawSurface::QueryInterface(REFIID riid, LPVOID FAR * ppvObj)
{
  OutptTxt("QueryInterface");
  return lpFront->QueryInterface(riid, ppvObj);
}

ULONG __stdcall IDDrawSurface::AddRef()
{
  OutptTxt("AddRef");
  return lpFront->AddRef();
}

ULONG __stdcall IDDrawSurface::Release()
{
  if(lpDDClipper)
    {
    lpDDClipper->Release();
    lpDDClipper = NULL;
    }
 /* if(lpBattleFieldClipper)
    {
    lpBattleFieldClipper->Release();
    lpBattleFieldClipper = NULL;
    }
  if(lpBattleFieldSurf)
    {
    lpBattleFieldSurf->Release();
    lpBattleFieldSurf = NULL;
    }         */

  OutptTxt("Release");
  ULONG result = lpFront->Release();

  delete ScreenRegion;
  delete BattleFieldRegion;

  delete this;
  return result;
}

HRESULT __stdcall IDDrawSurface::AddAttachedSurface(LPDIRECTDRAWSURFACE arg1)
{
  OutptTxt("AddAttachedSurface");
  return lpFront->AddAttachedSurface(arg1);
}

HRESULT __stdcall IDDrawSurface::AddOverlayDirtyRect(LPRECT arg1)
{
  OutptTxt("AddOverlayDirtyRect");
  return lpFront->AddOverlayDirtyRect(arg1);
}

HRESULT __stdcall IDDrawSurface::Blt(LPRECT arg1, LPDIRECTDRAWSURFACE arg2, LPRECT arg3, DWORD arg4, LPDDBLTFX arg5)
{
  OutptTxt("Blt");
  PlayingMovie = true;
  return lpFront->Blt(arg1, arg2, arg3, arg4, arg5);
}

HRESULT __stdcall IDDrawSurface::BltBatch(LPDDBLTBATCH arg1, DWORD arg2, DWORD arg3)
{
  OutptTxt("BltBatch");
  return lpFront->BltBatch(arg1, arg2, arg3);
}

HRESULT __stdcall IDDrawSurface::BltFast(DWORD arg1, DWORD arg2, LPDIRECTDRAWSURFACE arg3, LPRECT arg4, DWORD arg5)
{
  OutptTxt("BltFast");
  return lpFront->BltFast(arg1, arg2, arg3, arg4, arg5);
}

HRESULT __stdcall IDDrawSurface::DeleteAttachedSurface(DWORD arg1, LPDIRECTDRAWSURFACE arg2)
{
  OutptTxt("DeleteAttachedSurface");
  return lpFront->DeleteAttachedSurface(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::EnumAttachedSurfaces(LPVOID arg1, LPDDENUMSURFACESCALLBACK arg2)
{
  OutptTxt("EnumAttachedSurfaces");
  return lpFront->EnumAttachedSurfaces(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::EnumOverlayZOrders(DWORD arg1, LPVOID arg2, LPDDENUMSURFACESCALLBACK arg3)
{
  OutptTxt("EnumOverlayZOrders");
  return lpFront->EnumOverlayZOrders(arg1, arg2, arg3);
}

HRESULT __stdcall IDDrawSurface::Flip(LPDIRECTDRAWSURFACE arg1, DWORD arg2)
{
  OutptTxt("Flip");
  return lpFront->Flip(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::GetAttachedSurface(LPDDSCAPS arg1, LPDIRECTDRAWSURFACE FAR *arg2)
{
  OutptTxt("GetAttachedSurface");

  HRESULT result = lpFront->GetAttachedSurface(arg1, arg2);

  lpBack = *arg2;

  CreateClipplist();
  LocalShare->TADirectDrawBackSurface = *arg2;

  return result;
}

HRESULT __stdcall IDDrawSurface::GetBltStatus(DWORD arg1)
{
  OutptTxt("GetBltStatus");
  return lpFront->GetBltStatus(arg1);
}

HRESULT __stdcall IDDrawSurface::GetCaps(LPDDSCAPS arg1)
{
  OutptTxt("GetCaps");
  return lpFront->GetCaps(arg1);
}

HRESULT __stdcall IDDrawSurface::GetClipper(LPDIRECTDRAWCLIPPER FAR* arg1)
{
  OutptTxt("GetClipper");
  return lpFront->GetClipper(arg1);
}

HRESULT __stdcall IDDrawSurface::GetColorKey(DWORD arg1, LPDDCOLORKEY arg2)
{
  OutptTxt("GetColorKey");
  return lpFront->GetColorKey(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::GetDC(HDC FAR *arg1)
{
  OutptTxt("GetDC");
  return lpFront->GetDC(arg1);
}

HRESULT __stdcall IDDrawSurface::GetFlipStatus(DWORD arg1)
{
  OutptTxt("GetFlipStatus");
  return lpFront->GetFlipStatus(arg1);
}

HRESULT __stdcall IDDrawSurface::GetOverlayPosition(LPLONG arg1, LPLONG arg2)
{
  OutptTxt("GetOverlayPosition");
  return lpFront->GetOverlayPosition(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::GetPalette(LPDIRECTDRAWPALETTE FAR* arg1)
{
  OutptTxt("GetPalette");
  return lpFront->GetPalette(arg1);
}

HRESULT __stdcall IDDrawSurface::GetPixelFormat(LPDDPIXELFORMAT arg1)
{
  OutptTxt("GetPixelFormat");
  return lpFront->GetPixelFormat(arg1);
}

HRESULT __stdcall IDDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC arg1)
{
  OutptTxt("GetSurfaceDesc");
  return lpFront->GetSurfaceDesc(arg1);
}

HRESULT __stdcall IDDrawSurface::Initialize(LPDIRECTDRAW arg1, LPDDSURFACEDESC arg2)
{
  OutptTxt("Initialize");
  return lpFront->Initialize(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::IsLost()
{
  OutptTxt("IsLost");
  return lpFront->IsLost();
}

HRESULT __stdcall IDDrawSurface::Lock(LPRECT arg1, LPDDSURFACEDESC arg2, DWORD arg3, HANDLE arg4)
{
	HRESULT result = lpBack->Lock(arg1, arg2, arg3, arg4);

	TAHook.TABlit();

  	#ifdef _DEBUG
		DDDTA.Moo();
	#endif

	if(result == DD_OK)
		SurfaceMemory = arg2->lpSurface;
	else
		SurfaceMemory = NULL;

	lPitch = arg2->lPitch;

	return result;
}

HRESULT __stdcall IDDrawSurface::ReleaseDC(HDC arg1)
{
  OutptTxt("ReleaseDC");
  return lpFront->ReleaseDC(arg1);
}

HRESULT __stdcall IDDrawSurface::Restore()
{
	((CDDDTA*)LocalShare->DDDTA)->FrameUpdate();

	return lpFront->Restore();
}

HRESULT __stdcall IDDrawSurface::SetClipper(LPDIRECTDRAWCLIPPER arg1)
{
  OutptTxt("SetClipper");
  //lpDDClipper = arg1;
  return lpFront->SetClipper(arg1);
}

HRESULT __stdcall IDDrawSurface::SetColorKey(DWORD arg1, LPDDCOLORKEY arg2)
{
  OutptTxt("SetColorKey");
  return lpFront->SetColorKey(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::SetOverlayPosition(LONG arg1, LONG arg2)
{
  OutptTxt("SetOverlayPosition");
  return lpFront->SetOverlayPosition(arg1, arg2);
}

HRESULT __stdcall IDDrawSurface::SetPalette(LPDIRECTDRAWPALETTE arg1)
{
  OutptTxt("SetPalette");
  Palette = arg1;
  return lpFront->SetPalette(arg1);
}

HRESULT __stdcall IDDrawSurface::Unlock(LPVOID arg1)
{
 /* if(DataShare->UpdateLos == 0)
    {
    HRESULT result = lpFront->Unlock(arg1);
    /*if(lpFront->Flip(NULL, DDFLIP_DONOTWAIT | DDFLIP_NOVSYNC) != DD_OK)
      {
      if(lpFront->Flip(NULL, DDFLIP_NOVSYNC) != DD_OK)
        lpFront->Flip(NULL, DDFLIP_WAIT);
      } /
      return result;
    }   */

  if(PlayingMovie) //deinterlace and flip directly
    {
    if(!DisableDeInterlace)
      {
      DeInterlace();
      HRESULT result = lpBack->Unlock(arg1);
      if(lpFront->Flip(NULL, DDFLIP_DONOTWAIT | DDFLIP_NOVSYNC) != DD_OK)
        {
        if(lpFront->Flip(NULL, DDFLIP_NOVSYNC) != DD_OK)
          lpFront->Flip(NULL, DDFLIP_WAIT);
        }
      return result;
      }
    else
      PlayingMovie = false;
    }

  HRESULT result;
  if(DataShare->ehaOff == 1 && !DataShare->PlayingDemo) //disable everything
    {//just unlock flip and return
    lpDDClipper->SetClipList(ScreenRegion,0);
    result = lpBack->Unlock(arg1);
    if(result!=DD_OK)
      return result;
    }
  else
  {

	if(SurfaceMemory!=NULL)
		{
		WhiteBoard.LockBlit((char*)SurfaceMemory, lPitch);
		SharedRect.LockBlit((char*)SurfaceMemory, lPitch);
		}

	result = lpBack->Unlock(arg1);
	if(result!=DD_OK)
		return result;

  //if(lpBattleFieldSurf->IsLost() != DD_OK)
  //  lpBattleFieldSurf->Restore();
  //IdleUnits.Blit(lpBack);
	DDDTA.Blit(lpBack);

	lpDDClipper->SetClipList(BattleFieldRegion,0);
	WhiteBoard.Blit(lpBack);
	TAHook.Blit(lpBack);

	lpDDClipper->SetClipList(ScreenRegion,0);
	//SharedRect.Blit(lpBack);
	Income.BlitIncome(lpBack);
	CommanderWarp.Blit(lpBack);
	SettingsDialog.BlitDialog(lpBack);
	//ChangeQueue.Blit(lpBack);

  }
  
  if(VerticalSync)
    {
    if(lpFront->Flip(NULL, DDFLIP_DONOTWAIT) != DD_OK)
      {
      if(lpFront->Flip(NULL, DDFLIP_WAIT) != DD_OK)
        if(lpFront->Flip(NULL, DDFLIP_WAIT | DDFLIP_NOVSYNC) != DD_OK);
      }
    }
  else
    {
    if(lpFront->Flip(NULL, DDFLIP_DONOTWAIT | DDFLIP_NOVSYNC) != DD_OK)
      {
      if(lpFront->Flip(NULL, DDFLIP_NOVSYNC) != DD_OK)
        lpFront->Flip(NULL, DDFLIP_WAIT);
      }
    }

  if(lpBack->Blt(NULL, lpFront, NULL, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpBack->Blt(NULL, lpFront, NULL, DDBLT_WAIT, NULL);
    //OutptTxt("lpFront to lpBack Blit failed");
    }
  //DataShare->UpdateLos = 0;
  return result;
}

HRESULT __stdcall IDDrawSurface::UpdateOverlay(LPRECT arg1, LPDIRECTDRAWSURFACE arg2, LPRECT arg3, DWORD arg4, LPDDOVERLAYFX arg5)
{
  OutptTxt("UpdateOverlay");
  return lpFront->UpdateOverlay(arg1, arg2, arg3, arg4, arg5);
}

HRESULT __stdcall IDDrawSurface::UpdateOverlayDisplay(DWORD arg1)
{
  OutptTxt("UpdateOverlayDisplay");
  return lpFront->UpdateOverlayDisplay(arg1);
}

HRESULT __stdcall IDDrawSurface::UpdateOverlayZOrder(DWORD arg1, LPDIRECTDRAWSURFACE arg2)
{
  OutptTxt("UpdateOverlayZOrder");
  return lpFront->UpdateOverlayZOrder(arg1, arg2);
}

void IDDrawSurface::OutptTxt(char *string)
{
  if(!Log)
    return;
  //AnsiString CPath = "c:\\taddrawlog.txt";

  FILE *file;
  file = fopen("C:\\temp\\taddrawlog.txt", "a");

  fwrite(string, strlen(string), 1, file);
  fwrite("\n", 1, 1, file);

  fclose(file);
}

void IDDrawSurface::ReplaceTAProc()
{
  HWND TopWindow;
  TopWindow = GetForegroundWindow();

  LocalShare->TAWndProc = (WNDPROC)SetWindowLong(TopWindow, GWL_WNDPROC, (long)WinProc);

}

void IDDrawSurface::Set(bool EnableVSync)
{
  VerticalSync = EnableVSync;
}

void IDDrawSurface::CreateClipplist()
{

  LPDIRECTDRAW TADD = (IDirectDraw*)LocalShare->TADirectDraw;
  TADD->CreateClipper(0,&lpDDClipper,NULL);

  ScreenRegion = (LPRGNDATA)new char[sizeof(RGNDATAHEADER)+sizeof(RECT)];
  BattleFieldRegion = (LPRGNDATA)new char[sizeof(RGNDATAHEADER)+sizeof(RECT)];

  RECT ScreenRect = {0,0,ScreenWidth,ScreenHeight};
  memcpy(ScreenRegion->Buffer, &ScreenRect,sizeof(RECT));
  ScreenRegion->rdh.dwSize = sizeof(RGNDATAHEADER);
  ScreenRegion->rdh.iType = RDH_RECTANGLES;
  ScreenRegion->rdh.nCount = 1;
  ScreenRegion->rdh.nRgnSize = sizeof(RECT);
  ScreenRegion->rdh.rcBound.left = 0;
  ScreenRegion->rdh.rcBound.top = 0;
  ScreenRegion->rdh.rcBound.right = ScreenWidth;
  ScreenRegion->rdh.rcBound.bottom = ScreenHeight;

  RECT BattleFieldRect = {128,32,ScreenWidth,ScreenHeight-32};
  memcpy(BattleFieldRegion->Buffer, &BattleFieldRect,sizeof(RECT));
  BattleFieldRegion->rdh.dwSize = sizeof(RGNDATAHEADER);
  BattleFieldRegion->rdh.iType = RDH_RECTANGLES;
  BattleFieldRegion->rdh.nCount = 1;
  BattleFieldRegion->rdh.nRgnSize = sizeof(RECT);
  BattleFieldRegion->rdh.rcBound.left = 128;
  BattleFieldRegion->rdh.rcBound.top = 32;
  BattleFieldRegion->rdh.rcBound.right = ScreenWidth;
  BattleFieldRegion->rdh.rcBound.bottom = ScreenHeight-32;

  lpBack->SetClipper(lpDDClipper);
}

void IDDrawSurface::ScreenShot()
{
  //create the creenshot
  char ScreenShotName[MAX_PATH*2];

  int i = 0;
  CreateFileName(ScreenShotName, i);

  WIN32_FIND_DATA fs;
  HANDLE File = FindFirstFile(ScreenShotName, &fs);
  while(File!=INVALID_HANDLE_VALUE)
    {
    i++;
    CreateFileName(ScreenShotName, i);
    FindClose(File);
    File = FindFirstFile(ScreenShotName, &fs);
    }
  //FindClose(Handle);

  OutptTxt(ScreenShotName);

  PCX PCXScreen;

  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  if(lpFront->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)==DD_OK)
    {
    char *Buff = new char[LocalShare->ScreenWidth*LocalShare->ScreenHeight];
    for(int j=0; j<LocalShare->ScreenHeight; j++)
      {
      memcpy(Buff+j*LocalShare->ScreenWidth, ((char*)ddsd.lpSurface)+j*ddsd.lPitch, LocalShare->ScreenWidth);
      }

    PCXScreen.NewBuffer(LocalShare->ScreenWidth, LocalShare->ScreenHeight);
    PCXScreen.SetBuffer((UCHAR*)Buff);

    if(PCXScreen.Save(ScreenShotName) == false)
      OutptTxt("error writing screenshot");

    delete Buff;
    }
  lpFront->Unlock(NULL);

}

void IDDrawSurface::CreateFileName(char *Buff, int Num)
{
  int *PTR = (int*)0x00511de8;
  char *RootDir = (char*)(*PTR + 0x38A53);
  strcpy(Buff, RootDir);
  strcat(Buff, "\\screenshots\\");
  CreateDir(Buff); //creates the dir so it exist
  char CNum[10];
  sprintf(CNum, "%.4i", Num);

  char Date[100];
  struct tm *time_now;
  time_t timer;
  timer = time(NULL);
  time_now = localtime(&timer);
  strftime(Date, 100,"%x - ", time_now);
  CorrectName(Date);
  strcat(Buff, Date);

  if(DataShare->TAProgress != TAInGame)
    {
    strcat(Buff, "SHOT");
    strcat(Buff, CNum);
    strcat(Buff, ".pcx");
    return;
    }

  char TempBuff[100];
  strcpy(TempBuff, DataShare->MapName);
  CorrectName(TempBuff);
  strcat(Buff, TempBuff);
  strcat(Buff, " - ");
  strcpy(TempBuff, DataShare->PlayerNames[0]);
  CorrectName(TempBuff);
  strcat(Buff, TempBuff);
  for(int i=1; i<10; i++)
    {
    if(strlen(DataShare->PlayerNames[i])>0)
      {
      strcat(Buff, ", ");
 	   strcpy(TempBuff, DataShare->PlayerNames[i]);
  		CorrectName(TempBuff);
      strcat(Buff, TempBuff);
      }
    }
  strcat(Buff, " ");
  strcat(Buff, CNum);
  strcat(Buff, ".pcx");
}

void IDDrawSurface::CreateDir(char *Dir)
{
  char *ptr;
  ptr = strstr(Dir, "\\");
  ptr++;
  /*ptr = strstr(Dir, "\\");
  if(ptr!=NULL)
    ptr++;*/
  while(ptr!=NULL)
    {
    char CDir[MAX_PATH];
    memcpy(CDir, Dir, ptr-Dir);
    CDir[(ptr-Dir)] = '\0';
    CreateDirectory(CDir, NULL);
    ptr = strstr(ptr, "\\");
    if(ptr!=NULL)
      ptr++;
    }
  CreateDirectory(Dir, NULL);
}

void IDDrawSurface::CorrectName(char *Name)
{
  for(int i=0; i<strlen(Name); i++)
    {
    char C = Name[i];
/*    if((C<'a' || C>'z') && (C<'A' || C>'Z') && (C<'1' || C>'0') && (C<'!' || C>'&') && C!='(' && C!=')'
       && C!='[' && C!=']' && C!='\0')
         Name[i] = '_';        */
    if( strchr( "\\/:*?\"<>|", C ) )
      Name[i] = '_';
    }
}

void IDDrawSurface::DeInterlace()
{
  int PaletteEntry = 0;

  Palette->GetEntries(NULL, 1, 1, (PALETTEENTRY*)&PaletteEntry);
  if(PaletteEntry != 83886208)
    {
    if(SurfaceMemory != NULL)
      {
      char *SurfMem = (char*)SurfaceMemory;
      for(int i=1; i<(LocalShare->ScreenHeight); i+=2)
        {
        memcpy(&SurfMem[i*lPitch], &SurfMem[(i+1)*lPitch], LocalShare->ScreenWidth);
        }
      }  
    }
  else
    PlayingMovie = false;
}

LRESULT CALLBACK WinProc(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if(Msg == WM_KEYDOWN && wParam == 120)
    if((GetAsyncKeyState(17)&0x8000)>0)
      {
      ((IDDrawSurface*)LocalShare->DDrawSurfClass)->ScreenShot();
      return 0;
      }

  if(DataShare->ehaOff == 1 && !DataShare->PlayingDemo)
    return LocalShare->TAWndProc(WinProcWnd, Msg, wParam, lParam);

  if(LocalShare->Dialog)
    {
    if(((Dialog*)LocalShare->Dialog)->Message(WinProcWnd, Msg, wParam, lParam))
      return 0;  //message handled by the dialog
    }

  if(((AlliesWhiteboard*)LocalShare->Whiteboard)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;

  if(((CIncome*)LocalShare->Income)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;  //message handled by the income class

  if(((CTAHook*)LocalShare->TAHook)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;  //message handled by tahook class

  if(((CWarp*)LocalShare->CommanderWarp)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;

  if(((CIdleUnits*)LocalShare->IdleUnits)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;

  if(((CChangeQueue*)LocalShare->ChangeQueue)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;

  if(((CDDDTA*)LocalShare->DDDTA)->Message(WinProcWnd, Msg, wParam, lParam))
    return 0;

  //if(((CUnitRotate*)LocalShare->UnitRotate)->Message(WinProcWnd, Msg, wParam, lParam))
  //  return 0;

  if(DataShare->F1Disable)
    if(Msg == WM_KEYDOWN && wParam == 112)
      return 0;

  return LocalShare->TAWndProc(WinProcWnd, Msg, wParam, lParam);
}

