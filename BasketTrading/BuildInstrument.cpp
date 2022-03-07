/*
 * File:    BuildInstrument.cpp
 * Project: BestExTrading
 * Created on Feb 24, 2022, 21:52
 */

#include <TFTrading/Watch.h>
#include <TFTrading/InstrumentManager.h>

#include <TFIQFeed/BuildInstrument.h>

#include "BuildInstrument.h"

using pWatch_t = ou::tf::Watch::pWatch_t;

BuildInstrument::BuildInstrument( pProviderIQFeed_t pIQFeed, pProviderIBTWS_t pIB, fGetTableRowDef_t&& fGetTableRowDef )
: m_pIQ( std::move( pIQFeed ) ), m_pIB( std::move( pIB ) ),
  m_fGetTableRowDef( std::move( fGetTableRowDef ) )
{
  assert( m_pIQ );
  assert( m_pIB );
  assert( m_fGetTableRowDef );
}

void BuildInstrument::Add( const std::string& sIQFeedSymbol, fInstrument_t&& fInstrument ) {

  pInstrument_t pInstrument;

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );

  pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderIQF, sIQFeedSymbol );
  if ( pInstrument ) { // skip the build
    //std::cout << "BuildInstrument::Build existing: " << pInstrument->GetInstrumentName() << std::endl;
    fInstrument( pInstrument );
  }
  else { // bulid a new instrument

    {
      std::lock_guard<std::mutex> lock( m_mutexMap );
      m_mapSymbol.emplace( std::make_pair( sIQFeedSymbol, std::move( fInstrument ) ) );
    }

    Update();
  }

}

void BuildInstrument::Update() {

  bool bDoBuild( false );
  mapInProgress_t::iterator iterInProgress;

  {
    std::lock_guard<std::mutex> lock( m_mutexMap );

    if ( 5 > m_mapInProgress.size() ) { // 5 is set in m_pIB
      if ( 0 != m_mapSymbol.size() ) {

        mapSymbol_t::iterator iterSymbol = m_mapSymbol.begin();

        {
          auto& [ sIQFeedSymbol_, fInstrument_ ] = *iterSymbol;
          const std::string sIQFeedSymbol( sIQFeedSymbol_ );

          auto result = m_mapInProgress.emplace( std::make_pair( sIQFeedSymbol, InProgress( std::move( fInstrument_ ) ) ) );
          assert( result.second );
          iterInProgress = result.first;
          bDoBuild = true;
        }

        m_mapSymbol.erase( iterSymbol );

        //std::cout << "BuildInstrument::Update " << sSymbol << std::endl;
      }
    }
  }

  if ( bDoBuild ) Build( iterInProgress );

}

void BuildInstrument::Build( mapInProgress_t::iterator iterInProgress ) {

  auto& [ sIQFeedSymbol, ip ] = *iterInProgress;

  pInstrument_t pInstrument;

  // TODO: need to check that trd is a long lasting structure
  //const trd_t& trd( m_fGetTableRowDef( sIQFeedSymbol ) ); // TODO: check for errors

  // temporary instrument solely for obtaining fundamental data with which to build real instrument
  //pInstrument = ou::tf::iqfeed::BuildInstrument( "Acquire-" + sIQFeedSymbol, trd );
  pInstrument = std::make_shared<ou::tf::Instrument>( "Acquire-" + sIQFeedSymbol ); // just enough to obtain fundamentals
  pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sIQFeedSymbol );
  pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQ );

  AcquireFundamentals::pAcquireFundamentals_t pAcquireFundamentals
    = std::make_shared<AcquireFundamentals>(
        std::move( pWatch ),
        [this,iterInProgress]( pWatch_t pWatchOld ) { // async call once fundamentals arrive

          const ou::tf::Watch::Fundamentals& fundamentals( pWatchOld->GetFundamentals() );
          pInstrument_t pInstrument
            = ou::tf::iqfeed::BuildInstrument( fundamentals );
          pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, pWatchOld->GetProvider() );

          std::cout
            << "BuildInstrument: "
            << m_mapSymbol.size() << ","
            << m_mapInProgress.size() << ","
            << fundamentals.sExchangeRoot << ","
            << iterInProgress->first << ","
            << pInstrument->GetInstrumentName()
            << std::endl;

          m_pIB->RequestContractDetails(
            fundamentals.sExchangeRoot,  // needs to be the IB base name
            pInstrument,  // this is a filled-in, prepared instrument
            [this,pWatch,iterInProgress]( const ou::tf::ib::TWS::ContractDetails& details, pInstrument_t& pInstrument ){
              //std::cout << "BuildInstrument::Build contract: " << pInstrument->GetInstrumentName() << std::endl;
              assert( 0 != pInstrument->GetContract() );
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
              im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
              iterInProgress->second.fInstrument( pInstrument );
            },
            [this,iterInProgress](){
              // TODO: how to test for incomplete done?
              //std::cout << "BuildInstrument::Build done: " << iterInProgress->first << std::endl;
              {
                std::lock_guard<std::mutex> lock( m_mutexMap );
                m_mapInProgress.erase( iterInProgress );
              }
              Update();
            }
            );
          //std::cout << "BuildInstrument::Build begin: " << iterInProgress->first << std::endl;
        }
      );

  iterInProgress->second.pAcquireFundamentals = pAcquireFundamentals;
  pAcquireFundamentals->Start();
}

void BuildInstrument::Clear() {
  std::lock_guard<std::mutex> lock( m_mutexMap );
  assert( 0 == m_mapInProgress.size() );
  m_mapSymbol.clear();
}
