#include "stdafx.h"

#include "EventProviderConnected.h"

wxDEFINE_EVENT(EVENT_PROVIDER_CONNECTED, EventProviderConnected);

EventProviderConnected::EventProviderConnected( wxEventType eventType, int winid, pProvider_t pProvider )
  : wxEvent( winid, eventType ), m_pProvider( pProvider )
{
}

EventProviderConnected::~EventProviderConnected(void) {
}
