#ifndef idlevillagerH
#define idlevillagerH

#include "oddraw.h"

class CIdleUnits
{
  private:
    void FindIdleConst();
    void ScrollToCenter(int x, int y);
    void DeselectUnits();
  public:
    CIdleUnits();
    ~CIdleUnits();
    void Blit(LPDIRECTDRAWSURFACE DestSurf);
    bool Message(HWND WinProcWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};

#endif