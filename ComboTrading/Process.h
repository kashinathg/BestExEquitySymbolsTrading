#pragma once

#include <TFBitsNPieces/ReadCboeWeeklyOptionsCsv.h>

class Process {
public:
  Process();
  virtual ~Process();

  void LoadWeeklies( void );  // later, will need to pass in repository
protected:
private:

  ou::tf::cboe::csv::WeeklyOptions m_weekly;

};
