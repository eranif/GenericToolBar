#ifndef CLTOOLBAR_H
#define CLTOOLBAR_H

#include "codelite_exports.h"
#include <vector>
#include <wx/panel.h>

class clToolBarButtonBase;
class WXDLLIMPEXP_SDK clToolBar : public wxPanel
{
    std::vector<clToolBarButtonBase*> m_buttons;

protected:
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnEnterWindow(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
    wxRect CalculateRect(wxDC& dc) const;

public:
    clToolBar(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = "clToolBar");
    virtual ~clToolBar();

    /**
     * @brief add toolbar button
     */
    clToolBarButtonBase* Add(clToolBarButtonBase* button);

    /**
     * @brief This function should be called after you have added tools.
     */
    void Realize();
};

#endif // CLTOOLBAR_H
