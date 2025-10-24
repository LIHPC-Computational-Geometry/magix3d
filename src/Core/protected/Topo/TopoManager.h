/*----------------------------------------------------------------------------*/
#ifndef TOPO_MANAGER_H_
#define TOPO_MANAGER_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResult.h"
#include "Topo/TopoInfo.h"
#include "Topo/Block.h"
#include "Topo/Face.h"
#include "Topo/CoFace.h"
#include "Topo/Edge.h"
#include "Topo/CoEdge.h"
#include "Topo/Vertex.h"
#include "Utils/Container.h"
#include "Utils/Plane.h"
#include "Utils/Constants.h"
#include "Utils/SwigCompletion.h"
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
class TopoManager final : public Internal::CommandCreator {

    friend class CommandChangeDefaultNbMeshingEdges;

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    TopoManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     * Il se charge de la destruction des entités topologiques
     */
    ~TopoManager();

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	void clear();
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Création d'un sommet topologique à partir de coordonnées
     *
     *  \param pt le point contenant les coordonnées
     *  \param groupName le nom du groupe auquel est associée ou non l'entité créée
     */
    Mgx3D::Internal::M3DCommandResult*
    newTopoVertex(Utils::Math::Point pt, std::string groupName);


    /*------------------------------------------------------------------------*/
    /** \brief Création d'une entité topologique à partir de sommets existants
     *
     *  \param dim la dimension de l'entité à créer
     *  \param ve le nom des sommet topologiques qui permettent de créer l'entité
     *  \param groupName le nom du groupe auquel est associée ou non l'entité créée
     */
    Mgx3D::Internal::M3DCommandResult*
    newTopoEntity(std::vector<std::string>& ve, int dim, std::string groupName);
    SET_SWIG_COMPLETABLE_METHOD(newTopoEntity)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
    newTopoEntity(std::vector<Topo::Vertex*>& ve, int dim, std::string groupName);
#endif

	/*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie s'appuyant sur une géométrie
     *
     *  Ne fonctionne que pour une courbe ou un sommet
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     */
    Mgx3D::Internal::M3DCommandResult*
        newTopoOnGeometry(std::string ne);
    SET_SWIG_COMPLETABLE_METHOD(newTopoOnGeometry)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        newTopoOnGeometry(Geom::GeomEntity* ge);
#endif

    /** \brief Création d'une topologie structurée s'appuyant sur une géométrie
      *
      *  Retourne une exception si la topologie créée ne peut pas être rendue structurée
      *
      *  Ne fonctionne que pour un volume ou une surface
      *
      *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
      */
     Mgx3D::Internal::M3DCommandResult*
         newStructuredTopoOnGeometry(std::string ne);
     SET_SWIG_COMPLETABLE_METHOD(newStructuredTopoOnGeometry)

#ifndef SWIG
     Mgx3D::Internal::M3DCommandResult*
     	 newStructuredTopoOnGeometry(Geom::GeomEntity* ge);
#endif

     /** \brief Création d'une face structurée s'appuyant sur une surface
      *
      *  Retourne une exception si la face créée ne peut pas être rendue structurée
      *
      *  Ne fonctionne que pour une surface
      *
      *  \param ne le nom de la surface sur laquelle s'appuiera la topologie
      *  \param ve la liste des (3 ou 4) sommets qui sont aux extrémités topologiques
      */
     Mgx3D::Internal::M3DCommandResult*
	 newStructuredTopoOnSurface(std::string ne, std::vector<std::string>& ve);
    SET_SWIG_COMPLETABLE_METHOD(newStructuredTopoOnSurface)

#ifndef SWIG
     Mgx3D::Internal::M3DCommandResult*
	 newStructuredTopoOnSurface(Geom::Surface* surf, std::vector<Geom::Vertex*>& vertices);
#endif

     /*------------------------------------------------------------------------*/
     /** \brief Création d'un block (ou d'une face) topologique structuré sans association
      *
      *  Ne fonctionne que pour un volume ou une surface
      *  Seule l'entité de plus haut niveau (Block ou CoFace) sera associée à l'entité géométrique
      *
      *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
      */
     Mgx3D::Internal::M3DCommandResult*
         newFreeTopoOnGeometry(std::string ne);
     SET_SWIG_COMPLETABLE_METHOD(newFreeTopoOnGeometry)

#ifndef SWIG
     Mgx3D::Internal::M3DCommandResult*
     	 newFreeTopoOnGeometry(Geom::GeomEntity* ge);
#endif

     /** \brief Création d'un block topologique structuré sans association
        *
        *  L'entité topologique est mise dans un groupe
        *
       *  \param ng le nom du groupe dans lequel sera mis le bloc ou la face
       *  \param dim la dimension (2 ou 3) de ce que l'on veut créer
       */
      Mgx3D::Internal::M3DCommandResult*
          newFreeTopoInGroup(std::string ng,  int dim);
      SET_SWIG_COMPLETABLE_METHOD(newFreeTopoInGroup)

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
     Mgx3D::Internal::M3DCommandResult*
         newFreeBoundedTopoInGroup(std::string ng, int dim, const std::vector<std::string>& ve);
     SET_SWIG_COMPLETABLE_METHOD(newFreeBoundedTopoInGroup)

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie structurée en o-grid
     *
     *   La topologie créée s'adapte à un cylindre (5 blocs) ou à une sphère
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     *  \param rat le ratio de la distance entre l'axe du cylindre et son rayon pour placer le sommet du o-grid
     */
    Mgx3D::Internal::M3DCommandResult*
        newTopoOGridOnGeometry(std::string ne, const double& rat);
    SET_SWIG_COMPLETABLE_METHOD(newTopoOGridOnGeometry)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        newTopoOGridOnGeometry(Geom::GeomEntity* ge, const double& rat);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie non-structurée s'appuyant sur une géométrie
     *
     *   La topologie créée est non-structurée
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     */
    Mgx3D::Internal::M3DCommandResult*
        newUnstructuredTopoOnGeometry(std::string ne);
    SET_SWIG_COMPLETABLE_METHOD(newUnstructuredTopoOnGeometry)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        newUnstructuredTopoOnGeometry(Geom::GeomEntity* ge);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
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
    Mgx3D::Internal::M3DCommandResult*
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
    Mgx3D::Internal::M3DCommandResult*
        newCylinderWithTopo(const Point& pcentre, const double& dr,
                                 const Vector& dv, const double& da,
                                 bool meshStructured, const double& rat,
                                 const int naxe, const int ni, const int nr,
                                 std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newCylinderWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
        newHollowCylinderWithTopo(const Point& pcentre, const double& dr_int, const double& dr_ext,
                                 const Vector& dv, const double& da,
                                 bool meshStructured,
                                 const int naxe, const int ni, const int nr,
                                 std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newHollowCylinderWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
    newConeWithTopo(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da,
			bool meshStructured, const double& rat,
            const int naxe, const int ni, const int nr,
			std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newConeWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
        newSphereWithTopo(const Point& pcentre,  const double& dr,
                                const  Utils::Portion::Type& dt,
                                bool meshStructured, const double& rat,
                                const int ni, const int nr,
                                std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newSphereWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
        newHollowSphereWithTopo(const Point& pcentre,  const double& dr_int,  const double& dr_ext,
            const  Utils::Portion::Type& dt,
            bool meshStructured,
            const int ni, const int nr,
            std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newHollowSphereWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
		newSpherePartWithTopo(const double& radius,
                      const double& angleY,
                      const double& angleZ,
					  const int ni,
					  const int nj,
					  const int nk,
                      std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newSpherePartWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
		newHollowSpherePartWithTopo(const double& dr_int,
				const double& dr_ext,
				const double& angleY,
				const double& angleZ,
				const int ni,
				const int nj,
				const int nk,
				std::string groupName="");
    SET_SWIG_COMPLETABLE_METHOD(newHollowSpherePartWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
        newIJBoxesWithTopo(int ni, int nj, bool alternateStruture);
    SET_SWIG_COMPLETABLE_METHOD(newIJBoxesWithTopo)

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
    Mgx3D::Internal::M3DCommandResult*
        newIJKBoxesWithTopo(int ni, int nj, int nk, bool alternateStruture);
    SET_SWIG_COMPLETABLE_METHOD(newIJKBoxesWithTopo)

    /*------------------------------------------------------------------------*/
    /** \brief Destruction d'entités topologiques
     *
     * \param ve noms des entités topologiques à détruire
     * \param propagate booléen à faux si on ne détruit que les entités,
     *    à vrai si on détruit les entités de dimensions inférieures qui ne sont pas reliées à une autre entité de dimension dim
     */
    void destroy(std::vector<std::string>& ve, bool propagate);
    SET_SWIG_COMPLETABLE_METHOD(destroy)

#ifndef SWIG
    void destroy(std::vector<Topo::TopoEntity*>& ve, bool propagate);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités topologique par copie
     *
     *  \param vb les blocs topologiques que l'on veut copier
     *  \param vo le volume auquel on associe les blocs

     */
    Mgx3D::Internal::M3DCommandResult*
        copy(std::vector<std::string>& vb, std::string vo);
    SET_SWIG_COMPLETABLE_METHOD(copy)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        copy(std::vector<Topo::Block*>& vb, Geom::Volume* vo);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief extraction de blocs par copie et destruction des anciens
     *
     *  \param vb les blocs topologiques que l'on veut extraire
     *  \param ng le nom du groupe 3D dans lequel on va stoquer ces nouveaux blocs

     */
    Mgx3D::Internal::M3DCommandResult*
        extract(std::vector<std::string>& vb, const std::string ng);
    SET_SWIG_COMPLETABLE_METHOD(extract)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        extract(std::vector<Topo::Block*>& vb, const std::string ng);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief duplique des faces communes pour faire apparaitre un trou topologique.
     *  Les sommets et arêtes sont dupliqués s'ils sont reliés uniquement à des blocs touchés
     *
     *  \param faces les faces communes topologiques que l'on veut dupliquer

     */
    Mgx3D::Internal::M3DCommandResult*
        insertHole(std::vector<std::string>& faces);
    SET_SWIG_COMPLETABLE_METHOD(insertHole)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        insertHole(std::vector<CoFace*>& cofaces);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 arêtes communes proches
     *
     *  La 1ère Arête commune prend la place de l'autre.
     *  Les sommets doivent être le plus proche possible
     */
    Mgx3D::Internal::M3DCommandResult*
        fuse2Edges(std::string na, std::string nb);
    SET_SWIG_COMPLETABLE_METHOD(fuse2Edges)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		fuse2Edges(CoEdge* edge_A, CoEdge* edge_B);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion entre 2 groupes d'arêtes communes
     *
     *	Le collage se fait entre sommets les plus proches
     *  Les arêtes communes doivent toute avoir une arête avec qui faire le collage
     */
    Mgx3D::Internal::M3DCommandResult*
		fuse2EdgeList(std::vector<std::string>& coedge_names1, std::vector<std::string>& coedge_names2);
    SET_SWIG_COMPLETABLE_METHOD(fuse2EdgeList)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		fuse2EdgeList(std::vector<Topo::CoEdge* > &coedges1, std::vector<Topo::CoEdge* > &coedges2);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 faces communes
     *
     *  La 1ère Face commune prend la place de l'autre.
     *  Les sommets doivent être le plus proche possible
     */
    Mgx3D::Internal::M3DCommandResult*
        fuse2Faces(std::string na, std::string nb);
    SET_SWIG_COMPLETABLE_METHOD(fuse2Faces)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        fuse2Faces(CoFace* face_A, CoFace* face_B);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion entre 2 groupes de faces communes
     *
     *	Le collage se fait entre sommets les plus proches
     *  Les faces communes doivent toute avoir une face avec qui faire le collage
     */
    Mgx3D::Internal::M3DCommandResult*
        fuse2FaceList(std::vector<std::string>& coface_names1, std::vector<std::string>& coface_names2);
    SET_SWIG_COMPLETABLE_METHOD(fuse2FaceList)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
    	fuse2FaceList(std::vector<Topo::CoFace* > &cofaces1, std::vector<Topo::CoFace* > &cofaces2);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Collage des faces communes entre 2 blocs
     *
     *  Le premier bloc dont les faces prennent la place des autres.
     *  Le deuxième bloc dont les sommets d'une de ses faces doivent être le plus proche possible de celle de l'autre
     */
    Mgx3D::Internal::M3DCommandResult*
        glue2Blocks(std::string na, std::string nb);
    SET_SWIG_COMPLETABLE_METHOD(glue2Blocks)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        glue2Blocks(Block* bl_A, Block* bl_B);
#endif

    /*------------------------------------------------------------------------*/
    /** Collage entre 2 topologies suivant proximité des sommets.
     *  Topo prises dans 2 volumes.
     */
    Mgx3D::Internal::M3DCommandResult*
    	glue2Topo(std::string volName1, std::string volName2);
    SET_SWIG_COMPLETABLE_METHOD(glue2Topo)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
    	glue2Topo(Geom::Volume* vol1, Geom::Volume* vol2);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 sommets
     *
     *  Le 1er sommet prend la place de l'autre.
     */
    Mgx3D::Internal::M3DCommandResult*
        fuse2Vertices(std::string na, std::string nb);
    SET_SWIG_COMPLETABLE_METHOD(fuse2Vertices)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
    	fuse2Vertices(Topo::Vertex* vtx_A, Topo::Vertex* vtx_B);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
        splitBlock(std::string nbloc, std::string narete, const double& ratio);
    SET_SWIG_COMPLETABLE_METHOD(splitBlock)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitBlock(Block* bloc, CoEdge* arete, const double& ratio);

    Mgx3D::Internal::M3DCommandResult*
        splitBlock(Block* bloc, CoEdge* arete, const Point& pt);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un ensembles de blocs en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
     *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const double& ratio);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitBlocks(std::vector<Topo::Block* > &blocs, CoEdge* arete, const double& ratio);
#endif

    /** \brief Découpe un ensembles de blocs en deux
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
     *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitBlocks(std::vector<std::string> &blocs_names, std::string narete, const Point& pt);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitBlocks(std::vector<Topo::Block* > &blocs, CoEdge* arete, const Point& pt);
#endif

    /** \brief Découpe de tous les blocs en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
     *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
     */
    Mgx3D::Internal::M3DCommandResult*
	    splitAllBlocks(std::string narete, const double& ratio);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
	    splitAllBlocks(CoEdge* arete, const double& ratio);
#endif

    /** \brief Découpe de tous les blocs en deux
      *
      *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
      *  qui va être inséré sur l'arête.
      *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
      *  Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2.
      *  Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock.
      */
    Mgx3D::Internal::M3DCommandResult*
        splitAllBlocks(std::string narete, const Point& pt);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitAllBlocks(CoEdge* arete, const Point& pt);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un bloc en deux en utilisant une arête qui coupe déjà une de ses faces
     */
    Mgx3D::Internal::M3DCommandResult*
        extendSplitBlock(std::string nbloc, std::string narete);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        extendSplitBlock(Block* bloc, CoEdge* arete);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
        splitBlocksWithOgridV2(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio_ogrid, int nb_bras);
    SET_SWIG_COMPLETABLE_METHOD(splitBlocksWithOgrid)
    /// version avec comportement équivalent à ce qui est fait dans ICEM, utilisé par défaut jusqu'au début 2019
    Mgx3D::Internal::M3DCommandResult*
        splitBlocksWithOgrid(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);
    SET_SWIG_COMPLETABLE_METHOD(splitBlocksWithOgrid_propagate_neighbor_block)
    /// version avec comportement antérieur à la version 1.6 de Magix3D
    Mgx3D::Internal::M3DCommandResult*
        splitBlocksWithOgrid_old(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);
    SET_SWIG_COMPLETABLE_METHOD(splitBlocksWithOgrid_old)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitBlocksWithOgrid(std::vector<Topo::Block* > &blocs,
            std::vector<Topo::CoFace* > &cofaces,
            const double& ratio_ogrid, int nb_bras,
			bool create_internal_vertices,
			bool propagate_neighbor_block);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
        splitFacesWithOgrid(std::vector<std::string> &faces_names,
            std::vector<std::string> &coedges_names,
            const double& ratio_ogrid, int nb_bras);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitFacesWithOgrid(std::vector<Topo::CoFace*> &cofaces,
            std::vector<Topo::CoEdge*> &coedges,
            const double& ratio_ogrid, int nb_bras);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Découpe des faces structurées en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Si ratio_ogrid vaut 0, alors le tracé de l'ogrid peut passer par l'axe si une face n'a que 3 côtés,
     *  sinon on découpe la face en 3 (ogrid en 2D) avec un sommet placé avec ce ratio entre le sommet dans
     *  le prolongement sur l'axe et le sommet à l'entrée dans la face.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const double& ratio_dec, const double& ratio_ogrid,  bool project_on_meshing_edge = true);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitFaces(std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, const double& ratio_dec, const double& ratio_ogrid, bool project_on_meshing_edge);
#endif

    /** \brief Découpe des faces structurées 2D en deux suivant un point à projeter
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les autres arêtes parallèles sont aussi découpées avec le même ratio induit par le premier sommet.
     *  Si ratio_ogrid vaut 0, alors le tracé de l'ogrid peut passer par l'axe si une face n'a que 3 côté,
     *  sinon on découpe la face en 3 (ogrid en 2D) avec un sommet placé avec ce ratio entre le sommet dans
     *  le prolongement sur l'axe et le sommet à l'entrée dans la face.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitFaces(std::vector<std::string> &cofaces_names, std::string narete, const Point& pt, const double& ratio_ogrid, bool project_on_meshing_edge = true);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitFaces(std::vector<Topo::CoFace* > &cofaces, CoEdge* arete, const Point& pt, const double& ratio_ogrid, bool project_on_meshing_edge);
#endif

    /** \brief Découpe toutes les faces structurées 2D en deux
     * \see splitFaces
     */
    Mgx3D::Internal::M3DCommandResult*
        splitAllFaces(std::string narete, const double& ratio_dec, const double& ratio_ogrid, bool project_on_meshing_edge = true);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitAllFaces(CoEdge* arete, const double& ratio_dec, const double& ratio_ogrid, bool project_on_meshing_edge);
#endif

    /** \brief Découpe toutes les faces structurées en deux suivant un point à projeter
     * \see splitFaces
     */
    Mgx3D::Internal::M3DCommandResult*
        splitAllFaces(std::string narete, const Point& pt, const double& ratio_ogrid, bool project_on_meshing_edge = true);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitAllFaces(CoEdge* arete, const Point& pt, const double& ratio_ogrid, bool project_on_meshing_edge);
#endif

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un ratio
     *
     *	OBSOLETE: Issue201 - Homogénéisation de l'API : utiliser splitFaces
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  L'autre arête parallèle est aussi découpée avec le même ratio.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitFace(std::string coface_name, std::string narete, const double& ratio_dec, bool project_on_meshing_edges);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitFace(Topo::CoFace* coface, CoEdge* coedge, const double& ratio_dec, bool project_on_meshing_edges);
#endif

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un point à projeter
     *
     *	OBSOLETE: Issue201 - Homogénéisation de l'API : utiliser splitFaces
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  L'autre arête parallèle est aussi découpée avec le même ratio induit de la première coupe.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitFace(std::string coface_name, std::string narete, const Point& pt, bool project_on_meshing_edges = true);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitFace(Topo::CoFace* coface, CoEdge* coedge, const Point& pt, bool project_on_meshing_edges);
#endif

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un sommet
     */
    Mgx3D::Internal::M3DCommandResult*
        extendSplitFace(std::string coface_name, std::string nsommet);
    SET_SWIG_COMPLETABLE_METHOD(extendSplitFace)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        extendSplitFace(Topo::CoFace* coface, Vertex* vertex);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Découpe d'une arête commune en 2
     *
     *  On utilise un ratio pour positionner le sommet
     */
    Mgx3D::Internal::M3DCommandResult*
        splitEdge(std::string coedge_name, const double& ratio_dec);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitEdge(CoEdge* coedge, const double& ratio_dec);
#endif

    /** \brief Découpe d'une arête commune en 2
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     */
    Mgx3D::Internal::M3DCommandResult*
        splitEdge(std::string coedge_name, const Point& pt);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        splitEdge(CoEdge* coedge, const Point& pt);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
        snapVertices(std::string nom1, std::string nom2, bool project_on_first);
    SET_SWIG_COMPLETABLE_METHOD(snapVertices)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        snapVertices(Topo::Vertex* ve1, Topo::Vertex* ve2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
    Mgx3D::Internal::M3DCommandResult*
        snapVertices(Topo::CoEdge* ce1, Topo::CoEdge* ce2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
    Mgx3D::Internal::M3DCommandResult*
        snapVertices(Topo::CoFace* cf1, Topo::CoFace* cf2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
    Mgx3D::Internal::M3DCommandResult*
        snapVertices(std::vector<Topo::Vertex* > vertices1, std::vector<Topo::Vertex* > vertices2, bool project_on_first, TkUtil::UTF8String& scriptCommand);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Construction de la topologie 3D par extrusion/rotation de la topologie 2D
     *  tout en faisant apparaitre un o-grid au centre.
     *
     *  \param coedges_names Les noms des arêtes pour repérer le tracé de l'ogrid
     *  On fait la révolution pour les faces reliées à ces arêtes.
     *  \param dt la portion de révolution
     *  \param ratio_ogrid pour rapprocher de l'axe les sommets internes de l'o-grid
     */
    Mgx3D::Internal::M3DCommandResult*
        makeBlocksByRevol(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt);
    SET_SWIG_COMPLETABLE_METHOD(makeBlocksByRevol)

    Mgx3D::Internal::M3DCommandResult*
	    makeBlocksByRevolWithRatioOgrid(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt, const double& ratio_ogrid);
    SET_SWIG_COMPLETABLE_METHOD(makeBlocksByRevolWithRatioOgrid)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        makeBlocksByRevol(std::vector<CoEdge*>& coedges, const  Utils::Portion::Type& dt, const double& ratio_ogrid);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Construction de la topologie 3D par extrusion de faces de la topologie.
    *
    *  \param cofaces_names Les noms des faces qui vont servir d'origine à l'extrusion
    *  \param dv le vecteur que va suivre l'extrusion pour créer les blocs.
    */
    Mgx3D::Internal::M3DCommandResult*
        makeBlocksByExtrude(std::vector<std::string> &cofaces_names, const Utils::Math::Vector& dv);
    SET_SWIG_COMPLETABLE_METHOD(makeBlocksByExtrude)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
    makeBlocksByExtrude(std::vector<TopoEntity*>& cofaces, const Utils::Math::Vector& dv);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
        importMDL(std::string n, const bool all, const bool useAreaName=false,
        		std::string prefixName="", int deg_min=1, int deg_max=2);
    SET_SWIG_COMPLETABLE_METHOD(importMDL)

    /*------------------------------------------------------------------------*/
    /** \brief Retourne la liste de groupes de noms d'arêtes succeptible d'être fusionnées
     *
     *  \see fuse
     */
    std::vector<std::vector<std::string> > getFusableEdges();

#ifndef SWIG
    std::vector<std::vector<Topo::CoEdge*> > getFusableEdgesObj();
#endif

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des arêtes communes invalides
     * Par exemple les arêtes qui référencent une arête détruite
     */
    std::vector<std::string> getInvalidEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief Fait la fusion de contours (CoEdges) bout à bout
     *
     *  \see getFusableEdges
     */
    Mgx3D::Internal::M3DCommandResult*
	fuseEdges(std::vector<std::string> &coedges_names);
    SET_SWIG_COMPLETABLE_METHOD(fuseEdges)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
	fuseEdges(std::vector<CoEdge*> &coedges);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 blocs qui ne formeront plus qu'un
     *
     *   Ils doivent partager une face, être dans un même volume et être structurés
     */
    Mgx3D::Internal::M3DCommandResult*
        fuse2Blocks(std::string na, std::string nb);
    SET_SWIG_COMPLETABLE_METHOD(fuse2Blocks)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        fuse2Blocks(Block* bl_A, Block* bl_B);
#endif

    /*------------------------------------------------------------------------*/
    /** Affecte la projection des entités topologiques vers une entité géométrique
     *
     *  Si le nom de l'entité géométrique est vide, on supprime les associations existantes.
     */
    Mgx3D::Internal::M3DCommandResult*
        setGeomAssociation(std::vector<std::string> & topo_entities_names, std::string geom_entity_name, bool move_vertices);
    SET_SWIG_COMPLETABLE_METHOD(setGeomAssociation)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setGeomAssociation(std::vector<TopoEntity*> & topo_entities, Geom::GeomEntity* geom_entity, bool move_vertices);
#endif

    /*------------------------------------------------------------------------*/
    /** Projecte ces sommets topologiques vers l'entité géométrique la plus proche parmis
     *  celles passées en paramêtre
     */
    Mgx3D::Internal::M3DCommandResult*
        projectVerticesOnNearestGeomEntities(std::vector<std::string> & vertices_names, std::vector<std::string> & geom_entities_names, bool move_vertices);
    SET_SWIG_COMPLETABLE_METHOD(projectVerticesOnNearestGeomEntities)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        projectVerticesOnNearestGeomEntities(std::vector<Vertex*> & vertices, std::vector<Geom::GeomEntity*> & geom_entities, bool move_vertices);
#endif

    /*------------------------------------------------------------------------*/
    /** Projecte ces arêtes topologiques vers la courbe qui relie ses sommets topologiques.
     *  Il peut ne pas y avoir de courbe. La projection se fait également vers les surfaces.
     */
    Mgx3D::Internal::M3DCommandResult*
        projectEdgesOnCurves(std::vector<std::string> & coedges_names);
    SET_SWIG_COMPLETABLE_METHOD(projectEdgesOnCurves)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        projectEdgesOnCurves(std::vector<CoEdge*> & coedges);
#endif

    /*------------------------------------------------------------------------*/
    /** Projecte toutes les arêtes topologiques vers la courbe qui relie ses sommets topologiques.
     *  Il peut ne pas y avoir de courbe. La projection se fait également vers les surfaces.
     */
    Mgx3D::Internal::M3DCommandResult*
	    projectAllEdgesOnCurves();

    /*------------------------------------------------------------------------*/
    /** Projecte ces faces topologiques vers la surface en relation avec ses arêtes.
     *  Il peut ne pas y avoir de surface.
     */
    Mgx3D::Internal::M3DCommandResult*
	    projectFacesOnSurfaces(std::vector<std::string> & cofaces_names);
    SET_SWIG_COMPLETABLE_METHOD(projectFacesOnSurfaces)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        projectFacesOnSurfaces(std::vector<CoFace*> & cofaces);
#endif

    /*------------------------------------------------------------------------*/
    /** Projecte toutes les faces topologiques vers la surface en relation avec ses arêtes.
     *  Il peut ne pas y avoir de surface.
     */
    Mgx3D::Internal::M3DCommandResult*
	    projectAllFacesOnSurfaces();

    /*------------------------------------------------------------------------*/
    /** Opération de translation de la topologie (et de la géométrie suivant option)
     *
     * La translation des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     */
    Mgx3D::Internal::M3DCommandResult*
        translate(std::vector<std::string>& ve, const Vector& dp,
            const bool withGeom);
    SET_SWIG_COMPLETABLE_METHOD(translate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        translate(std::vector<TopoEntity*>& ve, const Vector& dp,
            const bool withGeom);
#endif

    /*------------------------------------------------------------------------*/
    /** Opération de rotation de la topologie (et de la géométrie suivant option)
     *
     * La rotation des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param rot      la rotation
     */
    Mgx3D::Internal::M3DCommandResult*
        rotate(std::vector<std::string>& ve,
        		const Utils::Math::Rotation& rot,
        		const bool withGeom);
    SET_SWIG_COMPLETABLE_METHOD(rotate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        rotate(std::vector<TopoEntity*>& ve,
        		const Utils::Math::Rotation& rot,
        		const bool withGeom);
#endif

    /*------------------------------------------------------------------------*/
    /** Opération d'homothétie de la topologie (et de la géométrie suivant option)
     * L'homothétie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param facteur supérieur à 0
     * \param withGeom si l'homothétie se fait également sur la géométrie
     */
    Mgx3D::Internal::M3DCommandResult*
        scale(std::vector<std::string>& ve,
            const double& facteur,
            const bool withGeom);

    /** Opération d'homothétie de la topologie (et de la géométrie suivant option)
     * L'homothétie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param facteur facteur supérieur à 0
     * \param pcentre le centre
     * \param withGeom si l'homothétie se fait également sur la géométrie
     */
    Mgx3D::Internal::M3DCommandResult*
            scale(std::vector<std::string>& ve,
                const double& facteur,
                const Point& pcentre,
                const bool withGeom);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        scale(std::vector<TopoEntity*>& ve,
            const double& facteur,
            const Point& pcentre,
            const bool withGeom);
#endif

    /** Opération d'homothétie de la topologie (et de la géométrie suivant option)
     * L'homothétie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param factorX facteur suivant X supérieur à 0
     * \param factorY facteur suivant Y supérieur à 0
     * \param factorZ facteur suivant Z supérieur à 0
     * \param withGeom si l'homothétie se fait également sur la géométrie
     */
    Mgx3D::Internal::M3DCommandResult*
        scale(std::vector<std::string>& ve,
        	const double factorX,
        	const double factorY,
        	const double factorZ,
            const bool withGeom);

    /** Opération d'homothétie de la topologie (et de la géométrie suivant option)
     * L'homothétie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param factorX facteur suivant X supérieur à 0
     * \param factorY facteur suivant Y supérieur à 0
     * \param factorZ facteur suivant Z supérieur à 0
     * \param pcentre le centre
     * \param withGeom si l'homothétie se fait également sur la géométrie
     */
    Mgx3D::Internal::M3DCommandResult*
        scale(std::vector<std::string>& ve,
            const double factorX,
            const double factorY,
            const double factorZ,
            const Point& pcentre,
            const bool withGeom);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        scale(std::vector<TopoEntity*>& ve,
        	const double factorX,
			const double factorY,
			const double factorZ,
            const Point& pcentre,
            const bool withGeom);
#endif

    /*------------------------------------------------------------------------*/
    /** Opération de symétrie de la topologie (et de la géométrie suivant option)
     *
     * La symétrie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param plane      le plan de symétrie
     */
    Mgx3D::Internal::M3DCommandResult*
        mirror(std::vector<std::string>& ve,
        		Utils::Math::Plane* plane,
        		const bool withGeom);
    SET_SWIG_COMPLETABLE_METHOD(mirror)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		mirror(std::vector<TopoEntity*>& ve,
				Utils::Math::Plane* plane,
        		const bool withGeom);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
        setVertexLocation(std::vector<std::string>& vertices_names,
            const bool changeX,
            const double& xPos,
            const bool changeY,
            const double& yPos,
            const bool changeZ,
            const double& zPos,
			std::string sysCoordName="");
    SET_SWIG_COMPLETABLE_METHOD(setVertexLocation)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setVertexLocation(std::vector<Vertex*>& vertices,
            const bool changeX,
            const double& xPos,
            const bool changeY,
            const double& yPos,
            const bool changeZ,
            const double& zPos,
			CoordinateSystem::SysCoord* rep);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
	setVertexSphericalLocation(std::vector<std::string>& vertices_names,
			const bool changeRho,
			const double& rhoPos,
			const bool changeTheta,
			const double& thetaPos,
			const bool changePhi,
			const double& phiPos,
			std::string sysCoordName="");
    SET_SWIG_COMPLETABLE_METHOD(setVertexSphericalLocation)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
	setVertexSphericalLocation(std::vector<Vertex*>& vertices,
			const bool changeRho,
			const double& rhoPos,
			const bool changeTheta,
			const double& thetaPos,
			const bool changePhi,
			const double& phiPos,
			CoordinateSystem::SysCoord* rep);
#endif

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
    Mgx3D::Internal::M3DCommandResult*
	setVertexCylindricalLocation(std::vector<std::string>& vertices_names,
			const bool changeRho,
			const double& rhoPos,
			const bool changePhi,
			const double& phiPos,
			const bool changeZ,
			const double& zPos,
			std::string sysCoordName="");
    SET_SWIG_COMPLETABLE_METHOD(setVertexCylindricalLocation)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
	setVertexCylindricalLocation(std::vector<Vertex*>& vertices,
			const bool changeRho,
			const double& rhoPos,
			const bool changePhi,
			const double& phiPos,
			const bool changeZ,
			const double& zPos,
			CoordinateSystem::SysCoord* rep);
#endif

    /*------------------------------------------------------------------------*/
    /** Opération de déplacement d'un sommet vers un autre sommet
     *
     * Non accessible depuis l'IHM
     *
     * \param vertex_name le nom du sommet modifié
     * \param targe_name le nom du sommet cible
     */
	Mgx3D::Internal::M3DCommandResult*
	setVertexSameLocation(std::string vertex_name,
			std::string target_name);
    SET_SWIG_COMPLETABLE_METHOD(setVertexSameLocation)

#ifndef SWIG
	Mgx3D::Internal::M3DCommandResult*
	setVertexSameLocation(Vertex* vtx,
			Vertex* target);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Déplace les sommets sélectionnés en fonction de leur projection
     *
     * \param vertices_names la liste des noms des sommets concernés
     */
    Mgx3D::Internal::M3DCommandResult*
        snapProjectedVertices(std::vector<std::string> &vertices_names);
    SET_SWIG_COMPLETABLE_METHOD(snapProjectedVertices)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        snapProjectedVertices(std::vector<Vertex*> &vertices);
#endif

    /** \brief Déplace tous les sommets en fonction de leur projection
     */
    Mgx3D::Internal::M3DCommandResult*
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
    Mgx3D::Internal::M3DCommandResult*
        alignVertices(std::vector<std::string> &vertices_names);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        alignVertices(std::vector<Vertex*> &vertices);
#endif


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
    Mgx3D::Internal::M3DCommandResult*
		alignVertices(const Point& p1, const Point& p2,
			std::vector<std::string> &vertices_names);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
		alignVertices(const Point& p1, const Point& p2,
			std::vector<Vertex*> &vertices);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste d'arête communes
     *
     *  La propriété emp est clonée.
     */
    Mgx3D::Internal::M3DCommandResult*
        setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<std::string> &edge_names);
    SET_SWIG_COMPLETABLE_METHOD(setMeshingProperty)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setMeshingProperty(CoEdgeMeshingProperty& emp, std::vector<Topo::CoEdge*> &coedges);
#endif

    /// Change la propriété de discrétisation pour toutes les arêtes
    Mgx3D::Internal::M3DCommandResult*
        setAllMeshingProperty(CoEdgeMeshingProperty& emp);
    SET_SWIG_COMPLETABLE_METHOD(setAllMeshingProperty)

    /// Change la propriété de discrétisation pour une arête et ses arêtes parallèles
    Mgx3D::Internal::M3DCommandResult*
        setParallelMeshingProperty(CoEdgeMeshingProperty& emp, std::string coedge_name);
    SET_SWIG_COMPLETABLE_METHOD(setParallelMeshingProperty)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setParallelMeshingProperty(CoEdgeMeshingProperty& emp, Topo::CoEdge* coedge);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Change le sens de la discrétisation pour une liste d'arête communes
     */
    Mgx3D::Internal::M3DCommandResult*
        reverseDirection(std::vector<std::string> &edge_names);
    SET_SWIG_COMPLETABLE_METHOD(reverseDirection)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        reverseDirection(std::vector<Topo::CoEdge*> &coedges);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste de faces communes
     *
     *  La propriété emp est clonée.
     */
    Mgx3D::Internal::M3DCommandResult*
        setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<std::string> &face_names);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setMeshingProperty(CoFaceMeshingProperty& emp, std::vector<Topo::CoFace*> &cofaces);
#endif

    /// Change la propriété de discrétisation pour toutes les faces
    Mgx3D::Internal::M3DCommandResult*
        setAllMeshingProperty(CoFaceMeshingProperty& emp);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste de blocs
     *
     *  La propriété emp est clonée.
     */
    Mgx3D::Internal::M3DCommandResult*
        setMeshingProperty(BlockMeshingProperty& emp, std::vector<std::string> &bloc_names);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setMeshingProperty(BlockMeshingProperty& emp, std::vector<Topo::Block*> &blocs);
#endif

    /// Change la propriété de discrétisation pour tous les blocs
    Mgx3D::Internal::M3DCommandResult*
        setAllMeshingProperty(BlockMeshingProperty& emp);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une arête commune
     *
     *  La propriété emp est clonée.
     *
     *  \see setMeshingProperty
     */
    Mgx3D::Internal::M3DCommandResult*
        setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, std::string ed);
    SET_SWIG_COMPLETABLE_METHOD(setEdgeMeshingProperty)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setEdgeMeshingProperty(CoEdgeMeshingProperty& emp, Topo::CoEdge* ed);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Retourne la propriété de discrétisation pour une arête
     */
    Topo::CoEdgeMeshingProperty&
        getEdgeMeshingProperty(std::string ed);
    SET_SWIG_COMPLETABLE_METHOD(getEdgeMeshingProperty)

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une face commune
     *
     *  La propriété emp est clonée.
     *
     *  \see setMeshingProperty
     */
    Mgx3D::Internal::M3DCommandResult*
        setFaceMeshingProperty(Topo::CoFaceMeshingProperty& emp, std::string cf);
    SET_SWIG_COMPLETABLE_METHOD(setFaceMeshingProperty)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setFaceMeshingProperty(Topo::CoFaceMeshingProperty& emp, Topo::CoFace* cf);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour un bloc
     *
     *  La propriété emp est clonée.
     *
     *  \see setMeshingProperty
     */
    Mgx3D::Internal::M3DCommandResult*
        setBlockMeshingProperty(Topo::BlockMeshingProperty& emp, std::string bl);
    SET_SWIG_COMPLETABLE_METHOD(setBlockMeshingProperty)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setBlockMeshingProperty(Topo::BlockMeshingProperty& emp, Topo::Block* bl);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Déraffine un bloc structuré suivant une direction
     *
     *  On utilise une arête pour déterminer la direction
     */
    Mgx3D::Internal::M3DCommandResult*
        unrefine(std::string nbloc, std::string narete, int ratio);
    SET_SWIG_COMPLETABLE_METHOD(unrefine)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        unrefine(Block* bloc, CoEdge* arete, int ratio);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Raffine toute les arêtes
     */
    Mgx3D::Internal::M3DCommandResult*
           refine(int ratio);
    SET_SWIG_COMPLETABLE_METHOD(refine)

    /*------------------------------------------------------------------------*/
	/** Retourne le nombre de bras du maillage pour une arête donnée */
	int getNbMeshingEdges(const std::string& name) const;

    /*------------------------------------------------------------------------*/
    /** Retourne la discrétisation par défaut pour les arêtes*/
    int getDefaultNbMeshingEdges();

    /** Commande qui change le nombre de bras par défaut pour une arête */
    Mgx3D::Internal::M3DCommandResult*
        setDefaultNbMeshingEdges(int nb);
    SET_SWIG_COMPLETABLE_METHOD(setDefaultNbMeshingEdges)

    /*------------------------------------------------------------------------*/
    /** \brief Change la discrétisation pour une arête commune (en nombre de bras seulement)
     * et propage le changement aux arêtes parallèles pour maintenir la structuration
     * suivant les faces communes (si structuré)
     *
     * Un ensemble d'arêtes peuvent être figées pour empécher le changement
     */
    Mgx3D::Internal::M3DCommandResult*
        setNbMeshingEdges(std::string edge, int nb, std::vector<std::string>& frozed_edges_names);
    SET_SWIG_COMPLETABLE_METHOD(setNbMeshingEdges)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
        setNbMeshingEdges(CoEdge* coedge, int nb, std::vector<CoEdge*>& frozed_coedges);
#endif

    /*------------------------------------------------------------------------*/
    /** Ajoute un groupe à un ensemble d'entités topologiques, suivant une dimension */
    Internal::M3DCommandResult* addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
    SET_SWIG_COMPLETABLE_METHOD(addToGroup)

    /** Enlève un groupe à un ensemble d'entités topologiques, suivant une dimension */
    Internal::M3DCommandResult* removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
    SET_SWIG_COMPLETABLE_METHOD(removeFromGroup)

    /** Défini le groupe pour un ensemble d'entités topologiques, suivant une dimension */
    Internal::M3DCommandResult* setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
    SET_SWIG_COMPLETABLE_METHOD(setGroup)

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes invalides
     * Par exemple les faces structurées avec des discrétisations inégales
     * pour 2 côtés opposés */
    std::vector<std::string> getInvalidFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes au bord
     *  Ce sont les cofaces reliés à un seul bloc
     * */
    std::vector<std::string> getBorderFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes non reliées à un bloc
     * */
    std::vector<std::string> getFacesWithoutBlock() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des faces communes semi-conformes
     * */
    std::vector<std::string> getSemiConformalFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des faces non-structurées
     */
    std::vector<std::string> getUnstructuredFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des faces avec comme méthode de maillage Transfinite
     */
    std::vector<std::string> getTransfiniteMeshLawFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des blocs invalides
    */
    std::vector<std::string> getInvalidBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des blocs visibles
     */
    std::vector<std::string> getVisibleBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des blocs non-structurés
     */
    std::vector<std::string> getUnstructuredBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des noms des blocs avec comme méthode de maillage Transfinite
     */
    std::vector<std::string> getTransfiniteMeshLawBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des blocks gérés par le manager (non détruits)
     */
    std::vector<std::string> getBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces gérées par le manager (non détruites)
     */
    std::vector<std::string> getCoFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des edges gérées par le manager (non détruites)
     */
    std::vector<std::string> getCoEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des vertices gérés par le manager (non détruits)
     */
    std::vector<std::string> getVertices() const;

#ifndef SWIG
    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des blocs gérées par le manager,
     * optionnellement triés par id
     * */
    void getBlocks(std::vector<Topo::Block* >& blocks, bool sort=false) const;

    /*------------------------------------------------------------------------*/
    /** Retourne le Block suivant le nom en argument */
    Block* getBlock(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute un Block au manager */
    void add(Block* b);

    /** Enlève un Block au manager */
    void remove(Block* b);

    /*------------------------------------------------------------------------*/
    /** Retourne la Face suivant le nom en argument */
    Face* getFace(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une Face au manager */
    void add(Face* f);

    /** Enlève une Face au manager */
    void remove(Face* f);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces communes (non détruites) accessibles depuis le manager
     * Les faces sont ordonnés suivant l'id
     * */
    void getCoFaces(std::vector<Topo::CoFace* >& faces) const;

    /** Retourne la Face commune suivant le nom en argument */
    CoFace* getCoFace(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une CoFace au manager */
    void add(CoFace* f);

    /** Enlève une CoFace au manager */
    void remove(CoFace* f);

    /*------------------------------------------------------------------------*/
    /** Retourne l'arête suivant le nom en argument */
    Edge* getEdge(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une Edge au manager */
    void add(Edge* ce);

    /** Enlève une Edge au manager */
    void remove(Edge* ce);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des arêtes communes (non détruites) accessibles depuis les blocs */
    void getCoEdges(std::vector<Topo::CoEdge* >& edges) const;

    /** Retourne l'arête suivant le nom en argument */
    CoEdge* getCoEdge(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute une CoEdge au manager */
    void add(CoEdge* ce);

    /** Enlève une CoEdge au manager */
    void remove(CoEdge* ce);

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des vertex (non détruits) accessibles depuis le manager */
    void getVertices(std::vector<Topo::Vertex* >& vertices) const;

    /** Retourne le sommet suivant le nom en argument */
    Vertex* getVertex(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Ajoute un Vertex au manager */
    void add(Vertex* v);

    /** Enlève un Vertex au manager */
    void remove(Vertex* v);

	/** Retourne l'entité suivant le nom en argument */
	TopoEntity* getEntity(const std::string& name, const bool exceptionIfNotFound=true) const;
#endif

    /** Retourne le nom du sommet en fonction d'une position géométrique */
    std::string getVertexAt(const Point& pt1) const;
    SET_SWIG_COMPLETABLE_METHOD(getVertexAt)

    /** Retourne le nom de l'arête en fonction d'un couple de positions géométriques */
    std::string getEdgeAt(const Point& pt1, const Point& pt2) const;
    SET_SWIG_COMPLETABLE_METHOD(getEdgeAt)

    /** Retourne le nom de la face en fonction des positions géométriques de ses sommets */
    std::string getFaceAt(std::vector<Point>& pts) const;

    /** Retourne le nom du bloc en fonction des positions géométriques de ses sommets */
    std::string getBlockAt(std::vector<Point>& pts) const;

    /** \brief Export dans un fichier au format BLK de toute la topologie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResult* exportBlocks(const std::string& n);

    /** \brief Import de la topologie à partir d'un fichier au format BLK
     *
     *  \param n le nom du ficher à partir duquel on importe
     */
    virtual Mgx3D::Internal::M3DCommandResult* importBlocks(const std::string& n);

    /*------------------------------------------------------------------------*/
    /// retourne le nombre de blocs non détruits référencés par le TopoManager
    int getNbBlocks() const;

    /** le nombre de faces communes non détruites référencées par le TopoManager */
    int getNbFaces() const;

    /** Retourne le nom du dernier Block non détruit */
    std::string getLastBlock();

    /*------------------------------------------------------------------------*/
    /// retourne le nombre de d'arêtes communes référencées par le TopoManager
    int getNbEdges() const;

    /*------------------------------------------------------------------------*/
    /** Récupère des informations relatives à l'entité topologique */
    Topo::TopoInfo getInfos(const std::string& name, int dim) const;
    SET_SWIG_COMPLETABLE_METHOD(getInfos)

    /** Retourne une string avec les informations sur la direction de l'arête dans le bloc */
    std::string getDirOnBlock(const std::string& aname, const std::string& bname) const;
    SET_SWIG_COMPLETABLE_METHOD(getDirOnBlock)

    /*------------------------------------------------------------------------*/
 	/** retourne les coordonnés d'un sommet */
	Point getCoord(const std::string& name) const;
    SET_SWIG_COMPLETABLE_METHOD(getCoord)

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des arêtes communes à 2 faces communes, ou à 2 blocs
     */
    std::vector<std::string> getCommonEdges(const std::string& face1, const std::string& face2, int dim) const;
    SET_SWIG_COMPLETABLE_METHOD(getCommonEdges)

    /*------------------------------------------------------------------------*/
    /** \brief Aligne le sommet sélectionné sur l'intersection entre la droite définie
     * par 2 points et une surface
     */
    Mgx3D::Internal::M3DCommandResult*
    alignVerticesOnSurface(const std::string &surface, const std::string &vertex, const Point &pnt1, const Point &pnt2);
    SET_SWIG_COMPLETABLE_METHOD(alignVerticesOnSurface)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResult*
    alignVerticesOnSurface(Geom::GeomEntity* surface, Vertex* vertex, const Point &pnt1, const Point &pnt2);
#endif

#ifndef SWIG
    /// Retourne les entités topologiques associées à une entité géométrique
    const std::vector<TopoEntity*>& getRefTopos(const Geom::GeomEntity* ge);
    /// Ajoute une association Geom-->Topo
    void addRefTopo(const Geom::GeomEntity* ge, TopoEntity* te);
    /// Positionne les associations Geom-->Topo
    void setRefTopos(const Geom::GeomEntity* ge, const std::vector<TopoEntity*>& tes);
    /// Enlève une association Geom-->Topo
    void removeRefTopo(const Geom::GeomEntity* ge, TopoEntity* te);
    /// Retourne les entités topologiques filtrées
    template <typename T, typename = std::enable_if<std::is_base_of<Topo::TopoEntity, T>::value>>
    std::vector<T*> getFilteredRefTopos(const Geom::GeomEntity* ge)
    {
        std::vector<T*> result;
        for (Topo::TopoEntity* te : getRefTopos(ge)) {
            if (T* casted = dynamic_cast<T*>(te)) {
                result.push_back(casted);
            }
        }
        return result;
    }
#endif

private:
    /** Recherche un vecteur d'entités topologiques suivant leur nom,
     *  lève une exception en cas d'erreur.
     *
     *  La recherche se limite aux entités du type Block, CoFace, CoEdge et Vertex.
     */
    void getTopoEntities(std::vector<std::string> & topo_entities_names,
            std::vector<TopoEntity*> & topo_entities,
            const char* nom_fonction);

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

    /// Relation Geom --> Topo
    std::map<const Geom::GeomEntity*, std::vector<Topo::TopoEntity*>> m_geom_associations;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_MANAGER_H_ */
/*----------------------------------------------------------------------------*/
