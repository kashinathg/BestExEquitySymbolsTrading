#pragma once

// Started 2022/02/22

#include <thread>

#include <TFBitsNPieces/FrameWork01.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "SignalGenerator.h"

class AppWeeklies:
  public wxApp, public ou::tf::FrameWork01<AppWeeklies> {
    friend ou::tf::FrameWork01<AppWeeklies>;
public:
protected:
private:

  std::thread m_worker;

  SignalGenerator m_sg;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );

  void HandleMenuActionRunScan( void );

};

DECLARE_APP(AppWeeklies)
