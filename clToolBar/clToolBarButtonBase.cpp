#include "clToolBarButtonBase.h"
#include <wx/settings.h>

wxDEFINE_EVENT(wxEVT_TOOLBAR_BUTTON_CLICKED, wxCommandEvent);
wxDEFINE_EVENT(wxEVT_TOOLBAR_BUTTON_MENU_CLICKED, wxCommandEvent);

clToolBarButtonBase::clToolBarButtonBase(
    clToolBar* parent, wxWindowID id, const wxBitmap& bmp, const wxString& label, size_t flags)
    : m_toolbar(parent)
    , m_id(id)
    , m_bmp(bmp)
    , m_label(label)
    , m_flags(flags)
    , m_renderFlags(0)
{
}

clToolBarButtonBase::~clToolBarButtonBase() {}

void clToolBarButtonBase::Render(wxDC& dc, const wxRect& rect)
{
    m_dropDownArrowRect = wxRect();
    m_buttonRect = rect;

    wxColour penColour = (IsHover() || IsPressed() || IsChecked())
        ? wxSystemSettings::GetColour(wxSYS_COLOUR_MENUHILIGHT)
        : wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);

    wxColour bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR);
    if(IsHover() || IsChecked()) {
        bgColour = penColour.ChangeLightness(125);
    } else if(IsPressed()) {
        bgColour = penColour;
    }

    wxColour textColour = (IsHover() || IsPressed() || IsChecked())
        ? wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT)
        : wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);

    dc.SetBrush(bgColour);
    dc.SetPen(penColour);
    dc.DrawRectangle(rect);

    wxCoord xx = rect.GetX();
    wxCoord yy = 0;
    xx += CL_TOOL_BAR_X_MARGIN;

    if(m_bmp.IsOk()) {
        yy = (rect.GetHeight() - m_bmp.GetScaledHeight()) / 2 + rect.GetY();
        dc.DrawBitmap(m_bmp, wxPoint(xx, yy));
        xx += m_bmp.GetScaledWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    if(!m_label.IsEmpty()) {
        dc.SetTextForeground(textColour);
        wxSize sz = dc.GetTextExtent(m_label);
        yy = (rect.GetHeight() - sz.GetHeight()) / 2 + rect.GetY();
        dc.DrawText(m_label, wxPoint(xx, yy));
        xx += sz.GetWidth();
        xx += CL_TOOL_BAR_X_MARGIN;
    }

    // Do we need to draw a drop down arrow?
    if(HasMenu()) {
        // draw a drop down menu
        m_dropDownArrowRect
            = wxRect(xx, rect.GetY(), (2 * CL_TOOL_BAR_X_MARGIN) + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE, rect.GetHeight());
        dc.DrawLine(wxPoint(xx, rect.GetY()), wxPoint(xx, rect.GetY() + rect.GetHeight()));
        xx += CL_TOOL_BAR_X_MARGIN;

        wxPoint points[3];
        points[0].x = xx;
        points[0].y = (rect.GetHeight() - CL_TOOL_BAR_DROPDOWN_ARROW_SIZE) / 2 + rect.GetY();

        points[1].x = xx + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        points[1].y = points[0].y;

        points[2].x = xx + (CL_TOOL_BAR_DROPDOWN_ARROW_SIZE / 2);
        points[2].y = points[0].y + CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
        dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
        dc.DrawPolygon(3, points);

        xx += CL_TOOL_BAR_DROPDOWN_ARROW_SIZE;
        xx += CL_TOOL_BAR_X_MARGIN;
    }
}
