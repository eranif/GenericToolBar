#ifndef MAINFRAME_H
#define MAINFRAME_H
#include "wxcrafter.h"

class clToolBar;
class MainFrame : public MainFrameBaseClass
{
    clToolBar* m_toolbar;

public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();

    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnFolder(wxCommandEvent& e);
    void OnFile(wxCommandEvent& e);
    void OnBookmark(wxCommandEvent& e);
};
#endif // MAINFRAME_H
