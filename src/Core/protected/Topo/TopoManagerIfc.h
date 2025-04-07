/*----------------------------------------------------------------------------*/
/** \file TopoManagerIfc.h
 *
 *  \author Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 01/02/2012
 *
 *
 *  Modified on: 23/02/2022
 *      Author: Simon C
 *      ajout de la fonction de création de blocs par extrusion de faces topologiques
 */
/*----------------------------------------------------------------------------*/
#ifndef TOPO_MANAGER_IFC_H_
#define TOPO_MANAGER_IFC_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Topo/TopoInfo.h"
#include "Utils/Constants.h"
#include "Utils/SwigCompletion.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
namespace Math {
class Point;
class Vector;
class Rotation;
class Plane;
}
}
using Mgx3D::Utils::Math::Point;
using Mgx3D::Utils::Math::Vector;
using Mgx3D::Utils::Math::Rotation;

/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
class CoEdgeMeshingProperty;
class CoFaceMeshingProperty;
class BlockMeshingProperty;
class Block;
class CoFace;
class CoEdge;
class Vertex;
class TopoEntity;

/*----------------------------------------------------------------------------*/
/**
 * \class TopoManagerIfc
 * \brief Interface de gestionnaire des opérations effectuées au niveau du
 *        module topologique.
 *
 * Pour une documentation plus complète, voir TopoManager
 */
class TopoManagerIfc: public Internal::CommandCreator
{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    TopoManagerIfc(const std::string& name, Internal::ContextIfc* c);

	/**
	 * \brief	Destructeur. RAS.
	 */
	virtual ~TopoManagerIfc ( );

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	virtual void clear();
#endif

            /*------------------------------------------------------------------------*/
            /** \brief Création d'un sommet topologique à partir de coordonnées
             *
             *  \param pt le point contenant les coordonnées
             *  \param groupName le nom du groupe auquel est associée ou non l'entité créée
             */
            virtual Mgx3D::Internal::M3DCommandResultIfc*
            newTopoVertex(Utils::Math::Point pt, std::string groupName);


            /*------------------------------------------------------------------------*/
    /** \brief Création d'une entité topologique à partir de sommets existants
     *
     *  \param dim la dimension de l'entité à créer
     *  \param ve le nom des sommet topologiques qui permettent de créer l'entité
     *  \param groupName le nom du groupe auquel est associée ou non l'entité créée
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
            newTopoEntity(std::vector<std::string>& ve, std::string groupName);
    SET_SWIG_COMPLETABLE_METHOD(newTopoEntity)

   /*------------------------------------------------------------------------*/
     /** \brief Création d'une topologie s'appuyant sur une géométrie
      *
      *  Ne fonctionne que pour une courbe ou un sommet
      *
      *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
      */
     virtual Mgx3D::Internal::M3DCommandResultIfc*
         newTopoOnGeometry(std::string ne);
	 SET_SWIG_COMPLETABLE_METHOD(newTopoOnGeometry)

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
	 SET_SWIG_COMPLETABLE_METHOD(newStructuredTopoOnGeometry)

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
	 SET_SWIG_COMPLETABLE_METHOD(newStructuredTopoOnSurface)

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
	SET_SWIG_COMPLETABLE_METHOD(newTopoOGridOnGeometry)

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une topologie non-structurée s'appuyant sur une géométrie
     *
     *   La topologie créée est non-structurée
     *
     *  \param ne le nom de l'entité géométrique sur laquelle s'appuiera la topologie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newUnstructuredTopoOnGeometry(std::string ne);
	SET_SWIG_COMPLETABLE_METHOD(newUnstructuredTopoOnGeometry)

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
	SET_SWIG_COMPLETABLE_METHOD(newFreeTopoOnGeometry)

    /** \brief Création d'un block topologique structuré sans association
       *
       *  L'entité topologique est mise dans un groupe
       *
       *  \param ng le nom du groupe dans lequel sera mis le bloc ou la face
       *  \param dim la dimension (2 ou 3) de ce que l'on veut créer
      */
     virtual Mgx3D::Internal::M3DCommandResultIfc*
         newFreeTopoInGroup(std::string ng, int dim);
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
     virtual Mgx3D::Internal::M3DCommandResultIfc*
         newFreeBoundedTopoInGroup(std::string ng, int dim, const std::vector<std::string>& ve);
	SET_SWIG_COMPLETABLE_METHOD(newFreeBoundedTopoInGroup)
	
    /*------------------------------------------------------------------------*/
    /** \brief Création d'une boite parallèle aux axes Ox,Oy et Oz à partir des
     *         points pmin et pmax où pmin est le point de plus petites
     *         coordonnées (x,y,z) et pmax le point de plus grandes coordonnées
     *         (x,y,z), avec sa topologie associée
     *
     *  \param pmin le point min de la boite
     *  \param pmax le point max de la boite
     *  \param meshStructured un booléen pour choisir si le maillage doit être structuré ou non
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
     newBoxWithTopo(const Point& pmin, const Point& pmax, bool meshStructured=true, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une boite parallèle aux axes Ox,Oy et Oz à partir des
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
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newBoxWithTopo(const Point& pmin, const Point& pmax,
    		const int ni, const int nj, const int nk,
    		std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief Création d'un cylindre avec une topologie, suivant un cercle,
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
    virtual Mgx3D::Internal::M3DCommandResultIfc*
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
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newConeWithTopo(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da,
			bool meshStructured, const double& rat,
            const int naxe, const int ni, const int nr,
			std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newConeWithTopo)

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une sphère avec une topologie, suivant un centre,
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
     *   NB: en passant par les angles, le Huitième équivaut à 45 deg, ce qui empèche la création
     *   d'une sphère ouverte à 45 deg. Mais est-ce nécessaire de proposer autre chose qu'une portion ?
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newHollowSphereWithTopo(const Point& pcentre,
        	const double& dr_int,  const double& dr_ext,
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
    virtual Mgx3D::Internal::M3DCommandResultIfc*
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
    virtual Mgx3D::Internal::M3DCommandResultIfc*
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
    /** \brief Création d'une grille de ni par nj boites répartis sur Ox et Oy
     *         les boites sont toutes de taille 1x1x1 et ont une topologie
     *         associée structurée si  alternateStruture == false
     *         et sinon alternativement une structurée une non-structurée
     *
     *  \param ni le nombre de boite dans la direction Ox
     *  \param nj le nombre de boite dans la direction Oy
     *  \param alternateStruture vrai si l'on veut des blocs alternativement st/non-st
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newIJBoxesWithTopo(int ni, int nj, bool alternateStruture);
	SET_SWIG_COMPLETABLE_METHOD(newIJBoxesWithTopo)

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une grille de ni par nj par nk boites répartis sur Ox, Oy et Oz
     *         les boites sont toutes de taille 1x1x1 et ont une topologie
     *         associée structurée si  alternateStruture == false
     *         et sinon alternativement une structurée une non-structurée
     *
     *  \param ni le nombre de boite dans la direction Ox
     *  \param nj le nombre de boite dans la direction Oy
     *  \param nk le nombre de boite dans la direction Oz
     *  \param alternateStruture vrai si l'on veut des blocs alternativement st/non-st
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newIJKBoxesWithTopo(int ni, int nj, int nk, bool alternateStruture);
	SET_SWIG_COMPLETABLE_METHOD(newIJKBoxesWithTopo)

    /*------------------------------------------------------------------------*/
    /** \brief Destruction d'entités topologiques
     *
     * \param ve noms des entités topologiques à détruire
     * \param propagate booléen à faux si on ne détruit que les entités,
     *    à vrai si on détruit les entités de dimensions inférieures qui ne sont pas reliées à une autre entité de dimension dim
     */
    virtual void destroy(std::vector<std::string>& ve, bool propagate);
	SET_SWIG_COMPLETABLE_METHOD(destroy)

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités topologique par copie
     *
     *  \param vb les blocs topologiques que l'on veut copier
     *  \param vo le volume auquel on associe les blocs

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::vector<std::string>& vb, std::string vo);
	SET_SWIG_COMPLETABLE_METHOD(copy)

    /*------------------------------------------------------------------------*/
    /** \brief extraction de blocs par copie et destruction des anciens
     *
     *  \param vb les blocs topologiques que l'on veut extraire
     *  \param ng le nom du groupe 3D dans lequel on va stoquer ces nouveaux blocs

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extract(std::vector<std::string>& vb, const std::string ng);
	SET_SWIG_COMPLETABLE_METHOD(extract)

    /*------------------------------------------------------------------------*/
    /** \brief duplique des faces communes pour faire apparaitre un trou topologique.
     *  Les sommets et arêtes sont dupliqués s'ils sont reliés uniquement à des blocs touchés
     *
     *  \param faces les faces communes topologiques que l'on veut dupliquer

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        insertHole(std::vector<std::string>& faces);
	SET_SWIG_COMPLETABLE_METHOD(insertHole)

    /*------------------------------------------------------------------------*/
    /** \brief Collage de 2 arêtes communes
     *
     *  La 1ère Arête commune prend la place de l'autre.
     *  Les sommets doivent être le plus proche possible
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Edges(std::string na, std::string nb);
	SET_SWIG_COMPLETABLE_METHOD(fuse2Edges)

    /*------------------------------------------------------------------------*/
    /** \brief Fusion entre 2 groupes d'arêtes communes
     *
     *	Le collage se fait entre sommets les plus proches
     *  Les arêtes communes doivent toute avoir une arête avec qui faire le collage
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		fuse2EdgeList(std::vector<std::string>& coedge_names1, std::vector<std::string>& coedge_names2);
	SET_SWIG_COMPLETABLE_METHOD(fuse2EdgeList)

   /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 faces communes
     *
     *  La 1ère Face commune prend la place de l'autre.
     *  Les sommets doivent être le plus proche possible
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Faces(std::string na, std::string nb);
	SET_SWIG_COMPLETABLE_METHOD(fuse2Faces)

    /*------------------------------------------------------------------------*/
    /** \brief Fusions entre 2 groupes de faces communes
     *
     *	Le collage se fait entre sommets les plus proches
     *  Les faces communes doivent toute avoir une face avec qui faire le collage
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2FaceList(std::vector<std::string>& coface_names1, std::vector<std::string>& coface_names2);
	SET_SWIG_COMPLETABLE_METHOD(fuse2FaceList)

    /*------------------------------------------------------------------------*/
    /** \brief Collage des faces communes entre 2 blocs
     *
     *  Le premier bloc dont les faces prennent la place des autres.
     *  Le deuxième bloc dont les sommets d'une de ses faces doivent être le plus proche possible de celle de l'autre
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        glue2Blocks(std::string na, std::string nb);
	SET_SWIG_COMPLETABLE_METHOD(glue2Blocks)

    /*------------------------------------------------------------------------*/
    /** Collage entre 2 topologies suivant proximité des sommets.
     *  Topo prises dans 2 volumes.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* glue2Topo(
    		std::string volName1, std::string volName2);
	SET_SWIG_COMPLETABLE_METHOD(glue2Topo)

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 sommets
     *
     *  Le 1er sommet prend la place de l'autre.
     *  Attention, les arêtes et les faces ne sont pas fusionnées pour autant
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Vertices(std::string na, std::string nb);
	SET_SWIG_COMPLETABLE_METHOD(fuse2Vertices)

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un bloc en deux
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio.
     *  Une face basée sur ces 4 arêtes est créée, elle coupe le bloc en 2.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlock(std::string nbloc, std::string narete, const double& ratio);
	SET_SWIG_COMPLETABLE_METHOD(splitBlock)

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

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un bloc en deux en utilisant une arête qui coupe déjà une de ses faces
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extendSplitBlock(std::string nbloc, std::string narete);

    /*------------------------------------------------------------------------*/
    /** \brief Découpe un ensemble de blocs suivant un o-grid
     *
     *  On utilise un ratio pour positionner les sommets par rapport
     *  aux centres des blocs
     *  \param blocs_names la liste des noms de blocs à découper
     *  \param cofaces_names la liste des faces communes à considérer comme étant entre 2 blocs
     *  (ce qui permet de ne pas construire le bloc contre ces faces communes)
     *  \param ratio ratio pour positionner les nouveaux sommets,
     *  entre les sommets du bord de la sélection et le centre des blocs ou faces concernées
     *  \param nb_bras le nombre de couches de mailles autour du centre du o-grid à créer
     *  (c.a.d. le nombre de bras pour les arêtes entre les sommets du bord de la sélection et ceux créés)
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgridV2(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);
	SET_SWIG_COMPLETABLE_METHOD(splitBlocksWithOgrid)
    /// version avec comportement équivalent à ce qui est fait dans ICEM, utilisé par défaut jusqu'au début 2019
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgrid(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);
	SET_SWIG_COMPLETABLE_METHOD(splitBlocksWithOgrid_propagate_neighbor_block)
    /// version avec comportement antérieur à la version 1.6 de Magix3D
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitBlocksWithOgrid_old(std::vector<std::string> &blocs_names,
            std::vector<std::string> &cofaces_names,
            const double& ratio, int nb_bras);
	SET_SWIG_COMPLETABLE_METHOD(splitBlocksWithOgrid_old)


    /*------------------------------------------------------------------------*/
    /** \brief Découpe un ensemble de faces suivant un o-grid
     *
     *  On utilise un ratio pour positionner les sommets par rapport
     *  aux centres des faces
     *  \param faces_names la liste des noms de faces à découper
     *  \param coedges_names la liste des arêtes communes à considérer comme étant entre 2 faces
     *  (ce qui permet de ne pas construire la face contre ces arêtes communes)
     *  \param ratio ratio pour positionner les nouveaux sommets,
     *  entre les sommets du bord de la sélection et le centre des faces ou arêtes concernées
     *  \param nb_bras le nombre de couches de mailles autour du centre du o-grid à créer
     *  (c.a.d. le nombre de bras pour les arêtes entre les sommets du bord de la sélection et ceux créés)
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFacesWithOgrid(std::vector<std::string> &faces_names,
            std::vector<std::string> &coedges_names,
            const double& ratio, int nb_bras);


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

    /** \brief Découpe toutes les faces structurées 2D en deux
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllFaces(std::string narete, const double& ratio_dec = 0.5, const double& ratio_ogrid = 0.5);

    /** \brief Découpe toutes les faces structurées 2D en deux suivant un point à projeter
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitAllFaces(std::string narete, const Point& pt, const double& ratio_ogrid = 0.5);

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un ratio
     *
     *  On utilise une arête et un ratio pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  L'autre arête parallèle est aussi découpée avec le même ratio.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFace(std::string coface_name, std::string narete, const double& ratio_dec, bool project_on_meshing_edges = true);

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un point à projeter
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     *  L'autre arête parallèle est aussi découpée avec le même ratio induit de la première coupe.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitFace(std::string coface_name, std::string narete, const Point& pt, bool project_on_meshing_edges = true);

    /** \brief Découpage d'une face structurée 2D ou 3D en deux suivant un sommet
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        extendSplitFace(std::string coface_name, std::string nsommet);
	SET_SWIG_COMPLETABLE_METHOD(extendSplitFace)

    /*------------------------------------------------------------------------*/
    /** \brief Découpe d'une arête commune en 2
     *
     *  On utilise un ratio pour positionner le sommet
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitEdge(std::string coedge_name, const double& ratio_dec = 0.5);

    /** \brief Découpe d'une arête commune en 2
     *
     *  On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet
     *  qui va être inséré sur l'arête.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        splitEdge(std::string coedge_name,  const Point& pt);

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
	SET_SWIG_COMPLETABLE_METHOD(snapVertices)

    /*------------------------------------------------------------------------*/
    /** \brief Construction de la topologie 3D par extrusion/rotation de la topologie 2D
     *  tout en faisant apparaitre un o-grid au centre.
     *
     *  \param coedges_names Les noms des arêtes pour repérer le tracé de l'ogrid
     *  On fait la révolution pour les faces reliées à ces arêtes.
     *  \param dt la portion de révolution.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        makeBlocksByRevol(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt);
	SET_SWIG_COMPLETABLE_METHOD(makeBlocksByRevol)

    /** \brief Construction de la topologie 3D par extrusion/rotation de la topologie 2D
     *  tout en faisant apparaitre un o-grid au centre.
     *
     *  \param coedges_names Les noms des arêtes pour repérer le tracé de l'ogrid
     *  On fait la révolution pour les faces reliées à ces arêtes.
     *  \param dt la portion de révolution.
     *  \param ratio_ogrid pour rapprocher de l'axe les sommets internes de l'o-grid
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        makeBlocksByRevolWithRatioOgrid(std::vector<std::string> &coedges_names, const  Utils::Portion::Type& dt, const double& ratio_ogrid);
	SET_SWIG_COMPLETABLE_METHOD(makeBlocksByRevolWithRatioOgrid)

    /*------------------------------------------------------------------------*/
    /** \brief Construction de la topologie 3D par extrusion de faces de la topologie.
    *
    *  \param cofaces_names Les noms des faces qui vont servir d'origine à l'extrusion
    *  \param dv le vecteur que va suivre l'extrusion pour créer les blocs.
    */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        makeBlocksByExtrude(std::vector<std::string> &cofaces_names, const Utils::Math::Vector& dv);
    SET_SWIG_COMPLETABLE_METHOD(makeBlocksByExtrude)

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste d'arête communes
     *
     *  La propriété emp est clonée.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(Topo::CoEdgeMeshingProperty& emp, std::vector<std::string> &edge_names);
	SET_SWIG_COMPLETABLE_METHOD(setMeshingProperty)

    /// Change la propriété de discrétisation pour toutes les arêtes
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setAllMeshingProperty(Topo::CoEdgeMeshingProperty& emp);
	SET_SWIG_COMPLETABLE_METHOD(setAllMeshingProperty)

    /// Change la propriété de discrétisation pour une arête et ses arêtes parallèles
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setParallelMeshingProperty(CoEdgeMeshingProperty& emp, std::string coedge_name);
	SET_SWIG_COMPLETABLE_METHOD(setParallelMeshingProperty)

    /*------------------------------------------------------------------------*/
    /** \brief Change le sens de la discrétisation pour une liste d'arête communes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        reverseDirection(std::vector<std::string> &edge_names);
	SET_SWIG_COMPLETABLE_METHOD(reverseDirection)

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste faces communes
     *
     *  La propriété emp est clonée.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(Topo::CoFaceMeshingProperty& emp, std::vector<std::string> &face_names);

    /// Change la propriété de discrétisation pour toutes les faces
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setAllMeshingProperty(Topo::CoFaceMeshingProperty& emp);

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une liste de blocs
     *
     *  La propriété emp est clonée.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setMeshingProperty(Topo::BlockMeshingProperty& emp, std::vector<std::string> &bloc_names);

    /// Change la propriété de discrétisation pour tous les blocs
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setAllMeshingProperty(Topo::BlockMeshingProperty& emp);


    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une arête
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setEdgeMeshingProperty(Topo::CoEdgeMeshingProperty& emp, std::string ed);
	SET_SWIG_COMPLETABLE_METHOD(setEdgeMeshingProperty)

    /*------------------------------------------------------------------------*/
    /** \brief Retourne la propriété de discrétisation pour une arête
     */
    virtual Topo::CoEdgeMeshingProperty&
        getEdgeMeshingProperty(std::string ed);
	SET_SWIG_COMPLETABLE_METHOD(getEdgeMeshingProperty)

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour une face commune
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setFaceMeshingProperty(Topo::CoFaceMeshingProperty& emp, std::string cf);
	SET_SWIG_COMPLETABLE_METHOD(setFaceMeshingProperty)

    /*------------------------------------------------------------------------*/
    /** \brief Change la propriété de discrétisation pour un bloc
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setBlockMeshingProperty(Topo::BlockMeshingProperty& emp, std::string bl);
	SET_SWIG_COMPLETABLE_METHOD(setBlockMeshingProperty)

    /*------------------------------------------------------------------------*/
    /** \brief Change la discrétisation pour une arête commune (en nombre de bras seulement)
     * et propage le changement aux arêtes parallèles pour maintenir la structuration
     * suivant les faces communes (si structuré)
     *
     * Un ensemble d'arêtes peuvent être figées pour empécher le changement
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setNbMeshingEdges(std::string edge, int nb, std::vector<std::string>& frozed_edges_names);
	SET_SWIG_COMPLETABLE_METHOD(setNbMeshingEdges)

    /*------------------------------------------------------------------------*/
    /** \brief Déraffine un bloc structuré suivant une direction
     *
     *  On utilise une arête pour déterminer la direction
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        unrefine(std::string nbloc, std::string narete, int ratio);
	SET_SWIG_COMPLETABLE_METHOD(unrefine)

    /*------------------------------------------------------------------------*/
    /** \brief Raffine toute les arêtes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        refine(int ratio);
	SET_SWIG_COMPLETABLE_METHOD(refine)

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
        importMDL(std::string n, const bool all, const bool useAreaName=false,
        		std::string prefixName="",int deg_min=1, int deg_max=2);
	SET_SWIG_COMPLETABLE_METHOD(importMDL)

    /*------------------------------------------------------------------------*/
    /** \brief Retourne la liste de groupes de noms d'arêtes succeptible d'être fusionnées
     *
     */
    virtual std::vector<std::vector<std::string> > getFusableEdges();

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des arêtes communes invalides
     * Par exemple les arêtes qui référencent une arête détruite
     */
    virtual std::vector<std::string> getInvalidEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief Fait la fusion de contours (Topo::CoEdges)
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    fuseEdges(std::vector<std::string> &coedges_names);
	SET_SWIG_COMPLETABLE_METHOD(fuseEdges)

    /*------------------------------------------------------------------------*/
    /** \brief Fusion de 2 blocs qui ne formeront plus qu'un
     *
     *   Ils doivent partager une face, être dans un même volume et être structurés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse2Blocks(std::string na, std::string nb);
	SET_SWIG_COMPLETABLE_METHOD(fuse2Blocks)

    /*------------------------------------------------------------------------*/
    /** Affecte la projection des entités topologiques vers une entité géométrique
     *
     *  Si le nom de l'entité géométrique est vide, on supprime les associations existantes.
     *  Si l'association se fait sur une courbe mais que l'extrémité de celle-ci est très proche, alors on associe automatiquement avec le point extrémité.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setGeomAssociation(std::vector<std::string> & topo_entities_names, std::string geom_entity_name, bool move_vertices);
	SET_SWIG_COMPLETABLE_METHOD(setGeomAssociation)

    /*------------------------------------------------------------------------*/
    /** Projecte ces sommets topologiques vers l'entité géométrique la plus proche parmis
     *  celles passées en paramêtre
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectVerticesOnNearestGeomEntities(std::vector<std::string> & vertices_names, std::vector<std::string> & geom_entities_names, bool move_vertices);
	SET_SWIG_COMPLETABLE_METHOD(projectVerticesOnNearestGeomEntities)

    /*------------------------------------------------------------------------*/
    /** Projecte ces arêtes topologiques vers la courbe qui relie ses sommets topologiques.
     *  Il peut ne pas y avoir de courbe. La projection se fait également vers les surfaces.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        projectEdgesOnCurves(std::vector<std::string> & coedges_names);
	SET_SWIG_COMPLETABLE_METHOD(projectEdgesOnCurves)

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
	SET_SWIG_COMPLETABLE_METHOD(projectFacesOnSurfaces)

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
     *
     * \param dp le vecteur de translation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        translate(std::vector<std::string>& ve, const Vector& dp,
            const bool withGeom);
	SET_SWIG_COMPLETABLE_METHOD(translate)

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
	SET_SWIG_COMPLETABLE_METHOD(rotate)

    /*------------------------------------------------------------------------*/
    /** Opération d'homothétie de la topologie (et de la géométrie suivant option)
     * L'homothétie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param facteur supérieur à 0
     * \param withGeom si l'homothétie se fait également sur la géométrie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
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
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& ve,
            const double& facteur,
            const Point& pcentre,
            const bool withGeom);
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
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& ve,
        	const double factorX,
        	const double factorY,
        	const double factorZ,
            const bool withGeom);

    /*------------------------------------------------------------------------*/
    /** Opération de symétrie de la topologie (et de la géométrie suivant option)
     *
     * La symétrie des objets topologiques de haut niveau (bloc par exemple)
     * entraine celle des objets de niveau inférieurs (face, arêtes et sommets)
     * et réciproquement.
     *
     * \param plane      le plan de symétrie
     * \param withGeom   vrai si l'on souhaite effectuer la symétrie des entités géométriques sous-jacentes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        mirror(std::vector<std::string>& ve,
        		Utils::Math::Plane* plane,
        		const bool withGeom);
	SET_SWIG_COMPLETABLE_METHOD(mirror)


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
	SET_SWIG_COMPLETABLE_METHOD(setVertexLocation)

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
	SET_SWIG_COMPLETABLE_METHOD(setVertexSphericalLocation)

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
	SET_SWIG_COMPLETABLE_METHOD(setVertexCylindricalLocation)

    /*------------------------------------------------------------------------*/
    /** Opération de déplacement d'un sommet vers un autre sommet
     *
     * \param vertex_name le nom du sommet modifié
     * \param target_name le nom du sommet cible
     */
	virtual Mgx3D::Internal::M3DCommandResultIfc*
	setVertexSameLocation(std::string vertex_name,
			std::string target_name);
	SET_SWIG_COMPLETABLE_METHOD(setVertexSameLocation)

   /*------------------------------------------------------------------------*/
    /** Déplace les sommets sélectionnés en fonction de leur projection
     *
     * \param vertices_names la liste des noms des sommets concernés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        snapProjectedVertices(std::vector<std::string> &vertices_names);
	SET_SWIG_COMPLETABLE_METHOD(snapProjectedVertices)

    /** Déplace tous les sommets en fonction de leur projection
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
     *  Les sommets sont replacés entre les points extrémités dans le cas où ces derniers
     *  seraient supperposés aux extrémités
     *
     * \param vertices_names la liste des noms des sommets concernés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        alignVertices(std::vector<std::string> &vertices_names);

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

    /*------------------------------------------------------------------------*/
    /** le nombre de blocs référencés par le TopoManager
     */
    virtual int getNbBlocks() const;

    /** le nombre de faces référencées par le TopoManager */
     virtual int getNbFaces() const;

    /** Récupère des informations relatives à l'entité topologique */
    virtual Topo::TopoInfo getInfos(const std::string& name, int dim) const;
	SET_SWIG_COMPLETABLE_METHOD(getInfos)

    /** Retourne une string avec les informations sur la direction de l'arête dans le bloc */
    virtual std::string getDirOnBlock(const std::string& aname, const std::string& bname) const;
	SET_SWIG_COMPLETABLE_METHOD(getDirOnBlock)

	/** retourne les coordonnés d'un sommet */
	virtual Point getCoord(const std::string& name) const;
	SET_SWIG_COMPLETABLE_METHOD(getCoord)

    /*------------------------------------------------------------------------*/
	/** Retourne le nombre de bras du maillage pour une arête donnée */
	virtual int getNbMeshingEdges(const std::string& name) const;

	/*------------------------------------------------------------------------*/
    /** Retourne la discrétisation par défaut pour les arêtes*/
    virtual int getDefaultNbMeshingEdges();

    /** Commande qui change le nombre de bras par défaut pour une arête */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        setDefaultNbMeshingEdges(int nb);
	SET_SWIG_COMPLETABLE_METHOD(setDefaultNbMeshingEdges)

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
    /** Retourne la liste des faces communes invalides
     * Par exemple les faces structurées avec des discrétisations inégales
     * pour 2 côtés opposés */
    virtual std::vector<std::string> getInvalidFaces() const;

    /** \brief retourne la liste des noms des faces non-structurées
     */
    virtual std::vector<std::string> getUnstructuredFaces() const;

    /** \brief retourne la liste des noms des faces avec comme méthode de maillage Transfinite
     */
    virtual std::vector<std::string> getTransfiniteMeshLawFaces() const;

    /*------------------------------------------------------------------------*/
    /** Retourne la liste des blocs invalides
    */
    virtual std::vector<std::string> getInvalidBlocks() const;

    /** \brief retourne la liste des noms des blocs visibles
     */
    virtual std::vector<std::string> getVisibleBlocks() const;

    /** \brief retourne la liste des noms des blocs non-structurés
     */
    virtual std::vector<std::string> getUnstructuredBlocks() const;

    /** \brief retourne la liste des noms des blocs avec comme méthode de maillage Transfinite
     */
    virtual std::vector<std::string> getTransfiniteMeshLawBlocks() const;

    /*------------------------------------------------------------------------*/
    /** Ajoute un groupe à un ensemble d'entités topologiques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
	SET_SWIG_COMPLETABLE_METHOD(addToGroup)

    /** Enlève un groupe à un ensemble d'entités topologiques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
	SET_SWIG_COMPLETABLE_METHOD(removeFromGroup)

    /** Défini le groupe pour un ensemble d'entités topologiques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
	SET_SWIG_COMPLETABLE_METHOD(setGroup)

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des blocks gérés par le manager (non détruits)
     */
    virtual std::vector<std::string> getBlocks() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces gérées par le manager (non détruites)
     */
    virtual std::vector<std::string> getCoFaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces gérées par le manager (non détruites)
     */
    virtual std::vector<std::string> getCoEdges() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces gérées par le manager (non détruites)
     */
    virtual std::vector<std::string> getVertices() const;
#ifndef SWIG

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des blocs gérées par le manager (non détruits),
     * optionnellement triés par id
     * */
    virtual void getBlocks(std::vector<Topo::Block* >& blocks, bool sort=false) const;

    /** Retourne le Block suivant le nom en argument */
    virtual Block* getBlock(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des faces communes (non détruites) accessibles depuis le manager
     * Les faces sont ordonnés suivant l'id
     * */
    virtual void getCoFaces(std::vector<Topo::CoFace* >& faces) const;

    /** Retourne la Face commune suivant le nom en argument */
    virtual CoFace* getCoFace(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des arêtes communes (non détruites) accessibles depuis les blocs */
    virtual void getCoEdges(std::vector<Topo::CoEdge* >& edges) const;

    /** Retourne l'arête suivant le nom en argument */
    virtual CoEdge* getCoEdge(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des vertex (non détruits) accessibles depuis les blocs */
    virtual void getVertices(std::vector<Topo::Vertex* >& vertices) const;

    /** Retourne le sommet suivant le nom en argument */
    virtual Vertex* getVertex(const std::string& name, const bool exceptionIfNotFound=true) const;

    /** Retourne l'entité suivant le nom en argument */
    virtual TopoEntity* getEntity(const std::string& name, const bool exceptionIfNotFound=true) const;

#endif	// #ifndef SWIG

    /*------------------------------------------------------------------------*/
    /** \brief Aligne le sommet sélectionné sur l'intersection entre la droite définie
     * par 2 points et une surface
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* alignVerticesOnSurface(const std::string &surface, const std::string &vertex,
                                                                         const Point &pnt1,const Point &pnt2);
    SET_SWIG_COMPLETABLE_METHOD(alignVerticesOnSurface)

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des arêtes communes à 2 faces communes, ou à 2 blocs
     */
    virtual std::vector<std::string> getCommonEdges(const std::string& face1, const std::string& face2, int dim) const;
    SET_SWIG_COMPLETABLE_METHOD(getCommonEdges)

    /** Retourne le nom du sommet en fonction d'une position géométrique */
    virtual std::string getVertexAt(const Point& pt1) const;
	SET_SWIG_COMPLETABLE_METHOD(getVertexAt)

    /** Retourne le nom de l'arête en fonction d'un couple de positions géométriques */
    virtual std::string getEdgeAt(const Point& pt1, const Point& pt2) const;
	SET_SWIG_COMPLETABLE_METHOD(getEdgeAt)

    /** Retourne le nom de la face en fonction des positions géométriques de ses sommets */
    virtual std::string getFaceAt(std::vector<Point>& pts) const;
//	SET_SWIG_COMPLETABLE_METHOD(getFaceAt) [EB] pas de complétion possible pour cas vector<...>

    /** Retourne le nom du bloc en fonction des positions géométriques de ses sommets */
    virtual std::string getBlockAt(std::vector<Point>& pts) const;
//	SET_SWIG_COMPLETABLE_METHOD_RET(std::string, getBlockAt)

    virtual Mgx3D::Internal::M3DCommandResultIfc* exportBlocks(const std::string& n);
    SET_SWIG_COMPLETABLE_METHOD(exportBlocksForCGNS)

private:

	/** Constructeur de copie : interdit. */
	TopoManagerIfc (const TopoManagerIfc&);

	/** Opérateur = : interdit. */
	TopoManagerIfc& operator = (const TopoManagerIfc&);

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* TOPO_MANAGER_IFC_H_ */
/*----------------------------------------------------------------------------*/
