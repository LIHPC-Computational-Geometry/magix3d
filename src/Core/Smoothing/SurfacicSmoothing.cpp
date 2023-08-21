/*----------------------------------------------------------------------------*/
/*
 * \file SurfacicSmoothing.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 24/2/2014
 */
/*----------------------------------------------------------------------------*/
#include "Smoothing/SurfacicSmoothing.h"
#include "Smoothing/MesquiteDomainAdapter.h"
#include "Smoothing/MesquiteMeshAdapter.h"
#include "Smoothing/OrthogonalQualityMetric.h"
#include "Smoothing/NormaleQualityMetric.h"
#include "Smoothing/OrthogonalSmoothing.h"
#include "Smoothing/DistanceMdlQualityMetric.h"

#include "Mesh/MeshManager.h"
#include "Utils/SerializedRepresentation.h"
#include "Utils/Common.h"
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/Node.h>
/*----------------------------------------------------------------------------*/
//Mesquite
#include "InstructionQueue.hpp"
#include "LPtoPTemplate.hpp"
#include "ConjugateGradient.hpp"
#include "FeasibleNewton.hpp"
#include "PatchData.hpp"

#include "QualityAssessor.hpp"
#include "TerminationCriterion.hpp"


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
SurfacicSmoothing::
SurfacicSmoothing()
 : MeshModificationItf()
, m_useDefaults(true)
, m_nbIterations(10)
, m_methodeLissage(surfacicOrthogonalSmoothingElliptic)
, m_solver(gradientConjugue)
{
}
/*----------------------------------------------------------------------------*/
SurfacicSmoothing::
SurfacicSmoothing(int nbIterations, eSurfacicMethod methodeLissage, eSolver solver)
 : MeshModificationItf()
, m_useDefaults(false)
, m_nbIterations(nbIterations)
, m_methodeLissage(methodeLissage)
, m_solver(solver)
{
}
/*----------------------------------------------------------------------------*/
SurfacicSmoothing::
~SurfacicSmoothing()
{
}
/*----------------------------------------------------------------------------*/
SurfacicSmoothing::SurfacicSmoothing(const SurfacicSmoothing& lissage)
:MeshModificationItf()
, m_useDefaults(lissage.m_useDefaults)
, m_nbIterations(lissage.m_nbIterations)
, m_methodeLissage(lissage.m_methodeLissage)
, m_solver(lissage.m_solver)
{

}
/*----------------------------------------------------------------------------*/
void SurfacicSmoothing::setNbIterations(int nbIterations)
{
	m_useDefaults = false;
	m_nbIterations = nbIterations;
}
/*----------------------------------------------------------------------------*/
void SurfacicSmoothing::setMethod(eSurfacicMethod methodeLissage)
{
	m_useDefaults = false;
	m_methodeLissage = methodeLissage;
}
/*----------------------------------------------------------------------------*/
void SurfacicSmoothing::setSolver(eSolver solver)
{
	m_useDefaults = false;
	m_solver = solver;
}
/*----------------------------------------------------------------------------*/
SurfacicSmoothing& SurfacicSmoothing::operator = (const SurfacicSmoothing&)
{
	MGX_FORBIDDEN("SurfacicSmoothing::operator = is not allowed.");
	return *this;
}
/*----------------------------------------------------------------------------*/
SurfacicSmoothing* SurfacicSmoothing::
clone() const
{
	return new SurfacicSmoothing(*this);
}
/*----------------------------------------------------------------------------*/
void SurfacicSmoothing::
addToDescription (Mgx3D::Utils::SerializedRepresentation* description) const
{
	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Lissage", std::string("Surfacique")));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Nombre itérations", (long)getNbIterations()));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Méthode", toString(getMethod())));

	description->addProperty (
			Utils::SerializedRepresentation::Property (
					"Solver", toString(getSolver())));
}
/*----------------------------------------------------------------------------*/
void SurfacicSmoothing::
applyModification(std::vector<gmds::Node >& gmdsNodes,
			std::vector<gmds::Face>& gmdsPolygones,
			std::map<gmds::TCellID, uint>& filtre_nodes,
			std::map<gmds::TCellID, bool>& isPolyInverted,
			uint maskFixed,
			Geom::Surface* surface)
{

	// définition du modèle pour Mesquite
	MesquiteDomainAdapter* mdl_geom = new MesquiteDomainAdapter(surface);

	// création d'une structure de maillage pour Mesquite
	MesquiteMeshAdapter* mesh = new MesquiteMeshAdapter(gmdsPolygones, gmdsNodes,
			filtre_nodes, isPolyInverted, maskFixed);

	MeshDomainAssoc myAssoc (mesh, mdl_geom, false, false, true);

	if (m_nbIterations){

		if (m_methodeLissage == surfacicOrthogonalSmoothingElliptic){

			MESQUITE_NS::OrthogonalSmoothing algo(m_nbIterations);

			const Mesquite2::Settings dummySettings;
			MESQUITE_NS::MsqError err;

			algo.loop_over_mesh (&myAssoc, &dummySettings, err);
			MSQ_CHKERR(err);

		} else {

			MESQUITE_NS::QualityMetric* qual =0;
			switch (m_methodeLissage){
			case surfacicNormalSmoothing:    qual = new MESQUITE_NS::NormaleQualityMetric(mdl_geom); break;
			case surfacicOrthogonalSmoothing: qual = new MESQUITE_NS::OrthogonalQualityMetric; break;
			}

			if (qual == 0){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
				messErr <<"Erreur Interne, méthode de lissage non initialisée";
				throw TkUtil::Exception(messErr);
			}

			// creates an intruction queue
			MESQUITE_NS::InstructionQueue queue1;
			MESQUITE_NS::MsqError err;

			// ... and builds an objective function with it
			MESQUITE_NS::LPtoPTemplate obj_func (qual, 2, err);
			MSQ_CHKERR (err);

			// distance à la modélisation (juste pour l'affichage de la qualité en terme de distance)
			DistanceMdlQualityMetric *distqual = new DistanceMdlQualityMetric(mdl_geom);

			// Choix de la méthode d'optimisation
			MESQUITE_NS::VertexMover* pass1 = 0;
			if (m_solver == gradientConjugue) {

				MESQUITE_NS::ConjugateGradient* cg = new MESQUITE_NS::ConjugateGradient(&obj_func, err);
				cg->use_global_patch();
				pass1 = cg;
			}
			else if (m_solver == newton) {
				MESQUITE_NS::FeasibleNewton* fn = new MESQUITE_NS::FeasibleNewton (&obj_func);

				fn->use_global_patch();
				pass1 = fn;
			}

			if (pass1 == 0){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
				messErr <<"Erreur Interne, solver pour lissage non initialisé";
				throw TkUtil::Exception(messErr);
			}

			QualityAssessor qa = MESQUITE_NS::QualityAssessor (qual);
    		qa.add_quality_assessment(distqual);

			// Critères d'arret
    		MESQUITE_NS::TerminationCriterion tc_inner;
			tc_inner.add_iteration_limit (m_nbIterations);

			MESQUITE_NS::TerminationCriterion tc_outer;
			tc_outer.add_iteration_limit (1);

			// boucle interne
			pass1->set_inner_termination_criterion (&tc_inner);
			// boucle externe
			pass1->set_outer_termination_criterion (&tc_outer);

			// Fait une mesure de la qualité et l'affiche (initiale)
			queue1.add_quality_assessor (&qa, err);
			MSQ_CHKERR (err);
			// Optimisation <=> lissage
			queue1.set_master_quality_improver (pass1, err);
			MSQ_CHKERR (err);
			// Fait une mesure de la qualité et l'affiche (finale)
			queue1.add_quality_assessor (&qa, err);
			MSQ_CHKERR (err);

			// c'est maintenant que l'on fait ce qui est dans queue1
			MESQUITE_NS::MeshDomainAssoc myAssoc(mesh, mdl_geom);
			queue1.run_instructions(&myAssoc, err);
			MSQ_CHKERR (err);

			delete pass1;
			delete qual;
		} // end else if (m_methodeLissage == surfacicOrthogonalSmoothingElliptic)
	} // end if (m_nbIterations)

	else {
		// on fait juste une projection
		MESQUITE_NS::MsqError err;

		std::vector<MESQUITE_NS::Mesh::VertexHandle> handles;
		mesh->get_all_vertices(handles, err);
		MSQ_CHKERR (err);

		std::vector<MESQUITE_NS::MsqVertex> coords(handles.size());
		mesh->vertices_get_coordinates(&handles[0], &coords[0], handles.size(), err);
		MESQUITE_NS::Mesh::EntityHandle entity_handle = 0;
		for (uint i = 0; i < handles.size(); i++)
		{
			mdl_geom->snap_to (entity_handle, coords[i]);
			mesh->vertex_set_coordinates (handles[i], coords[i], err);
		}
		MSQ_CHKERR (err);

	}

	delete mesh;

	delete mdl_geom;
}
/*----------------------------------------------------------------------------*/
TkUtil::UTF8String SurfacicSmoothing::getScriptCommand() const
{
	TkUtil::UTF8String	o (TkUtil::Charset::UTF_8);
	if (m_useDefaults)
		o << getMgx3DAlias() << ".SurfacicSmoothing()";
	else {
		o << getMgx3DAlias() << ".SurfacicSmoothing(";
		o << (short)getNbIterations();
		o <<", "<<getMgx3DAlias()<<".SurfacicSmoothing." <<toString(getMethod());
		o <<", "<<getMgx3DAlias()<<".SurfacicSmoothing." <<toString(getSolver());
		o << ")";
	}
	return o;
}
/*----------------------------------------------------------------------------*/
std::string SurfacicSmoothing::toString(eSurfacicMethod method)
{
	if (method == surfacicNormalSmoothing)
		return "surfacicNormalSmoothing";
	else if (method == surfacicOrthogonalSmoothing)
		return "surfacicOrthogonalSmoothing";
	else if (method == surfacicOrthogonalSmoothingElliptic)
		return "surfacicOrthogonalSmoothingElliptic";
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, une méthode de lissage surfacique n'est pas encore prévue pour toString", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
std::string SurfacicSmoothing::toString(eSolver solver)
{
	if (solver == gradientConjugue)
			return "gradientConjugue";
	else if (solver == newton)
		return "newton";
	else
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur interne, un solver n'est pas encore prévue pour toString", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
