#pragma once

// Started 2022/02/24

#include <string>

#include <wx/panel.h>

#define SYMBOL_PANELPORTFOLIOSTATS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPORTFOLIOSTATS_TITLE _("PanelPortfolioStats")
#define SYMBOL_PANELPORTFOLIOSTATS_IDNAME ID_PANELPORTFOLIOSTATS
//#define SYMBOL_PANELPORTFOLIOSTATS_SIZE wxSize(400, 300)
#define SYMBOL_PANELPORTFOLIOSTATS_SIZE wxDefaultSize
#define SYMBOL_PANELPORTFOLIOSTATS_POSITION wxDefaultPosition


class PanelPortfolioStats: public wxPanel {
public:
  PanelPortfolioStats(void);
  PanelPortfolioStats(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPORTFOLIOSTATS_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPORTFOLIOSTATS_POSITION,
    const wxSize& size = SYMBOL_PANELPORTFOLIOSTATS_SIZE,
    long style = SYMBOL_PANELPORTFOLIOSTATS_STYLE );
  bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOSTATS_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPORTFOLIOSTATS_POSITION,
    const wxSize& size = SYMBOL_PANELPORTFOLIOSTATS_SIZE,
    long style = SYMBOL_PANELPORTFOLIOSTATS_STYLE );
  ~PanelPortfolioStats(void);
  void SetStats(
    const std::string& sMin, const std::string& sCurrent, const std::string& sMax,
    const std::string& sUp, const std::string& sDown
  );
protected:
private:
  enum { ID_NULL=wxID_HIGHEST, SYMBOL_PANELPORTFOLIOSTATS_IDNAME,
    ID_TxtPortfolioLow, ID_TxtPortfolioCurrent, ID_TxtPortfolioHigh,
    ID_TxtSentimentUp, ID_TxtSentimentDown
  };

    wxTextCtrl* m_txtPortfolioLow;
    wxTextCtrl* m_txtPortfolioCurrent;
    wxTextCtrl* m_txtPortfolioHigh;
    wxTextCtrl* m_txtSentimentUp;
    wxTextCtrl* m_txtSentimentDown;

  void Init();
  void CreateControls();
  bool ShowToolTips( void ) { return true; };

};
