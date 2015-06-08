#include "dialog.h"
#include "iddrawsurface.h"
#include "pcxread.h"
#include "font.h"
#include <stdio.h>

Dialog::Dialog()
{
  CursorPosX = -1;
  CursorPosY = -1;
  Move = false;
  StagedButton3State = 0;
  LocalShare->Dialog = this;

  LPDIRECTDRAW TADD = (IDirectDraw*)LocalShare->TADirectDraw;

  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
  ddsd.dwWidth = DialogWidth;
  ddsd.dwHeight = DialogHeight;
  TADD->CreateSurface(&ddsd, &lpDialogSurf, NULL);

  lpBackground = CreateSurfPCXResource(2, false);
  lpUCFont = CreateSurfPCXResource(3, true);
  lpCursor = CreateSurfPCXResource(4, true);
  lpOKButton = CreateSurfPCXResource(5, false);
  lpStagedButton3 = CreateSurfPCXResource(6, false);
  lpLCFont = CreateSurfPCXResource(7, true);
  lpSmallUCFont = CreateSurfPCXResource(8, true);
  lpSmallLCFont = CreateSurfPCXResource(9, true);
  lpCheckBox = CreateSurfPCXResource(10, false);
  lpStagedButton1 = CreateSurfPCXResource(11, false);
  lpStandardButton = CreateSurfPCXResource(12, false);

  posX = 0;
  posY = 0;
  ShareHeight = 0;
  First = true;
  DialogVisible = false;
  SetVisiblePushed = false;
  ReadPos();
  ReadSettings();
}

Dialog::~Dialog()
{
  if(lpDialogSurf)
    lpDialogSurf->Release();
  if(lpBackground)
    lpBackground->Release();
  if(lpUCFont)
    lpUCFont->Release();
  if(lpCursor)
    lpCursor->Release();
  if(lpOKButton)
    lpOKButton->Release();
  if(lpStagedButton3)
    lpStagedButton3->Release();
  if(lpLCFont)
    lpLCFont->Release();
  if(lpSmallUCFont)
    lpSmallUCFont->Release();
  if(lpSmallLCFont)
    lpSmallLCFont->Release();
  if(lpCheckBox)
    lpCheckBox->Release();
  if(lpStagedButton1)
    lpStagedButton1->Release();
  if(lpStandardButton)
    lpStandardButton->Release();

  WritePos();
  WriteSettings();
  LocalShare->Dialog = NULL;
}

void Dialog::ShowDialog()
{
  CorrectPos(); //make sure dialog is inside screen

  OKButtonPushed = false;
  StartedIn = None;
  
  RenderDialog();
  DialogVisible = true;
}

void Dialog::HideDialog()
{
  DialogVisible = false;
  CursorPosX = -1;
  CursorPosY = -1;
}

void Dialog::BlitDialog(LPDIRECTDRAWSURFACE DestSurf)
{
  if(First)
    {
    SetAll();
    First = false;
    }

  if(lpDialogSurf->IsLost() != DD_OK)
    {
    RestoreAll();
    }

  if(!DialogVisible)
    return;

  RECT Dest;
  Dest.left = posX;
  Dest.top = posY;
  Dest.right = posX + DialogWidth;
  Dest.bottom = posY + DialogHeight;

  if(DestSurf->Blt(&Dest, lpDialogSurf, NULL, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    DestSurf->Blt(&Dest, lpDialogSurf, NULL, DDBLT_WAIT, NULL);
    }

  if(CursorPosX!=-1 && CursorPosY!=-1)
    {
    BlitCursor(DestSurf, CursorPosX, CursorPosY);
    }
}

void Dialog::RestoreAll()
{
  lpDialogSurf->Restore();
  lpBackground->Restore();
  lpCursor->Restore();
  lpUCFont->Restore();
  lpLCFont->Restore();
  lpSmallUCFont->Restore();
  lpSmallLCFont->Restore();
  lpOKButton->Restore();
  lpStagedButton3->Restore();
  lpCheckBox->Restore();
  lpStagedButton1->Restore();
  lpStandardButton->Restore();

  RestoreFromPCX(2, lpBackground);
  RestoreFromPCX(3, lpUCFont);
  RestoreFromPCX(4, lpCursor);
  RestoreFromPCX(5, lpOKButton);
  RestoreFromPCX(6, lpStagedButton3);
  RestoreFromPCX(7, lpLCFont);
  RestoreFromPCX(8, lpSmallUCFont);
  RestoreFromPCX(9, lpSmallLCFont);
  RestoreFromPCX(10, lpCheckBox);
  RestoreFromPCX(11, lpStagedButton1);
  RestoreFromPCX(12, lpStandardButton);

  RenderDialog();
}

bool Dialog::Message(HWND WinProchWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if(!DialogVisible)
    {
    if(Msg == WM_KEYDOWN)
      if(wParam == 113)
        if((GetAsyncKeyState(17)&0x8000)>0/*ctrl*/)
          {
          ShowDialog();
          return true;
          }
    return false;
    }

  switch(Msg)
    {
    case WM_LBUTTONDBLCLK:
      if(LOWORD(lParam)>posX && LOWORD(lParam)<(posX+DialogWidth) && HIWORD(lParam)>posY && HIWORD(lParam)<(posY+DialogHeight))
        {
        return true;
        }
      break;

    case WM_LBUTTONDOWN:
      if(KeyCodeFocus)
        {
        KeyCodeFocus = false;
        RenderDialog();
        }
      if(WhiteboardKeyFocus)
        {
        WhiteboardKeyFocus = false;
        RenderDialog();
        }
      if(ShareBoxFocus)
        {
        ShareBoxFocus = false;
        RenderDialog();
        }
      if(AutoClickDelayFocus)
        {
        AutoClickDelayFocus = false;
        RenderDialog();
        }
      if(LOWORD(lParam)>posX && LOWORD(lParam)<(posX+DialogWidth) && HIWORD(lParam)>posY && HIWORD(lParam)<(posY+DialogHeight))
        {
        if(Inside(LOWORD(lParam), HIWORD(lParam), OKButton))
          {
          OKButtonPushed = true;
          StartedIn = OKButton;
          RenderDialog();
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), StagedButton3))
          {
          StagedButton3Pushed = true;
          StartedIn = StagedButton3;
          RenderDialog();
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), SetVisible))
          {
          SetVisiblePushed = true;
          StartedIn = SetVisible;
          RenderDialog();
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), VSync))
          {
          VSyncPushed = true;
          StartedIn = VSync;
          RenderDialog();
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), KeyCode))
          {
          KeyCodeFocus = true;
          RenderDialog();
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), WhiteboardKey))
          {
          WhiteboardKeyFocus = true;
          RenderDialog();
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), ShareBox))
          {
          ShareBoxFocus = true;
          RenderDialog();
          }
        /*else if(Inside(LOWORD(lParam), HIWORD(lParam), AutoClickDelay))
          {
          AutoClickDelayFocus = true;
          RenderDialog();
          }*/
        else if(Inside(LOWORD(lParam), HIWORD(lParam), OptimizeDT))
          {
          StartedIn = OptimizeDT;
          }
        else if(Inside(LOWORD(lParam), HIWORD(lParam), FullRings))
          {
          StartedIn = FullRings;
          }
        else  //only move if outside button
          {
          StartedIn = None;
          Move = true;
          }
        return true;
        }
      break;

    case WM_LBUTTONUP:
      if(Inside(LOWORD(lParam), HIWORD(lParam), OKButton) && StartedIn==OKButton)
        {
        SetAll();
        HideDialog();
        }
      else if(Inside(LOWORD(lParam), HIWORD(lParam), StagedButton3) && StartedIn==StagedButton3)
        {
        StagedButton3State += 1;
        StagedButton3State = StagedButton3State%3;
        StagedButton3Pushed = false;
        RenderDialog();
        }
      else if(Inside(LOWORD(lParam), HIWORD(lParam), SetVisible) && StartedIn==SetVisible)
        {
        SetVisibleList();
        SetVisiblePushed = false;
        RenderDialog();
        }
      else if(Inside(LOWORD(lParam), HIWORD(lParam), VSync) && StartedIn==VSync)
        {
        if(VSyncEnabled)
          VSyncEnabled = false;
        else
          VSyncEnabled = true;
        VSyncPushed = false;
        RenderDialog();
        }
      else if(Inside(LOWORD(lParam), HIWORD(lParam), OptimizeDT) && StartedIn==OptimizeDT)
        {
        if(OptimizeDTEnabled)
          OptimizeDTEnabled = false;
        else
          OptimizeDTEnabled = true;
        RenderDialog();
        }
      else if(Inside(LOWORD(lParam), HIWORD(lParam), FullRings) && StartedIn==FullRings)
        {
        if(FullRingsEnabled)
          FullRingsEnabled = false;
        else
          FullRingsEnabled = true;
        RenderDialog();
        }
      StartedIn = None;
      Move = false;
      break;

    case WM_MOUSEMOVE:
      if(LOWORD(lParam)>=(posX-10) && LOWORD(lParam)<(posX+DialogWidth) && HIWORD(lParam)>=(posY-20) && HIWORD(lParam)<(posY+DialogHeight))
        {
        CursorPosX = LOWORD(lParam);
        CursorPosY = HIWORD(lParam);
        }
      else
        {
        CursorPosX = -1;
        CursorPosY = -1;
        }
      if(Move)
        {
        posX += LOWORD(lParam)-X;
        posY += HIWORD(lParam)-Y;
        X = LOWORD(lParam);
        Y = HIWORD(lParam);
        return true;
        }
      X = LOWORD(lParam);
      Y = HIWORD(lParam);
      if(LOWORD(lParam)>=posX && LOWORD(lParam)<(posX+DialogWidth) && HIWORD(lParam)>=posY && HIWORD(lParam)<(posY+DialogHeight))
        {
        if(!Inside(LOWORD(lParam), HIWORD(lParam), OKButton))
          {
          if(OKButtonPushed == true)
            {
            OKButtonPushed = false;
            RenderDialog();
            }
          }
        else if(StartedIn == OKButton)
          {
          if(OKButtonPushed == false)
            {
            OKButtonPushed = true;
            RenderDialog();
            }
          }
        if(!Inside(LOWORD(lParam), HIWORD(lParam), StagedButton3))
          {
          if(StagedButton3Pushed == true)
            {
            StagedButton3Pushed = false;
            RenderDialog();
            }
          }
        else if(StartedIn == StagedButton3)
          {
          if(StagedButton3Pushed == false)
            {
            StagedButton3Pushed = true;
            RenderDialog();
            }
          }
        if(!Inside(LOWORD(lParam), HIWORD(lParam), SetVisible))
          {
          if(SetVisiblePushed == true)
            {
            SetVisiblePushed = false;
            RenderDialog();
            }
          }
        else if(StartedIn == SetVisible)
          {
          if(SetVisiblePushed == false)
            {
            SetVisiblePushed = true;
            RenderDialog();
            }
          }
        if(!Inside(LOWORD(lParam), HIWORD(lParam), VSync))
          {
          if(VSyncPushed == true)
            {
            VSyncPushed = false;
            RenderDialog();
            }
          }
        else if(StartedIn == VSync)
          {
          if(VSyncPushed == false)
            {
            VSyncPushed = true;
            RenderDialog();
            }
          }
        return true;
        }
      break;
    case WM_KEYDOWN:
      if(KeyCodeFocus)
        {
        VirtualKeyCode = (int)wParam;
        RenderDialog();
        return true;
        }
      if(WhiteboardKeyFocus)
        {
        VirtualWhiteboardKey = (int)wParam;
        RenderDialog();
        return true;
        }
      break;
    case WM_CHAR:
      if(KeyCodeFocus)
        {
        return true;
        }
      if(WhiteboardKeyFocus)
        {
        return true;
        }
      if(ShareBoxFocus)
        {
        if(wParam == 8) //backspace
          {
          if(strlen(ShareText)>0)
            ShareText[strlen(ShareText)-1] = '\0';
          }
        else
          {
          if(Lines!=MaxLines)
            {
            char App[2];
            App[0] = (TCHAR)wParam;
            App[1] = '\0';
            strcat(ShareText, App);
            }
          }
        RenderDialog();
        return true;
        }
      if(AutoClickDelayFocus)
        {
        if(wParam == 8) //backspace
          {
          if(strlen(cAutoClickDelay)>0)
            cAutoClickDelay[strlen(cAutoClickDelay)-1] = '\0';
          }
        else
          {
          if(strlen(cAutoClickDelay)!=3)
            {
            if(wParam>='0' && wParam<='9')
              {
              char App[2];
              App[0] = (TCHAR)wParam;
              App[1] = '\0';
              strcat(cAutoClickDelay, App);
              }
            }
          }
        RenderDialog();
        return true;
        }
      break;
    }

  return false;//mesage not handled by this dialog
}

void Dialog::RenderDialog()
{
  RECT Dest;
  RECT Source;

  if(lpDialogSurf->Blt(NULL, lpBackground, NULL, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(NULL, lpBackground, NULL, DDBLT_WAIT , NULL);
    }

  Dest.left = OKButtonPosX;
  Dest.top = OKButtonPosY;
  Dest.right = OKButtonPosX + OKButtonWidth;
  Dest.bottom = OKButtonPosY + OKButtonHeight;
  Source.left = OKButtonPushed*OKButtonWidth;
  Source.top = 0;
  Source.right = OKButtonWidth + OKButtonPushed*OKButtonWidth;
  Source.bottom = OKButtonHeight;
  if(lpDialogSurf->Blt(&Dest, lpOKButton, &Source, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(&Dest, lpOKButton, &Source, DDBLT_WAIT , NULL);
    }

  DrawBackgroundButton();
  DrawKeyCode();
  DrawShareBox();
  DrawOptimizeDT();
  DrawVSync();
  DrawFullRings();
  //DrawDelay();
  DrawWhiteboardKey();
  //DrawVisibleButton();

}

bool Dialog::Inside(int x, int y, int Control)
{
  x = x-posX;
  y = y-posY;

  switch(Control)
    {
    case OKButton:
      if(x>=OKButtonPosX && x<OKButtonPosX+OKButtonWidth && y>=OKButtonPosY && y<OKButtonPosY+OKButtonHeight)
        return true;
      else
        return false;
    case StagedButton3:
      if(x>=StagedButton3PosX && x<StagedButton3PosX+StagedButton3Width && y>=StagedButton3PosY && y<StagedButton3PosY+StagedButton3Height)
        return true;
      else
        return false;
    case KeyCode:
      if(x>=KeyCodePosX && x<KeyCodePosX+KeyCodeWidth && y>=KeyCodePosY && y<KeyCodePosY+KeyCodeHeight)
        return true;
      else
        return false;
    case ShareBox:
      if(x>=ShareBoxPosX && x<ShareBoxPosX+ShareBoxWidth && y>=ShareBoxPosY && y<ShareBoxPosY+ShareBoxHeight)
        return true;
      else
        return false;
    case OptimizeDT:
      if(x>=OptimizeDTPosX && x<OptimizeDTPosX+ShareBoxWidth && y>=OptimizeDTPosY && y<OptimizeDTPosY+OptimizeDTHeight)
        return true;
      else
        return false;
    case VSync:
      if(x>=VSyncPosX && x<VSyncPosX+VSyncWidth && y>=VSyncPosY && y<VSyncPosY+VSyncHeight)
        return true;
      else
        return false;
    case FullRings:
      if(x>=FullRingsPosX && x<FullRingsPosX+FullRingsWidth && y>=FullRingsPosY && y<FullRingsPosY+FullRingsHeight)
        return true;
      else
        return false;
    case AutoClickDelay:
      if(x>=AutoClickDelayPosX && x<AutoClickDelayPosX+AutoClickDelayWidth && y>=AutoClickDelayPosY && y<AutoClickDelayPosY+AutoClickDelayHeight)
        return true;
      else
        return false;
    case WhiteboardKey:
      if(x>=WhiteboardKeyPosX && x<WhiteboardKeyPosX+WhiteboardKeyWidth && y>=WhiteboardKeyPosY && y<WhiteboardKeyPosY+WhiteboardKeyHeight)
        return true;
      else
        return false;
    case SetVisible:
      if(x>=SetVisiblePosX && x<SetVisiblePosX+SetVisibleWidth && y>=SetVisiblePosY && y<SetVisiblePosY+SetVisibleHeight)
        return true;
      else
        return false;
    }

  return false;
}

void Dialog::DrawText(LPDIRECTDRAWSURFACE DestSurf, int x, int y, char *Text)
{
  RECT Dest;
  Dest.left = x;
  Dest.top = y;
  Dest.bottom = Dest.top + 14;
  RECT Source;
  Source.left = 0;
  Source.top = 0;
  Source.bottom = 14;
  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;

  for(int i=0; i<strlen(Text); i++)
    {
    if(Text[i]<91 && Text[i]>=33) //upper case or special character
      {
      Dest.right = Dest.left + FontOffsetUC[Text[i]-33][0];
      Source.left = FontOffsetUC[Text[i]-33][1];
      Source.right = Source.left + FontOffsetUC[Text[i]-33][0];
      if(DestSurf->Blt(&Dest, lpUCFont, &Source, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
        {
        DestSurf->Blt(&Dest, lpUCFont, &Source, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
        }
      Dest.left += FontOffsetUC[Text[i]-33][0];
      }
    else if(Text[i]<123 && Text[i]>=97)
      {
      Dest.right = Dest.left + FontOffsetLC[Text[i]-97][0];
      Source.left = FontOffsetLC[Text[i]-97][1];
      Source.right = Source.left + FontOffsetLC[Text[i]-97][0];
      if(DestSurf->Blt(&Dest, lpLCFont, &Source, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
        {
        DestSurf->Blt(&Dest, lpLCFont, &Source, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
        }
      Dest.left += FontOffsetLC[Text[i]-97][0];
      }
    if(Text[i] == ' ')
      Dest.left += 7;
    }
}

void Dialog::DrawSmallText(LPDIRECTDRAWSURFACE DestSurf, int x, int y, char *Text)
{
  RECT Dest;
  Dest.left = x;
  Dest.top = y;
  Dest.bottom = Dest.top + 12;
  RECT Source;
  Source.left = 0;
  Source.top = 0;
  Source.bottom = 12;
  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;

  for(int i=0; i<strlen(Text); i++)
    {
    if(Text[i]<91 && Text[i]>=33) //upper case
      {
      Dest.right = Dest.left + SmallFontOffsetUC[Text[i]-33][0];
      Source.left = SmallFontOffsetUC[Text[i]-33][1];
      Source.right = Source.left + SmallFontOffsetUC[Text[i]-33][0];
      if(DestSurf->Blt(&Dest, lpSmallUCFont, &Source, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
        {
        DestSurf->Blt(&Dest, lpSmallUCFont, &Source, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
        }
      Dest.left += SmallFontOffsetUC[Text[i]-33][0];
      }
    else if(Text[i]<123 && Text[i]>=97)
      {
      Dest.right = Dest.left + SmallFontOffsetLC[Text[i]-97][0];
      Source.left = SmallFontOffsetLC[Text[i]-97][1];
      Source.right = Source.left + SmallFontOffsetLC[Text[i]-97][0];
      if(DestSurf->Blt(&Dest, lpSmallLCFont, &Source, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
        {
        DestSurf->Blt(&Dest, lpSmallLCFont, &Source, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
        }
      Dest.left += SmallFontOffsetLC[Text[i]-97][0];
      }
    if(Text[i] == ' ')
      Dest.left += 6;
    }
}

void Dialog::SetAll()
{
  CIncome *Income = (CIncome*)LocalShare->Income;
  Income->Set(StagedButton3State);

  IDDrawSurface *SurfClass = (IDDrawSurface*)LocalShare->DDrawSurfClass;
  SurfClass->Set(VSync);

  CTAHook *TAHook = (CTAHook*)LocalShare->TAHook;
  int Delay = atoi(cAutoClickDelay);
  if(Delay<1)
    Delay = 1;
  TAHook->Set(VirtualKeyCode, ShareText, OptimizeDTEnabled, FullRingsEnabled, Delay);

  AlliesWhiteboard *WB = (AlliesWhiteboard*)LocalShare->Whiteboard;
  WB->Set(VirtualWhiteboardKey);
}

//reads dialog position from registry
void Dialog::ReadPos()
{
  HKEY hKey;
  DWORD dwDisposition;
  DWORD Size = sizeof(int);

  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

  RegQueryValueEx(hKey, "DialogPosX", NULL, NULL, (unsigned char*)&posX, &Size);
  RegQueryValueEx(hKey, "DialogPosY", NULL, NULL, (unsigned char*)&posY, &Size);

  RegCloseKey(hKey);
}

void Dialog::WriteSettings()
{
  HKEY hKey;
  DWORD dwDisposition;

  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

  RegSetValueEx(hKey, "BackGround", NULL, REG_DWORD, (unsigned char*)&StagedButton3State, sizeof(int));
  RegSetValueEx(hKey, "VSync", NULL, REG_BINARY, (unsigned char*)&VSyncEnabled, sizeof(bool));
  RegSetValueEx(hKey, "KeyCode", NULL, REG_DWORD, (unsigned char*)&VirtualKeyCode, sizeof(int));
  RegSetValueEx(hKey, "OptimizeDT", NULL, REG_BINARY, (unsigned char*)&OptimizeDTEnabled, sizeof(bool));
  RegSetValueEx(hKey, "FullRings", NULL, REG_BINARY, (unsigned char*)&FullRingsEnabled, sizeof(bool));
  RegSetValueEx(hKey, "ShareText", NULL, REG_SZ, (unsigned char*)ShareText, strlen(ShareText));
  RegSetValueEx(hKey, "Delay", NULL, REG_SZ, (unsigned char*)cAutoClickDelay, strlen(cAutoClickDelay));
  RegSetValueEx(hKey, "WhiteboardKey", NULL, REG_DWORD, (unsigned char*)&VirtualWhiteboardKey, sizeof(int));
  RegCloseKey(hKey);
}

void Dialog::ReadSettings()
{
  HKEY hKey;
  DWORD dwDisposition;
  DWORD Size;

  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

  Size = sizeof(int);
  if(RegQueryValueEx(hKey, "BackGround", NULL, NULL, (unsigned char*)&StagedButton3State, &Size) != ERROR_SUCCESS)
    {
    StagedButton3State = 0;
    }
  Size = sizeof(bool);
  if(RegQueryValueEx(hKey, "VSync", NULL, NULL, (unsigned char*)&VSyncEnabled, &Size) != ERROR_SUCCESS)
    {
    VSyncEnabled = true;
    }
  Size = sizeof(int);
  if(RegQueryValueEx(hKey, "KeyCode", NULL, NULL, (unsigned char*)&VirtualKeyCode, &Size) != ERROR_SUCCESS)
    {
    VirtualKeyCode = 88;
    }
  Size = sizeof(bool);
  if(RegQueryValueEx(hKey, "OptimizeDT", NULL, NULL, (unsigned char*)&OptimizeDTEnabled, &Size) != ERROR_SUCCESS)
    {
    OptimizeDTEnabled = true;
    }
  Size = sizeof(bool);
  if(RegQueryValueEx(hKey, "FullRings", NULL, NULL, (unsigned char*)&FullRingsEnabled, &Size) != ERROR_SUCCESS)
    {
    FullRingsEnabled = true;
    }
  Size = 1000;
  if(RegQueryValueEx(hKey, "ShareText", NULL, NULL, (unsigned char*)ShareText, &Size) != ERROR_SUCCESS)
    {
    strcpy(ShareText, "+setshareenergy 1000\255+setsharemetal 1000\255+shareall\255+shootall");
    ShareText[20] = 13;
    ShareText[40] = 13;
    ShareText[50] = 13;
    }
  Size = 10;
  if(RegQueryValueEx(hKey, "Delay", NULL, NULL, (unsigned char*)cAutoClickDelay, &Size) != ERROR_SUCCESS)
    {
    strcpy(cAutoClickDelay, "10");
    }
  Size = sizeof(int);
  if(RegQueryValueEx(hKey, "WhiteboardKey", NULL, NULL, (unsigned char*)&VirtualWhiteboardKey, &Size) != ERROR_SUCCESS)
    {
    VirtualWhiteboardKey = 220;
    }
  RegCloseKey(hKey);
}

void Dialog::WritePos()
{
  HKEY hKey;
  DWORD dwDisposition;

  RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Yankspankers\\Eye", NULL, "Moo", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);

  RegSetValueEx(hKey, "DialogPosX", NULL, REG_DWORD, (unsigned char*)&posX, sizeof(int));
  RegSetValueEx(hKey, "DialogPosY", NULL, REG_DWORD, (unsigned char*)&posY, sizeof(int));

  RegCloseKey(hKey);
}

void Dialog::CorrectPos()
{
  if(posX<0)
    posX = 0;
  if(posX>(LocalShare->ScreenWidth-DialogWidth))
    posX = LocalShare->ScreenWidth-DialogWidth;

  if(posY<0)
    posY = 0;
  if(posY>(LocalShare->ScreenHeight-DialogHeight))
    posY = LocalShare->ScreenHeight-DialogHeight;
}

void Dialog::DrawTinyText(char *String, int posx, int posy, char Color)
{
  if(SurfaceMemory == NULL)
    return;

  char *SurfMem = (char*)SurfaceMemory;

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

void Dialog::FillRect(int x, int y, int x2, int y2, char Color)
{
  if(SurfaceMemory == NULL)
    return;

  char *SurfMem = (char*)SurfaceMemory;

  for(int i=y; i<y2; i++)
    {
    memset(&SurfMem[x+i*lPitch], Color, x2-x);
    }
}

void Dialog::DrawBackgroundButton()
{
  RECT Dest;
  RECT Source;

  DrawSmallText(lpDialogSurf, StagedButton3PosX, StagedButton3PosY-12, "Background");
  Dest.left = StagedButton3PosX;
  Dest.top = StagedButton3PosY;
  Dest.right = StagedButton3PosX + StagedButton3Width;
  Dest.bottom = StagedButton3PosY + StagedButton3Height;
  if(StagedButton3Pushed)
    Source.left = StagedButton3Width*3;
  else
    Source.left = StagedButton3State*StagedButton3Width;
  Source.top = 0;
  Source.right = Source.left + StagedButton3Width;
  Source.bottom = StagedButton3Height;
  if(lpDialogSurf->Blt(&Dest, lpStagedButton3, &Source, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(&Dest, lpStagedButton3, &Source, DDBLT_WAIT , NULL);
    }
  int y;
  if(StagedButton3Pushed)
    y = StagedButton3PosY+4;
  else
    y = StagedButton3PosY+3;
  int x;
  if(StagedButton3Pushed)
    x = StagedButton3PosX+5;
  else
    x = StagedButton3PosX+4;
  switch(StagedButton3State)
    {
    case 0:
      DrawText(lpDialogSurf, x, y, "None");
      break;
    case 1:
      DrawText(lpDialogSurf, x, y, "Text");
      break;
    case 2:
      DrawText(lpDialogSurf, x, y, "Solid");
      break;
    }
}

void Dialog::DrawKeyCode()
{
  DrawSmallText(lpDialogSurf, KeyCodePosX, KeyCodePosY-13, "Autoclick Key");
  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  if(lpDialogSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL)==DD_OK)
    {
    SurfaceMemory = ddsd.lpSurface;
    lPitch = ddsd.lPitch;

    FillRect(KeyCodePosX, KeyCodePosY, KeyCodePosX+KeyCodeWidth, KeyCodePosY+KeyCodeHeight, 0);

    char String[20];
    sprintf(String, "%i", VirtualKeyCode);
    if(KeyCodeFocus)
      DrawTinyText(String, KeyCodePosX + 2, KeyCodePosY + 3, 255);
    else
      DrawTinyText(String, KeyCodePosX + 2, KeyCodePosY + 3, 208);

    lpDialogSurf->Unlock(NULL);
    }
}

void Dialog::DrawShareBox()
{
  DrawSmallText(lpDialogSurf, ShareBoxPosX, ShareBoxPosY-13, "Chat Macro (F11)");
  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  if(lpDialogSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL)==DD_OK)
    {
    SurfaceMemory = ddsd.lpSurface;
    lPitch = ddsd.lPitch;

    FillRect(ShareBoxPosX, ShareBoxPosY, ShareBoxPosX+ShareBoxWidth, ShareBoxPosY+ShareBoxHeight, 0);

    //DrawTinyText(ShareText, ShareBoxPosX + 2, ShareBoxPosY + 3, 208);
    int CharsPerLine = (ShareBoxWidth-4)/8;
    char Line[100];
    Line[0] = '\0';
    char LineNum = 0;
    char LinePos = 0;
    int i;
    bool WasLineBreak = false;
    for(i=0; i<strlen(ShareText); i++)
      {
      if(LinePos > CharsPerLine)
        {
        if(ShareBoxFocus)
          DrawTinyText(Line, ShareBoxPosX + 2, ShareBoxPosY + 3 + LineNum*9, 255);
        else
          DrawTinyText(Line, ShareBoxPosX + 2, ShareBoxPosY + 3 + LineNum*9, 208);
        LineNum++;
        LinePos = 0;
        Line[0] = ' ';
        Line[1] = '\0';
        LinePos++;
        i--;
        WasLineBreak = true;
        }
      else
        {
        if(ShareText[i] != 13/*enter*/)
          {
          Line[LinePos] = ShareText[i];
          Line[LinePos+1] = '\0';
          LinePos++;
          WasLineBreak = false;
          }
        else
          {
          if(ShareBoxFocus)
            DrawTinyText(Line, ShareBoxPosX + 2, ShareBoxPosY + 3 + LineNum*9, 255);
          else
            DrawTinyText(Line, ShareBoxPosX + 2, ShareBoxPosY + 3 + LineNum*9, 208);
          if(!WasLineBreak)
            LineNum++;
          LinePos = 0;
          Line[0] = '\0';
          WasLineBreak = false;
          }
        }
      }
    if(ShareBoxFocus)
      DrawTinyText(Line, ShareBoxPosX + 2, ShareBoxPosY + 3 + LineNum*9, 255);
    else
      DrawTinyText(Line, ShareBoxPosX + 2, ShareBoxPosY + 3 + LineNum*9, 208);
    Lines = LineNum+1;
    lpDialogSurf->Unlock(NULL);
    }
  MaxLines = (ShareBoxHeight-4)/8;
}

void Dialog::DrawOptimizeDT()
{
  RECT Dest;
  RECT Source;

  DrawSmallText(lpDialogSurf, OptimizeDTPosX+20, OptimizeDTPosY+3, "Optmize DT rows");
  Dest.left = OptimizeDTPosX;
  Dest.top = OptimizeDTPosY;
  Dest.right = OptimizeDTPosX + OptimizeDTWidth;
  Dest.bottom = OptimizeDTPosY + OptimizeDTHeight;
  Source.left = OptimizeDTEnabled*16;
  Source.top = 0;
  Source.right = Source.left + OptimizeDTWidth;
  Source.bottom = OptimizeDTHeight;
  if(lpDialogSurf->Blt(&Dest, lpCheckBox, &Source, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(&Dest, lpCheckBox, &Source, DDBLT_WAIT , NULL);
    }
}

void Dialog::DrawVSync()
{
  RECT Dest;
  RECT Source;

  DrawSmallText(lpDialogSurf, VSyncPosX, VSyncPosY-12, "VSync");
  Dest.left = VSyncPosX;
  Dest.top = VSyncPosY;
  Dest.right = VSyncPosX + VSyncWidth;
  Dest.bottom = VSyncPosY + VSyncHeight;
  if(VSyncPushed)
    Source.left = VSyncWidth*2;
  else
    Source.left = VSyncEnabled*VSyncWidth;
  Source.top = 0;
  Source.right = Source.left + VSyncWidth;
  Source.bottom = VSyncHeight;
  if(lpDialogSurf->Blt(&Dest, lpStagedButton1, &Source, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(&Dest, lpStagedButton1, &Source, DDBLT_WAIT , NULL);
    }
  int y;
  if(VSyncPushed)
    y = VSyncPosY+4;
  else
    y = VSyncPosY+3;
  int x;
  if(VSyncPushed)
    x = VSyncPosX+5;
  else
    x = VSyncPosX+4;
  switch(VSyncEnabled)
    {
    case true:
      DrawText(lpDialogSurf, x, y, "Enabled");
      break;
    case false:
      DrawText(lpDialogSurf, x, y, "Disabled");
      break;
    }
}

void Dialog::DrawFullRings()
{
  RECT Dest;
  RECT Source;

  DrawSmallText(lpDialogSurf, FullRingsPosX+20, FullRingsPosY+3, "Enable FullRings");
  Dest.left = FullRingsPosX;
  Dest.top = FullRingsPosY;
  Dest.right = FullRingsPosX + FullRingsWidth;
  Dest.bottom = FullRingsPosY + FullRingsHeight;
  Source.left = FullRingsEnabled*16;
  Source.top = 0;
  Source.right = Source.left + FullRingsWidth;
  Source.bottom = FullRingsHeight;
  if(lpDialogSurf->Blt(&Dest, lpCheckBox, &Source, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(&Dest, lpCheckBox, &Source, DDBLT_WAIT , NULL);
    }
}

void Dialog::DrawDelay()
{
  DrawSmallText(lpDialogSurf, AutoClickDelayPosX, AutoClickDelayPosY-13, "Autoclick delay");
  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  if(lpDialogSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL)==DD_OK)
    {
    SurfaceMemory = ddsd.lpSurface;
    lPitch = ddsd.lPitch;

    FillRect(AutoClickDelayPosX, AutoClickDelayPosY, (AutoClickDelayPosX)+AutoClickDelayWidth, AutoClickDelayPosY+AutoClickDelayHeight, 0);

    if(AutoClickDelayFocus)
      DrawTinyText(cAutoClickDelay, AutoClickDelayPosX+2, AutoClickDelayPosY+3, 255);
    else
      DrawTinyText(cAutoClickDelay, AutoClickDelayPosX+2, AutoClickDelayPosY+3, 208);

    lpDialogSurf->Unlock(NULL);
    }
}

void Dialog::DrawWhiteboardKey()
{
  DrawSmallText(lpDialogSurf, WhiteboardKeyPosX, WhiteboardKeyPosY-13, "Whiteboard Key");
  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);
  if(lpDialogSurf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL)==DD_OK)
    {
    SurfaceMemory = ddsd.lpSurface;
    lPitch = ddsd.lPitch;

    FillRect(WhiteboardKeyPosX, WhiteboardKeyPosY, WhiteboardKeyPosX+WhiteboardKeyWidth, WhiteboardKeyPosY+WhiteboardKeyHeight, 0);

    char String[20];
    sprintf(String, "%i", VirtualWhiteboardKey);
    if(WhiteboardKeyFocus)
      DrawTinyText(String, WhiteboardKeyPosX + 2, WhiteboardKeyPosY + 3, 255);
    else
      DrawTinyText(String, WhiteboardKeyPosX + 2, WhiteboardKeyPosY + 3, 208);

    lpDialogSurf->Unlock(NULL);
    }
}

void Dialog::DrawVisibleButton()
{
  DrawSmallText(lpDialogSurf, SetVisiblePosX, SetVisiblePosY-13, "Set Visible list");

  RECT Dest;
  RECT Source;
  Dest.left = SetVisiblePosX;
  Dest.top = SetVisiblePosY;
  Dest.right = SetVisiblePosX + SetVisibleWidth;
  Dest.bottom = SetVisiblePosY + SetVisibleHeight;
  Source.left = SetVisiblePushed*SetVisibleWidth;
  Source.top = 0;
  Source.right = SetVisibleWidth + SetVisiblePushed*SetVisibleWidth;
  Source.bottom = SetVisibleHeight;
  if(lpDialogSurf->Blt(&Dest, lpStandardButton, &Source, DDBLT_ASYNC, NULL)!=DD_OK)
    {
    lpDialogSurf->Blt(&Dest, lpStandardButton, &Source, DDBLT_WAIT , NULL);
    }

  DrawText(lpDialogSurf, SetVisiblePosX+10+SetVisiblePushed, SetVisiblePosY+4+SetVisiblePushed, "Set Visible");
}

void Dialog::BlitCursor(LPDIRECTDRAWSURFACE DestSurf, int x, int y)
{
  //blit cursor
  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 91;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 91;

  RECT Dest;
  Dest.left = x;
  Dest.top = y;
  Dest.right = x + 10;
  Dest.bottom = y + 20;
  if(DestSurf->Blt(&Dest, lpCursor, NULL, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
    {
    DestSurf->Blt(&Dest, lpCursor, NULL, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
    }
}

void Dialog::SetVisibleList()
{

}
