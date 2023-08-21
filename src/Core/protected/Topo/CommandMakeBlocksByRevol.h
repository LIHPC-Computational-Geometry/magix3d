/*----------------------------------------------------------------------------*/
/*
 * \file CommandMakeBlocksByRevol.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 12/6/2012
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDMAKEBLOCKSBYREVOL_H_
#define COMMANDMAKEBLOCKSBYREVOL_H_
/*----------------------------------------------------------------------------*/
#include "Topo/CommandEditTopo.h"
#include "Utils/Matrix33.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
class CommandExtrudeRevolution;
class Surface;
class Vertex;
class Curve;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/** \class CommandMakeBlocksByRevol
 *  \brief Commande permettant la construction de la topologie 3D à partir d'une topologie 2D
 *  avec insertion d'un ogrid au centre
 *
 */
/*----------------------------------------------------------------------------*/
class CommandMakeBlocksByRevol: public Topo::CommandEditTopo {

public:
    /// Direction pour la révolution
    enum eDirRevol {XAxis, YAxis, ZAxis, AnotherAxis};

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param coedges les arêtes qui servent à déterminer le tracé de l'ogrid
     *  \param dt un paramètre pour déterminer la fraction pour l'extrusion
     *  \param ratio_ogrid pour rapprocher de l'axe les sommets internes de l'o-grid
     */
    CommandMakeBlocksByRevol(Internal::Context& c,
    		Geom::CommandExtrudeRevolution* commandGeom,
    		std::vector<CoEdge*>& coedges,
    		const  Utils::Portion::Type& dt,
			double ratio_ogrid);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandMakeBlocksByRevol();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /** Retourne vrai si le point est sur l'axe de symétrie
     */
    bool isOnAxis(Vertex* vtx);

    /** Retourne vrai si la première CoFace est la plus proche de l'axe
     */
    bool isNearestAxis(CoFace* coface1, CoFace* coface2);

    /** Remplit les filtres en démarrant le parcours depuis l'arête sur l'o-grid
     *
     * Marque à
     * 1 ce qui est sur l'ogrid,
     * 2 ce qui est dessous
     * 3 ce qui est au dessus
     * 4 ce qui est en dehors du plan 2D ou d'une de ses copies
     * 6 ce qui est sur l'ogrid avec un lien sur un sommet entre l'axe et l'ogrid
     * */
    void computeOgridFilters(std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface);

    /**  Remplit les filtres en sélectionnant toutes les faces existantes
     *   Marque tout à 3
     */
    void computeFiltersWithAllFaces(std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface);

    /** Modifie les filtres en se basant sur les entités marquées
     *
     * Ajoute :
     * 10 ce qui touche l'axe
     * 20 ce qui est entièrement sur l'axe (pour les arêtes)
     * */
    void computeAxisFilters(std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface);

    /** Détection des sommets entre l'axe et l'ogrid,
     * marque les sommets de l'ogrid à 6 lorsqu'ils sont reliés à un tel sommet
     */
    void detectVerticesUnderOgrid(std::map<Vertex*, uint>& filtre_vertex);

    /** calcul le nombre de bras (ni) entre l'axe et le tracé de l'o-grid.
     * C'est ce nombre de bras que l'on retrouve dans le bloc central de l'o-grid.
     * Calcul égallement les ni locaux pour les sommets sur l'ogrid et
     * pour les cofaces de part et d'autre de cet ogrid
      */
    void computeNi(std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<Vertex*, uint>& ni_vtx,
            std::map<CoEdge*, uint>& ni_coedge,
            std::map<CoFace*, uint>& ni_coface);

    /** Copie des ni locaux d'une liste de cofaces et d'arêtes en relation avec une autre liste */
    void copyNi(std::map<Vertex*, uint>& ni_vtx,
            std::map<CoEdge*, uint>& ni_coedge,
            std::map<CoFace*, uint>& ni_coface,
    		std::vector<CoFace*>& cofaces_dep,
    		std::vector<CoFace*>& cofaces_arr);

    /** marque les entités qui ne sont pas déjà marquées pour les CoFace voisines d'une arête
     *  ainsi que pour les CoEdge et Vertex associé
     */
    void marqueCoFaceCoEdgeVertices(CoEdge* coedge, const uint marque,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface);

    /** Recherche les arêtes marquées comme étant sur l'ogrid en parcourant les faces
     *  */
    void getMarquedOnOgrid(std::vector<CoFace*>& cofaces,
            std::map<Vertex*, uint>& filtre_vertex,
            std::vector<Vertex*>& vertices_ogrid);

    /** Effectue une rotation de la topologie
     * \param angle en degrés
     */
    void rotate(uint angle,
            std::vector<CoFace*>& cofaces);

    /** Effectue une rotation d'un sommet topologique
     * \param angle en degrés
     */
    void rotate(uint angle,
            Vertex* vtx);

    /** Effectue une rotation des données d'une arête topologique
     * \param angle en degrés
     */
    void rotate(uint angle,
            CoEdge* coedge);

    /** Effectue une homothétie par rapport à la distance à l'axe */
    void scale(double ratio,
            std::vector<Vertex*>& vertices);

    /** suppression des entités sous l'ogrid,
     * les marques sont mises à 0 pour ces dernières */
    void freeUnused(std::vector<CoFace*>& cofaces,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface,
            bool memorizeFreezedCofaces = false);

    /** pour le cas des arêtes détruites (sur l'axe)
     *  qui seraient référencées via l'interpolation
     *  les fait pointer sur l'arête sur l'ogrid
     */
    void updateInterpolate(std::vector<CoFace*>& cofaces,
    		std::map<CoEdge*, uint>& filtre_coedge,
			std::map<CoFace*, uint>& filtre_coface);

    /** Construction des arêtes (CoEdge) entre les 2 groupes de faces,
     * \param nbDemiOgrid 1 ou 2 suivant que l'on est sur un quart d'angle de révol ou la demi (ce qui correspond au nb de moitiés du côté de l'ogrid)
     * \param vtx2coedges est renseigné en retour avec pour chacun des sommet la liste des coedges par révolution
     * \param preserveSurfaceAssociation pour conserver l'association vers une surface même si elle est détruite (nécessaire sur la surface de départ)
     * */
    void constructRevolCoEdges(std::vector<CoFace*>& cofaces_0,
            std::vector<CoFace*>& cofaces_1,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            uint nbDemiOgrid,
            bool sensNormal,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<Vertex*, uint>& ni_vtx,
			bool preserveSurfaceAssociation);

    /** Construction des  faces (CoFace et Edge) entre les différents couples de topo 2D
     *  sauf pour les zones sous l'ogrid
     *  \param coedge2cofaces est renseigné en retour
     */
    void constructRevolFaces(std::vector<CoFace*>& cofaces_0,
            std::vector<CoFace*>& cofaces_1,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
            std::map<CoEdge*, uint>& ni_coedge,
            uint facteur_ni);

    /** Construction des faces (CoFace et Edge) situées sous l'ogrid
     * \param coedge2cofaces est renseigné en retour
     */
    void constructRevolFacesInnerOgrid(std::vector<CoFace*>& cofaces_0,
            std::vector<CoFace*>& cofaces_1,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces);

    /** Construction des blocs entre les différents couples de topo 2D
     *  sauf pour les zones sous l'ogrid
     */
    void constructRevolBlocks(std::vector<CoFace*>& cofaces_0,
            std::vector<CoFace*>& cofaces_1,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
            std::map<Vertex*, uint>& ni_vtx,
            std::map<CoFace*, uint>& ni_coface,
            uint facteur_ni);

    /** Construction des blocs situés au centre de l'ogrid, dans le cas d'un quart de révolution
     */
    void constructRevolBlocksInnerOgrid_4(std::vector<CoFace*>& cofaces_0,
            std::vector<CoFace*>& cofaces_45,
            std::vector<CoFace*>& cofaces_90,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
            std::map<Vertex*, uint>& ni_vtx,
            std::map<CoFace*, uint>& ni_coface);

    /** Construction des blocs situés au centre de l'ogrid, dans le cas d'une révolution d'un demi tour
     */
    void constructRevolBlocksInnerOgrid_2(std::vector<CoFace*>& cofaces_0,
            std::vector<CoFace*>& cofaces_45,
            std::vector<CoFace*>& cofaces_135,
            std::vector<CoFace*>& cofaces_180,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
            std::map<Vertex*, uint>& ni_vtx,
            std::map<CoFace*, uint>& ni_coface);

    /** Construction des blocs situés au centre de l'ogrid, dans le cas d'une révolution d'un tour entier
     */
    void constructRevolBlocksInnerOgrid_1(std::vector<CoFace*>& cofaces_45,
            std::vector<CoFace*>& cofaces_135,
            std::vector<CoFace*>& cofaces_225,
            std::vector<CoFace*>& cofaces_315,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge,
            std::map<CoFace*, uint>& filtre_coface,
            std::map<Vertex*, std::vector<CoEdge* > >& vtx2coedges,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
            std::map<Vertex*, uint>& ni_vtx,
            std::map<CoFace*, uint>& ni_coface);

    /** Projette les sommets du tracé de l'ogrid sur les surfaces issues de la révol des courbes
     * sur lesquelles sont projettées les arêtes en dehors du tracé et voisines des sommets.
     */
    void updateGeomAssociationOGrid(std::vector<Vertex*>& vertices,
            std::map<Vertex*, uint>& filtre_vertex,
            std::map<CoEdge*, uint>& filtre_coedge);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une révolution (utilisation de V2C ou de C2S)
     */
    virtual void updateGeomAssociation(Geom::GeomEntity* ge, CoEdge* coedge);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une révolution (utilisation de C2S)
     */
    virtual void updateGeomAssociation(Geom::GeomEntity* ge, Block* block);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une révolution (utilisation de S2V)
     */
    virtual void updateGeomAssociation(Geom::GeomEntity* ge, CoFace* coface);

    /** Met à jour l'association vers la nouvelle entité géométrique
     *  qui a subit une révolution (utilisation de C2S)
     *  Qui est limité aux surfaces (on ne projette pas sur une courbe)
     */
    void updateGeomAssociationToSurf(Geom::GeomEntity* ge, Vertex* vertex);

    /** Construction d'une Face à partir d'une unique coface prise dans la table de correspondance
     * entre une CoEdge et une CoFace
     * Ne fonctionne que pour les Edge ayant une unique CoEdge
     * \return La face construite
     */
    Face* constructFaceWith1RevolCoFace(Edge* edge,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces);

    /** Construction d'une Face à partir de plusieurs cofaces prise dans la table de correspondance
     * entre une CoEdge et une CoFace
     * L'Edge ayant plusieurs CoEdge
     * \return La face construite
     */
    Face* constructFaceWithRevolCoFaces(Edge* edge,
            std::map<CoEdge*, std::vector<CoFace* > >& coedge2cofaces,
            Vertex* vtx0, Vertex* vtx1, Vertex* vtx2, Vertex* vtx3);

    /** Calcul les ratios pour les CoFaces d'une Face,
     * pour une direction donnée (0 ou 1, et si 2 alors 0 et 1)
     * Se base sur le ni local au bloc et les discrétisations aux bords de la face
     */
    void computeFaceRatio(Face* face, uint dir, uint ni_loc_coface);


private:
    /// la commande de révolution de la géométrie
    Geom::CommandExtrudeRevolution* m_commandGeom;

    /// les arêtes qui servent à déterminer le tracé de l'ogrid
    std::vector<CoEdge*> m_coedges;

    /// paramètre pour déterminer la portion pour l'extrusion
    Utils::Portion::Type m_portion;

    /// Axe pour la révolution
    eDirRevol m_axis;

    /// 2 points pour définir l'axe de rotation pour la révolution dans le cas AnotherAxis
    Utils::Math::Point m_pt1;
    Utils::Math::Point m_pt2;

    /// matrice de rotation
    Utils::Math::Matrix33 m_rotMatrix;

    /// nombre de bras entre axe et o-grid
    uint m_ni;

    /** ratio pour rapprocher de l'axe les sommets internes (au volume créé par révolution) du ogrid
     * 1 par défaut pour être proche d'un carré
     * 0 pour être à la même distance que dans le plan de sym du 2D
     */
    double m_ratio_ogrid;

    /// Liste des CoFaces supprimées, utile pour le undo-redo
    std::vector<CoFace* > m_deleted_cofaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDMAKEBLOCKSBYREVOL_H_ */
/*----------------------------------------------------------------------------*/
