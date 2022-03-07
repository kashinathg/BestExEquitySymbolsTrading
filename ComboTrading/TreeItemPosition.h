#pragma once

#include "TreeItem.h"

class TreeItemPosition: public TreeItemResources {
  friend class boost::serialization::access;
public:
  TreeItemPosition( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    TreeItemResources( id, baseResources, resources ) {}
  virtual ~TreeItemPosition( void ) {}

  //virtual void ShowContextMenu( void );

protected:
private:
//  enum {
//    ID_Null = wxID_HIGHEST,
//    MIAddGroup
//  };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemResources>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemResources>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
