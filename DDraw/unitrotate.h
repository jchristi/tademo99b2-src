#ifndef unitrotateH
#define unitrotateH

#include "iddrawsurface.h"

class CUnitRotate
{
  private:
    void Rotate();
    void SetMem();
  public:
    CUnitRotate();
    ~CUnitRotate();
    bool Message(HWND WinProchWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

};

#endif
