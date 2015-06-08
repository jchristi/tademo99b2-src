#include "maprect.h"
#include "iddrawsurface.h"

CMapRect::CMapRect()
{
  lpMapRect = NULL;
  if(DataShare->ehaOff == 1)
    return;

  int *PTR = (int*)0x00511de8;
  MapX = (int*)(*PTR + 0x1431f);
  MapY = (int*)(*PTR + 0x14323);

  MapSizeX = (int*)(*PTR + 0x1422b);
  MapSizeY = (int*)(*PTR + 0x1422f);

  if(GetMapMaxX()==0 || GetMapMaxY()==0)
  {
    lpMapRect = NULL;
  }
  else
    {
    LPDIRECTDRAW TADD = (IDirectDraw*)LocalShare->TADirectDraw;

    DDSURFACEDESC ddsd;
    DDRAW_INIT_STRUCT(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;

    if(GetMapMaxX() == GetMapMaxY())
      {
      MiniMapWidth = 126;
      MiniMapHeight = 126;
      XOffset = 0;
      YOffset = 0;
      }
    else if(GetMapMaxX() > GetMapMaxY())
      {
      MiniMapWidth = 126;
      MiniMapHeight = ((float)GetMapMaxY()/(float)GetMapMaxX())*126;
      XOffset = 0;
      YOffset = (126-MiniMapHeight)/2;
      }
    else if(GetMapMaxX() < GetMapMaxY())
      {
      MiniMapWidth = ((float)GetMapMaxX()/(float)GetMapMaxY())*126;
      MiniMapHeight = 126;
      XOffset = (126-MiniMapWidth)/2;
      YOffset = 0;
      }

    ddsd.dwWidth = ((float)MiniMapWidth/(float)GetMapMaxX())*(float)RectWidth;
    ddsd.dwHeight = ((float)MiniMapHeight/(float)GetMapMaxY())*(float)RectHeight;

    Width = ddsd.dwWidth;
    Height = ddsd.dwHeight;
    TADD->CreateSurface(&ddsd, &lpMapRect, NULL);

    PaintRect();
    }

  for(int i=0; i<10; i++)
    {
    DataShare->OtherMapX[i] = -1;
    DataShare->OtherMapY[i] = -1;
    }
  WidthAdd = (LocalShare->ScreenWidth-128)/2;
  HeightAdd = (LocalShare->ScreenHeight-64)/2;

  LocalShare->MapRect = this;
}

CMapRect::~CMapRect()
{
  if(lpMapRect)
    lpMapRect->Restore();
}

void CMapRect::LockBlit(char *VidBuf, int Pitch)
{
  DataShare->MapX = (*MapX)+WidthAdd;
  DataShare->MapY = (*MapY)+HeightAdd;

  AlliesWhiteboard *WhiteBoard = (AlliesWhiteboard*)LocalShare->Whiteboard;

  WhiteBoard->min_clip_y = 0;
  WhiteBoard->min_clip_x = 0;
  WhiteBoard->max_clip_x = 128;
  WhiteBoard->max_clip_y = 128;

  if(DataShare->TAProgress == TAInGame)
    {
    RECT Dest;

    for(int i=1; i<10; i++)
      {
      if((DataShare->allies[i] || DataShare->PlayingDemo) && DataShare->OtherMapX[i]!=-1 && DataShare->PlayerNames[i][0]!='\0')
        {
        Dest.left = ((((float)(DataShare->OtherMapX[i]) / (float)(*MapSizeX))*MiniMapWidth) + XOffset) - Width/2;
        Dest.top = ((((float)(DataShare->OtherMapY[i]) / (float)(*MapSizeY))*MiniMapHeight) + YOffset) - Height/2;
        Dest.right = Dest.left + Width;
        Dest.bottom = Dest.top + Height;

        if(DataShare->PlayingDemo)
          {
          if(DataShare->LockOn==i)
           ((AlliesWhiteboard*)LocalShare->Whiteboard)->ScrollToCenter(DataShare->OtherMapX[i], DataShare->OtherMapY[i]);
          }

        char C;

		  switch(DataShare->PlayerColors[i])
    		{
   	   case 0:
           C = 227;
		     break;
  		  case 1:
     		 C = 212;
     		 break;
  		  case 2:
    		 C = 80;
    		 break;
        case 3:
    		 C = 235;
    		 break;
        case 4:
    		 C = 108;
    		 break;
        case 5:
    		 C = 219;
    		 break;
        case 6:
    		 C = 208;
     		 break;
        case 7:
      	 C = 93;
    		 break;
  		  case 8:
          C = 130;
          break;
        case 9:
          C = 67;
    		 break;
        default:
        C = 1;
        }
        
        WhiteBoard->DrawFreeLine(Dest.left, Dest.top, Dest.right, Dest.top, C, VidBuf, Pitch);
        WhiteBoard->DrawFreeLine(Dest.right, Dest.top, Dest.right, Dest.bottom, C, VidBuf, Pitch);
        WhiteBoard->DrawFreeLine(Dest.left, Dest.top, Dest.left, Dest.bottom, C, VidBuf, Pitch);
        WhiteBoard->DrawFreeLine(Dest.right, Dest.bottom, Dest.left, Dest.bottom, C, VidBuf, Pitch);
        }
      }
    }
  //DrawFreeLine(int x1, int y1, int x2, int y2, char C, char *VidBuf, int Pitch)
}

void CMapRect::Blit(LPDIRECTDRAWSURFACE DestSurf)
{
  DataShare->MapX = (*MapX)+WidthAdd;
  DataShare->MapY = (*MapY)+HeightAdd;

  if(lpMapRect == NULL)
    return;

  if(lpMapRect->IsLost() != DD_OK)
    {
    lpMapRect->Restore();
    PaintRect();
    }

  //if(*MapSizeX==0 || *MapSizeY==0)
  //  return;
  if(DataShare->TAProgress == TAInGame)
    {
    RECT Dest;
    DDBLTFX ddbltfx;
    DDRAW_INIT_STRUCT(ddbltfx);
    ddbltfx.ddckSrcColorkey.dwColorSpaceLowValue = 102;
    ddbltfx.ddckSrcColorkey.dwColorSpaceHighValue = 102;

    for(int i=1; i<10; i++)
      {
      if(DataShare->allies[i] && DataShare->OtherMapX[i] != -1)
        {
        Dest.left = ((((float)((DataShare->OtherMapX[i])) / (float)(*MapSizeX))*MiniMapWidth) + XOffset) - Width/2;
        Dest.top = ((((float)(DataShare->OtherMapY[i]) / (float)(*MapSizeY))*MiniMapHeight) + YOffset) - Height/2;
        Dest.right = Dest.left + Width;
        Dest.bottom = Dest.top + Height;

        if(DestSurf->Blt(&Dest, lpMapRect, NULL, DDBLT_ASYNC | DDBLT_KEYSRCOVERRIDE, &ddbltfx)!=DD_OK)
          {
          DestSurf->Blt(&Dest, lpMapRect, NULL, DDBLT_WAIT | DDBLT_KEYSRCOVERRIDE, &ddbltfx);
          }
        }
      }
    }
  //DrawFreeLine(int x1, int y1, int x2, int y2, char C, char *VidBuf, int Pitch)
}

int CMapRect::GetMapMaxX()
{
  //int *PTR = (int*)0x00511de8;
  //MapSizeX = (int*)(*PTR + 0x1422b);
  return *MapSizeX;
}

int CMapRect::GetMapMaxY()
{
  //int *PTR = (int*)0x00511de8;
  //MapSizeY = (int*)(*PTR + 0x1422f);
  return *MapSizeY;
}

void CMapRect::PaintRect()
{
  DDSURFACEDESC ddsd;
  DDRAW_INIT_STRUCT(ddsd);

  //clear the surface
  DDBLTFX ddbltfx;
  DDRAW_INIT_STRUCT(ddbltfx);
  ddbltfx.dwFillColor = 102;
  if(lpMapRect->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_ASYNC, &ddbltfx)!=DD_OK)
    {
    lpMapRect->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
    }

  if(lpMapRect->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL)!=DD_OK)
    return; //???

  //paint the top and bottom rows
  for(int i=0; i<Width; i++)
    ((char*)ddsd.lpSurface)[i] = RectColor;
  for(i=0; i<Width; i++)
    ((char*)ddsd.lpSurface)[i+((Height-1)*ddsd.lPitch)] = RectColor;

  for(i=0; i<Height; i++)
    {
    ((char*)ddsd.lpSurface)[i*ddsd.lPitch] = RectColor;
    ((char*)ddsd.lpSurface)[(Width-1)+i*ddsd.lPitch] = RectColor;
    }

  lpMapRect->Unlock(NULL);
}

int CMapRect::WorldToMiniX(int x)
{
  return ((((float)(x) / (float)(*MapSizeX))*MiniMapWidth) + XOffset);
}

int CMapRect::WorldToMiniY(int y)
{
  return ((((float)(y) / (float)(*MapSizeY))*MiniMapHeight) + YOffset);
}
