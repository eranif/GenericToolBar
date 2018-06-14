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
     * @brief insert a button after the button identified by 'otherButton'
     * @param where insert the button after this button. 
     * If 'where' == -1 our button is placed first insert first
     * If 'where' == INT_MAX our button is placed last
     * if a button with id == 'where' can not be found, we append the button
     */
    clToolBarButtonBase* InsertBefore(wxWindowID where, clToolBarButtonBase* button);
    clToolBarButtonBase* AddButton(wxWindowID id, const wxBitmap& bmp, const wxString& label = "");
    clToolBarButtonBase* AddMenuButton(wxWindowID id, const wxBitmap& bmp, const wxString& label = "");

    /**
     * @brief This function should be called after you have added tools.
     */
    void Realize();
};

#endif // CLTOOLBAR_H
