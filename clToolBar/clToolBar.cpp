#include "clToolBar.h"
#include "clToolBarButton.h"
#include "clToolBarButtonBase.h"
#include "clToolBarMenuButton.h"
#include "clToolBarToggleButton.h"
#include <algorithm>
#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>

#ifdef __WXGTK__
#include <gtk/gtk.h>
static wxColour GtkGetBgColourFromWidget(GtkWidget* widget, const wxColour& defaultColour)
{
    wxColour bgColour = defaultColour;
    GtkStyle* def = gtk_rc_get_style(widget);
    if(!def) { def = gtk_widget_get_default_style(); }

    if(def) {
        GdkColor col = def->bg[GTK_STATE_NORMAL];
        bgColour = wxColour(col);
    }
    gtk_widget_destroy(widget);
    return bgColour;
}
#endif

static wxColour GetMenuBarColour()
{
#if defined(__WXGTK__) && !defined(__WXGTK3__)
    static bool intitialized(false);
    // initialise default colour
    static wxColour bgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));

    if(!intitialized) {
        // try to get the background colour from a menu
        GtkWidget* menuBar = gtk_menu_bar_new();
        bgColour = GtkGetBgColourFromWidget(menuBar, bgColour);
        intitialized = true;
    }
    return bgColour;
#else
    return wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
#endif
}

clToolBar::clToolBar(
    wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
    : wxPanel(parent, winid, pos, size, style, name)
    , m_popupShown(false)
{
    Bind(wxEVT_PAINT, &clToolBar::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &clToolBar::OnEraseBackground, this);
    Bind(wxEVT_LEFT_UP, &clToolBar::OnLeftUp, this);
    Bind(wxEVT_LEFT_DOWN, &clToolBar::OnLeftDown, this);
    Bind(wxEVT_MOTION, &clToolBar::OnMotion, this);
    Bind(wxEVT_ENTER_WINDOW, &clToolBar::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &clToolBar::OnLeaveWindow, this);
}

clToolBar::~clToolBar()
{
    Unbind(wxEVT_PAINT, &clToolBar::OnPaint, this);
    Unbind(wxEVT_ERASE_BACKGROUND, &clToolBar::OnEraseBackground, this);
    Unbind(wxEVT_LEFT_UP, &clToolBar::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &clToolBar::OnMotion, this);
    Unbind(wxEVT_ENTER_WINDOW, &clToolBar::OnEnterWindow, this);
    Unbind(wxEVT_LEAVE_WINDOW, &clToolBar::OnLeaveWindow, this);
    Unbind(wxEVT_LEFT_DOWN, &clToolBar::OnLeftDown, this);

    for(size_t i = 0; i < m_buttons.size(); ++i) { delete m_buttons[i]; }
    m_buttons.clear();
}

void clToolBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    wxRect clientRect = GetClientRect();
    dc.SetBrush(GetMenuBarColour());
    dc.SetPen(GetMenuBarColour());
    dc.DrawRectangle(clientRect);

    int xx = 0;
    std::for_each(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(dc);
        wxRect r(xx, 0, buttonSize.GetWidth(), clientRect.GetHeight());
        button->Render(dc, r);
        xx += buttonSize.GetWidth();
    });
}

void clToolBar::OnEraseBackground(wxEraseEvent& event) { wxUnusedVar(event); }

wxRect clToolBar::CalculateRect(wxDC& dc) const
{
    wxRect rect;
    std::for_each(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* button) {
        wxSize buttonSize = button->CalculateSize(dc);
        rect.width += buttonSize.GetWidth();
        rect.height = wxMax(rect.GetHeight(), buttonSize.GetHeight());
    });
    return rect;
}

void clToolBar::Realize()
{
    wxBitmap bmp(1, 1);
    wxMemoryDC dc(bmp);
    dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
    SetSizeHints(CalculateRect(dc).GetSize());
}

void clToolBar::OnLeftUp(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        clToolBarButtonBase* btn = m_buttons[i];
        if(btn->Contains(pos)) {
            if(btn->IsToggle()) {
                // Change the button state
                btn->Check(!btn->IsChecked());
                // Fire an event with proper IsChecked() set
                wxCommandEvent clicked(wxEVT_TOOLBAR_BUTTON_CLICKED, btn->GetId());
                clicked.SetEventObject(this);
                clicked.SetInt(btn->IsChecked() ? 1 : 0);
                GetEventHandler()->AddPendingEvent(clicked);
            } else if(btn->InsideMenuButton(pos)) {
                wxCommandEvent clicked(wxEVT_TOOLBAR_BUTTON_MENU_CLICKED, btn->GetId());
                clicked.SetEventObject(this);
                GetEventHandler()->AddPendingEvent(clicked);
            } else {
                wxCommandEvent clicked(wxEVT_TOOLBAR_BUTTON_CLICKED, btn->GetId());
                clicked.SetEventObject(this);
                GetEventHandler()->AddPendingEvent(clicked);
            }
            break;
        }
    }
}

void clToolBar::OnLeftDown(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i]->ClearRenderFlags();
        if(m_buttons[i]->Contains(pos)) { m_buttons[i]->SetPressed(true); }
    }
    Refresh();
}

void clToolBar::OnMotion(wxMouseEvent& event)
{
    wxPoint pos = event.GetPosition();
    bool refreshNeeded = false;
    for(size_t i = 0; i < m_buttons.size(); ++i) {
        if(m_buttons[i]->Contains(pos)) {
            if(!m_buttons[i]->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            m_buttons[i]->SetHover(true);
        } else {
            if(m_buttons[i]->IsHover()) {
                // a refresh is needed
                refreshNeeded = true;
            }
            m_buttons[i]->ClearRenderFlags();
        }
    }
    if(refreshNeeded) { Refresh(); }
}

void clToolBar::OnEnterWindow(wxMouseEvent& event) { OnMotion(event); }

void clToolBar::OnLeaveWindow(wxMouseEvent& event)
{
    if(!m_popupShown) {
        for(size_t i = 0; i < m_buttons.size(); ++i) { m_buttons[i]->ClearRenderFlags(); }
        Refresh();
    }
}

clToolBarButtonBase* clToolBar::AddButton(wxWindowID id, const wxBitmap& bmp, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarButton(this, id, bmp, label);
    return Add(button);
}

clToolBarButtonBase* clToolBar::AddMenuButton(wxWindowID id, const wxBitmap& bmp, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarMenuButton(this, id, bmp, label);
    return Add(button);
}

clToolBarButtonBase* clToolBar::AddToggleButton(wxWindowID id, const wxBitmap& bmp, const wxString& label)
{
    clToolBarButtonBase* button = new clToolBarToggleButton(this, id, bmp, label);
    return Add(button);
}

clToolBarButtonBase* clToolBar::InsertBefore(wxWindowID where, clToolBarButtonBase* button)
{
    if(where == wxID_ANY) {
        m_buttons.insert(m_buttons.begin(), button);
    } else if(where == INT_MAX) {
        m_buttons.push_back(button);
    } else {
        std::vector<clToolBarButtonBase*>::iterator iter = std::find_if(
            m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == where); });
        if(iter == m_buttons.end()) {
            m_buttons.push_back(button);
        } else {
            m_buttons.insert(iter, button);
        }
    }
    return button;
}

clToolBarButtonBase* clToolBar::Add(clToolBarButtonBase* button) { return InsertBefore(INT_MAX, button); }

void clToolBar::ShowMenuForButton(wxWindowID buttonID, wxMenu* menu)
{
    std::vector<clToolBarButtonBase*>::iterator iter = std::find_if(
        m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == buttonID); });
    if(iter == m_buttons.end()) { return; }
    clToolBarButtonBase* button = *iter;
    m_popupShown = true;
    PopupMenu(menu, button->GetButtonRect().GetBottomLeft());
    m_popupShown = false;

    wxPoint pt = ::wxGetMousePosition();
    pt = ScreenToClient(pt);
    if(!GetClientRect().Contains(pt)) {
        wxMouseEvent dummy;
        OnLeaveWindow(dummy);
    }
}

clToolBarButtonBase* clToolBar::FindById(wxWindowID id) const
{
    std::vector<clToolBarButtonBase*>::const_iterator iter
        = std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return NULL; }
    return (*iter);
}

bool clToolBar::DeleteById(wxWindowID id)
{
    std::vector<clToolBarButtonBase*>::iterator iter
        = std::find_if(m_buttons.begin(), m_buttons.end(), [&](clToolBarButtonBase* b) { return (b->GetId() == id); });
    if(iter == m_buttons.end()) { return false; }
    clToolBarButtonBase* button = (*iter);
    delete button;
    m_buttons.erase(iter);
    Refresh();
    return true;
}
