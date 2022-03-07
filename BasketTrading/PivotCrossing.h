#ifndef PIVOTCROSSING_H
#define PIVOTCROSSING_H

class PivotCrossing {
public:

  enum class EState { unknown, gtR2, R1R2, PVR1, PVS1, S1S2, ltS2, _size };
  enum class ECrossing { none, R2up, R2dn, R1up, R1dn, PVup, PVdn, S1up, S1dn, S2up, S2dn };

  PivotCrossing();
  PivotCrossing( double R2, double R1, double PV, double S1, double S2 );
  virtual ~PivotCrossing( );

  void Set( double R2, double R1, double PV, double S1, double S2 );

  ECrossing Update( const double price );

  EState State() const { return m_state; }
  ECrossing Crossing() const { return m_crossing; }
protected:
private:

  EState m_state;
  ECrossing m_crossing;

  double m_R2, m_R1, m_PV, m_S1, m_S2;

  void Init();

};

#endif /* PIVOTCROSSING_H */
