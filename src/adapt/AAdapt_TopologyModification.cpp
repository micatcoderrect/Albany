//*****************************************************************//
//    Albany 3.0:  Copyright 2016 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

#include <Teuchos_TimeMonitor.hpp>

#include <stk_util/parallel/ParallelReduce.hpp>

#include "AAdapt_TopologyModification.hpp"
#include "LCM/utils/topology/Topology_FractureCriterion.h"

namespace AAdapt {

//
//
//
TopologyMod::
TopologyMod(Teuchos::RCP<Teuchos::ParameterList> const & params,
            Teuchos::RCP<ParamLib>               const & param_lib,
            Albany::StateManager                 const & state_mgr,
            Teuchos::RCP<Teuchos_Comm const>     const & comm)
 : AbstractAdapter (params, param_lib, state_mgr, comm)
 , remesh_file_index_(1)
{
  discretization_ = state_mgr_.getDiscretization();

  stk_discretization_ =
    static_cast<Albany::STKDiscretization*>(discretization_.get());

  stk_mesh_struct_ = stk_discretization_->getSTKMeshStruct();

  bulk_data_ = stk_mesh_struct_->bulkData;
  meta_data_ = stk_mesh_struct_->metaData;

  num_dim_ = stk_mesh_struct_->numDim;

  // Save the initial output file name
  base_exo_filename_ = stk_mesh_struct_->exoOutFile;

  std::string const
  bulk_block_name = params->get<std::string>("Bulk Block Name");

  std::string const
  interface_block_name = params->get<std::string>("Interface Block Name");

  std::string const
  stress_name = "nodal_Cauchy_Stress";

  double const
  critical_traction = params->get<double>("Critical Traction");

  double const
  beta = params->get<double>("beta");

  topology_ =
    Teuchos::rcp(new LCM::Topology(
        discretization_,
        bulk_block_name,
        interface_block_name));

  fracture_criterion_ =
    Teuchos::rcp(
        new LCM::FractureCriterionTraction(
            *topology_,
            stress_name,
            critical_traction,
            beta));

  topology_->set_fracture_criterion(fracture_criterion_);
}

bool TopologyMod::queryAdaptationCriteria(int) {
  size_t const number_fractured_faces = topology_->setEntitiesOpen();

  return number_fractured_faces > 0;
}

bool TopologyMod::adaptMesh() {

  *output_stream_
      << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
      << "Adapting mesh using TopologyMod method      \n"
      << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

  // Save the current results and close the exodus file

  // Create a remeshed output file naming convention by
  // adding the remesh_file_index_ ahead of the period
  std::ostringstream ss;
  std::string str = base_exo_filename_;
  ss << "_" << remesh_file_index_ << ".";
  str.replace(str.find('.'), 1, ss.str());

  *output_stream_ << "Remeshing: renaming output file to - " << str << '\n';

  // Open the new exodus file for results
  stk_discretization_->reNameExodusOutput(str);

  remesh_file_index_++;

  // Print element connectivity before the mesh topology is modified

  //  *output_stream_
  //   << "*************************\n"
  //   << "Before element separation\n"
  //   << "*************************\n";

  // Start the mesh update process

  topology_->splitOpenFaces();

  // Throw away all the Albany data structures and re-build them from the mesh

  stk_discretization_->updateMesh();

  return true;
}

Teuchos::RCP<Teuchos::ParameterList const>
TopologyMod::getValidAdapterParameters() const {

  Teuchos::RCP<Teuchos::ParameterList>
  valid_pl_ = this->getGenericAdapterParams("ValidTopologyModificationParams");

  valid_pl_->set<double>(
    "Critical Traction",
    1.0,
    "Critical traction at which two elements separate t_eff >= t_cr");

  valid_pl_->set<double>(
    "beta",
    1.0,
    "Weight factor t_eff = sqrt[(t_s/beta)^2 + t_n^2]");

  return valid_pl_;
}

} // namespace AAdapt
