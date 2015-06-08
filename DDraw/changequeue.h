#ifndef changequeueH
#define changequeueH

#include <windows.h>
#include "oddraw.h"

class CChangeQueue
{
  public:
    CChangeQueue();
    ~CChangeQueue();
    bool Message(HWND WinProchWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    void Blit(LPDIRECTDRAWSURFACE DestSurf);

};

#endif
