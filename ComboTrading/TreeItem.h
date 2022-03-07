#pragma once

#include <boost/signals2.hpp>

#include <TFBitsNPieces/TreeOpsItems.h>

#include "InstrumentActions.h"

struct Resources {

  typedef InstrumentActions::pInstrumentActions_t pInstrumentActions_t;

  typedef boost::signals2::signal<pInstrumentActions_t (const wxTreeItemId&),
                                     ou::tf::FirstOrDefault<pInstrumentActions_t> > signalGetInstrumentActions_t;
  typedef signalGetInstrumentActions_t::slot_type slotInstrumentActions_t;
  signalGetInstrumentActions_t signalGetInstrumentActions;
};

// ================

class TreeItemResources: public ou::tf::TreeItemBase {
  friend class boost::serialization::access;
public:
  TreeItemResources( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    ou::tf::TreeItemBase( id, baseResources ), m_resources( resources ) {}
  virtual ~TreeItemResources( void ) {};
protected:
  Resources& m_resources;
private:

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const ou::tf::TreeItemBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<ou::tf::TreeItemBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
