/*----------------------------------------------------------------------------*/
/*
 * \file FacetedHelper.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6/4/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef FACETEDHELPER_H_
#define FACETEDHELPER_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"

#include <GMDS/IG/Node.h>
#include <GMDS/IG/Face.h>

// OCC
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
class MeshImplementation;
}
/*----------------------------------------------------------------------------*/
namespace Geom {
class FacetedSurface;
class FacetedCurve;
class FacetedVertex;

/*----------------------------------------------------------------------------*/
/**
 * \brief       Services utilitaires concernant les entités géométriques facétisées
 */
class FacetedHelper
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
	static void computeBoundingBox(const std::vector<gmds::Node>& nodes,
			Utils::Math::Point& pmin,
			Utils::Math::Point& pmax);


	static void transform(std::vector<gmds::Node>& nodes,
			gp_Trsf* transf);
	static void transform(std::vector<gmds::Node>& nodes,
			gp_GTrsf* transf);

	/// constrction des lignes internes pour la représentatio en filaire
	static void buildIsoCurve(const std::vector<gmds::Face>& poly,
            std::vector<Utils::Math::Point>& points,
            std::vector<size_t>& indices);

    /// Retourne la liste des noeuds GMDS pour un ensemble de polygones
    static void getGMDSNodes(const std::vector<gmds::Face>& poly,
             std::vector<gmds::Node >& ANodes);

    /// Calcul les couples de points intersections entre un plan et les triangles
    static void addPointsIsoCurve(std::vector<gmds::Node>& nodes,
             std::map<gmds::TCellID, uint>& filtre_nodes,
             std::vector<double>& coordPl,
             uint dirPlan,
             std::vector<Utils::Math::Point>& points,
             std::vector<size_t>& indices);

    /// calcul le point intection entre un plan et le segment entre 2 noeuds
    static Utils::Math::Point getIntersectionPlan(gmds::Node& nd1, gmds::Node& nd2, double plVal, uint &dirPlan);

    /// préparation de la liste des noeuds en mettant en premier ceux qui intersectent le plan
    static void sortNodes(std::vector<gmds::Node>& nodes, std::map<gmds::TCellID, uint>& filtre_nodes);

    /// duplique le maillage pour toutes ces entités
    static void duplicateMesh(Mesh::MeshImplementation* mesh, std::vector<FacetedSurface*>& fs, std::vector<FacetedCurve*>& fc, std::vector<FacetedVertex*>& fv);

    /// ordonne les sommets des différentes courbes facétisées en partant d'un noeud
    static void computeOrdonatedNodes(gmds::Node first_node, std::vector<FacetedCurve*>& fc, std::vector<gmds::Node>& nodes);

    /// déplace les noeuds en fonction des normales des polygones avoisinants et de l'offset
    static void transformOffset(Mesh::MeshImplementation* mesh, FacetedSurface* fs, double& offset);

private :

    /**
     * Constructeurs et destructeurs. Opérations interdites.
     */
    //@{
    FacetedHelper ( );
    FacetedHelper (const FacetedHelper&);
    FacetedHelper& operator = (const FacetedHelper&);
    ~FacetedHelper ( );
    //@}

};  // class FacetedHelper
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* FACETEDHELPER_H_ */
