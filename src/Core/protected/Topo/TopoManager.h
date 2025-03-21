/*----------------------------------------------------------------------------*/
/** \file TopoManager.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 2/12/2010
 *
 *  Modified on: 23/02/2022
 *      Author: Simon C
 *      ajout de la fonction de création de blocs par extrusion de faces topologiques
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_MANAGER_H_
#define TOPO_MANAGER_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Topo/TopoManagerIfc.h"
#include "Topo/TopoInfo.h"
#include "Utils/Container.h"
#include "Utils/Plane.h"
/*----------------------------------------------------------------------------*/
//#define _DEBUG_TIMER
/* Bilan des tests de perf:
 *  La recherche des entités en fonction des noms prend près d'un dixième de seconde
 *  Cela reste négligeable.
 */
#ifdef _DEBUG_TIMER
#include <TkUtil/Timer.h>
/// utilisation d'un timer pour évaluer les temps de recherche par noms des entités
static clock_t _cpuDuration;
#endif
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
namespace Math {
class Point;
class Vector;
class Rotation;
}
}
using Mgx3D::Utils::Math::Point;
using Mgx3D::Utils::Math::Vector;

namespace Internal {
class ContextIfc;
class Context;
}

namespace Geom {
class GeomEntity;
class Volume;
class Surface;
class Vertex;
}
namespace CoordinateSystem {
class SysCoord;
}
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdgeMeshingProperty;
class CoFaceMeshingProperty;
class BlockMeshingProperty;
/*----------------------------------------------------------------------------*/
/**
 * \class TopoManager
 * \brief Gestionnaire des opérations effectuées au niveau du module
 *        topologique. Le gestionnaire topologique est l'objet qui stocke tous
 *        les blocs topologique pour une session donnée. La responsabilité
 *        de créer et de détruire les entités topologique est
 *        liée à celle du Manager.
 *
 */
class TopoManager: public Topo::TopoManagerIfc {

    friend class CommandChangeDefaultNbMeshingEdges;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    TopoManager(const std::string& name, Internal::ContextIfc* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     * Il se charge de la destruction des entités topologiques
     */
    ~TopoManager();

	/*------------------------------------------------------------------------*/
	/** Réinitialisation     */
	virtual void clear();

	/*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie s'appuyant sur une géométrie
     *
     *  Ne fonctionne que pour une courbe ou un sommet
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newTopoOnGeometry(std::string ne);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newTopoOnGeometry(Geom::GeomEntity* ge);

    /** \brief Création d'une topologie structurée s'appuyant sur une géométrie
      *
      *  Retourne une exception si la topologie créée ne peut pas être rendue structurée
      *
      *  Ne fonctionne que pour un volume ou une surface
      *
      *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
      */
     virtual Mgx3D::Internal::M3DCommandResultIfc*
         newStructuredTopoOnGeometry(std::string ne);

     virtual Mgx3D::Internal::M3DCommandResultIfc*
     	 newStructuredTopoOnGeometry(Geom::GeomEntity* ge);

     /** \brief Création d'une face structurée s'appuyant sur une surface
      *
      *  Retourne une exception si la face créée ne peut pas être rendue structurée
      *
      *  Ne fonctionne que pour une surface
      *
      *  \param ne le nom de la surface sur laquelle s'appuiera la topologie
      *  \param ve la liste des (3 ou 4) sommets qui sont aux extrémités topologiques
      */
     virtual Mgx3D::Internal::M3DCommandResultIfc*
	 newStructuredTopoOnSurface(std::string ne, std::vector<std::string>& ve);

     virtual Mgx3D::Internal::M3DCommandResultIfc*
	 newStructuredTopoOnSurface(Geom::Surface* surf, std::vector<Geom::Vertex*>& vertices);

     /*------------------------------------------------------------------------*/
     /** \brief Création d'un block (ou d'une face) topologique structuré sans association
      *
      *  Ne fonctionne que pour un volume ou une surface
      *  Seule l'entité de plus haut niveau (Block ou CoFace) sera associée à l'entité géométrique
      *
      *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
      */
     virtual Mgx3D::Internal::M3DCommandResultIfc*
         newFreeTopoOnGeometry(std::string ne);

     virtual Mgx3D::Internal::M3DCommandResultIfc*
     	 newFreeTopoOnGeometry(Geom::GeomEntity* ge);

     /** \brief Création d'un block topologique structuré sans association
        *
        *  L'entité topologique est mise dans un groupe
        *
       *  \param ng le nom du groupe dans lequel sera mis le bloc ou la face
       *  \param dim la dimension (2 ou 3) de ce que l'on veut créer
       */
      virtual Mgx3D::Internal::M3DCommandResultIfc*
          newFreeTopoInGroup(std::string ng,  int dim);

    /** \brief Création d'un block topologique structuré sans association
       *
       *  L'entité topologique est mise dans un groupe et ses sommets aux coins de la boite
       *  englobante définie par les entités transmises en arguments
       *
       *  \param ng le nom du groupe dans lequel sera mis le bloc ou la face
       *  \param dim la dimension (2 ou 3) de ce que l'on veut créer
       *  \param ve est la liste des noms d'entités définissant la boite englobante de positionnement
       *         des sommets
      */
     virtual Mgx3D::Internal::M3DCommandResultIfc*
         newFreeBoundedTopoInGroup(std::string ng, int dim, const std::vector<std::string>& ve);

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie structurée en o-grid
     *
     *   La topologie créée s'adapte à un cylindre (5 blocs) ou à une sphère
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     *  \param rat le ratio de la distance entre l'axe du cylindre et son rayon pour placer le sommet du o-grid
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newTopoOGridOnGeometry(std::string ne, const double& rat);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newTopoOGridOnGeometry(Geom::GeomEntity* ge, const double& rat);

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie non-structurée s'appuyant sur une géométrie
     *
     *   La topologie créée est non-structurée
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newUnstructuredTopoOnGeometry(std::string ne);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newUnstructuredTopoOnGeometry(Geom::GeomEntity* ge);

    /*------------------------------------------------------------------------*/
    /** \brief création d'une boite parallèle aux axes Ox,Oy et Oz à partir des
     *         points pmin et pmax où pmin est le point de plus petites
     *         coordonnées (x,y,z) et pmax le point de plus grandes coordonnées
     *         (x,y,z), avec sa topologie associée
     *
     *  \param pmin le point min de la boite
     *  \param pmax le point max de la boite
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param groupName optionnellement un nom de groupe
     *
     *  \see GeomManager::newBox
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newBoxWithTopo(const Point& pmin, const Point& pmax,
    		bool meshStructured=true, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'une boite parallèle aux axes Ox,Oy et Oz à partir des
     *         points pmin et pmax où pmin est le point de plus petites
     *         coordonnées (x,y,z) et pmax le point de plus grandes coordonnées
     *         (x,y,z), avec sa topologie associée en ni X nj X nk mailles
     *
     *  \param pmin le point min de la boite
     *  \param pmax le point max de la boite
     *  \param ni le nombre de bras sur la direction des x
     *  \param nj le nombre de bras sur la direction des y
     *  \param nk le nombre de bras sur la direction des z
     *  \param groupName optionnellement un nom de groupe
     *
     *  \see GeomManager::newBox
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newBoxWithTopo(const Point& pmin, const Point& pmax,
    		const int ni, const int nj, const int nk,
    		std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cylindre avec une topologie, suivant un cercle,
     *         sa base (un cercle défini par un centre et un rayon)
     *         et son axe
     *
     *  \param pcentre le centre du cercle de base
     *  \param dr le rayon du cercle
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param da l'angle de la portion de cylindre
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param rat le ratio pour placer les sommets de l'o-grid, entre le centre et le rayon  (vaut 1 dans le cas d'un seul bloc)
     *  \param naxe le nombre de bras suivant l'axe du cylindre
     *  \param ni le nombre de bras entre le point triple et le plan de symétrie
     *  \param nr le nombre de couches entre le bloc central et la surface externe
     *  \param groupName optionnellement un nom de groupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newCylinderWithTopo(const Point& pcentre, const double& dr,
                                 const Vector& dv, const double& da,
                                 bool meshStructured, const double& rat,
                                 const int naxe, const int ni, const int nr,
                                 std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief Création d'un cylindre creux avec une topologie, suivant un cercle,
     *         sa base (un cercle creux défini par un centre et deux rayons)
     *         et son axe
     *
     *  \param pcentre le centre du cercle de base
     *  \param dr_int le rayon interne du cercle
     *  \param dr_ext le rayon externe du cercle
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param da l'angle de la portion de cylindre
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param naxe le nombre de bras suivant l'axe du cylindre
     *  \param ni le nombre de bras entre le point triple et le plan de symétrie
     *  \param nr le nombre de couches entre la surface interne et la surface externe
     *  \param groupName optionnellement un nom de groupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newHollowCylinderWithTopo(const Point& pcentre, const double& dr_int, const double& dr_ext,
                                 const Vector& dv, const double& da,
                                 bool meshStructured,
                                 const int naxe, const int ni, const int nr,
                                 std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cone avec topologie suivant un axe, avec deux rayons
     *         et une longueur
     *
     *  \param dr1 le premier rayon du cone (à l'origine)
     *  \param dr2 le deuxième rayon du cone
     *  \param dv le vecteur pour l'axe et la longueur
     *  \param da l'angle de la portion de cone
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param rat le ratio pour placer les sommets de l'o-grid, entre le centre et le rayon  (vaut 1 dans le cas d'un seul bloc)
     *  \param naxe le nombre de bras suivant l'axe du cône
     *  \param ni le nombre de bras entre le point triple et le plan de symétrie
     *  \param nr le nombre de couches entre la surface interne et la surface externe
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newConeWithTopo(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da,
			bool meshStructured, const double& rat,
            const int naxe, const int ni, const int nr,
			std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'une sphère avec une topologie, suivant un centre,
     *         son rayon et pour une portion donnée
     *
     *  \param pcentre le centre de la sphère
     *  \param dr le rayon de la sphère
     *  \param dt portion à créer
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param rat le ratio pour placer les sommets de l'o-grid, entre le centre et le rayon  (vaut 1 dans le cas d'un seul bloc)
     *  \param ni le nombre de bras entre le point triple et le plan de symétrie
     *  \param nr le nombre de couches entre le bloc central et la surface externe
     *  \param groupName optionnellement un nom de groupe
     *
     *   NB: en passant par les angles, le Huitième équivaut à 45 deg, ce qui empèche la création
     *   d'une sphère ouverte à 45 deg. Mais est-ce nécessaire de proposer autre chose qu'une portion ?
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newSphereWithTopo(const Point& pcentre,  const double& dr,
                                const  Utils::Portion::Type& dt,
                                bool meshStructured, const double& rat,
                                const int ni, const int nr,
                                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une sphère creuse avec une topologie, suivant un centre,
     *         ses rayons et pour une portion donnée
     *
     *  \param pcentre le centre de la sphère
     *  \param dr_int le rayon interne de la sphère
     *  \param dr_ext le rayon externe de la sphère
     *  \param dt portion à créer
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param ni le nombre de bras entre le point triple et le plan de symétrie
     *  \param nr le nombre de couches entre la surface interne et la surface externe
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newHollowSphereWithTopo(const Point& pcentre,  const double& dr_int,  const double& dr_ext,
            const  Utils::Portion::Type& dt,
            bool meshStructured,
            const int ni, const int nr,
            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une aiguille pleine du type partie de sphère avec topologie
     *
     *  \param  radius  Le rayon de l'aiguille
     *  \param  angleY  L'angle d'ouverture autour de Ox dans le plan Oxy
     *  \param  angleZ  L'angle d'ouverture autour de Ox dans le plan Oxz
     *  \param ni le nombre de couches entre centre et la surface externe
     *  \param nj le nombre de bras pour le côté parallèle à Y
     *  \param nk le nombre de bras pour le côté parallèle à Z
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newSpherePartWithTopo(const double& radius,
                      const double& angleY,
                      const double& angleZ,
					  const int ni,
					  const int nj,
					  const int nk,
                      std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une aiguille creuse du type partie de sphère avec topologie
     *
     *  \param dr_int le rayon interne de l'aiguille creuse
     *  \param dr_ext le rayon externe de l'aiguille creuse
     *  \param  angleY  L'angle d'ouverture autour de Ox dans le plan Oxy
     *  \param  angleZ  L'angle d'ouverture autour de Ox dans le plan Oxz
     *  \param ni le nombre de couches entre centre et la surface externe
     *  \param nj le nombre de bras pour le côté parallèle à Y
     *  \param nk le nombre de bras pour le côté parallèle à Z
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newHollowSpherePartWithTopo(const double& dr_int,
				const double& dr_ext,
				const double& angleY,
				const double& angleZ,
				const int ni,
				const int nj,
				const int nk,
				std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'une grille de ni par nj boites répartis sur Ox et Oy
     *         les boites sont toutes de taille 1x1x1 et ont une topologie
     *         associée structurée si  alternateStruture == false
     *         et sinon alternativement une structurée une non-structurée
     *
     *  \param ni le nombre de boite dans la direction Ox
     *  \param nj le nombre de boite dans la direction Oy
     *  \param alternateStruture vrai si l'on veut des blocs alternativement st/non-st
     *
     *  \see TopoManager::newBoxWithTopo
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newIJBoxesWithTopo(int ni, int nj, bool alternateStruture);

    /*------------------------------------------------------------------------*/
    /** \brief création d'une grille de ni par nj par nk boites répartis sur Ox, Oy et Oz
     *         les boites sont toutes de taille 1x1x1 et ont une topologie
     *         associée structurée si  alternateStruture == false
     *         et sinon alternativement une structurée une non-structurée
     *
     *  \param ni le nombre de boite dans la direction Ox
     *  \param nj le nombre de boite dans la direction Oy
     *  \param nk le nombre de boite dans la direction Oz
     *  \param alternateStruture vrai si l'on veut des blocs alternativement st/non-st
     *
     *  \see TopoManager::newBoxWithTopo
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newIJKBoxesWithTopo(int ni, int nj, int nk, bool alternateStruture);


    /*------------------------------------------------------------------------*/
    /** \brief Destruction d'entités topologiques
     *
     * \param ve noms des entités topologiques à détruire
     * \param propagate booléen à faux si on ne détruit que les entités,
     *    à vrai si on détruit les entités de dimensions inférieures qui ne sont pas reliées à une autre entité de dimension dim
     */
    virtual void destroy(std::vector<std::string>& ve, bool propagate);

    virtual void destroy(std::vector<Topo::TopoEntity*>& ve, bool propagate);


    /*------------------------------------------------------------------------*/
    /** \brief création d'entités topologique par copie
     *
     *  \param vb les blocs topologiques que l'on veut copier
     *  \param vo le volume auquel on associe les blocs

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::vector<std::string>& vb, std::string vo);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::vector<Topo::Block*>& vb, Geom::Volume* vo);

    /*------------------------------------------------------------------------*/
    /** \brief extraction de blocs par copie et destruction des anciens
     *
     *  \param vb les blocs topologiques que l'on veut extraire
     *  \param ng le nom du groupe 3D dans lequel on va stoquer ces nouveaux blocs

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extract(std::vector<std::string>& vb, const std::string ng);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extract(std::vector<Topo::Block*>& vb, const std::string ng);

    /*------------------------------------------------------------------------*/
    /** \brief duplique des faces communes pour faire apparaitre un trou topologique.
     *  Les sommets et arêtes sont dupliqués s'ils sont reliés uniquement à des blocs touchés
     *
     *  \param faces les faces communes topologiques que l'on veut dupliquer

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        insertHole(std::vector<std::string>& faces);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        insertHole(std::vector<CoFace*>& cofaces);

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 arêtes communes proches
     *
     *  La 1ère Arête commune prend la place de l'autre.
     *  Les sommets doivent être le plus proche possible
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Edges(std::string na, std::string nb);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		fuse2Edges(CoEdge* edge_A, CoEdge* edge_B);

    /*------------------------------------------------------------------------*/
    /** \brief Fusion entre 2 groupes d'arêtes communes
     *
     *	Le collage se fait entre sommets les plus proches
     *  Les arêtes communes doivent toute avoir une arête avec qui faire le collage
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		fuse2EdgeList(std::vector<std::string>& coedge_names1, std::vector<std::string>& coedge_names2);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		fuse2EdgeList(std::vector<Topo::CoEdge* > &coedges1, std::vector<Topo::CoEdge* > &coedges2);

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 faces communes
     *
     *  La 1ère Face commune prend la place de l'autre.
     *  Les sommets doivent être le plus proche possible
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Faces(std::string na, std::string nb);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Faces(CoFace* face_A, CoFace* face_B);

    /*------------------------------------------------------------------------*/
    /** \brief Fusion entre 2 groupes de faces communes
     *
     *	Le collage se fait entre sommets les plus proches
     *  Les faces communes doivent toute avoir une face avec qui faire le collage
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2FaceList(std::vector<std::string>& coface_names1, std::vector<std::string>& coface_names2);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
    	fuse2FaceList(std::vector<Topo::CoFace* > &cofaces1, std::vector<Topo::CoFace* > &cofaces2);

    /*------------------------------------------------------------------------*/
    /** \brief Collage des faces communes entre 2 blocs
     *
     *  Le premier bloc dont les faces prennent la place des autres.
     *  Le deuxième bloc dont les sommets d'une de ses faces doivent être le plus proche possible de celle de l'autre
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        glue2Blocks(std::string na, std::string nb);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        glue2Blocks(Block* bl_A, Block* bl_B);

    /*------------------------------------------------------------------------*/
    /** Collage entre 2 topologies suivant proximité des sommets.
     *  Topo prises dans 2 volumes.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    	glue2Topo(std::string volName1, std::string volName2);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    	glue2Topo(Geom::Volume* vol1, Geom::Volume* vol2);

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 sommets
     *
     *  Le 1er sommet prend la place de l'autre.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Vertices(std::string na, std::string nb);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
    	fuse2Vertices(Topo::Vertex* vtx_A, Topo::Vertex* vtx_B);

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un bloc structuré en deux
     *
     *	OBSOLETE: Issue79 - Homogénéisation de l'API : utiliser splitBlocks
     * 
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe le bloc en 2.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlock(std::string nbloc, std::string narete, const double& ratio);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlock(Block* bloc, CoEdge* arete, const double& ratio);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlock(Block* bloc, CoEdge* arete, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un ensembles de blocs en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
     *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const double& ratio);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocks(std::vector<Topo::Block* > &blocs, CoEdge* arete, const double& ratio);

    /** \brief Découpe de tous les blocs en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
     *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    splitAllBlocks(std::string narete, const double& ratio);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    splitAllBlocks(CoEdge* arete, const double& ratio);

    /** \brief Découpe un ensembles de blocs en deux
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
     *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const Point& pt);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocks(std::vector<Topo::Block* > &blocs, CoEdge* arete, const Point& pt);

    /** \brief Découpe de tous les blocs en deux
      *
      *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
      *  qui va être inséré sur l'arête.
      *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
      *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
      *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
      */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllBlocks(std::string narete, const Point& pt);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllBlocks(CoEdge* arete, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un bloc en deux en utilisant une arête qui coupe déjà une de ses faces
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extendSplitBlock(std::string nbloc, std::string narete);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extendSplitBlock(Block* bloc, CoEdge* arete);

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un ensemble de blocs suivant un o-grid
     *
     *  On utilise un ratio pour positionner les sommets par rapport
     *  aux centres des blocs
     *  \param blocs_names la liste des noms de blocs à découper
     *  \param cofaces_names la liste des faces communes à considérer comme étant entre 2 blocs
     *  (ce qui permet de ne pas construire le bloc contre ces faces communes)
     *  \param ratio_ogrid ratio pour positionner les nouveaux sommets,
     *  entre les sommets du bord de la sélection et le centre des blocs ou faces concernées
     *  \param nb_bras le nombre de couches de mailles autour du centre du o-grid à créer
     *  (c.a.d. le nombre de bras pour les arêtes entre les sommets du bord de la sélection et ceux créés)
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgridV2(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio_ogrid, int nb_bras);
    /// version avec comportement équivalent à ce qui est fait dans ICEM, utilisé par défaut jusqu'au début 2019
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgrid(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);
    /// version avec comportement antérieur à la version 1.6 de Magix3D
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgrid_old(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgrid(std::vector<Topo::Block* > &blocs,
            std::vector<Topo::CoFace* > &cofaces,
            const double& ratio_ogrid, int nb_bras,
			bool create_internal_vertices,
			bool propagate_neighbor_block);


    /*------------------------------------------------------------------------*/
    /** \brief Découpe un ensemble de faces suivant un o-grid
     *
     *  On utilise un ratio pour positionner les sommets par rapport
     *  aux centres des faces
     *  \param faces_names la liste des noms de faces à découper
     *  \param coedges_names la liste des arêtes communes à considérer comme étant entre 2 faces
     *  (ce qui permet de ne pas construire la face contre ces arêtes communes)
     *  \param ratio_ogrid ratio pour positionner les nouveaux sommets,
     *  entre les sommets du bord de la sélection et le centre des faces ou arêtes concernées
     *  \param nb_bras le nombre de couches de mailles autour du centre du o-grid à créer
     *  (c.a.d. le nombre de bras pour les arêtes entre les sommets du bord de la sélection et ceux créés)
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFacesWithOgrid(std::vector<std::string> &faces_names,
            std::vector<std::string> &coedges_names,
            const double& ratio_ogrid, int nb_bras);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFacesWithOgrid(std::vector<Topo::CoFace*> &cofaces,
            std::vector<Topo::CoEdge*> &coedges,
            const double& ratio_ogrid, int nb_bras);

    /*------------------------------------------------------------------------*/
    /** \brief Découpe des faces structurées 2D en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Si ratio_ogrid vaut 0, alors le tracé de l'ogrid peut passer par l'axe si une face n'a que 3 côté,
     *  sinon on découpe la face en 3 (ogrid en 2D) avec un sommet placé avec ce ratio entre le sommet dans
     *  le prolongement sur l'axe et le sommet à l'entrée dans la face.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const double& ratio_dec, const double& ratio_ogrid);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFaces(std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, const double& ratio_dec, const double& ratio_ogrid);

    /** \brief Découpe des faces structurées 2D en deux suivant un point à projeter
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les autres arêtes parallèles sont aussi découpées avec le même ratio induit par le premier sommet.
     *  Si ratio_ogrid vaut 0, alors le tracé de l'ogrid peut passer par l'axe si une face n'a que 3 côté,
     *  sinon on découpe la face en 3 (ogrid en 2D) avec un sommet placé avec ce ratio entre le sommet dans
     *  le prolongement sur l'axe et le sommet à l'entrée dans la face.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const Point& pt, const double& ratio_ogrid);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFaces(std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, const Point& pt, const double& ratio_ogrid);

    /** \brief Découpe toutes les faces structurées 2D en deux
     * \see splitFaces
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllFaces(std::string narete, const double& ratio_dec, const double& ratio_ogrid);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllFaces(CoEdge* arete, const double& ratio_dec, const double& ratio_ogrid);

    /** \brief Découpe toutes les faces structurées 2D en deux suivant un point à projeter
     * \see splitFaces
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllFaces(std::string narete, const Point& pt, const double& ratio_ogrid);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllFaces(CoEdge* arete, const Point& pt, const double& ratio_ogrid);

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un ratio
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  L'autre arête parallèle est aussi découpée avec le même ratio.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFace(std::string coface_name, std::string narete, const double& ratio_dec, bool project_on_meshing_edges);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFace(Topo::CoFace* coface, CoEdge* coedge, const double& ratio_dec, bool project_on_meshing_edges);

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un point à projeter
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  L'autre arête parallèle est aussi découpée avec le même ratio induit de la première coupe.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFace(std::string coface_name, std::string narete, const Point& pt, bool project_on_meshing_edges = true);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFace(Topo::CoFace* coface, CoEdge* coedge, const Point& pt, bool project_on_meshing_edges);


    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un sommet
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extendSplitFace(std::string coface_name, std::string nsommet);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extendSplitFace(Topo::CoFace* coface, Vertex* vertex);


    /*------------------------------------------------------------------------*/
    /** \brief Découpe d'une arête commune en 2
     *
     *  On utilise un ratio pour positionner le sommet
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitEdge(std::string coedge_name, const double& ratio_dec);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitEdge(CoEdge* coedge, const double& ratio_dec);

    /** \brief Découpe d'une arête commune en 2
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitEdge(std::string coedge_name, const Point& pt);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitEdge(CoEdge* coedge, const Point& pt);

    /*------------------------------------------------------------------------*/
    /** \brief Fusionne un sommet avec un autre, pour un couple de sommets
     *  ou pour les sommets de 2 faces, ou pour 2 arêtes
     *
     * On sélectionne une première face maitresse et une seconde qui va aller
     * se coller sur la première.
     * On peut aussi ne sélectionner que 2 sommets.
     * Si désiré, on place chacun des sommets au milieu des couples.
     *
     * Des entités sont ammenées à disparaitre.
     *
     * Les entités sélectionnées doivent être sur un même bloc topologique.
     *
     * \param nom1 une première entité du type sommet, arête ou face commune
     * \param nom2 une deuxième entité du même type que la précédente
     * \param project_on_first si l'on projette sur la première entité, sinon on place au milieu des sommets
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapVertices(std::string nom1, std::string nom2, bool project_on_first);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapVertices(Topo::Vertex* ve1, Topo::Vertex* ve2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapVertices(Topo::CoEdge* ce1, Topo::CoEdge* ce2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapVertices(Topo::CoFace* cf1, Topo::CoFace* cf2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapVertices(std::vector<Topo::Vertex* > vertices1, std::vector<Topo::Vertex* > vertices2, bool project_on_first, TkUtil::UTF8String& scriptCommand);

    /*------------------------------------------------------------------------*/
    /** \brief Construction de la topologie 3D par extrusion/rotation de la topologie 2D
     *  tout en faisant apparaitre un o-grid au centre.
     *
     *  \param coedges_names Les noms des arêtes pour repérer le tracé de l'ogrid
     *  On fait la révolution pour les faces reliées à ces arêtes.
     *  \param dt la portion de révolution
     *  \param ratio_ogrid pour rapprocher de l'axe les sommets internes de l'o-grid
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        makeBlocksByRevol(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    makeBlocksByRevolWithRatioOgrid(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt, const double& ratio_ogrid);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        makeBlocksByRevol(std::vector<CoEdge*>& coedges, const  Utils::Portion::Type& dt, const double& ratio_ogrid);

    /*------------------------------------------------------------------------*/
    /** \brief Construction de la topologie 3D par extrusion de faces de la topologie.
    *
    *  \param cofaces_names Les noms des faces qui vont servir d'origine à l'extrusion
    *  \param dv le vecteur que va suivre l'extrusion pour créer les blocs.
    */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        makeBlocksByExtrude(std::vector<std::string> &cofaces_names, const Utils::Math::Vector& dv);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
    makeBlocksByExtrude(std::vector<TopoEntity*>& cofaces, const Utils::Math::Vector& dv);


    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format MDL (Géométrie et topologie)
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param all à vrai si l'on veut importer toutes les entités, faux si l'on ne prend que ce qui est utile au 2D
     *  \param useAreaName à vrai si l'on souhaite conserver les noms des zones
     *  \param prefixName préfixe pour les noms des groupes des entités importées
     *  \param deg_min degré minimum pour les polynomes des B-Splines
     *  \param deg_max degré maximum pour les polynomes des B-Splines
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        importMDL(std::string n, const bool all, const bool useAreaName,
        		std::string prefixName="", int deg_min=1, int deg_max=2);

    /*------------------------------------------------------------------------*/
    /** \brief Retourne la liste de groupes de noms d'arêtes succeptible d'être fusionnées
     *
     *  \see fuse
     */
    virtual std::vector<std::vector<std::string> > getFusableEdges();
    virtual std::vector<std::vector<Topo::CoEdge*> > getFusableEdgesObj();

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des arêtes communes invalides
     * Par exemple les arêtes qui référencent une arête détruite
     */
    virtual std::vector<std::string> getInvalidEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief Fait la fusion de contours (CoEdges) bout à bout
     *
     *  \see getFusableEdges
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	fuseEdges(std::vector<std::string> &coedges_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	fuseEdges(std::vector<CoEdge*> &coedges);

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 blocs qui ne formeront plus qu'un
     *
     *   Ils doivent partager une face, être dans un même volume et être structurés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Blocks(std::string na, std::string nb);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Blocks(Block* bl_A, Block* bl_B);

    /*------------------------------------------------------------------------*/
    /** Affecte la projection des entités topologiques vers une entité géométrique
     *
     *  Si le nom de l'entité géométrique est vide, on supprime les associations existantes.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setGeomAssociation(std::vector<std::string> & topo_entities_names, std::string geom_entity_name, bool move_vertices);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setGeomAssociation(std::vector<TopoEntity*> & topo_entities, Geom::GeomEntity* geom_entity, bool move_vertices);

    /*------------------------------------------------------------------------*/
    /** Projecte ces sommets topologiques vers l'entité géométrique la plus proche parmis
     *  celles passées en paramêtre
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectVerticesOnNearestGeomEntities(std::vector<std::string> & vertices_names, std::vector<std::string> & geom_entities_names, bool move_vertices);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectVerticesOnNearestGeomEntities(std::vector<Vertex*> & vertices, std::vector<Geom::GeomEntity*> & geom_entities, bool move_vertices);

    /*------------------------------------------------------------------------*/
    /** Projecte ces arêtes topologiques vers la courbe qui relie ses sommets topologiques.
     *  Il peut ne pas y avoir de courbe. La projection se fait également vers les surfaces.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectEdgesOnCurves(std::vector<std::string> & coedges_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectEdgesOnCurves(std::vector<CoEdge*> & coedges);

    /*------------------------------------------------------------------------*/
    /** Projecte toutes les arêtes topologiques vers la courbe qui relie ses sommets topologiques.
     *  Il peut ne pas y avoir de courbe. La projection se fait également vers les surfaces.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    projectAllEdgesOnCurves();

    /*------------------------------------------------------------------------*/
    /** Projecte ces faces topologiques vers la surface en relation avec ses arêtes.
     *  Il peut ne pas y avoir de surface.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    projectFacesOnSurfaces(std::vector<std::string> & cofaces_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectFacesOnSurfaces(std::vector<CoFace*> & cofaces);

    /*------------------------------------------------------------------------*/
    /** Projecte toutes les faces topologiques vers la surface en relation avec ses arêtes.
     *  Il peut ne pas y avoir de surface.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    projectAllFacesOnSurfaces();

    /*------------------------------------------------------------------------*/
    /** Opération de translation de la topologie (et de la géométrie suivant option)
     *
     * La translation des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        translate(std::vector<std::string>& ve, const Vector& dp,
            const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        translate(std::vector<TopoEntity*>& ve, const Vector& dp,
            const bool withGeom);

    /*------------------------------------------------------------------------*/
    /** Opération de rotation de la topologie (et de la géométrie suivant option)
     *
     * La rotation des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param rot      la rotation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        rotate(std::vector<std::string>& ve,
        		const Utils::Math::Rotation& rot,
        		const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        rotate(std::vector<TopoEntity*>& ve,
        		const Utils::Math::Rotation& rot,
        		const bool withGeom);

    /*------------------------------------------------------------------------*/
    /** Opération d'homothétie de la topologie (et de la géométrie suivant option)
     *
     * L'homothétie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param facteur supérieur à 0
     * \param center   le centre (optionnel)
     * \param withGeom si l'homothétie se fait également sur la géométrie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& ve,
            const double& facteur,
            const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& ve,
            const double& facteur,
            const Point& pcentre,
            const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<TopoEntity*>& ve,
            const double& facteur,
            const Point& pcentre,
            const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& ve,
        	const double factorX,
        	const double factorY,
        	const double factorZ,
            const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& ve,
            const double factorX,
            const double factorY,
            const double factorZ,
            const Point& pcentre,
            const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<TopoEntity*>& ve,
        	const double factorX,
			const double factorY,
			const double factorZ,
            const Point& pcentre,
            const bool withGeom);


    /*------------------------------------------------------------------------*/
    /** Opération de symétrie de la topologie (et de la géométrie suivant option)
     *
     * La symétrie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param plane      le plan de symétrie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        mirror(std::vector<std::string>& ve,
        		Utils::Math::Plane* plane,
        		const bool withGeom);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		mirror(std::vector<TopoEntity*>& ve,
				Utils::Math::Plane* plane,
        		const bool withGeom);

    /*------------------------------------------------------------------------*/
    /** Opération de déplacement de sommets
     *
     * Le déplacement se fait pour un ensemble de points,
     * Il est spécifié les coordonnées modifiées et leur  nouvelle valeur
     *
     * \param vertices_names les noms des sommets modifiés
     * \param changeX vrai si la coordonnée en X est à modifier
     * \param xPos la nouvelle valeur pour la coordonnées en X
     * \param changeY vrai si la coordonnée en Y est à modifier
     * \param yPos la nouvelle valeur pour la coordonnées en Y
     * \param changeZ vrai si la coordonnée en Z est à modifier
     * \param zPos la nouvelle valeur pour la coordonnées en Z
     * \param sysCoordName nom du repère dans lequel se fait le changement
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setVertexLocation(std::vector<std::string>& vertices_names,
            const bool changeX,
            const double& xPos,
            const bool changeY,
            const double& yPos,
            const bool changeZ,
            const double& zPos,
			std::string sysCoordName="");

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setVertexLocation(std::vector<Vertex*>& vertices,
            const bool changeX,
            const double& xPos,
            const bool changeY,
            const double& yPos,
            const bool changeZ,
            const double& zPos,
			CoordinateSystem::SysCoord* rep);

    /** Opération de déplacement de sommets
      *
      * Le déplacement se fait pour un ensemble de points,
      * Il est spécifié les coordonnées modifiées et leur  nouvelle valeur
      *
      * \param vertices_names les noms des sommets modifiés
      * \param changeRho vrai si la coordonnée en Rho est à modifier
      * \param rhoPos la nouvelle valeur pour la coordonnées en Rho
      * \param changeTheta vrai si la coordonnée en Theta est à modifier
      * \param thetaPos la nouvelle valeur pour la coordonnées en Theta
      * \param changePhi vrai si la coordonnée en Phi est à modifier
      * \param phiPos la nouvelle valeur pour la coordonnées en Phi
      * \param sysCoordName nom du repère dans lequel se fait le changement
      */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexSphericalLocation(std::vector<std::string>& vertices_names,
			const bool changeRho,
			const double& rhoPos,
			const bool changeTheta,
			const double& thetaPos,
			const bool changePhi,
			const double& phiPos,
			std::string sysCoordName="");

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexSphericalLocation(std::vector<Vertex*>& vertices,
			const bool changeRho,
			const double& rhoPos,
			const bool changeTheta,
			const double& thetaPos,
			const bool changePhi,
			const double& phiPos,
			CoordinateSystem::SysCoord* rep);

    /** Opération de déplacement de sommets
     *
     * Le déplacement se fait pour un ensemble de points,
     * Il est spécifié les coordonnées modifiées et leur  nouvelle valeur
     *
     * \param vertices_names les noms des sommets modifiés
     * \param changeRho vrai si la coordonnée en Rho est à modifier
     * \param rhoPos la nouvelle valeur pour la coordonnées en Rho
     * \param changePhi vrai si la coordonnée en Phi est à modifier
     * \param phiPos la nouvelle valeur pour la coordonnées en Phi
     * \param changeZ vrai si la coordonnée en Z est à modifier
     * \param zPos la nouvelle valeur pour la coordonnées en Z
     * \param sysCoordName nom du repère dans lequel se fait le changement
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexCylindricalLocation(std::vector<std::string>& vertices_names,
			const bool changeRho,
			const double& rhoPos,
			const bool changePhi,
			const double& phiPos,
			const bool changeZ,
			const double& zPos,
			std::string sysCoordName="");

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexCylindricalLocation(std::vector<Vertex*>& vertices,
			const bool changeRho,
			const double& rhoPos,
			const bool changePhi,
			const double& phiPos,
			const bool changeZ,
			const double& zPos,
			CoordinateSystem::SysCoord* rep);


    /*------------------------------------------------------------------------*/
    /** Opération de déplacement d'un sommet vers un autre sommet
     *
     * Non accessible depuis l'IHM
     *
     * \param vertex_name le nom du sommet modifié
     * \param targe_name le nom du sommet cible
     */
	virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexSameLocation(std::string vertex_name,
			std::string target_name);

	virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexSameLocation(Vertex* vtx,
			Vertex* target);

    /*------------------------------------------------------------------------*/
    /** \brief Déplace les sommets sélectionnés en fonction de leur projection
     *
     * \param vertices_names la liste des noms des sommets concernés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapProjectedVertices(std::vector<std::string> &vertices_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapProjectedVertices(std::vector<Vertex*> &vertices);

    /** \brief Déplace tous les sommets en fonction de leur projection
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapAllProjectedVertices();

    /*------------------------------------------------------------------------*/
    /** \brief Aligne les sommets sélectionnés en fonction de leur projection
     *  et des extrémités de la sélection
     *
     *  Il faut sélectionner un ensemble de sommets reliés entre eux par des arêtes.
     *  Les sommets extrémités ne sont pas déplacés.
     *  Les sommets entre ces extrémités sont placés sur l'intersection entre le segment
     *  délimités par les 2 sommets et par la courbe ou surface sur laquelle est faite la projection
     *  et simplement projeté sur le segment s'il n'y a pas de projection
     *  (ou si elle est faite sur un point).
     *
     * \param vertices_names la liste des noms des sommets concernés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        alignVertices(std::vector<std::string> &vertices_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        alignVertices(std::vector<Vertex*> &vertices);


    /** \brief Aligne les sommets sélectionnés en fonction de leur projection
     *  sur la droite définie par 2 points
     *
     *  Il faut sélectionner un ensemble de sommets et 2 points.
     *  Les sommets sont placés sur l'intersection entre la droite définie par les 2 points
     *  et par la courbe ou surface sur laquelle est faite la projection
     *  et simplement projeté sur la droite s'il n'y a pas de projection
     *  (ou si elle est faite sur un point).
     *
     * \param vertices_names la liste des noms des sommets concernés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		alignVertices(const Point& p1, const Point& p2,
			std::vector<std::string> &vertices_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
		alignVertices(const Point& p1, const Point& p2,
			std::vector<Vertex*> &vertices);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste d'arête communes
     *
     *  La propriété emp est clonée.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<std::string> &edge_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<Topo::CoEdge*> &coedges);

    /// Change la propriété de discrétisation pour toutes les arêtes
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setAllMeshingProperty(CoEdgeMeshingProperty& emp);

    /// Change la propriété de discrétisation pour une arête et ses arêtes parallèles
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setParallelMeshingProperty(CoEdgeMeshingProperty& emp, std::string coedge_name);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setParallelMeshingProperty(CoEdgeMeshingProperty& emp, Topo::CoEdge* coedge);

    /*------------------------------------------------------------------------*/
    /** \brief Change le sens de la discrétisation pour une liste d'arête communes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        reverseDirection(std::vector<std::string> &edge_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        reverseDirection(std::vector<Topo::CoEdge*> &coedges);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste de faces communes
     *
     *  La propriété emp est clonée.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<std::string> &face_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<Topo::CoFace*> &cofaces);

    /// Change la propriété de discrétisation pour toutes les faces
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setAllMeshingProperty(CoFaceMeshingProperty& emp);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste de blocs
     *
     *  La propriété emp est clonée.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(BlockMeshingProperty& emp, std::vector<std::string> &bloc_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(BlockMeshingProperty& emp, std::vector<Topo::Block*> &blocs);

    /// Change la propriété de discrétisation pour tous les blocs
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setAllMeshingProperty(BlockMeshingProperty& emp);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une arête commune
     *
     *  La propriété emp est clonée.
     *
     *  \see setMeshingProperty
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, std::string ed);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, Topo::CoEdge* ed);

    /*------------------------------------------------------------------------*/
    /** \brief Retourne la propriété de discrétisation pour une arête
     */
    virtual Topo::CoEdgeMeshingProperty&
        getEdgeMeshingProperty(std::string ed);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une face commune
     *
     *  La propriété emp est clonée.
     *
     *  \see setMeshingProperty
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setFaceMeshingProperty(Topo::CoFaceMeshingProperty& emp, std::string cf);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setFaceMeshingProperty(Topo::CoFaceMeshingProperty& emp, Topo::CoFace* cf);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour un bloc
     *
     *  La propriété emp est clonée.
     *
     *  \see setMeshingProperty
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setBlockMeshingProperty(Topo::BlockMeshingProperty& emp, std::string bl);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setBlockMeshingProperty(Topo::BlockMeshingProperty& emp, Topo::Block* bl);

    /*------------------------------------------------------------------------*/
    /** \brief Déraffine un bloc structuré suivant une direction
     *
     *  On utilise une arête pour déterminer la direction
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        unrefine(std::string nbloc, std::string narete, int ratio);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        unrefine(Block* bloc, CoEdge* arete, int ratio);

    /*------------------------------------------------------------------------*/
    /** \brief Raffine toute les arêtes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
           refine(int ratio);

    /*------------------------------------------------------------------------*/
	/** Retourne le nombre de bras du maillage pour une arête donnée */
	virtual int getNbMeshingEdges(const std::string& name) const;

    /*------------------------------------------------------------------------*/
    /** Retourne la discrétisation par défaut pour les arêtes*/
    virtual int getDefaultNbMeshingEdges();

    /** Commande qui change le nombre de bras par défaut pour une arête */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setDefaultNbMeshingEdges(int nb);

    /*------------------------------------------------------------------------*/
    /** \brief Change la discrétisation pour une arête commune (en nombre de bras seulement)
     * et propage le changement aux arêtes parallèles pour maintenir la structuration
     * suivant les faces communes (si structuré)
     *
     * Un ensemble d'arêtes peuvent être figées pour empécher le changement
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setNbMeshingEdges(std::string edge, int nb, std::vector<std::string>& frozed_edges_names);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setNbMeshingEdges(CoEdge* coedge, int nb, std::vector<CoEdge*>& frozed_coedges);

    /*------------------------------------------------------------------------*/
    /** Ajoute un groupe à un ensemble d'entités topologiques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);

    /** Enlève un groupe à un ensemble d'entités topologiques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);

    /** Défini le groupe pour un ensemble d'entités topologiques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes invalides
     * Par exemple les faces structurées avec des discrétisations inégales
     * pour 2 côtés opposés */
    virtual std::vector<std::string> getInvalidFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes au bord
     *  Ce sont les cofaces reliés à un seul bloc
     * */
    virtual std::vector<std::string> getBorderFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes non reliées à un bloc
     * */
    virtual std::vector<std::string> getFacesWithoutBlock() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes semi-conformes
     * */
    virtual std::vector<std::string> getSemiConformalFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des faces non-structurées
     */
    virtual std::vector<std::string> getUnstructuredFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des faces avec comme méthode de maillage Transfinite
     */
    virtual std::vector<std::string> getTransfiniteMeshLawFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des blocs invalides
    */
    virtual std::vector<std::string> getInvalidBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des blocs visibles
     */
    virtual std::vector<std::string> getVisibleBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des blocs non-structurés
     */
    virtual std::vector<std::string> getUnstructuredBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des blocs avec comme méthode de maillage Transfinite
     */
    virtual std::vector<std::string> getTransfiniteMeshLawBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des blocks gérés par le manager (non détruits)
     */
    virtual std::vector<std::string> getBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces gérées par le manager (non détruites)
     */
    virtual std::vector<std::string> getCoFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des edges gérées par le manager (non détruites)
     */
    virtual std::vector<std::string> getCoEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des vertices gérés par le manager (non détruits)
     */
    virtual std::vector<std::string> getVertices() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des blocs gérées par le manager,
     * optionnellement triés par id
     * */
    virtual void getBlocks(std::vector<Topo::Block* >& blocks, bool sort=false) const;

    /*------------------------------------------------------------------------*/
    /** Retourne le Block suivant le nom en argument */
    virtual Block* getBlock(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute un Block au manager */
    virtual void add(Block* b);

    /** Enlève un Block au manager */
    virtual void remove(Block* b);

    /*------------------------------------------------------------------------*/
    /** Retourne la Face suivant le nom en argument */
    virtual Face* getFace(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une Face au manager */
    virtual void add(Face* f);

    /** Enlève une Face au manager */
    virtual void remove(Face* f);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces communes (non détruites) accessibles depuis le manager
     * Les faces sont ordonnés suivant l'id
     * */
    virtual void getCoFaces(std::vector<Topo::CoFace* >& faces) const;

    /** Retourne la Face commune suivant le nom en argument */
    virtual CoFace* getCoFace(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une CoFace au manager */
    virtual void add(CoFace* f);

    /** Enlève une CoFace au manager */
    virtual void remove(CoFace* f);

    /*------------------------------------------------------------------------*/
    /** Retourne l'arête suivant le nom en argument */
    virtual Edge* getEdge(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une Edge au manager */
    virtual void add(Edge* ce);

    /** Enlève une Edge au manager */
    virtual void remove(Edge* ce);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des arêtes communes (non détruites) accessibles depuis les blocs */
    virtual void getCoEdges(std::vector<Topo::CoEdge* >& edges) const;

    /** Retourne l'arête suivant le nom en argument */
    virtual CoEdge* getCoEdge(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une CoEdge au manager */
    virtual void add(CoEdge* ce);

    /** Enlève une CoEdge au manager */
    virtual void remove(CoEdge* ce);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des vertex (non détruits) accessibles depuis le manager */
    virtual void getVertices(std::vector<Topo::Vertex* >& vertices) const;

    /** Retourne le sommet suivant le nom en argument */
    virtual Vertex* getVertex(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute un Vertex au manager */
    virtual void add(Vertex* v);

    /** Enlève un Vertex au manager */
    virtual void remove(Vertex* v);

	/** Retourne l'entité suivant le nom en argument */
	virtual TopoEntity* getEntity(const std::string& name, const bool exceptionIfNotFound=true) const;


    /** Retourne le nom du sommet en fonction d'une position géométrique */
    virtual std::string getVertexAt(const Point& pt1) const;

    /** Retourne le nom de l'arête en fonction d'un couple de positions géométriques */
    virtual std::string getEdgeAt(const Point& pt1, const Point& pt2) const;

    /** Retourne le nom de la face en fonction des positions géométriques de ses sommets */
    virtual std::string getFaceAt(std::vector<Point>& pts) const;

    /** Retourne le nom du bloc en fonction des positions géométriques de ses sommets */
    virtual std::string getBlockAt(std::vector<Point>& pts) const;

    virtual Mgx3D::Internal::M3DCommandResultIfc* exportBlocks(const std::string& n);

    /*------------------------------------------------------------------------*/
    /// retourne le nombre de blocs non détruits référencés par le TopoManager
    virtual int getNbBlocks() const;

    /** le nombre de faces communes non détruites référencées par le TopoManager */
     virtual int getNbFaces() const;

    /** Retourne le nom du dernier Block non détruit */
    virtual std::string getLastBlock();

    /*------------------------------------------------------------------------*/
    /// retourne le nombre de d'arêtes communes référencées par le TopoManager
    virtual int getNbEdges() const;

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    virtual Topo::TopoInfo getInfos(const std::string& name, int dim) const;

    /** Retourne une string avec les informations sur la direction de l'arête dans le bloc */
    virtual std::string getDirOnBlock(const std::string& aname, const std::string& bname) const;

    /*------------------------------------------------------------------------*/
 	/** retourne les coordonnés d'un sommet */
	virtual Point getCoord(const std::string& name) const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des arêtes communes à 2 faces communes, ou à 2 blocs
     */
    virtual std::vector<std::string> getCommonEdges(const std::string& face1, const std::string& face2, int dim) const;


    /*------------------------------------------------------------------------*/
    /** \brief Aligne le sommet sélectionné sur l'intersection entre la droite définie
     * par 2 points et une surface
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    alignVerticesOnSurface(const std::string &surface, const std::string &vertex, const Point &pnt1, const Point &pnt2);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
    alignVerticesOnSurface(Geom::GeomEntity* surface, Vertex* vertex, const Point &pnt1, const Point &pnt2);


private:
    /** Recherche un vecteur d'entités topologiques suivant leur nom,
     *  lève une exception en cas d'erreur.
     *
     *  La recherche se limite aux entités du type Block, CoFace, CoEdge et Vertex.
     */
    void getTopoEntities(std::vector<std::string> & topo_entities_names,
            std::vector<TopoEntity*> & topo_entities,
            const char* nom_fonction);


private:
    /** blocs accessibles depuis le manager */
    Utils::Container<Block> m_blocks;

    /** faces accessibles depuis le manager */
    Utils::Container<Face> m_faces;

    /** faces communes accessibles depuis le manager */
    Utils::Container<CoFace> m_cofaces;

    /** arêtes accessibles depuis le manager */
    Utils::Container<Edge> m_edges;

    /** arêtes communes accessibles depuis le manager */
    Utils::Container<CoEdge> m_coedges;

    /** sommets accessibles depuis le manager */
    Utils::Container<Vertex> m_vertices;

    /// Nombre de bras par défaut pour une arête
    int m_defaultNbMeshingEdges;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
