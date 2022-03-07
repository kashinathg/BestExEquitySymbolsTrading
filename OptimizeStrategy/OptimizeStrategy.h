

//#include <TFBitsNPieces/FrameWork01.h>



#include <TFTrading/ProviderManager.h>
#include <TFTrading/InstrumentManager.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

#include "StrategyWrapper.h"

class AppOptimizeStrategy: public wxApp {
public:
protected:
private:

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::gp::Population::vGeneration_t vGeneration_t;

  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;

  pInstrument_t m_pInstrument;

  virtual bool OnInit();
  virtual int OnExit();

  void Optimizer( void );

  void HandlePopulateDatabase( void );

  void HandleBtnStart( void );
  void HandleBtnStop( void );

};

// Implements MyApp& wxGetApp()
DECLARE_APP(AppOptimizeStrategy)
