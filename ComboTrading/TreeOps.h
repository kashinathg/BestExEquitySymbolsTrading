#pragma once

#include <TFBitsNPieces/TreeOps.h>

#include "TreeItem.h"

class TreeOps: public ou::tf::TreeOps {
public:
  TreeOps();
  TreeOps(
          wxWindow* parent,
          wxWindowID id = SYMBOL_TREEOPS_IDNAME,
          const wxPoint& pos = SYMBOL_TREEOPS_POSITION,
          const wxSize& size = SYMBOL_TREEOPS_SIZE,
          long style = SYMBOL_TREEOPS_STYLE );

  bool Create(
    wxWindow* parent,
          wxWindowID id = SYMBOL_TREEOPS_IDNAME,
          const wxPoint& pos = SYMBOL_TREEOPS_POSITION,
          const wxSize& size = SYMBOL_TREEOPS_SIZE,
          long style = SYMBOL_TREEOPS_STYLE );
  virtual ~TreeOps();

  void PopulateResources( Resources& resources );

  template<typename RootItemType>
  void Save( boost::archive::text_oarchive& oa) {
    ou::tf::TreeOps::Save<RootItemType>( oa );
  }

  template<typename RootItemType>
  void Load( boost::archive::text_iarchive& ia) {
    ou::tf::TreeOps::Load<RootItemType>( ia );
  }



protected:
private:

};
