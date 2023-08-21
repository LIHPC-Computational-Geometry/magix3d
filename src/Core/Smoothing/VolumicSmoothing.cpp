/*----------------------------------------------------------------------------*/
/*
 * \file VolumicSmoothing.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 4/3/2014
 */
/*----------------------------------------------------------------------------*/
#include "Smoothing/VolumicSmoothing.h"
#include "Mesh/MeshManager.h"
#include "Utils/Common.h"
#include "Geom/Volume.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/Node.h>
#include <GMDS/IG/Region.h>
/*----------------------------------------------------------------------------*/
//#include <MachineTypes/machine_types.h>
#include <cstdint>

#ifdef USE_SMOOTH3D
#include <smooth3D/smooth3d.h>
#endif	// USE_SMOOTH3D
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
VolumicSmoothing::
VolumicSmoothing()
 : MeshModificationItf()
, m_useDefaults(true)
, m_nbIterations(10)
, m_methodeLissage(laplace)
{
}
/*----------------------------------------------------------------------------*/
VolumicSmoothing::
~VolumicSmoothing()
{
}
/*----------------------------------------------------------------------------*/
VolumicSmoothing::VolumicSmoothing(const VolumicSmoothing& vs)
:MeshModificationItf()
, m_useDefaults(vs.m_useDefaults)
, m_nbIterations(vs.m_nbIterations)
, m_methodeLissage(vs.m_methodeLissage)
{
}
VolumicSmoothing::VolumicSmoothing(int nbIterations, eVolumicMethod methodeLissage)
:MeshModificationItf()
, m_useDefaults(false)
, m_nbIterations(nbIterations)
, m_methodeLissage(methodeLissage)
{
}
/*----------------------------------------------------------------------------*/
void VolumicSmoothing::setNbIterations(int nbIterations)
{
	m_useDefaults = false;
	m_nbIterations = nbIterations;
}
/*----------------------------------------------------------------------------*/
void VolumicSmoothing::setMethod(eVolumicMethod methodeLissage)
{
	m_useDefaults = false;
	m_methodeLissage = methodeLissage;
}
/*----------------------------------------------------------------------------*/
VolumicSmoothing& VolumicSmoothing::operator = (const VolumicSmoothing&)
{
	MGX_FORBIDDEN("VolumicSmoothing::operator = is not allowed.");
	return *this;
}
/*----------------------------------------------------------------------------*/
VolumicSmoothing* VolumicSmoothing::
clone() const
{
	return new VolumicSmoothing(*this);
}
/*----------------------------------------------------------------------------*/
void VolumicSmoothing::
addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const
{
	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Lissage", std::string("Volumique")));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Nombre itérations", (long)getNbIterations()));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Méthode", toString(getMethod())));
}
/*----------------------------------------------------------------------------*/
void VolumicSmoothing::
applyModification(std::vector<gmds::Node>& gmdsNodes,
			std::vector<gmds::Region>& gmdsPolyedres,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			uint maskFixed,
			Geom::Volume* volume)
{
	uint nb_cells = gmdsPolyedres.size();
	uint nb_vtx = gmdsNodes.size();

#ifdef _DEBUG2
	std::cout<<"VolumicSmoothing::applyModification pour "<<nb_vtx<<" noeuds "
    		 << " et "<<nb_cells<<" polyèdres, dans le volume "
 			 << volume->getName()<<std::endl;
#endif

	  double * weights = new double [nb_cells + nb_vtx];
	  double * relax = new double [nb_vtx];

	  for (uint i=0; i<nb_cells + nb_vtx; i++)
		  weights[i] = 1.0;

	  for (uint i=0; i<nb_vtx; i++)
		  if (filtre_nodes[gmdsNodes[i].getID()] == maskFixed)
			  relax[i] = 0.0;
		  else
			  relax[i] = 1.0;

	  // indirection des gmds::Node vers id dans structure Mesquite
	  std::map<gmds::TCellID, uint> gmds2mesquite;
	  for (uint i=0; i<nb_vtx; i++)
		  gmds2mesquite[gmdsNodes[i].getID()] = i;

	  // nombre de noeuds par maille
	  //int_type *nb_node_cell = new int_type[nb_cells];
          int64_t *nb_node_cell = new int64_t[nb_cells];
	  uint nb_tt_node_cell = 0;
	  for (uint i=0; i<nb_cells; i++){
		  nb_node_cell[i] = gmdsPolyedres[i].getNbNodes();
		  nb_tt_node_cell += nb_node_cell[i];
	  }

	  //int_type * nodes_number = new int_type[nb_tt_node_cell];
	  //int_type * f_nodes_number = nodes_number;
	  int64_t * nodes_number = new int64_t[nb_tt_node_cell];
	  int64_t * f_nodes_number = nodes_number;
	  for (int cell = 0; cell < nb_cells; ++cell){
		  std::vector<gmds::TCellID> nodesIDs = gmdsPolyedres[cell].getAllIDs<gmds::Node>();
		  for (int vtx = 0; vtx < nodesIDs.size(); ++ vtx)
			  *f_nodes_number++ = gmds2mesquite[nodesIDs[vtx]];
	  }

	  double *x = new double [nb_vtx];
	  double *y = new double [nb_vtx];
	  double *z = new double [nb_vtx];
	  for (int vtx = 0; vtx < nb_vtx; ++ vtx){
		  x[vtx] = gmdsNodes[vtx].X();
		  y[vtx] = gmdsNodes[vtx].Y();
		  z[vtx] = gmdsNodes[vtx].Z();
	  }

	  int err = 0;
	  switch (m_methodeLissage){
#ifdef USE_SMOOTH3D
	  case laplace:

		  err = S3_laplace(nb_cells, nb_vtx,
				  nb_node_cell, nodes_number,
				  x, y, z,
				  weights, relax, m_nbIterations);
		  break;
	  case tipton:
		  err = S3_Tipton(nb_cells, nb_vtx,
				  nb_node_cell, nodes_number,
				  x, y, z,
				  weights, relax, m_nbIterations);
		  break;
	  case jun:
		  err = S3_Jun(nb_cells, nb_vtx,
				  nb_node_cell, nodes_number,
				  x, y, z,
				  weights, relax, m_nbIterations);
		  break;
	  case conditionNumber:
		  err = S3_ConditionNumber(nb_cells, nb_vtx,
				  nb_node_cell, nodes_number,
				  x, y, z,
				  weights, relax, m_nbIterations);
		  break;
	  case inverseMeanRatio:
		  err = S3_InverseMeanRatio(nb_cells, nb_vtx,
				  nb_node_cell, nodes_number,
				  x, y, z,
				  weights, relax, m_nbIterations);
		  break;
#endif	// USE_SMOOTH3D
	  default:
		  throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, VolumicSmoothing appelé avec une méthode non prévue", TkUtil::Charset::UTF_8));
	  }

	  delete [] weights;
	  delete [] nb_node_cell;
	  delete [] nodes_number;

#ifdef _DEBUG2
	  std::cout<<" => err = "<<err<<std::endl;
#endif

	  // on ne fait pas confiance au lisseur, capable de déplacer n'importe où un noeud figé
	  for (int vtx = 0; vtx < nb_vtx; ++ vtx)
		  if (relax[vtx] == 1.0){
			  gmdsNodes[vtx].setX(x[vtx]);
			  gmdsNodes[vtx].setY(y[vtx]);
			  gmdsNodes[vtx].setZ(z[vtx]);
		  }

	  delete [] x;
	  delete [] y;
	  delete [] z;
	  delete [] relax;


}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String VolumicSmoothing::getScriptCommand() const
{
	TkUtil::UTF8String	o (TkUtil::Charset::UTF_8);
	if (m_useDefaults)
		o << getMgx3DAlias() << ".VolumicSmoothing()";
	else {
			o << getMgx3DAlias() << ".VolumicSmoothing(";
			o << (short)getNbIterations();
			o <<", "<<getMgx3DAlias()<<".VolumicSmoothing." <<toString(getMethod());
			o << ")";
		}
	return o;
}
/*----------------------------------------------------------------------------*/
std::string VolumicSmoothing::toString(eVolumicMethod method)
{
	if (method == laplace)
		return "laplace";
	else if (method == tipton)
		return "tipton";
	else if (method == jun)
		return "jun";
	else if (method == conditionNumber)
		return "conditionNumber";
	else if (method == inverseMeanRatio)
		return "inverseMeanRatio";
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, une méthode de lissage volumique n'est pas encore prévue pour toString", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
