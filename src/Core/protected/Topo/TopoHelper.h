/*----------------------------------------------------------------------------*/
/*
 * \file TopoHelper.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 13 mars 2012
 *
 *  Fonctions communes aux différents type d'entités topologiques
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_HELPER_H_
#define TOPO_HELPER_H_
/*----------------------------------------------------------------------------*/

#include "Utils/Entity.h"

#include <sys/types.h>
#include <map>
#include <list>
#include <vector>
#include <set>
#include "Utils/Vector.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
class InfoCommand;
}
namespace Geom {
class GeomEntity;
class Volume;
class Surface;
class Curve;
class Vertex;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class TopoEntity;
class Block;
class Face;
class CoFace;
class Edge;
class CoEdge;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \brief       Services utilitaires concernant les entités topologiques
 */
class TopoHelper
{
public:

    /** tableau qui donne les indices de sommets par face pour un bloc.
     *
     *  Les indices sont circulaires par face.
     */
    static const uint tabIndVtxByFaceOnBlock[6][4];

    /// tableau qui donne les indices de sommets par face pour un bloc dégénéré
    static const uint tabIndVtxByFaceOnDegBlock[5][4];

    /** tableau qui donne les indices de sommets par face pour un bloc.
     *
     *  Les indices sont suivant la direction (et non pas circulaire)
     */
    static const uint tab2IndVtxByFaceOnBlock[6][4];

    /// tableau des indices de sommets des arêtes par direction dans un bloc
    static const uint tabIndVtxByEdgeAndDirOnBlock[3][4][2];

    /// tableau des indices de sommets des arêtes par direction dans une face
    static const uint tabIndVtxByEdgeOnFace[4][2];

    /// tableau des indices de sommets des arêtes par direction dans une face dégénérée
    static const uint tabIndVtxByEdgeOnDegFace[3][2];

    /// tableau des indices de sommets des arêtes par direction alternées dans une face
    static const uint tabIndVtxByEdgeOnAlternateFace[4][2];

    /// recherche les CoEdges entre 2 Vertex et retourne vrai si tout est ok, faux en cas d'échec
    static bool getCoEdgesBetweenVertices(Vertex* v1, Vertex* v2,
            const std::vector<CoEdge* >& coedges,
            std::vector<CoEdge* >& coedges_between);

    /// recherche les sommets internes à une liste triée d'arêtes
    static void getInnerVertices(std::vector<CoEdge* >& coedges, std::vector<Topo::Vertex*>& innerVertices);

    /** recherche la direction définie par deux points à partir des CoEdges suivant les différentes directions
     * ces points pouvant être ailleurs que sur une extrémité.
     * On donne la liste des CoEdges par direction.
     * retourne 0 ou 1 suivant le direction trouvée, 2 si les deux peuvent convenir
     */
    static uint getInternalDir(Vertex* v1, Vertex* v2,
            std::vector<CoEdge* >& iCoedges,
            std::vector<CoEdge* >& jCoedges);

    /// création d'une liste de CoFaces à partir d'une liste de Block
    static void getCoFaces(const std::vector<Block* > & blocs,
            std::vector<CoFace* > & cofaces);

    /// création d'une liste de CoEdge à partir d'une liste d'Edge
    static void getCoEdges(const std::vector<Edge* > & edges,
            std::vector<CoEdge* > & coedges);

    /// création d'une liste de CoEdge à partir d'une liste de CoFace
    static void getCoEdges(const std::vector<CoFace* > & faces,
            std::vector<CoEdge* > & coedges);

    /// création d'une liste de CoEdge à partir d'une liste de Block
    static void getCoEdges(const std::vector<Block* > & blocs,
            std::vector<CoEdge* > & coedges);

    /// création d'une liste de Vertex à partir d'une liste de CoFace
    static void getVertices(const std::vector<CoFace* > & faces,
    		std::vector<Vertex* > & vertices);

    /// création d'une liste de Vertex à partir d'une liste de Block
    static void getVertices(const std::vector<Block* > & blocs,
    		std::vector<Vertex* > & vertices);

    /// remplit le set avec les blocs accessibles depuis les volumes
    static void get(std::vector<Geom::Volume*>& volumes, std::set<Topo::Block*>& blocks);

    /// remplit le set avec les cofaces accessibles depuis les volumes
    static void get(std::vector<Geom::Volume*>& volumes, std::set<Topo::CoFace*>& cofaces, bool propagate);

    /// remplit le set avec les coedges accessibles depuis les volumes
    static void get(std::vector<Geom::Volume*>& volumes, std::set<Topo::CoEdge*>& coedges, bool propagate);

    /// remplit le set avec les vertices accessibles depuis les volumes
    static void get(std::vector<Geom::Volume*>& volumes, std::set<Topo::Vertex*>& vertices, bool propagate);

    /// remplit le set avec les cofaces accessibles depuis les surfaces
    static void get(std::vector<Geom::Surface*>& surfaces, std::set<Topo::CoFace*>& cofaces);

    /// remplit le set avec les coedges accessibles depuis les surfaces
    static void get(std::vector<Geom::Surface*>& surfaces, std::set<Topo::CoEdge*>& coedges, bool propagate);

    /// remplit le set avec les vertices accessibles depuis les surfaces
    static void get(std::vector<Geom::Surface*>& surfaces, std::set<Topo::Vertex*>& vertices, bool propagate);

    /// remplit le set avec les coedges accessibles depuis les courbes
    static void get(std::vector<Geom::Curve*>& curves, std::set<Topo::CoEdge*>& coedges);

    /// remplit le set avec les vertices accessibles depuis les courbes
    static void get(std::vector<Geom::Curve*>& curves, std::set<Topo::Vertex*>& vertices, bool propagate);

    /// remplit le set avec les vertices accessibles depuis les sommets
    static void get(std::vector<Geom::Vertex*>& vtx, std::set<Topo::Vertex*>& vertices);

    /// remplit le set avec les cofaces accessibles depuis les blocs
    static void get(std::vector<Topo::Block*>& blocks, std::set<Topo::CoFace*>& cofaces);

    /// remplit le set avec les coedges accessibles depuis les blocs
    static void get(std::vector<Topo::Block*>& blocks, std::set<Topo::CoEdge*>& coedges);

    /// remplit le set avec les vertices accessibles depuis les blocs
    static void get(std::vector<Topo::Block*>& blocks, std::set<Topo::Vertex*>& vertices);

    /// remplit le set avec les coedges accessibles depuis les cofaces
    static void get(std::vector<Topo::CoFace*>& cofaces, std::set<Topo::CoEdge*>& coedges);

    /// remplit le set avec les vertices accessibles depuis les cofaces
    static void get(std::vector<Topo::CoFace*>& cofaces, std::set<Topo::Vertex*>& vertices);

    /// remplit le set avec les vertices accessibles depuis les coedges
    static void get(std::vector<Topo::CoEdge*>& coedges, std::set<Topo::Vertex*>& vertices);


    /// permute 2 vecteurs de CoEdges
    static void permuteVector(std::vector<CoEdge* > & coedges1,
            std::vector<CoEdge* > & coedges2);

    /// recherche les 2 sommets aux extrémités d'un ensemble de CoEdges, les coedges pouvant ne pas être triées
    static void getVerticesTip(const std::vector<CoEdge* > & coedges, Vertex* &v1, Vertex* &v2, bool isSorted);

    /** découpes les faces communes suivant une arête et un ratio comme paramètre
     *  \param cofaces les cofaces à couper
     *  \param arete l'arête commune qui sert de départ
     *  \param ratio_dec le ratio entre le nombre de bras de part et d'autre du point à insérer
     *  \param ratio_ogrid le ratio entre entre le sommet de dégénéresce d'une face et le sommet de l'ogrid
     *  \param boucle vrai si le parcours doit former une boucle
     *  \param rebondAutorise vrai si l'on autorise un rebond sur un point dégénéré
     *  \param splitingEdges les arêtes créées ou non sur la coupe
     *  \param icmd pour stocker les types de modifications par entités
     */
    static void splitFaces(std::vector<CoFace* > cofaces,
            CoEdge* arete, double ratio_dec, double ratio_ogrid,
            bool boucle, bool rebondAutorise,
            std::vector<Edge* > &splitingEdges,
            Internal::InfoCommand* icmd);

    /** Constitue la liste des entités marquées à marque */
    static void getMarqued(std::map<CoEdge*, uint> filtre, const uint marque, std::vector<CoEdge*>& out);

    /** Constitue la liste des entités marquées à autre chose que 0 */
    static void getMarqued(std::map<CoFace*, uint> filtre, std::vector<CoFace*>& out);

    /** Sauvegarde des relations topologiques pour toutes les entités
     * et celles de niveau inférieur
     * */
    static void saveTopoEntities(std::vector<CoFace*>& cofaces, Internal::InfoCommand* icmd);

    /** Copie les propriétés de discrétisation
     * Si l'arête de départ est composée de plusieurs arêtes communes,
     * on applique une propriété de discrétisation uniforme
     */
    static void copyMeshingProperty(const Edge* edge, CoEdge* coedge);

    /** Permute les 2 entités, si nécessaire, de manière à ce que l'unique id de vtx0
     *  soit < à celui de vtx1
     */
    static void sortVertices(Vertex* & vtx0, Vertex* & vtx1);

    /** Suppression des associations vers la géométrie pour le sommet vtx
     * ainsi que pour tout ce qui touche ce sommet
     */
    static void deleteAllAdjacentTopoEntitiesGeomAssociation(Vertex* vtx);

    /** Recherche le sommet commun à 2 arêtes
     * lance une exception s'il n'y en a pas
     */
    static Vertex* getCommonVertex(CoEdge* coedge1, CoEdge* coedge2);

    /** Recherche le sommet commun à 2 faces
     * lance une exception s'il n'y en a pas
     */
    static Vertex* getCommonVertex(Face* face1, Face* face2);

    /** Retourne vrai si les 4 groupes d'arêtes permettent de définir un maillage
     *  suivant une même direction */
    static bool isUnidirectionalMeshable(std::vector<std::vector<CoEdge* > > coedges_dirs);

    /** Retourne les entités topologiques à partir de celles contenues dans une liste d'entités géométriques */
    static std::vector<Topo::TopoEntity*> getTopoEntities(std::vector<Geom::GeomEntity*>& geomEntities);

    /** Retourne les entités topologiques à partir de celles contenues contenues dans une liste d'entités géométriques */
    static std::vector<Geom::GeomEntity*> getGeomEntities(std::vector<Topo::TopoEntity*>& topoEntities);


    /** Retourne le sommet qui est à la position géométrique donnée (à la tolérance près) */
    static Topo::Vertex* getVertexAtPosition(std::vector<Topo::Vertex*>& vertices, const Utils::Math::Point& pt, const double& tol);

    /** Retourne tous les sommets reliés à un sommet par une arête */
    static std::vector<Vertex*> getVerticesNeighbour(Vertex* vtx);


    /** Retourne la liste des coedges au bord de l'ensemble des cofaces,
     *
     * Ce sont celles qui ne sont reliées qu'à une coface de la liste passée
     */
    static std::vector<Topo::CoEdge*> getBorder(std::vector<Topo::CoFace*>& cofaces);

    /** Retourne la liste des cofaces au bord de l'ensemble des blocs,
     *
     * Ce sont celles qui ne sont reliées qu'à un bloc de la liste passée
     */
    static std::vector<Topo::CoFace*> getBorder(std::vector<Topo::Block*>& blocks);

    /** Retourne les arêtes communes à 2 blocs
     */
    static std::vector<CoEdge*> getCommonCoEdges(Block* bloc1, Block* bloc2);

    /** Retourne les arêtes communes à 2 faces
     */
    static std::vector<CoEdge*> getCommonCoEdges(CoFace* face1, CoFace* face2);

    /** Retourne l'arête commune entre 2 sommet (la 1ère), exception si pas trouvée
     */
    static CoEdge* getCommonCoEdge(Topo::Vertex* v0, Topo::Vertex* v1);

    /// recherche une coedge dans une liste, cette coedge doit posséder les sommets passés en argument
    static CoEdge* getCoedge(std::vector<Vertex*>& vertices, std::vector<CoEdge*>& coedges);

    /// recherche une coface dans une liste, cette coface doit posséder les sommets passés en argument
    static CoFace* getCoface(std::vector<Vertex*>& vertices, std::vector<CoFace*>& cofaces);

    /// recherche des points qui correspondent à la discrétisation d'un ensemble d'arêtes
    static void getPoints(Topo::Vertex* vtx1, Topo::Vertex* vtx2,
    		std::vector<Topo::CoEdge*>& coedges_between,
    		std::map<Topo::CoEdge*,uint>& ratios,
    		std::vector<Utils::Math::Point>& points);

    /// vérifie que les 2 listes contiennent les mêmes sommets
    static bool haveSame(std::vector<Topo::Vertex*>& verticesA, std::vector<Topo::Vertex*>& verticesB);

    /// retourne le sommet opposé d'un sommet dans un bloc connaissant l'indice de direction et celui de côté
    static Topo::Vertex* getOppositeVertex(Block* bloc, Topo::Vertex* vtx, uint indDir, uint indCote);


    /// calcul une normale en un point à partir des coedges des cofaces orthogonales à l'arête
    static Utils::Math::Vector computeNormale(Topo::Vertex* vtx, const Topo::CoEdge* coedge);

    /// calcul le sens des cofaces les unes par rapport aux autres et détermine celle qui ont une normale opposée
    static void computeInvertedDir(std::vector<Topo::CoFace*>& cofaces, std::map<Topo::CoFace*, bool>& isInverted);

    /// recherche si 2 cofaces sont orientées dans la même direction (cofaces avec une coedge commune)
    static bool cofacesInverted(Topo::CoFace* coface, Topo::CoFace* coface_vois, Topo::CoEdge* coedge);


private :

    /**
     * Constructeurs et destructeurs. Opérations interdites.
     */
    //@{
    TopoHelper ( );
    TopoHelper (const TopoHelper&);
    TopoHelper& operator = (const TopoHelper&);
    ~TopoHelper ( );
    //@}
};  // class TopoHelper

/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_HELPER_H_ */
