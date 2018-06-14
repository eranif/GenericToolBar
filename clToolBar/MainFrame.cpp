#include "MainFrame.h"
#include "clToolBar.h"
#include "clToolBarButton.h"
#include <wx/aboutdlg.h>
#include <wx/msgdlg.h>
#include "clToolBarMenuButton.h"
#include <wx/menu.h>

MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBaseClass(parent)
{
    MyImages images;
    m_toolbar = new clToolBar(this);
    m_toolbar->AddButton(wxID_OPEN, images.Bitmap("folder"));
    m_toolbar->AddButton(wxID_NEW, images.Bitmap("file"));
    m_toolbar->AddButton(XRCID("bookmark"), images.Bitmap("bookmark"), "Bookmark");

    // Add button with menu
    clToolBarMenuButton* menuButton =
        new clToolBarMenuButton(m_toolbar, XRCID("bookmark_menu"), images.Bitmap("bookmark"), "Menu!");
    m_toolbar->InsertBefore(XRCID("bookmark"), menuButton);
    m_toolbar->Realize();

    GetSizer()->Add(m_toolbar, 0, wxEXPAND, 0);
    SetSize(600, 400);

    m_toolbar->Bind(wxEVT_TOOLBAR_BUTTON_CLICKED, &MainFrame::OnFolder, this, wxID_OPEN);
    m_toolbar->Bind(wxEVT_TOOLBAR_BUTTON_CLICKED, &MainFrame::OnFile, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOLBAR_BUTTON_CLICKED, &MainFrame::OnBookmark, this, XRCID("bookmark"));
    m_toolbar->Bind(wxEVT_TOOLBAR_BUTTON_CLICKED, &MainFrame::OnBookmarkMenu, this, XRCID("bookmark_menu"));
    m_toolbar->Bind(wxEVT_TOOLBAR_BUTTON_MENU_CLICKED, &MainFrame::OnBookmarkMenuPart, this, XRCID("bookmark_menu"));
}

MainFrame::~MainFrame() {}

void MainFrame::OnExit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxAboutDialogInfo info;
    info.SetCopyright(_("My MainFrame"));
    info.SetLicence(_("GPL v2 or later"));
    info.SetDescription(_("Short description goes here"));
    ::wxAboutBox(info);
}

void MainFrame::OnFolder(wxCommandEvent& e) { ::wxMessageBox("Folder clicked!"); }

void MainFrame::OnFile(wxCommandEvent& e) { ::wxMessageBox("File clicked!"); }

void MainFrame::OnBookmark(wxCommandEvent& e) { ::wxMessageBox("Bookmark clicked!"); }

void MainFrame::OnBookmarkMenu(wxCommandEvent& e) { ::wxMessageBox("Bookmark menu clicked!"); }

void MainFrame::OnBookmarkMenuPart(wxCommandEvent& e) 
{  
    wxMenu menu;
    menu.Append(wxID_OPEN);
    menu.Append(wxID_CLOSE);
    menu.AppendSeparator();
    menu.Append(wxID_CLOSE_ALL);
    
    m_toolbar->ShowMenuForButton(XRCID("bookmark_menu"), &menu);
}
