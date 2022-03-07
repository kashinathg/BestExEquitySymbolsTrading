// Project:  Weeklies

#include <TFBitsNPieces/ReadCboeWeeklyOptionsCsv.h>
//#include <TFBitsNPieces/ReadCboeWeeklyOptionsXls.h>
#include <TFBitsNPieces/InstrumentFilter.h>

#include <TFStatistics/Pivot.h>
#include <TFStatistics/HistoricalVolatility.h>

#include "SignalGenerator.h"

namespace pt = boost::posix_time;
namespace gregorian = boost::gregorian;

namespace {
const std::string sSpreadSheetName( "weeklies.xls" );
}

SignalGenerator::SignalGenerator(void)
  : m_fmt_mgr( m_xls)
{
}

SignalGenerator::~SignalGenerator(void) {
}

void SignalGenerator::Run( void ) {

  pt::ptime dtLast( gregorian::date( 2021, 5, 28 ), pt::time_duration( 23, 59, 59 ) );  // use date of last bar to retrieve

  std::cout << "SignalGenerator parsing cboe spreadsheet ..." << std::endl;

  std::vector<std::string> vSymbol;

  try {
    ou::tf::cboe::csv::ReadCboeWeeklyOptions(
      [&vSymbol]( const ou::tf::cboe::csv::SymbolEntry& entry ){
        vSymbol.push_back( entry.sSymbol );
      });
  }
  catch(...) {
    std::cout << "SignalGenerator failed." << std::endl;
  }

  std::cout << "SignalGenerator pre-processing cboe spreadsheet ..." << std::endl;

  for ( const std::string& sSymbol: vSymbol ) {
      //ScanBars( iter->sSymbol );
      BarSummary bs;
      //bs.sType = iter->sProductType;
      m_mapSymbol.insert( mapSymbol_t::value_type( sSymbol, bs ) );
  }

  std::cout << "SignalGenerator running eod and building output spreadsheet (" << sSpreadSheetName << ") ..." << std::endl;

  if ( 0 != m_mapSymbol.size() ) {
    ScanBars( dtLast );
  }

  std::cout << "SignalGenerator Complete" << std::endl;

}

void SignalGenerator::ScanBars( pt::ptime dtLast ) {

  pt::ptime dtBegin( dtLast.date() - gregorian::date_duration( 52 * 7 ), pt::time_duration( 0, 0, 0 ) ); // process ~year of bars

  try {

    m_xls.New( 1 );
    m_sheet = m_xls.GetWorksheet( 0 );
    ExcelFormat::BasicExcelCell* cell;
    ExcelFormat::CellFormat fmt(m_fmt_mgr);
    fmt.set_format_string( XLS_FORMAT_DECIMAL );
    fmt.set_alignment( ExcelFormat::EXCEL_HALIGN_CENTRED );
    int ix( 0 );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "Type" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "Symbol" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "Price" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "DlyHistVol" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "EmaVolm" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "BolRange" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA Sig" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "BolLwr" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "BolUpr" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA(8)" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA(21)" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "SMA(55)" );

    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "> PV" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "X Down" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "< R1 X Up" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "< R1 X Down" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "< PV" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "X Up" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "> S1 X Up" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "> S1 X Down" );
    cell = m_sheet->Cell( 0, ix++ );
    cell->SetFormat( fmt );
    cell->SetString( "PV X" );

    namespace ph = std::placeholders;
    mapSymbol_t::iterator iter;
    ou::tf::InstrumentFilter<mapSymbol_t::iterator,ou::tf::Bars> filter(
      "/bar/86400",  // at least a year's worth of bars
      dtBegin, dtLast, 200, iter,
      std::bind( &SignalGenerator::HandleCallBackUseGroup, this, ph::_1, ph::_2, ph::_3 ),
      std::bind( &SignalGenerator::HandleCallBackFilter,   this, ph::_1, ph::_2, ph::_3 ),
      std::bind( &SignalGenerator::HandleCallBackResults,  this, ph::_1, ph::_2, ph::_3, ph::_4 )
      );

    m_xls.SaveAs( sSpreadSheetName.c_str() );
    m_xls.Close();  }

  catch( ... ) {
    std::cout << "Scan Problems" << std::endl;
  }
}

bool SignalGenerator::HandleCallBackUseGroup( mapSymbol_t::iterator& iter, const std::string& sPath, const std::string& sGroup ) {
  return true;
}

bool SignalGenerator::HandleCallBackFilter( mapSymbol_t::iterator& iter, const std::string& sObject, const ou::tf::Bars& bars ) {
  iter = m_mapSymbol.find( sObject );
  return m_mapSymbol.end() != iter;
}

void SignalGenerator::HandleCallBackResults( mapSymbol_t::iterator& iter, const std::string& sPath, const std::string& sObject, const ou::tf::Bars& bars ) {
  // process bars here
  bool bFoundFirst( false );
  for ( ou::tf::Bars::const_iterator iterBars = bars.begin(); bars.end() != iterBars; ++iterBars ) {
    ou::tf::Price price( iterBars->DateTime(), iterBars->Close() );
    iter->second.prices.Append( price ); // automatically updates indicators
    if ( bFoundFirst ) {
      iter->second.emaVolume = ( ( iter->second.emaVolume * 19.0 + iterBars->Volume() ) / 20.0 );  // 20 day exponential moving average
    }
    else { // preseed a starting value
      iter->second.emaVolume = iterBars->Volume();
      bFoundFirst = true;
    }
  }

  ExcelFormat::CellFormat fmtNum( m_fmt_mgr );
  fmtNum.set_format_string( XLS_FORMAT_DECIMAL );

  ExcelFormat::CellFormat fmtInt( m_fmt_mgr );
  fmtInt.set_format_string( XLS_FORMAT_INTEGER );

  ExcelFormat::CellFormat fmtCenterTxt(m_fmt_mgr);
  fmtCenterTxt.set_alignment( ExcelFormat::EXCEL_HALIGN_CENTRED );

  int iy = m_sheet->GetTotalRows();
  ExcelFormat::BasicExcelCell* cell;
  int ix( 0 );

  cell = m_sheet->Cell( iy, ix++ ); // Type
  //cell->SetString( iter->second.sType.c_str() );  // no longer available
  cell->SetString( "" );

  cell = m_sheet->Cell( iy, ix++ ); // Symbol
  cell->SetString( sObject.c_str() );

  double last = bars.last().Close(); // Price
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetDouble( last );
  cell->SetFormat( fmtNum );

  double hv = std::for_each( bars.at( bars.Size() - 20 ), bars.end(), ou::HistoricalVolatility() );
  cell = m_sheet->Cell( iy, ix++ ); // Daily Hist Vol
  cell->SetDouble( 100.0 * hv );
  cell->SetFormat( fmtNum );

  cell = m_sheet->Cell( iy, ix++ ); // Ema Volume
  cell->SetInteger( iter->second.emaVolume );
  cell->SetFormat( fmtInt );

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtCenterTxt );
  double upper = iter->second.pricesBollinger20.BBUpper();
  double lower = iter->second.pricesBollinger20.BBLower();
  double third = ( upper - lower ) / 3.0;
  if ( last > ( upper - third ) ) {
    std::cout << "u";
    cell->SetString( "u" );
  }
  else {
    if ( last < ( lower + third ) ) {
      std::cout << "l";
      cell->SetString( "l" );
    }
    else {
      std::cout << "m";
      cell->SetString( "m" );
    }
  }

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtCenterTxt );
  double sma1 = iter->second.pricesSMA1.MeanY();
  double sma2 = iter->second.pricesSMA2.MeanY();
  double sma3 = iter->second.pricesSMA3.MeanY();
  if ( ( last > sma1 ) && ( sma1 > sma2 ) && ( sma2 > sma3 ) ) {
    std::cout << ">";
    cell->SetString( ">" );
  }
  if ( ( last < sma1 ) && ( sma1 < sma2 ) && ( sma2 < sma3 ) ) {
    std::cout << "<";
    cell->SetString( "<" );
  }

  std::cout << sObject
    << ": boll(" << lower << "," << upper
    << "),sma1(" << sma1
    << "),sma2(" << sma2
    << "),sma3(" << sma3
    << ")"
    ;

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( lower );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( upper );

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( sma1 );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( sma2 );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( sma3 );

  ou::tf::statistics::Pivot pivot( bars );

  std::cout
    << ",pivot("
    <<        pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::AbovePV )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::AbovePV_X_Down )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVR1_X_Up )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVR1_X_Down )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BelowPV )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BelowPV_X_Up )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVS1_X_Up )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVS1_X_Down )
    << "," << pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::CrossPV )
    << ")"
    ;

  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::AbovePV ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::AbovePV_X_Down ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVR1_X_Up ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVR1_X_Down ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BelowPV ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BelowPV_X_Up ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVS1_X_Up ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::BtwnPVS1_X_Down ) );
  cell = m_sheet->Cell( iy, ix++ );
  cell->SetFormat( fmtNum );
  cell->SetDouble( pivot.ItemOfInterest( ou::tf::statistics::Pivot::EItemsOfInterest::CrossPV ) );

  std::cout
    << std::endl;
}
