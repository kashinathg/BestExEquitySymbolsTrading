#ifndef SENTIMENT_H
#define SENTIMENT_H

#include <TFTimeSeries/DatedDatum.h>

class Sentiment {
public:
  Sentiment();
  void Update( const ou::tf::Bar& bar );
  void Get( size_t& nUp_, size_t& nDown_ ) const;
protected:
private:

  size_t nUp;
  size_t nDown;
  ptime dtCurrent; // late arrivals don't count

  void Reset( ptime dtNew );
};

#endif /* SENTIMENT_H */
