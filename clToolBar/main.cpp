#include "MainFrame.h"
#include <wx/app.h>
#include <wx/event.h>
#include <wx/image.h>

#ifdef __WXMSW__
typedef BOOL WINAPI (*SetProcessDPIAwareFunc)();
#endif

// Define the MainApp
class MainApp : public wxApp
{
public:
    MainApp() {}
    virtual ~MainApp() {}

    virtual bool OnInit()
    {
#ifdef __WXMSW__

        HINSTANCE m_user32Dll = LoadLibrary(L"User32.dll");
        if(m_user32Dll) {
            SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(m_user32Dll, "SetProcessDPIAware");
            if(pFunc) { pFunc(); }
            FreeLibrary(m_user32Dll);
            m_user32Dll = NULL;
        }
#endif

        // Add the common image handlers
        wxImage::AddHandler(new wxPNGHandler);
        wxImage::AddHandler(new wxJPEGHandler);

        MainFrame* mainFrame = new MainFrame(NULL);
        SetTopWindow(mainFrame);
        return GetTopWindow()->Show();
    }
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)
