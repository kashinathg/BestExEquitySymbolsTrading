#pragma once

// Started 2022/02/27

#include <string>

#include <TFBitsNPieces/FrameWork01.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include <TFIQFeed/LoadMktSymbols.h>

class AppCollectAndView:
  public wxApp, public ou::tf::FrameWork01<AppCollectAndView> {
public:
protected:
private:

  FrameMain* m_pFrameMain;
//  PanelOptionsParameters* m_pPanelOptionsParameters;
  ou::tf::PanelLogging* m_pPanelLogging;

  DBOps m_db;

  ou::tf::iqfeed::InMemoryMktSymbolList m_list;

  virtual bool OnInit();
  virtual int OnExit();

  void HandlePopulateDatabase( void );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppCollectAndView)
