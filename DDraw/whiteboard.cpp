#include "whiteboard.h"
#include "iddrawsurface.h"
#include "pcxread.h"
#include <stdio.h>
#include <math.h>

const double PI = 3.1415926535;

AlliesWhiteboard::AlliesWhiteboard()
{
  lpInputBox = NULL;
  lpSmallCircle = NULL;

  if(DataShare->ehaOff == 1)
    return;

  LocalShare->Whiteboard = this;
  Disabled = false;
  VirtualKeyCode = 220;

  int *PTR = (int*)0x00511de8;

  MapX = (int*)(*PTR + 0x1431f);
  MapY = (int*)(*PTR + 0x14323);
  
  WBKeyDown = false;
  InputShown = false;
  Text[0] = '\0';

  lpInputBox = CreateSurfPCXResource(13, false);
  lpSmallCircle = CreateSurfPCXResource(14, true);

  int *cPTR = (int*)(*PTR+0x1b8a);

  PlayerColor = (char*)((*cPTR) + 0x96);

  SizeX = LocalShare->ScreenWidth;
  SizeY = LocalShare->ScreenHeight-32;
  MidX = SizeX/2;
  MidY = SizeY/2;

  max_clip_x = SizeX-1;
  max_clip_y = SizeY-1;

  GenerateLookupTables();
}


AlliesWhiteboard::~AlliesWhiteboard()
{
  if(lpInputBox)
    lpInputBox->Release();
  if(lpSmallCircle)
    lpSmallCircle->Release();

}

void AlliesWhiteboard::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
  if(lpInputBox->IsLost() != DD_OK)
    {
    RestoreAll();
    }

  ReceiveMarkers();
  SendMarkers();

  DrawMarkers(DestSurf);

  if(InputShown)
    DrawTextInput(DestSurf);
}

void AlliesWhiteboard::LockBlit(char *VidBuf, int Pitch)
{
  min_clip_y = 32;
  min_clip_x = 128;
  max_clip_x = SizeX-1;
  max_clip_y = SizeY-1;

  std::vector<GraphicElement*> ElementPTR;
  int x1 = *MapX-75;
  if(x1<0)
    x1=0;
  int y1 = *MapY-75;
  if(y1<0)
    y1=0;
  ElementPTR = ElementHandler.GetArea(x1, y1, *MapX+SizeX+75, *MapY+SizeY+75);

  for(int i=0; i<ElementPTR.size(); i++)
    {
    GraphicLine *MarkerPTR = (GraphicLine*)ElementPTR[i];
    if(MarkerPTR->Type == ClassGraphicLine)
      {
      DrawLine(MarkerPTR->x1 + 128, MarkerPTR->y1 + 32, MarkerPTR->x2 + 128, MarkerPTR->y2 + 32, MarkerPTR->Color, VidBuf, Pitch);
      }
    }

  if(!MinimapMarkerHandler.empty())
    {
    min_clip_y = 0;
    min_clip_x = 0;
    if(VidBuf!=NULL)
      DrawMinimapMarkers(VidBuf, Pitch, false);
    }
}

bool AlliesWhiteboard::Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  static bool Move = false;
  static bool Paint = false;

  switch(Msg)
    {
    case WM_KEYDOWN:
      if(InputShown)
        {
        TextInputKeyDown(wParam);
        return true;
        }
      if(wParam == VirtualKeyCode)
        {
  		  int *PTR1 = (int*)0x511de8;
        short NumUnits = *((int*)(*PTR1+0x1ca7));
        if(NumUnits == 0)
          return true;

        WBKeyDown = true;
        if((GetAsyncKeyState(17)&0x8000)>0) //ctrl
          ScrollToCenter(LastMarkerX, LastMarkerY);
        return true;
        }
      break;

    case WM_KEYUP:
      if(wParam == VirtualKeyCode)
        {
        WBKeyDown = false;
        return true;
        }
      if(InputShown)
        {
        if(wParam == 13 || wParam == 27)
          InputShown = false;
        return true;
        }
      break;

    case WM_LBUTTONDOWN:
      {
      if(WBKeyDown)
        {
        CurrentElement = GetTextElementAt(*MapX+LOWORD(lParam)-128, *MapY+HIWORD(lParam)-32, 5);
        if(Move)
          {
          return true;
          }
        else if(CurrentElement)
          {
          Move = true;
          return true;
          }
        Paint = true;
        LastMouseX = LOWORD(lParam);
        LastMouseY = HIWORD(lParam);
        MarkerX = *MapX;
        MarkerY = *MapY;
        return true;
        }
      }

    case WM_LBUTTONUP:
      Move = false;
      Paint = false;
      break;

    case WM_LBUTTONDBLCLK:
      if(WBKeyDown)
        {
        CurrentElement = GetTextElementAt(*MapX+LOWORD(lParam)-128, *MapY+HIWORD(lParam)-32, 5);
        if(CurrentElement)
          strcpy(Text, ((GraphicText*)CurrentElement)->text);

        InputShown = true;
        MarkerX = *MapX;
        MarkerY = *MapY;
        LastMouseX = LOWORD(lParam);
        LastMouseY = HIWORD(lParam);
        return true;
        }
      break;

    case WM_RBUTTONDBLCLK:
      if(WBKeyDown)
        {
        DeleteMarker(*MapX+LOWORD(lParam)-128, *MapY+HIWORD(lParam)-32);
        return true;
        }
      break;

    case WM_MBUTTONDOWN:
      if(WBKeyDown)
        {
        if(GetTextElementAt(*MapX+LOWORD(lParam)-128, *MapY+HIWORD(lParam)-32, 5)==0)
          {
          ElementHandler.AddElement(new GraphicText(*MapX + (LOWORD(lParam)-128), *MapY + (HIWORD(lParam)-32), "", *PlayerColor));
          PacketHandler.push_back(new GraphicText(*MapX + (LOWORD(lParam)-128), *MapY + (HIWORD(lParam)-32), "", *PlayerColor));
          }
        }
      break;

    case WM_CHAR:
      if(InputShown)
        {
        TextInputChar((TCHAR)wParam);
        return true;
        }
      break;

    case WM_RBUTTONDOWN:
      if(WBKeyDown)
        {
        return true;
        }
      break;

    case WM_RBUTTONUP:
      if(WBKeyDown)
        {
        return true;
        }
      break;

    case WM_MOUSEMOVE:
      if(WBKeyDown)
        {
        if(Paint)
          {
          MouseMove(LastMouseX-128, LOWORD(lParam)-128, LastMouseY-32, HIWORD(lParam)-32);
          LastMouseX = LOWORD(lParam);
          LastMouseY = HIWORD(lParam);
          MarkerX = *MapX;
          MarkerY = *MapY;
          }
        else if((wParam&MK_RBUTTON)>0)
          {
          EreaseArea(LOWORD(lParam)-128, HIWORD(lParam)-32);
          }
        if(Move)
          {
			if(CurrentElement)
			{
				PacketHandler.push_back(new GraphicMoveText(CurrentElement->x1, CurrentElement->y1, *MapX+LOWORD(lParam)-128, *MapY+HIWORD(lParam)-32, *PlayerColor));
				CurrentElement = ElementHandler.MoveTextElement(CurrentElement, *MapX+LOWORD(lParam)-128, *MapY+HIWORD(lParam)-32);
			}
          }
        }
      break;
    }

  /*if(WBKeyDown)
    {
    if(Msg != WM_MOUSEMOVE)
      return true;
    }      */

  return false;
}

void AlliesWhiteboard::Set(int iKeyCode)
{
  VirtualKeyCode = iKeyCode;
}

int AlliesWhiteboard::GetMapX()
{
  return *MapX;
}

int AlliesWhiteboard::GetMapY()
{
  return *MapY;
}

void AlliesWhiteboard::InstantScrollTo(int x, int y)
{
  *MapX = x;
  *(MapX + 2) = x;
  *MapY = y;
  *(MapY + 2)= y;
}

void AlliesWhiteboard::DrawTextInput(LPDIRECTDRAWSURFACE DestSurf)
{
  Dialog *DialogPTR = (Dialog*)LocalShare->Dialog;
  int BFHalfX = (LocalShare->ScreenWidth-128)/2 + 128;
  int BFHalfY = (LocalShare->ScreenHeight-64)/2 + 32;

  if(CurrentElement)
    DialogPTR->DrawText(DestSurf, BFHalfX-InputBoxWidth/2+5, BFHalfY-InputBoxHeight/2-13, "Edit Textmarker");
  else
    DialogPTR->DrawText(DestSurf, BFHalfX-InputBoxWidth/2+5, BFHalfY-InputBoxHeight/2-13, "Add Textmarker");

  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;
  RECT Dest;
  Dest.left = BFHalfX-InputBoxWidth/2;
  Dest.top = BFHalfY-InputBoxHeight/2;
  Dest.right = Dest.left+InputBoxWidth;
  Dest.bottom = Dest.top+InputBoxHeight;
  if(DestSurf->Blt(&Dest, lpInputBox, NULL, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
    {
    DestSurf->Blt(&Dest, lpInputBox, NULL, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
    }

  DialogPTR->DrawSmallText(DestSurf, BFHalfX-(InputBoxWidth/2)+14, BFHalfY-5, Text);

  //draw cursor if textbox overlap
  if((MouseX-128)>Dest.left-10 && (MouseX-128)<Dest.right && (MouseY-32)>Dest.top-34 && (MouseY-32)<Dest.bottom)
    DialogPTR->BlitCursor(DestSurf, MouseX-128, MouseY-32);
}

void AlliesWhiteboard::TextInputKeyDown(int Key)
{
  int Length;

  switch(Key)
    {
    case 13: //enter
      if(CurrentElement)
        {
        delete ((GraphicText*)CurrentElement)->text;
        ((GraphicText*)CurrentElement)->text = new char[strlen(Text)+1];
        strcpy(((GraphicText*)CurrentElement)->text, Text);

        PacketHandler.push_back(new GraphicTextEdited(CurrentElement->x1, CurrentElement->y1, Text, *PlayerColor));
        }
      else
        AddTextMarker();
      //InputShown = false;
      Text[0] = '\0';
      break;
    case 8: //backspace
 	   Length = strlen(Text);
      if(Length == 0)
        return;
 	   Text[Length-1] = '\0';
      break;
    case 27:    //esc
      //InputShown = false;
      Text[0] = '\0';
      break;
    }
}

void AlliesWhiteboard::TextInputChar(char C)
{
  unsigned char uC = C;
  if(uC==' ' || (uC<91 && uC>=33) || (uC<123 && uC>=97))
    {
    int Length = strlen(Text);
    if(Length > 50)
      return;
    Text[Length] = C;
    Text[Length+1] = '\0';
    }
}

void AlliesWhiteboard::AddTextMarker()
{
  int PosX = MarkerX + (LastMouseX-128);
  int PosY = MarkerY + (LastMouseY-32);

  ElementHandler.AddElement(new GraphicText(PosX, PosY, Text, *PlayerColor));

  PacketHandler.push_back(new GraphicText(PosX, PosY, Text, *PlayerColor));
}

void AlliesWhiteboard::AddTextMarker(int X, int Y, char *cText, char C)
{
  ElementHandler.AddElement(new GraphicText(X, Y, cText, C));

  EchoMarker(cText);

  LastMarkerX = X;
  LastMarkerY = Y;
}

void AlliesWhiteboard::RestoreAll()
{
  lpInputBox->Restore();
  RestoreFromPCX(13, lpInputBox);
  lpSmallCircle->Restore();
  RestoreFromPCX(14, lpSmallCircle);
}

void AlliesWhiteboard::DrawMarkers(LPDIRECTDRAWSURFACE DestSurf)
{
  std::vector<GraphicElement*> ElementPTR;
  int x1 = *MapX-75;
  if(x1<0)
    x1=0;
  int y1 = *MapY-75;
  if(y1<0)
    y1=0;

  ElementPTR = ElementHandler.GetArea(x1, y1, *MapX+SizeX+75, *MapY+SizeY+75);

  for(int i=0; i<ElementPTR.size(); i++)
    {
    GraphicText *MarkerPTR = (GraphicText*)ElementPTR[i];
    if(MarkerPTR->Type == ClassGraphicText)
      DrawTextMarker(DestSurf, MarkerPTR->x1, MarkerPTR->y1, MarkerPTR->text, MarkerPTR->Color);
    }
}

void AlliesWhiteboard::DrawMinimapMarkers(char *VidBuf, int Pitch, bool Receive)
{
	if(Receive)
		ReceiveMarkers();

	if(MinimapMarkerHandler.empty())
		return;

  CMapRect *MapRect = (CMapRect*)LocalShare->MapRect;

  for(int i=0; i<MinimapMarkerHandler.size(); i++)
    {
    //DrawRotateRect(100, 100, 300, VidBuf, Pitch);
    int XPos = MapRect->WorldToMiniX(MinimapMarkerHandler[i].XPos);
    int YPos = MapRect->WorldToMiniY(MinimapMarkerHandler[i].YPos);
    if(MinimapMarkerHandler[i].State == 0)
      {
      if(MinimapMarkerHandler[i].SubState < 2)
        {
        MinimapMarkerHandler[i].State = 1;
        }
      else
        {
        MinimapMarkerHandler[i].SubState -= 3;
        DrawRotateRect(XPos, YPos, MinimapMarkerHandler[i].SubState, VidBuf, Pitch);
        }
      }
    else
      {
      if(MinimapMarkerHandler[i].State%2 == 1)
        {
        //show cross
        DrawFreeLine(XPos, YPos-2, XPos, YPos+2, 1, VidBuf, Pitch);
        DrawFreeLine(XPos-2, YPos, XPos+2, YPos, 1, VidBuf, Pitch);
        }
      else
        {
        //do nothing
        }
      MinimapMarkerHandler[i].SubState++;
      if(MinimapMarkerHandler[i].SubState == 20)
        {
        MinimapMarkerHandler[i].State++;
        MinimapMarkerHandler[i].SubState = 0;
        }
      }
    }

  if(MinimapMarkerHandler.back().State == 16)
    MinimapMarkerHandler.pop_back();
}

void AlliesWhiteboard::GetMarkers(MarkerArray *Markers)
{
	ReceiveMarkers();

	if(MinimapMarkerHandler.empty())
		return;

	for(int i=0; i<MinimapMarkerHandler.size(); i++)
	{
		for(int j=0; j<500; j++)
		{
			if(!Markers[j].IsNew)
			{
				Markers[j].x = MinimapMarkerHandler[i].XPos;
				Markers[j].y = MinimapMarkerHandler[i].YPos;
				Markers[j].IsNew = true;
				break;
			}
		}

	}


	MinimapMarkerHandler.clear();

}

void AlliesWhiteboard::DrawTextMarker(LPDIRECTDRAWSURFACE DestSurf, int X, int Y, char *cText, char C)
{
  int x = X - *MapX + 128;
  int y = Y - *MapY + 32;

  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
  ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;
  RECT Dest;
  Dest.left = x-TextMarkerWidth/2;
  Dest.top = y-TextMarkerHeight/2;
  Dest.right = Dest.left+TextMarkerWidth;
  Dest.bottom = Dest.top+TextMarkerHeight;
  RECT Source;
  Source.left = C*TextMarkerWidth;
  Source.top = 0;
  Source.right = Source.left+TextMarkerWidth;
  Source.bottom = TextMarkerHeight;
  if(DestSurf->Blt(&Dest, lpSmallCircle, &Source, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
    {
    DestSurf->Blt(&Dest, lpSmallCircle, &Source, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
    }

  Dialog *pDialog = (Dialog*)LocalShare->Dialog;
  pDialog->DrawText(DestSurf, x+TextMarkerWidth , y-5, cText);
}

void AlliesWhiteboard::DeleteMarker(int X, int Y)
{
  ElementHandler.DeleteOn(X, Y);

  PacketHandler.push_back(new DeleteGraphic(X, Y, PacketDeleteOn));
}

void AlliesWhiteboard::EreaseArea(int x, int y)
{
  x = *MapX + x;
  y = *MapY + y;
  ElementHandler.DeleteBetween(x-50, y-50, x+50, y+50);

  PacketHandler.push_back(new DeleteGraphic(x, y, PacketDeleteArea));
}

void AlliesWhiteboard::MouseMove(int XStart, int XEnd, int YStart, int YEnd)
{
  ElementHandler.AddElement(new GraphicLine(MarkerX+XStart, MarkerY+YStart, MarkerX+XEnd, MarkerY+YEnd, *PlayerColor));
  //ElementHandler.AddElement(new GraphicLine(10, 10, 100, 100, *PlayerColor));

  PacketHandler.push_back(new GraphicLine(MarkerX+XStart, MarkerY+YStart, MarkerX+XEnd, MarkerY+YEnd, *PlayerColor));
}

void AlliesWhiteboard::ReceiveMarkers()
{
  if(DataShare->FromAlliesLength==0)
    return;

  char *NumPackets = DataShare->FromAllies;
  char *Data = DataShare->FromAllies + 1;

  if(*NumPackets == 100) //reserved for feature use
    {
    DataShare->FromAlliesLength = 0;
    return;
    }

  for(int i=0; i<*NumPackets; i++)
    {
    char *Type = Data;
    if(*Type == PacketGraphicText)
      {
      PtC *ptc = (PtC*)Data;
      Data += sizeof(PtC);
      ElementHandler.AddElement(new GraphicText(ptc->x, ptc->y, Data, ptc->Color));
      EchoMarker(Data);
      LastMarkerX = ptc->x;
      LastMarkerY = ptc->y;

      MMHS mmhs;
      mmhs.XPos = ptc->x;
      mmhs.YPos = ptc->y;
      mmhs.State = 0;
      mmhs.SubState = 360;
      MinimapMarkerHandler.push_front(mmhs);
      }
    else if(*Type == PacketGraphicLine)
      {
      PtL *ptl = (PtL*)Data;
      ElementHandler.AddElement(new GraphicLine(ptl->x, ptl->y, ptl->x2, ptl->y2, ptl->Color));
      Data += sizeof(PtL);
      }
    else if(*Type == PacketDeleteOn)
      {
      Pts *pts = (Pts*)Data;
      ElementHandler.DeleteOn(pts->x, pts->y);
      Data += sizeof(Pts);
      }
    else if(*Type == PacketDeleteArea)
      {
      Pts *pts = (Pts*)Data;
      ElementHandler.DeleteBetween(pts->x-50, pts->y-50, pts->x+50, pts->y+50);
      Data += sizeof(Pts);
      }
    else if(*Type == PacketTextChanged)
      {
      Pts *pts = (Pts*)Data;
      Data += sizeof(Pts);

      GraphicText *GT = (GraphicText*)GetTextElementAt(pts->x, pts->y, 1);
      if(GT)
        {
        delete GT->text;
        GT->text = new char[strlen(Data)+1];
        strcpy(GT->text, Data);
        }
      }
    else if(*Type == PacketTextMoved)
      {
      PtL *ptl = (PtL*)Data;
      GraphicText *GT = (GraphicText*)GetTextElementAt(ptl->x, ptl->y, 1);
      if(GT)
        GT = (GraphicText*)ElementHandler.MoveTextElement(GT, ptl->x2, ptl->y2);
      Data += sizeof(PtL);
      }
    else //unknown type skip packet
      {
      DataShare->FromAlliesLength = 0;
      return;
      }
    }
  DataShare->FromAlliesLength = 0;

}

void AlliesWhiteboard::SendMarkers()
{
  if(DataShare->ToAlliesLength>0 || PacketHandler.empty())
    return;

  char *NumPackets = DataShare->ToAllies;
  char *Data = DataShare->ToAllies + 1;
  if(PacketHandler.front()->Type == ClassGraphicText)
    {
    (*NumPackets) = 1;
    GraphicText *GT = (GraphicText*)PacketHandler.front();
    PtC ptc;
    ptc.Type = PacketGraphicText;
    ptc.x = GT->x1;
    ptc.y = GT->y1;
    ptc.Color = GT->Color;
    memcpy(Data, (char*)&ptc, sizeof(ptc));
    Data += sizeof(PtC);
    strcpy(Data, GT->text);

    DataShare->ToAlliesLength = 1 + sizeof(PtC) + strlen(GT->text)+1;

    delete GT;
    PacketHandler.pop_front();
    return;
    }
  else if(PacketHandler.front()->Type == ClassTextEdited)
    {
    (*NumPackets) = 1;
    GraphicTextEdited *GTE = (GraphicTextEdited*)PacketHandler.front();
    Pts pts;
    pts.Type = PacketTextChanged;
    pts.x = GTE->x1;
    pts.y = GTE->y1;
    memcpy(Data, (char*)&pts, sizeof(Pts));
    Data += sizeof(pts);
    strcpy(Data, GTE->text);

    DataShare->ToAlliesLength = 1 + sizeof(Pts) + strlen(GTE->text)+1;

    delete GTE;
    PacketHandler.pop_front();
    return;
    }
  else
    {
    (*NumPackets) = 0;
    DataShare->ToAlliesLength = 1;

    while(PacketHandler.size()>0 && (DataShare->ToAlliesLength+sizeof(PtL))<99)
      {
      if(PacketHandler.front()->Type==ClassGraphicText || PacketHandler.front()->Type==ClassTextEdited)
        {
        return;
        }
      else if(PacketHandler.front()->Type == ClassGraphicLine)
        {
        GraphicLine *GL = (GraphicLine*)PacketHandler.front();
        PtL ptl;
        ptl.Type = PacketGraphicLine;
        ptl.x = GL->x1;
        ptl.y = GL->y1;
        ptl.x2 = GL->x2;
        ptl.y2 = GL->y2;
        ptl.Color = GL->Color;

        memcpy(Data, (char*)&ptl, sizeof(PtL));
        (*NumPackets)++;
        Data += sizeof(PtL);
        DataShare->ToAlliesLength += sizeof(PtL);

        delete GL;
        PacketHandler.pop_front();
        }
      else if(PacketHandler.front()->Type == ClassDeleteMarker)
        {
        DeleteGraphic *DG = (DeleteGraphic*)PacketHandler.front();
        Pts pts;
        pts.Type = DG->Color;
        pts.x = DG->x1;
        pts.y = DG->y1;

        memcpy(Data, (char*)&pts, sizeof(Pts));
        (*NumPackets)++;
        Data += sizeof(Pts);
        DataShare->ToAlliesLength += sizeof(Pts);

        delete DG;
        PacketHandler.pop_front();
        }
      else if(PacketHandler.front()->Type == ClassTextMoved)
        {
        GraphicMoveText *GL = (GraphicMoveText*)PacketHandler.front();
        PtL ptl;
        ptl.Type = PacketTextMoved;
        ptl.x = GL->x1;
        ptl.y = GL->y1;
        ptl.x2 = GL->x2;
        ptl.y2 = GL->y2;
        ptl.Color = GL->Color;

        memcpy(Data, (char*)&ptl, sizeof(PtL));
        (*NumPackets)++;
        Data += sizeof(PtL);
        DataShare->ToAlliesLength += sizeof(PtL);

        delete GL;
        PacketHandler.pop_front();
        }
      }
    }
}

void AlliesWhiteboard::DrawLine(int x1, int y1, int x2, int y2, char C, char *VidBuf, int Pitch)
{
  switch(C)
    {
    case 0:
      C = (char)227;
      break;
    case 1:
      C = (char)212;
      break;
    case 2:
      C = (char)80;
      break;
    case 3:
      C = (char)235;
      break;
    case 4:
      C = (char)108;
      break;
    case 5:
      C = (char)219;
      break;
    case 6:
      C = (char)208;
      break;
    case 7:
      C = (char)93;
      break;
    case 8:
      C = (char)130;
      break;
    case 9:
      C = (char)67;
      break;
    }

  x1 = x1 - *MapX;
  y1 = y1 - *MapY;
  x2 = x2 - *MapX;
  y2 = y2 - *MapY;

  if(Clip_Line(x1, y1, x2, y2))
    Draw_Line(x1, y1, x2, y2, C, (UCHAR*)VidBuf, Pitch);

  //Line(x1-*MapX, y1-*MapY, x2-*MapX, y2-*MapY, C, VidBuf, Pitch);
}

void AlliesWhiteboard::DrawFreeLine(int x1, int y1, int x2, int y2, char C, char *VidBuf, int Pitch)
{
  if(Clip_Line(x1, y1, x2, y2))
    Draw_Line(x1, y1, x2, y2, C, (UCHAR*)VidBuf, Pitch);
}

void AlliesWhiteboard::EchoMarker(char *cText)
{
  char OutString[80];
  strcpy(OutString, "New marker added: ");
  strcat(OutString, cText);

  strcpy(DataShare->Chat, OutString);
  DataShare->NewData = 1;
}

GraphicElement *AlliesWhiteboard::GetTextElementAt(int x, int y, int Area)
{
  GraphicElement *ReturnElement = NULL;

  std::vector<GraphicElement*> ElementPTR;
  ElementPTR = ElementHandler.GetArea(x-Area, y-Area, x+Area, y+Area);

  for(int i=0; i<ElementPTR.size(); i++)
    {
    if(ElementPTR[i]->Type == ClassGraphicText)
      ReturnElement = ElementPTR[i];
    }
  return ReturnElement;
}

void AlliesWhiteboard::ScrollToCenter(int x, int y)
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

void AlliesWhiteboard::DrawRotateRect(int x, int y, int Rotation, char *VidBuf, int Pitch)
{
  //Rotation = 20;

  int Length = 2+(Rotation/3);
  Length = Length*((float)Rotation/180);
  int x1,x2,x3,x4;
  int y1,y2,y3,y4;
  int V;

  V = Rotation+45;
  x1 = x-(cos_look[abs(V)%360]*Length);
  y1 = y+(sin_look[abs(V)%360]*Length);
  V = Rotation+135;
  x2 = x-(cos_look[abs(V)%360]*Length);
  y2 = y+(sin_look[abs(V)%360]*Length);
  V = Rotation+225;
  x3 = x-(cos_look[abs(V)%360]*Length);
  y3 = y+(sin_look[abs(V)%360]*Length);
  V = Rotation+315;
  x4 = x-(cos_look[abs(V)%360]*Length);
  y4 = y+(sin_look[abs(V)%360]*Length);

  DrawFreeLine(x1, y1, x2, y2, 1, VidBuf, Pitch);
  DrawFreeLine(x2, y2, x3, y3, 1, VidBuf, Pitch);
  DrawFreeLine(x3, y3, x4, y4, 1, VidBuf, Pitch);
  DrawFreeLine(x4, y4, x1, y1, 1, VidBuf, Pitch);

}

void AlliesWhiteboard::Line(int x1, int y1, int x2, int y2, byte Colour, char *VidBuf, int Pitch)
/*
    Draw a line from (x1, y1) to (x2, y2).
    It is assumed that x1 < x2 and the gradient is less than 1.
*/
{
int x, G, DeltaG1, DeltaG2;

    int a = y2 - y1;
    int b = x2 - x1;
    G = 2 * a - b;
    DeltaG1 = 2 * (a - b);
    DeltaG2 = 2 * a;
    byte *Screen = (UCHAR*) VidBuf + x1 + y1 * Pitch;
    *Screen = Colour;
    for (x = x1; x <= x2; x++)
    {   if (G > 0)
        {   G += DeltaG1;
            Screen += Pitch + 1;	// Next column and row.
        }
        else
        {   G += DeltaG2;
            Screen += 1;			// Next column.
        }
        *Screen = Colour;
    }
}

int AlliesWhiteboard::Clip_Line(int &x1,int &y1,int &x2, int &y2)
{
// this function clips the sent line using the globally defined clipping
// region

// internal clipping codes
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

/*#define min_clip_y 32
#define min_clip_x 128
#define max_clip_x (SizeX-1)
#define max_clip_y (SizeY-1)  */

int xc1=x1, 
    yc1=y1, 
	xc2=x2, 
	yc2=y2;

int p1_code=0, 
    p2_code=0;

// determine codes for p1 and p2
if (y1 < min_clip_y)
	p1_code|=CLIP_CODE_N;
else
if (y1 > max_clip_y)
	p1_code|=CLIP_CODE_S;

if (x1 < min_clip_x)
	p1_code|=CLIP_CODE_W;
else
if (x1 > max_clip_x)
	p1_code|=CLIP_CODE_E;

if (y2 < min_clip_y)
	p2_code|=CLIP_CODE_N;
else
if (y2 > max_clip_y)
	p2_code|=CLIP_CODE_S;

if (x2 < min_clip_x)
	p2_code|=CLIP_CODE_W;
else
if (x2 > max_clip_x)
	p2_code|=CLIP_CODE_E;

// try and trivially reject
if ((p1_code & p2_code)) 
	return(0);

// test for totally visible, if so leave points untouched
if (p1_code==0 && p2_code==0)
	return(1);

// determine end clip point for p1
switch(p1_code)
	  {
	  case CLIP_CODE_C: break;

	  case CLIP_CODE_N:
		   {
		   yc1 = min_clip_y;
		   xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
		   } break;
	  case CLIP_CODE_S:
		   {
		   yc1 = max_clip_y;
		   xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);
		   } break;

	  case CLIP_CODE_W:
		   {
		   xc1 = min_clip_x;
		   yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);
		   } break;
		
	  case CLIP_CODE_E:
		   {
		   xc1 = max_clip_x;
		   yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
		   } break;

	// these cases are more complex, must compute 2 intersections
	  case CLIP_CODE_NE:
		   {
		   // north hline intersection
		   yc1 = min_clip_y;
		   xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);

		   // test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
				{
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
				} // end if

		   } break;
	  
	  case CLIP_CODE_SE:
      	   {
		   // south hline intersection
		   yc1 = max_clip_y;
		   xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);	

		   // test if intersection is valid, of so then done, else compute next
		   if (xc1 < min_clip_x || xc1 > max_clip_x)
		      {
			  // east vline intersection
			  xc1 = max_clip_x;
			  yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
			  } // end if

		   } break;
	    
	  case CLIP_CODE_NW: 
      	   {
		   // north hline intersection
		   yc1 = min_clip_y;
		   xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc1 < min_clip_x || xc1 > max_clip_x)
		      {
			  xc1 = min_clip_x;
		      yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);	
			  } // end if

		   } break;
	  	  
	  case CLIP_CODE_SW:
		   {
		   // south hline intersection
		   yc1 = max_clip_y;
		   xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);	
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc1 < min_clip_x || xc1 > max_clip_x)
		      {
			  xc1 = min_clip_x;
		      yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);	
			  } // end if

		   } break;

	  default:break;

	  } // end switch

// determine clip point for p2
switch(p2_code)
	  {
	  case CLIP_CODE_C: break;

	  case CLIP_CODE_N:
		   {
		   yc2 = min_clip_y;
		   xc2 = x2 + (min_clip_y-y2)*(x1-x2)/(y1-y2);
		   } break;

	  case CLIP_CODE_S:
		   {
		   yc2 = max_clip_y;
		   xc2 = x2 + (max_clip_y-y2)*(x1-x2)/(y1-y2);
		   } break;

	  case CLIP_CODE_W:
		   {
		   xc2 = min_clip_x;
		   yc2 = y2 + (min_clip_x-x2)*(y1-y2)/(x1-x2);
		   } break;
		
	  case CLIP_CODE_E:
		   {
		   xc2 = max_clip_x;
		   yc2 = y2 + (max_clip_x-x2)*(y1-y2)/(x1-x2);
		   } break;

		// these cases are more complex, must compute 2 intersections
	  case CLIP_CODE_NE:
		   {
		   // north hline intersection
		   yc2 = min_clip_y;
		   xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);

		   // test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
				{
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
				} // end if

		   } break;
	  
	  case CLIP_CODE_SE:
      	   {
		   // south hline intersection
		   yc2 = max_clip_y;
		   xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);	

		   // test if intersection is valid, of so then done, else compute next
		   if (xc2 < min_clip_x || xc2 > max_clip_x)
		      {
			  // east vline intersection
			  xc2 = max_clip_x;
			  yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
			  } // end if

		   } break;
	    
	  case CLIP_CODE_NW: 
      	   {
		   // north hline intersection
		   yc2 = min_clip_y;
		   xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc2 < min_clip_x || xc2 > max_clip_x)
		      {
			  xc2 = min_clip_x;
		      yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);	
			  } // end if

		   } break;
	  	  
	  case CLIP_CODE_SW:
		   {
		   // south hline intersection
		   yc2 = max_clip_y;
		   xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);	
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc2 < min_clip_x || xc2 > max_clip_x)
		      {
			  xc2 = min_clip_x;
		      yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);	
			  } // end if

		   } break;
	
	  default:break;

	  } // end switch

// do bounds check
if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
	(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
	(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
	(yc2 < min_clip_y) || (yc2 > max_clip_y) )
	{
	return(0);
	} // end if

// store vars back
x1 = xc1;
y1 = yc1;
x2 = xc2;
y2 = yc2;

return(1);

} // end Clip_Line

int AlliesWhiteboard::Draw_Line(int x0, int y0, // starting position
              int x1, int y1, // ending position
              UCHAR color,    // color index
              UCHAR *vb_start, int lpitch) // video buffer and memory pitch
{
// this function draws a line from xo,yo to x1,y1 using differential error
// terms (based on Bresenahams work)

int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping

// pre-compute first pixel address in video buffer
vb_start = vb_start + x0 + y0*lpitch;

// compute horizontal and vertical deltas
dx = x1-x0;
dy = y1-y0;

// test which direction the line is going in i.e. slope angle
if (dx>=0)
   {
   x_inc = 1;

   } // end if line is moving right
else
   {
   x_inc = -1;
   dx    = -dx;  // need absolute value

   } // end else moving left

// test y component of slope

if (dy>=0)
   {
   y_inc = lpitch;
   } // end if line is moving down
else
   {
   y_inc = -lpitch;
   dy    = -dy;  // need absolute value

   } // end else moving up

// compute (dx,dy) * 2
dx2 = dx << 1;
dy2 = dy << 1;

// now based on which delta is greater we can draw the line
if (dx > dy)
   {
   // initialize error term
   error = dy2 - dx; 

   // draw the line
   for (index=0; index <= dx; index++)
       {
       // set the pixel
       *vb_start = color;

       // test if error has overflowed
       if (error >= 0) 
          {
          error-=dx2;

          // move to next line
          vb_start+=y_inc;

	   } // end if error overflowed

       // adjust the error term
       error+=dy2;

       // move to the next pixel
       vb_start+=x_inc;

       } // end for

   } // end if |slope| <= 1
else
   {
   // initialize error term
   error = dx2 - dy; 

   // draw the line
   for (index=0; index <= dy; index++)
       {
       // set the pixel
       *vb_start = color;

       // test if error overflowed
       if (error >= 0)
          {
          error-=dy2;

          // move to next line
          vb_start+=x_inc;

          } // end if error overflowed

       // adjust the error term
       error+=dx2;

       // move to the next pixel
       vb_start+=y_inc;

       } // end for

   } // end else |slope| > 1

// return success
return(1);

} // end Draw_Line

void AlliesWhiteboard::GenerateLookupTables()
{
  for (int ang = 0; ang < 360; ang++)
    {
    float theta = (float)ang*PI/(float)180;
    cos_look[ang] = cos(theta);
    sin_look[ang] = sin(theta);
    }
}
