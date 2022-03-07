

#pragma once

#include <OUCommon/FastDelegate.h>

#include <TFTrading/Position.h>

class PositionState {
public:

  typedef ou::tf::Position::pPosition_t pPosition_t;

  PositionState( void );
  PositionState( size_t ix, pPosition_t pPosition );
  PositionState( const PositionState& rhs );
  virtual ~PositionState(void);

  typedef fastdelegate::FastDelegate1<const PositionState&> OnPositionClosed_t;
  OnPositionClosed_t OnPositionClosed;

  void Buy( uint32_t quan );
  void Sell( uint32_t quan );

  void ExitLong( void );
  void ExitShort( void );

  pPosition_t Position( void ) { return m_pPosition; }
  size_t Index( void ) const { return m_ix; }

protected:
  pPosition_t m_pPosition;
private:

  size_t m_ix;  // index to use during callbacks

  double m_dblStop;
  double m_dblTarget;

  void HandlePositionChanged( const ou::tf::Position& position );

};
