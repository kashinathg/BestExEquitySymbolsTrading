

#pragma once

#include <TFOptions/Strike.h>

class EventUpdateOptionTree: public wxEvent {
public:

  EventUpdateOptionTree( wxEventType eventType, int winid, ou::tf::option::Strike&, bool );
  ~EventUpdateOptionTree(void);

  ou::tf::option::Strike& GetStrike( void ) { return m_strike; }
  bool GetWatch( void ) const { return m_bOn; }

  virtual EventUpdateOptionTree* Clone( void ) const { return new EventUpdateOptionTree( *this ); }

protected:
private:

  ou::tf::option::Strike& m_strike;
  bool m_bOn;  // true = watch on, false = watch off

};

wxDECLARE_EVENT(EVENT_UPDATE_OPTION_TREE, EventUpdateOptionTree);
