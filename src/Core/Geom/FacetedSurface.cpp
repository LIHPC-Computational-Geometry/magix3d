/*----------------------------------------------------------------------------*/
/*
 * \file FacetedSurface.cpp
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 27 févr. 2018
 */
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
#include "Geom/FacetedSurface.h"
#include "Geom/FacetedHelper.h"
#include "Geom/Surface.h"
#include "Geom/Curve.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Mesh/MeshImplementation.h"
#include "Utils/MgxNumeric.h"
// TkUtil
#include <TkUtil/Exception.h>
#include <TkUtil/MemoryError.h>
// GMDS
#include <GMDS/Utils/Exception.h>
// OCC
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Vec.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
FacetedSurface::FacetedSurface(Internal::Context& c, uint gmds_id, std::vector<gmds::Face>& faces)
:m_context(c), m_gmds_id(gmds_id)
, m_aabbFacesTree(0)
{
	// NB: si la surface contient des quadrangles, on créé des triangles que l'on utilisera par la suite
	// mais la surface GMDS conserve ses entitées initiales


	Mesh::MeshItf* meshItf = m_context.getMeshManager ( ).getMesh ( );
	Mesh::MeshImplementation* meshImpl = dynamic_cast<Mesh::MeshImplementation*> (meshItf);
	CHECK_NULL_PTR_ERROR(meshImpl)
	gmds::IGMesh&	gmdsMesh	= meshImpl->getGMDSMesh (m_gmds_id);

	// transforme les quadrangles en 4 triangles, avec insertion d'un sommet au centre
	for (std::vector<gmds::Face>::iterator iface=faces.begin(); iface!=faces.end(); ++iface){
		gmds::Face face = *iface;

		std::vector<gmds::Node> nodes;
		face.get(nodes);
		if (nodes.size() == 3)
			m_poly.push_back(face);
		else if (nodes.size() == 4){
			//std::cout<<"FacetedSurface découpe "<<" id "<<(long int)face.getID()<<" nb nodes "<<face.getNbNodes()<<std::endl;
			gmds::math::Point pt = face.center();
			gmds::Node nd = gmdsMesh.newNode(pt.X(), pt.Y(), pt.Z());

			for (uint i=0; i<4; i++){
				gmds::Face newFace = gmdsMesh.newTriangle(nodes[i], nodes[(i+1)%4], nd);
				m_poly.push_back(newFace);
				//std::cout<<"FacetedSurface ajoute "<<" id "<<(long int)newFace.getID()<<" nb nodes "<<newFace.getNbNodes()<<std::endl;
			}
		}
		else
			throw TkUtil::Exception(TkUtil::UTF8String ("polygone autre que triangle ou quadrangle dans FacetedSurface", TkUtil::Charset::UTF_8));


	}
	buildAABBTree();
}
/*----------------------------------------------------------------------------*/
FacetedSurface::FacetedSurface(const FacetedSurface& rep)
:m_context(rep.m_context), m_gmds_id(rep.m_gmds_id)
, m_aabbFacesTree(0)
{
    //std::cout<<"FacetedSurface::FacetedSurface(rep) copy"<<std::endl;
	m_poly.insert(m_poly.end(), rep.m_poly.begin(), rep.m_poly.end());
}
/*----------------------------------------------------------------------------*/
FacetedSurface::~FacetedSurface()
{
}
/*----------------------------------------------------------------------------*/
GeomRepresentation* FacetedSurface::clone() const
{
    //std::cout<<"FacetedSurface::clone()..."<<std::endl;
    FacetedSurface* e= new FacetedSurface(*this);
	e->buildAABBTree();
	return e;
}
/*----------------------------------------------------------------------------*/
double FacetedSurface::computeVolumeArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::computeVolumeArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
double FacetedSurface::computeSurfaceArea()
{
	double area = 0.0;
	const size_t	polygonNum	= m_poly.size ( );
	for (size_t id = 0; id < polygonNum; id++){
		gmds::Face			poly	= m_poly [id];

		std::vector<gmds::Node> nodes;

		m_poly[id].get(nodes);
		const size_t		count	= nodes.size ( );
		if (count == 3){
			Utils::Math::Vector v1(nodes[1].X()-nodes[0].X(),
					nodes[1].Y()-nodes[0].Y(),
					nodes[1].Z()-nodes[0].Y());
			Utils::Math::Vector v2(nodes[2].X()-nodes[0].X(),
					nodes[2].Y()-nodes[0].Y(),
					nodes[2].Z()-nodes[0].Y());
			area += v1.dot(v2)/2.0;
		}
		else if (count == 4){
			Utils::Math::Vector v1(nodes[1].X()-nodes[0].X(),
					nodes[1].Y()-nodes[0].Y(),
					nodes[1].Z()-nodes[0].Y());
			Utils::Math::Vector v2(nodes[3].X()-nodes[0].X(),
					nodes[3].Y()-nodes[0].Y(),
					nodes[3].Z()-nodes[0].Y());

			Utils::Math::Vector v3(nodes[1].X()-nodes[2].X(),
					nodes[1].Y()-nodes[2].Y(),
					nodes[1].Z()-nodes[2].Y());
			Utils::Math::Vector v4(nodes[3].X()-nodes[2].X(),
					nodes[3].Y()-nodes[2].Y(),
					nodes[3].Z()-nodes[2].Y());

			area += v1.dot(v2)/2.0 + v3.dot(v4)/2.0;

		}
		else {
			throw TkUtil::Exception(TkUtil::UTF8String ("Faire FacetedSurface::computeSurfaceArea pour le cas avec autre chose que des triangles ou des quads", TkUtil::Charset::UTF_8));

		}

	}
	return area;
}
/*----------------------------------------------------------------------------*/
double FacetedSurface::computeCurveArea()
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::computeCurveArea non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax, double tol) const
{
	std::vector<gmds::Node>	nodes;
	FacetedHelper::getGMDSNodes(m_poly, nodes);
	FacetedHelper::computeBoundingBox(nodes, pmin, pmax);
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::split(std::vector<Surface* >& surf,
           std::vector<Curve*>& curv,
           std::vector<Vertex* >&  vert,
           Volume* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::split(std::vector<Curve*>& curv,
           std::vector<Vertex* >&  vert,
           Surface* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::split( std::vector<Vertex* >&  vert,
            Curve* owner)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::split non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::project( Utils::Math::Point& P, const Curve* C)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project (Pt, Courbe) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::project( Utils::Math::Point& P, const Surface* S)
{
	// test si la structure de projection est à jour
	CHECK_NULL_PTR_ERROR(m_aabbFacesTree);
	try {

		// recherche du polygone le plus proche
		gmds::math::Point APoint(P.getX(), P.getY(), P.getZ());
		//std::cout<<"FacetedSurface::project pour "<<P<<std::endl;
		gmds::Face face = findTriangleNearestAABBTree(APoint);
		//gmds::Face face = findTriangleNearest(APoint);

		//std::cout<<"FacetedSurface::project nb nodes pour face "<<face.getNbNodes()<<std::endl;

		// projection
		gmds::math::Point projectPoint = face.project(APoint);
		P.setXYZ(projectPoint.X(), projectPoint.Y(), projectPoint.Z());
	}
	catch (gmds::GMDSException& exc){
		TkUtil::UTF8String	message (TkUtil::Charset::UTF_8);
		message<<"message remonté par GMDS : "<<exc.what();
		std::cerr<<message<<std::endl;
		throw TkUtil::Exception (message);
	}
	catch (...) {
		std::cerr<<"Erreur non documentée dans FacetedSurface::project"<<std::endl;
		throw TkUtil::Exception (TkUtil::UTF8String ("Erreur non documentée dans FacetedSurface::project", TkUtil::Charset::UTF_8));
	}
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Surface* S)
{
	P2 = P1;
	project(P2, S);
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::project(const Utils::Math::Point& P1, Utils::Math::Point& P2, const Curve* C)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("project Pt, Pt, Courbe) non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S)
{
	MGX_NOT_YET_IMPLEMENTED("normal à faire");
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::getPoint(const double& p, Utils::Math::Point& Pt,
        const bool in01)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::getPoint non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::getIntersection non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::getParameter(const Utils::Math::Point& Pt, double& p, const Curve* C)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::getParameter non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/

void FacetedSurface::getParameters(double& first, double& last)
{
	throw TkUtil::Exception(TkUtil::UTF8String ("fonction FacetedSurface::getParameters non prévue", TkUtil::Charset::UTF_8));
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::translate(const Utils::Math::Vector& V)
{
	std::vector<gmds::Node>	nodes;
    FacetedHelper::getGMDSNodes(m_poly, nodes);

    // création de l'opérateur de translation via OCC
    gp_Trsf transf;
    gp_Vec vec(V.getX(),V.getY(),V.getZ());
    transf.SetTranslation(vec);

	FacetedHelper::transform(nodes, &transf);

    update();
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::scale(const double F, const Utils::Math::Point& center)
{
	std::vector<gmds::Node>	nodes;
    FacetedHelper::getGMDSNodes(m_poly, nodes);

	// création de l'opérateur d'homothétie via OCC
	gp_Trsf transf;
	transf.SetScale(gp_Pnt(center.getX(), center.getY(), center.getZ()), F);

	FacetedHelper::transform(nodes, &transf);

    update();
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::scale(const double factorX,
            const double factorY,
            const double factorZ)
{
	std::vector<gmds::Node>	nodes;
    FacetedHelper::getGMDSNodes(m_poly, nodes);

    // création de l'opérateur d'homothétie via OCC
    gp_GTrsf transf;
    transf.SetValue(1,1, factorX);
    transf.SetValue(2,2, factorY);
    transf.SetValue(3,3, factorZ);

	FacetedHelper::transform(nodes, &transf);

    update();
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::rotate(const Utils::Math::Point& P1,
        const Utils::Math::Point& P2, double Angle)
{
	std::vector<gmds::Node>	nodes;
    FacetedHelper::getGMDSNodes(m_poly, nodes);

	// création de l'opérateur de rotation via OCC
    gp_Trsf transf;

    gp_Pnt p1(P1.getX(),P1.getY(),P1.getZ());
    gp_Dir dir( P2.getX()-P1.getX(),
                P2.getY()-P1.getY(),
                P2.getZ()-P2.getZ());
    gp_Ax1 axis(p1,dir);
    transf.SetRotation(axis, Angle);

    FacetedHelper::transform(nodes, &transf);

    update();
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::mirror(const Utils::Math::Plane& plane)
{
    //std::cout<<"FacetedSurface::mirror avec "<<plane<<std::endl;
	std::vector<gmds::Node>	nodes;
    FacetedHelper::getGMDSNodes(m_poly, nodes);

    // création de l'opérateur d'homothétie via OCC
    gp_Trsf transf;
    Utils::Math::Point plane_pnt = plane.getPoint();
    Utils::Math::Vector plane_vec = plane.getNormal();
    transf.SetMirror(gp_Ax2(gp_Pnt(plane_pnt.getX(), plane_pnt.getY(), plane_pnt.getZ()),
    		gp_Dir(plane_vec.getX(), plane_vec.getY(), plane_vec.getZ())));

    FacetedHelper::transform(nodes, &transf);

    update();
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
                                const GeomEntity* ge) const
{
    // si la surface n'est pas composée on va pouvoir récupérer les contours
    const Geom::Surface* surface = 0;
    if (ge->getComputationalProperties().size() == 1)
        surface = dynamic_cast<const Geom::Surface*>(ge);

    std::vector<Utils::Math::Point>& points = dr.getPoints();
    std::vector<size_t>& indicesFilaire = dr.getCurveDiscretization();
    std::vector<size_t>& indicesSurf = dr.getSurfaceDiscretization();

    points.clear();
    indicesFilaire.clear();
    indicesSurf.clear();

    if (dr.hasRepresentation(Utils::DisplayRepresentation::WIRE)){

        std::vector<Geom::Curve*> curves;
        if (ge) {
            // recherche des courbes facétisées du contour

            ge->get(curves);
            // représentation pour une courbe
            GeomDisplayRepresentation gdr(Utils::DisplayRepresentation::WIRE);

            // on cumule les représentations des courbes
            for (uint i=0; i<curves.size(); i++){
                curves[i]->getRepresentation(gdr, true);
                std::vector<Utils::Math::Point>& curve_points = gdr.getPoints();
                std::vector<size_t>& curve_indices = gdr.getCurveDiscretization();
                uint dec = points.size();
                points.insert(points.end(), curve_points.begin(), curve_points.end());
                for (uint j=0; j<curve_indices.size(); j++)
                    indicesFilaire.push_back(curve_indices[j] + dec);
            } // end for
        } //end if (ge)
    } // end if ... WIRE
    if (dr.hasRepresentation(Utils::DisplayRepresentation::ISOCURVE)){

        // création des lignes internes pour la représentation
        FacetedHelper::buildIsoCurve(m_poly,
                points, indicesFilaire);

    }

    if (dr.hasRepresentation(Utils::DisplayRepresentation::SOLID)){

    	// Constitution d'un vecteur avec les noeuds de la surface
    	// et une map pour l'indirection entre gmds::Node vers indice local
    	std::vector<gmds::Node>	nodes;
    	std::map<gmds::TCellID, int>	node2id;
        FacetedHelper::getGMDSNodes(m_poly, nodes);
    	for(int iNode=0; iNode<nodes.size(); iNode++) 
    		node2id[nodes[iNode].getID()] = iNode;

    	for (std::vector<gmds::Node>::iterator itn = nodes.begin ( );
    			itn != nodes.end ( ); itn++)
    		points.push_back (Utils::Math::Point ((*itn).X(), (*itn).Y(), (*itn).Z()));


    	const size_t	polygonNum	= m_poly.size ( );
    	for (size_t id = 0; id < polygonNum; id++)
    	{
    		gmds::Face			poly	= m_poly [id];
    		std::vector<gmds::TCellID>	nds		= poly.getIDs<gmds::Node>( );
    		const size_t		count	= nds.size ( );
    		if (count == 3){
    			indicesSurf.push_back(node2id [nds [0]]);
    			indicesSurf.push_back(node2id [nds [1]]);
    			indicesSurf.push_back(node2id [nds [2]]);
    		}
    		else if (count == 4){
    			indicesSurf.push_back(node2id [nds [0]]);
    			indicesSurf.push_back(node2id [nds [1]]);
    			indicesSurf.push_back(node2id [nds [2]]);

    			indicesSurf.push_back(node2id [nds [2]]);
    			indicesSurf.push_back(node2id [nds [3]]);
    			indicesSurf.push_back(node2id [nds [0]]);

    		}
    		else {
    			MGX_NOT_YET_IMPLEMENTED("Faire FacetedSurface::getRepresentation pour le cas avec autre chose que des triangles ou des quads");
    		}
    	}	// for (size_t id = 0; id < polygonNum; id++)

    }

}
/*----------------------------------------------------------------------------*/
void FacetedSurface::getFacetedRepresentation(
        std::vector<gmds::math::Triangle >& AVec,
        const GeomEntity* caller) const
{
	MGX_NOT_YET_IMPLEMENTED("getFacetedRepresentation à faire ?");
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::facetedRepresentationForwardOrient(
		const GeomEntity* ACaller,
		const GeomEntity* AEntityOrientation,
		std::vector<gmds::math::Triangle>*) const
{
	MGX_NOT_YET_IMPLEMENTED("facetedRepresentationForwardOrient à faire ?");
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2)
{
	MGX_NOT_YET_IMPLEMENTED("tangent à faire");
}
/*----------------------------------------------------------------------------*/
double FacetedSurface::getPrecision()
{
	return Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon;
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::buildAABBTree()
{
//    std::cout<<"FacetedSurface::buildAABBTree()"<<std::endl;
	GSList* list = NULL;

	for(int index=0; index<m_poly.size(); index++) {
		double minXYZ[3];
		double maxXYZ[3];

		gmds::Face current_face = m_poly[index];
		current_face.computeBoundingBox(minXYZ,maxXYZ);

		gpointer pointer = &m_poly[index];//GINT_TO_POINTER(index);
		GtsBBox* bbox = gts_bbox_new(
				gts_bbox_class (),
				pointer,
				minXYZ[0],minXYZ[1],minXYZ[2],
				maxXYZ[0],maxXYZ[1],maxXYZ[2]);

		list = g_slist_prepend(list,bbox);
	}

	m_aabbFacesTree = gts_bb_tree_new(list);

}
/*----------------------------------------------------------------------------*/
gmds::Face FacetedSurface::findTriangleNearestAABBTree(gmds::math::Point& APoint)
{
	GtsPoint*   gtsPoint = gts_point_new (
			gts_point_class (),
			APoint.X(),
			APoint.Y(),
			APoint.Z()
		);

	GtsBBox* bbox;

	// trying with a pointer to a non-static member function does not work.
	// It is due to the "this" hidden parameter that will offset
	// the parameters passed to the  GtsBBoxDistFunc function.
	//gdouble (Separatrice::*fptr) (GtsPoint*,gpointer);
	//fptr = &Separatrice::distanceAABBTree;

	gdouble (*fptr) (GtsPoint *,gpointer) = &FacetedSurface::distanceAABBTree;

	gdouble distance = gts_bb_tree_point_distance(
			m_aabbFacesTree,
			gtsPoint,
			(GtsBBoxDistFunc) fptr,
	        &bbox);

	if(bbox == NULL) {
		throw TkUtil::Exception(TkUtil::UTF8String ("FacetedSurface::findTriangleNearestAABBTree nearest triangle not found, this should not happen.", TkUtil::Charset::UTF_8));
	}

	gmds::Face* face = (gmds::Face*) bbox->bounded;
	return *face;
}
/*----------------------------------------------------------------------------*/
gmds::Face FacetedSurface::findTriangleNearest(gmds::math::Point& APoint)
{
	gmds::Face f0 = m_poly[0];
	double min_dist = f0.distance(APoint);
	int index = 0;
	for(int i=1; i<m_poly.size(); i++) {
        double dist = m_poly[i].distance(APoint);
        if(dist<min_dist) {
            min_dist=dist;
            index = i;
        }
	}
	return m_poly[index];
}
/*----------------------------------------------------------------------------*/
gdouble FacetedSurface::distanceAABBTree(
		GtsPoint *p,
        gpointer bounded)
{
	gmds::Face* face = (gmds::Face*) bounded;
	//std::cout<<"FacetedSurface::distanceAABBTree nb nodes pour face "<<face->getNbNodes()<<" id "<<(long int)face->getID()<<std::endl;
	double dist = face->distance(gmds::math::Point(p->x,p->y,p->z));
	return (gdouble) dist;
}
/*----------------------------------------------------------------------------*/
void FacetedSurface::update()
{

    // TODO est-ce qu'il n'y aurait pas une fuite mémoire ?
    // ne faudrait-il pas libérer la structure ?
    // la doc gts n'est pas claire

    buildAABBTree();
}
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
