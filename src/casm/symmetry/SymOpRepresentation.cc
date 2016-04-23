#include "casm/symmetry/SymOpRepresentation.hh"
#include "casm/symmetry/SymGroupRep.hh"
#include "casm/symmetry/SymMatrixXd.hh"
#include "casm/symmetry/SymPermutation.hh"

namespace CASM {

  //*******************************************************************************************
  Eigen::MatrixXd const *SymOpRepresentation::get_matrix_rep(SymGroupRepID _rep_ID) const {
    assert(has_valid_master() && !_rep_ID.empty());
    return (master_group().representation(_rep_ID)[index()])->get_MatrixXd();
  }

  //*******************************************************************************************

  SymBasisPermute const *SymOpRepresentation::get_basis_permute_rep(SymGroupRepID _rep_ID) const {
    assert(has_valid_master() && !_rep_ID.empty());
    return (master_group().representation(_rep_ID)[index()])->get_ucc_permutation();
  }
  //*******************************************************************************************

  Permutation const *SymOpRepresentation::get_permutation_rep(SymGroupRepID _rep_ID) const {
    assert(has_valid_master() && !_rep_ID.empty());
    return (master_group().representation(_rep_ID)[index()])->get_permutation();
  }

  //*******************************************************************************************

  Array<Eigen::MatrixXd const * > SymOpRepresentation::get_matrix_reps(Array<SymGroupRepID> _rep_IDs) const {
    Array<Eigen::MatrixXd const * > tmat;
    for(Index i = 0; i < _rep_IDs.size(); i++) {
      tmat.push_back(get_matrix_rep(_rep_IDs[i]));

    }
    return tmat;
  }

  //**********************************************************
  void SymOpRepresentation::set_rep(SymGroupRepID _rep_ID, const SymOpRepresentation &op_rep) const {
    assert(has_valid_master() && !_rep_ID.empty());
    return master_group().representation(_rep_ID).set_rep(index(), op_rep);
  }

  //*******************************************************************************************

  void SymOpRepresentation::set_identifiers(const MasterSymGroup &new_group, SymGroupRepID new_rep_ID) {
    m_master_group = &new_group;
    m_rep_ID = new_rep_ID;
    SymGroupRep const &trep(new_group.representation(m_rep_ID));
    Index i;
    for(i = 0; i < trep.size(); i++) {
      if(this == trep[i]) {
        m_op_index = i;
        break;
      }
    }

    if(i == new_group.size())
      m_op_index = -1;

    return;
  }

  //*******************************************************************************************

  void SymOpRepresentation::set_identifiers(const MasterSymGroup &new_group, SymGroupRepID new_rep_ID, Index new_op_index) {
    m_master_group = &new_group;
    m_rep_ID = new_rep_ID;
    m_op_index = new_op_index;

    return;
  }

  //*******************************************************************************************

  Index SymOpRepresentation::ind_inverse()const {
    assert(has_valid_master() && "In SymOpRepresentation::ind_inverse(), head_group is uninitialized!!");
    return master_group().ind_inverse(index());
  }

  //*******************************************************************************************

  Index SymOpRepresentation::ind_prod(const SymOpRepresentation &RHS)const {
    assert(has_valid_master() && "In SymOpRepresentation::ind_prod(), head_group is uninitialized!!");
    return master_group().ind_prod(index(), RHS.index());
  }

  //*******************************************************************************************


  /// creates jsonParser using polymorphism
  jsonParser &to_json(const SymOpRepresentation *rep, jsonParser &json) {
    return rep->to_json(json);
  }

  //*******************************************************************************************

  /// This allocates a new object to 'rep'.
  void from_json(SymOpRepresentation *rep, const jsonParser &json) {
    try {
      if(json["SymOpRep_type"] == "SymPermutation") {

        // prepare a SymPermutation and then read from json
        Array<Index> perm;
        SymPermutation trep(perm);
        CASM::from_json(trep, json);

        // copy to rep
        rep = new SymPermutation(trep);

      }
      else if(json["SymOpRep_type"] == "SymMatrixXd") {

        // prepare a SymMatrixXd and then read from json
        SymMatrixXd  *op_ptr = new SymMatrixXd(Eigen::MatrixXd::Identity(1, 1));

        CASM::from_json(*op_ptr, json);

        // copy to rep
        rep = op_ptr;

      }
      else if(json["SymOpRep_type"] == "SymOp") {
        // prepare a SymOp and then read from json
        SymOp *op_ptr = new SymOp();

        CASM::from_json(*op_ptr, json);

        // copy to rep
        rep = op_ptr;

      }
      else {
        std::cout << "Error in 'void from_json(SymOpRepresentation *rep, const jsonParser &json)'" << std::endl;
        std::cout << "Unrecognized 'SymOpRep_type': '" << json["SymOpRep_type"] << "'." << std::endl;
        std::cout << "Options are: 'SymPermutation', 'SymMatrixXd', or 'SymOp'." << std::endl;
        exit(1);
      }
    }
    catch(...) {
      /// re-throw exceptions
      throw;
    }
  }
  //*******************************************************************************************

  //enum symmetry_type {identity_op, mirror_op, glide_op, rotation_op, screw_op, inversion_op, rotoinversion_op, invalid_op};

  jsonParser &to_json(const SymOpRepresentation::symmetry_type &stype, jsonParser &json) {

    if(stype == SymOpRepresentation::identity_op) {
      json = "identity";
    }
    else if(stype == SymOpRepresentation::mirror_op) {
      json = "mirror";
    }
    else if(stype == SymOpRepresentation::glide_op) {
      json = "glide";
    }
    else if(stype == SymOpRepresentation::rotation_op) {
      json = "rotation";
    }
    else if(stype == SymOpRepresentation::screw_op) {
      json = "screw";
    }
    else if(stype == SymOpRepresentation::inversion_op) {
      json = "inversion";
    }
    else if(stype == SymOpRepresentation::rotoinversion_op) {
      json = "rotoinversion";
    }
    else if(stype == SymOpRepresentation::invalid_op) {
      json = "invalid";
    }
    return json;
  }

  //*******************************************************************************************

  void from_json(SymOpRepresentation::symmetry_type &stype, const jsonParser &json) {
    try {
      std::cout << "Reading in type" << std::endl;
      std::string type = json.get<std::string>();
      if(type == "identity")
        stype = SymOpRepresentation::identity_op;
      else if(type == "mirror")
        stype = SymOpRepresentation::mirror_op;
      else if(type == "glide")
        stype = SymOpRepresentation::glide_op;
      else if(type == "rotation")
        stype = SymOpRepresentation::rotation_op;
      else if(type == "screw")
        stype = SymOpRepresentation::screw_op;
      else if(type == "inversion")
        stype = SymOpRepresentation::inversion_op;
      else if(type == "rotoinversion")
        stype = SymOpRepresentation::rotoinversion_op;
      else if(type == "invalid")
        stype = SymOpRepresentation::invalid_op;
      else {
        std::cout << "Error in 'void from_json(SymOpRepresentation::symmetry_type &stype, const jsonParser &json)'" << std::endl;
        std::cout << "Unrecognized SymOpRepresentation::symmetry_type: '" << type << "'" << std::endl;
        exit(1);
      }
    }
    catch(...) {
      /// re-throw exceptions
      throw;
    }
  }
}
