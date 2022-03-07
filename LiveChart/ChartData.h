#pragma once



#include <OUCharting/ChartDVBasics.h>

#include <TFTrading/ProviderManager.h>
#include <TFTrading/Watch.h>

class ChartData: public ou::ChartDVBasics {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  ChartData( pProvider_t );
  ~ChartData(void);

  ou::tf::Watch* GetWatch( void ) { return m_pWatch; };

protected:
private:

  ou::tf::Watch* m_pWatch;

};
