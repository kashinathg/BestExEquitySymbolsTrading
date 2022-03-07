// Project:  Weeklies

#pragma once

#include <string>
#include <map>

#include <ExcelFormat/ExcelFormat.h>

#include <TFTimeSeries/TimeSeries.h>
#include <TFIndicators/TSSWStats.h>

// Started 2022/02/25

namespace pt = boost::posix_time;
namespace gregorian = boost::gregorian;

class SignalGenerator {
public:
  SignalGenerator(void);
  ~SignalGenerator(void);
  void Run( void );
protected:
private:

  ExcelFormat::BasicExcel m_xls;
  ExcelFormat::BasicExcelWorksheet* m_sheet;
  ExcelFormat::XLSFormatManager m_fmt_mgr;

  struct BarSummary {
    //std::string sType;
    ou::tf::Prices prices;
    ou::tf::Price::volume_t emaVolume;
    ou::tf::TSSWStatsPrice pricesBollinger20;
    ou::tf::TSSWStatsPrice pricesSMA1;
    ou::tf::TSSWStatsPrice pricesSMA2;
    ou::tf::TSSWStatsPrice pricesSMA3;
    BarSummary( void )
      : pricesBollinger20( prices, time_duration( 0, 0, 0 ), 20 ),
        pricesSMA1( prices, time_duration( 0, 0, 0 ),  8 ),
        pricesSMA2( prices, time_duration( 0, 0, 0 ), 21 ),
        pricesSMA3( prices, time_duration( 0, 0, 0 ), 55 ),
        emaVolume {}
    {}
    BarSummary( const BarSummary& rhs )  // ensures map has local assignments.
      : pricesBollinger20( prices, time_duration( 0, 0, 0 ), 20 ),
        pricesSMA1( prices, time_duration( 0, 0, 0 ),  8 ),
        pricesSMA2( prices, time_duration( 0, 0, 0 ), 21 ),
        pricesSMA3( prices, time_duration( 0, 0, 0 ), 55 ),
        //sType( rhs.sType ),
        emaVolume {}
    { /* need to copy data over, but shouldn't be anything to copy */ }
  };

  typedef std::map<std::string,BarSummary> mapSymbol_t;
  mapSymbol_t m_mapSymbol;

  void ScanBars( pt::ptime dtLast );
  bool HandleCallBackUseGroup( mapSymbol_t::iterator&, const std::string& sPath, const std::string& sGroup );
  bool HandleCallBackFilter( mapSymbol_t::iterator&, const std::string& sObject, const ou::tf::Bars& bars );
  void HandleCallBackResults( mapSymbol_t::iterator&, const std::string& sPath, const std::string& sObject, const ou::tf::Bars& bars );

};
