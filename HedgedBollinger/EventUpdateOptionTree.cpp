
#include "stdafx.h"

#include "EventUpdateOptionTree.h"

wxDEFINE_EVENT(EVENT_UPDATE_OPTION_TREE, EventUpdateOptionTree);

EventUpdateOptionTree::EventUpdateOptionTree( wxEventType eventType, int winid, ou::tf::option::Strike& strike, bool bOn )
  : wxEvent( winid, eventType), m_strike( strike ), m_bOn( bOn )
{
}

EventUpdateOptionTree::~EventUpdateOptionTree(void) {
}
