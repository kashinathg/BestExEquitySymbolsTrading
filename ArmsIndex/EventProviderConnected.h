#pragma once

#include <TFTrading/ProviderInterface.h>

class EventProviderConnected: public wxEvent {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  EventProviderConnected( wxEventType eventType, int winid, pProvider_t pProvider );
  ~EventProviderConnected(void);

  pProvider_t& GetProvider( void ) { return m_pProvider; };

  virtual EventProviderConnected* Clone( void ) const { return new EventProviderConnected( *this ); }

protected:
private:
  pProvider_t m_pProvider;
};

wxDECLARE_EVENT(EVENT_PROVIDER_CONNECTED, EventProviderConnected);
