/*----------------------------------------------------------------------------*/
/*
 * GeomSplitCurveImplementation.cpp
 *
 *  Created on: 12/4/2018
 *      Author: Eric B
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include "Geom/GeomSplitCurveImplementation.h"
#include "Geom/Curve.h"
#include "Geom/OCCGeomRepresentation.h"
#include "Geom/FacetedCurve.h"
#include "Geom/FacetedVertex.h"
#include "Geom/EntityFactory.h"
/*----------------------------------------------------------------------------*/
#include "TkUtil/Exception.h"
#include <TkUtil/MemoryError.h>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
GeomSplitCurveImplementation::
GeomSplitCurveImplementation(Internal::Context& c, Curve* crv, const Point& pt)
:GeomModificationBaseClass(c)
, m_entity_param(crv)
, m_pt(pt)
{
}
/*----------------------------------------------------------------------------*/
GeomSplitCurveImplementation::~GeomSplitCurveImplementation()
{
}
/*----------------------------------------------------------------------------*/
void GeomSplitCurveImplementation::prePerform()
{
    // Mise à jour des connectivés de références
	std::vector<GeomEntity*> entities_param;
	entities_param.push_back(m_entity_param);
    init(entities_param);
}
/*----------------------------------------------------------------------------*/
void GeomSplitCurveImplementation::perform(std::vector<GeomEntity*>& res)
{
	GeomRepresentation* gr = m_entity_param->getComputationalProperty();
	OCCGeomRepresentation* occ_gr = dynamic_cast<OCCGeomRepresentation*>(gr);
	FacetedCurve* faceted_crv = dynamic_cast<FacetedCurve*>(gr);

	if (occ_gr){
		MGX_NOT_YET_IMPLEMENTED("découpage d'une courbe OCC à faire")

		// peut être utiliser la section avec plan orthogonal à la tangente à la courbe
		// La construction d'une courbe à l'aide de Geom_TrimmedCurve ne permettra pas
		// facilement de réutiliser les vertex
	}
	else if (faceted_crv){
		// construire les 2 courbes
		// les appuyer sur les points initiaux et sur le point central

		size_t indiceMin = faceted_crv->getNodeIdNear(m_pt);
		const std::vector<gmds::Node>& nodes = faceted_crv->getNodes();

		if ((indiceMin<nodes.size()-1) && (indiceMin>0)){


			std::vector<gmds::Node> nodes1;
			std::vector<gmds::Node> nodes2;
			for (uint i=0; i<=indiceMin; i++)
				nodes1.push_back(nodes[i]);
			for (uint i=indiceMin; i<nodes.size(); i++)
				nodes2.push_back(nodes[i]);

			Curve* crv1 = EntityFactory(getContext()).newFacetedCurve(faceted_crv->getGMDSID(), nodes1);
			Curve* crv2 = EntityFactory(getContext()).newFacetedCurve(faceted_crv->getGMDSID(), nodes2);
			Vertex* vtx = EntityFactory(getContext()).newFacetedVertex(faceted_crv->getGMDSID(), nodes[indiceMin]);

			vtx->add(crv1);
			vtx->add(crv2);
			crv1->add(vtx);
			crv2->add(vtx);

			// les sommets initiaux
			std::vector<Vertex*> vertices;
			m_entity_param->get(vertices);
			if (vertices.size() == 1){
				Vertex* vtx1 = vertices[0];
				vtx1->add(crv1);
				vtx1->add(crv2);
				crv1->add(vtx1);
				crv2->add(vtx1);
			}
			else if (vertices.size() == 2){
				// il faut déterminer si le premier sommet est du côté de la 1ère courbe
				Vertex* vtx1 = vertices[0];
				Vertex* vtx2 = vertices[1];
				double param = 0;
				Utils::Math::Point Pt0 = vtx1->getCoord();
				faceted_crv->getParameter(Pt0, param, 0);
				if (param != 0){
					Vertex* vtmp = vtx1;
					vtx1 = vtx2;
					vtx2 = vtmp;
				}

				vtx1->add(crv1);
				vtx2->add(crv2);
				crv1->add(vtx1);
				crv2->add(vtx2);

			}
			m_removedEntities.push_back(m_entity_param);
			m_newCurves.push_back(crv1);
			m_newCurves.push_back(crv2);
			m_newVertices.push_back(vtx);
			m_replacedEntities[m_entity_param].push_back(crv1);
			m_replacedEntities[m_entity_param].push_back(crv2);
			m_newEntities.push_back(crv1);
			m_newEntities.push_back(crv2);
			m_newEntities.push_back(vtx);

			std::vector<Surface*> surfaces;
			m_entity_param->get(surfaces);
			for (uint i=0; i<surfaces.size(); i++){
				surfaces[i]->add(crv1);
				surfaces[i]->add(crv2);
			}

		}
		else {
			throw TkUtil::Exception(TkUtil::UTF8String ("impossible de couper cette courbe, on ne trouve pas de point entre les extrémités", TkUtil::Charset::UTF_8));
		}


	}
	else {
		throw TkUtil::Exception (TkUtil::UTF8String ("type de GeomRepresentation non pris en charge", TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
