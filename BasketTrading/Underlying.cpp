#include "Underlying.h"

Underlying::Underlying(
  pWatch_t pWatch,
  pPortfolio_t pPortfolio
)
:
  m_pWatch( pWatch ),
  m_GexCalc( pWatch ),
  m_pPortfolio( pPortfolio ),
  m_bfTrades06Sec( 6 )
{
  assert( pWatch );
  assert( m_pPortfolio );

  const std::string& sName( m_pWatch->GetInstrument()->GetInstrumentName() );

  m_pChartDataView = std::make_shared<ou::ChartDataView>();
  m_pChartDataView->SetNames( "Profit / Loss", sName );

  m_cePrice.SetName( "Price" );
  m_ceVolume.SetName( "Volume" );

  m_cePLCurrent.SetColour( ou::Colour::Fuchsia );
  m_cePLUnRealized.SetColour( ou::Colour::DarkCyan );
  m_cePLRealized.SetColour( ou::Colour::MediumSlateBlue );
  m_ceCommissionPaid.SetColour( ou::Colour::SteelBlue );

  m_cePLCurrent.SetName( "P/L Current" );
  m_cePLUnRealized.SetName( "P/L UnRealized" );
  m_cePLRealized.SetName( "P/L Realized" );
  m_ceCommissionPaid.SetName( "Commissions Paid" );

  PopulateChartDataView( m_pChartDataView );

  m_pChartDataView->Add( EChartSlot::PL, &m_cePLCurrent );
  m_pChartDataView->Add( EChartSlot::PL, &m_cePLUnRealized );
  m_pChartDataView->Add( EChartSlot::PL, &m_cePLRealized );
  m_pChartDataView->Add( EChartSlot::PL, &m_ceCommissionPaid );

  m_bfTrades06Sec.SetOnBarComplete( MakeDelegate( this, &Underlying::HandleBarTrades06Sec ) );

  m_pWatch->OnQuote.Add( MakeDelegate( this, &Underlying::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Underlying::HandleTrade ) );
  m_pWatch->StartWatch();

}

Underlying::~Underlying() {

  m_pWatch->StopWatch();
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Underlying::HandleQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Underlying::HandleTrade ) );

}

void Underlying::SetPivots( double dblR2, double dblR1, double dblPV, double dblS1, double dblS2 ) {
  // TFIndicators/Pivots.h has R3, S3 plus colour assignments
  //m_pivotCrossing.Set( dblR2, dblR1, dblPV, dblS1, dblS2 ); // belongs back in ManageStrategy?
  m_cePivots.AddMark( dblR2, ou::Colour::Red, "R2" );
  m_cePivots.AddMark( dblR1, ou::Colour::Red, "R1" );
  m_cePivots.AddMark( dblPV, ou::Colour::Green, "PV" );
  m_cePivots.AddMark( dblS1, ou::Colour::Blue, "S1" );
  m_cePivots.AddMark( dblS2, ou::Colour::Blue, "S2" );
  std::cout << m_pWatch->GetInstrumentName() << " pivots: "
    << dblR2 << "," << dblR1 << "," << dblPV << "," << dblS1 << "," << dblS2
    << std::endl;
}

void Underlying::SaveSeries( const std::string& sPrefix ) {
  m_pWatch->SaveSeries( sPrefix );
}

void Underlying::ReadDailyBars( const std::string& sDailyBarPath ) {
  m_BollingerTransitions.ReadDailyBars( sDailyBarPath, m_cePivots );
}

void Underlying::PopulateChartDataView( pChartDataView_t pChartDataView ) {
  pChartDataView->Add( EChartSlot::Price, &m_cePrice );
  pChartDataView->Add( EChartSlot::Price, &m_cePivots );
  pChartDataView->Add( EChartSlot::Volume, &m_ceVolume );
}

void Underlying::PopulateChains( fGatherOptions_t&& f ) {
  m_GexCalc.LoadChains( std::move( f ) );
}

void Underlying::WalkChains( fOption_t&& fOption ) const {
  m_GexCalc.WalkChains( std::move( fOption ) );
}

void Underlying::WalkChain( boost::gregorian::date date, fOption_t&& fOption  ) const {
  m_GexCalc.WalkChain( date, std::move( fOption ) );
}

void Underlying::HandleQuote( const ou::tf::Quote& quote ) {
}

void Underlying::HandleTrade( const ou::tf::Trade& trade ) {
  m_bfTrades06Sec.Add( trade );
}

void Underlying::HandleBarTrades06Sec( const ou::tf::Bar& bar ) {

  m_cePrice.AppendBar( bar );
  m_ceVolume.Append( bar );

}

void Underlying::UpdateChart( boost::posix_time::ptime dt ) {

  assert( m_pPortfolio );

  double dblPLUnRealized {};
  double dblPLRealized {};
  double dblCommissionPaid {};
  double dblPLCurrent {};

  m_pPortfolio->QueryStats( dblPLUnRealized, dblPLRealized, dblCommissionPaid, dblPLCurrent );
  //double dblCurrent = dblUnRealized + dblRealized - dblCommissionsPaid;

  m_cePLCurrent.Append( dt, dblPLCurrent );
  m_cePLUnRealized.Append( dt, dblPLUnRealized );
  m_cePLRealized.Append( dt, dblPLRealized );
  m_ceCommissionPaid.Append( dt, dblCommissionPaid );

}
