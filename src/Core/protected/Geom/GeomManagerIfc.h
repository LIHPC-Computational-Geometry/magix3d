/*----------------------------------------------------------------------------*/
/** \file GeomManagerIfc.h
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle, Charles Pignerol
 *
 *  \date 01/02/2012
 *
 *
 *
 *  Modified on: 21/02/2022
 *      Author: Simon C
 *      ajout de la possibilité de conserver les entités pour la fonction makeExtrude
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GEOMMANAGERIFC_H_
#define MGX3D_GEOM_GEOMMANAGERIFC_H_
/*----------------------------------------------------------------------------*/
#include <vector>
#include <string>
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Utils/Constants.h"
#include "Geom/GeomInfo.h"
#include "Utils/Vector.h"
#include "Utils/SwigCompletion.h"

/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
namespace Math {
class Point;
class Plane;
//class Vector;
class Rotation;
}
}
using Mgx3D::Utils::Math::Plane;
using Mgx3D::Utils::Math::Point;
using Mgx3D::Utils::Math::Vector;
using Mgx3D::Utils::Math::Rotation;

/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
class GeomEntity;

/*----------------------------------------------------------------------------*/
/**
 * \class GeomManagerIfc
 * \brief Interface du gestionnaire des opérations effectuées au niveau du
 *        module géométrique.
 *
 *        Ce gestionnaire joue le rôle de fabrique de commande, mais aussi d'
 *        entités géométriques. La raison est technique et non pas conceptuelle.
 *        Lorsque l'on crée une entité géométrique via une commande, les entités
 *        incidentes de dimension inférieure doivent aussi être crées puis
 *        stockées au niveau du GeomManager. Il est donc le plus à même de remplir
 *        ce rôle.
 */
class GeomManagerIfc : public Internal::CommandCreator {
public:


	/*------------------------------------------------------------------------*/
	/**
	 * \brief		Destructeur. RAS.
	 */
	virtual ~GeomManagerIfc ( );

	/*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	virtual void clear();
#endif
    /*------------------------------------------------------------------------*/
    /** \brief récupère des informations sur les entités géométriques.
     *
     *  \param e l'entité géométrique dont on veut des informations.
     */
    virtual GeomInfo getInfos(std::string e, int dim);
	SET_SWIG_COMPLETABLE_METHOD(getInfos)

	/** retourne les coordonnés d'un sommet */
	virtual Point getCoord(const std::string& name) const;
	SET_SWIG_COMPLETABLE_METHOD(getCoord)

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités géométrique par copie
     *
     *  \param e les entités géométriques que l'on veut copier
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     * NB: [EB] pas de paramètre par défaut (groupName="") lorsqu'il y a un std::vector en argument (pb dans swig)
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::vector<std::string>& e, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copy)

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités géométrique par copie
     *
     *  \param e les surfaces géométriques qui entoure le volume à créer
     *  \param groupName un nom de groupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* newVolume(std::vector<std::string>& e, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newVolume)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'un point p(x,y,z)
     *         et d'une courbe ou surface. Le sommet crée est le projet de p sur la
     *         courbe ou surface.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(std::string vertexName, std::string curveName,
                           std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'une courbe et d'un
     *         parametrage dans [0,1] indiquant la position du point sur la
     *         courbe.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(std::string curveName, const double& param,
                           std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique à partir de deux points
     *         et d'un ratio positionnant ce point sur la droite passant par
     *         ces 2 points.
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(std::string vertex1Name, std::string vertex2Name,
                           const double& param,
                           std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'un point (x,y,z)
     *
     *  \param p le point géométrique
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* newVertex(const Point& p,
            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'un point (x,y,z)
     *
     *  \param x coordonnées en x
     *  \param y coordonnées en y
     *  \param z coordonnées en z
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newVertex(const double& x, const double& y, const double& z,
            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un segment à partir des sommets v1, v2
     *
     *  \param v1 le premier sommet
     *  \param v2 le second somment
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newSegment(std::string n1,std::string n2,
            std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newSegment)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un prisme à partir d'une surface et d'un vecteur de
     *          direction
     *
     *  \param base la surface à extruder
     *  \param dp   le vecteur d'extrusion
     *  \param groupName optionnellement un nom de groupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newPrism(std::string base, const Vector& dp,
                          std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newPrism)

    /** \brief création d'un prisme à partir d'une surface et d'un vecteur de
     *          direction, extrusion de la topologie avec
     *
     *  \param base la surface à extruder
     *  \param dp   le vecteur d'extrusion
     *  \param groupName optionnellement un nom de groupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newPrismWithTopo(std::string base, const Vector& dp,
                          std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newPrismWithTopo)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cone suivant un axe, avec deux rayons
     *         et une longueur
     *
     *  \param dr1 le premier rayon du cone (à l'origine)
     *  \param dr2 le deuxième rayon du cone
     *  \param lg la longueur
     *  \param dv le vecteur pour l'axe et la longueur
     *  \param da l'angle de la portion de cone
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newCone(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da, std::string groupName="");

    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newCone(const double& dr1, const double& dr2,
    		const Vector& dv, const  Utils::Portion::Type& dt, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'une entité surfacique ou volumique par révolution
     *         d'une entité géométrique de dimension 1 ou 2 respectivement.
     *
     *  \param entities les entities dont on fait la révolution
     *  \param rot      une rotation
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    makeRevol( std::vector<std::string>& entities,
            const Utils::Math::Rotation& rot, const bool keep);
	SET_SWIG_COMPLETABLE_METHOD(makeRevol)

    /*------------------------------------------------------------------------*/
    /** \brief création de prismes à partir de surfaces et d'un vecteur de
     *          direction (extrusion dans une direction)
     *
     *  \param entities les entities dont on fait l'extrusion
     *  \param dp   le vecteur d'extrusion
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		makeExtrude(std::vector<std::string>& entities,
				const Vector& dp, const bool keep);
	SET_SWIG_COMPLETABLE_METHOD(makeExtrude)

    /*------------------------------------------------------------------------*/
    /** \brief création de prismes et de blocs à partir de surfaces et d'un vecteur de
     *          direction (extrusion dans une direction)
     *
     *  \param entities les entities dont on fait l'extrusion
     *  \param dp   le vecteur d'extrusion
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		makeBlocksByExtrude(std::vector<std::string>& entities,
				const Vector& dp, const bool keep);
	SET_SWIG_COMPLETABLE_METHOD(makeBlocksByExtrude)

    /*------------------------------------------------------------------------*/
    /** \brief création d'une surface à partir d'une autre à une distance donnée
     *
     *  \param base le nom de la surface à copier
     *  \param offset la distance
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newSurfaceByOffset(std::string name, const double& offset, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newSurfaceByOffset)

	/*------------------------------------------------------------------------*/
    /** \brief création d'une surface planaire à partir d'un ensemble de courbes
     *
     *  \param curves le nom des courbes définissant le contour
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newPlanarSurface(std::vector<std::string>& curves,
            std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newPlanarSurface)

    /**\brief Création d'une surface planaire à partir d'un ensemble de points
         *  Création des sommets géométriques et des segments en même temps
         */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newVerticesCurvesAndPlanarSurface(std::vector<Utils::Math::Point>& points, std::string groupName);
	//SET_SWIG_COMPLETABLE_METHOD(newVerticesCurvesAndPlanarSurface)


    /*------------------------------------------------------------------------*/
    /** \brief création d'une surface facétisée à partir d'un fichier, pour tests
     *
     *  \param nom du fichier mli
     */
	virtual Mgx3D::Internal::M3DCommandResultIfc*
			newFacetedSurface(std::string nom);
	SET_SWIG_COMPLETABLE_METHOD(newFacetedSurface)


    /*------------------------------------------------------------------------*/
   /** \brief création d'une courbe par projection orthogonale d'une courbe sur une surface
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newCurveByCurveProjectionOnSurface(const std::string& curveName, const std::string& surfaceName, std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newCurveByCurveProjectionOnSurface)


    /*------------------------------------------------------------------------*/
    /** \brief suppression d'entités géométriques. Si on supprime une entité
     *         géométrique incidente à des entités géométriques de dimension
     *         supérieure, ces dernières sont aussi supprimées. Pour les
     *         entités géométriques incidentes de dimension inférieure, elles
     *         sont supprimés si propagateDown vaut true, sinon elles sont
     *         conservées.
     *
     *  \param es les entités géométriques à supprimer
     *  \param propagateDown indique si l'on supprime les entités incidentes de
     *                       dimension inférieure
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        destroy(std::vector<std::string>& es, bool propagateDown);
	SET_SWIG_COMPLETABLE_METHOD(destroy)

    /*------------------------------------------------------------------------*/
    /** \brief suppression d'entités géométriques. Si on supprime une entité
     *         géométrique incidente à des entités géométriques de dimension
     *         supérieure, ces dernières sont aussi supprimées. Pour les
     *         entités géométriques incidentes de dimension inférieure, elles
     *         sont supprimés si propagateDown vaut true, sinon elles sont
     *         conservées.
     *         Les entités topologiques dépendant des entités géométriques
     *         détruites sont également détruites.
     *
     *  \param es les entités géométriques à supprimer
     *  \param propagateDown indique si l'on supprime les entités incidentes de
     *                       dimension inférieure
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        destroyWithTopo(std::vector<std::string>& es, bool propagateDown);
	SET_SWIG_COMPLETABLE_METHOD(destroyWithTopo)

    /*------------------------------------------------------------------------*/
    /** \brief homothétie d'objets géométriques
     *
     *  \param geo      les objets d'origine
     *  \param factor   le facteur d'homothétie
     *  \param center   le centre (optionnel)
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo, const double factor);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo, const double factor, const Point& pcentre);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scaleAll(const double factor, const Point& pcentre = Point(0,0,0));

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo,
        	const double factorX,
			const double factorY,
			const double factorZ);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		scaleAll(const double factorX,
			const double factorY,
			const double factorZ);
	SET_SWIG_COMPLETABLE_METHOD(scaleAll)

    /*------------------------------------------------------------------------*/
    /** \brief homothétie d'une copie d'objets géométriques
     *
     *  \param geo      les objets d'origine
     *  \param factor   le facteur d'homothétie
     *  \param center   le centre (optionnel)
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo,
			const double factor,
			bool withTopo,
			std::string groupName);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo,
			const double factor,
			const Point& pcentre,
			bool withTopo,
			std::string groupName);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScaleAll(const double factor,
			const Point& pcentre,
			std::string groupName);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo,
			const double factorX,
			const double factorY,
			const double factorZ,
			bool withTopo,
			std::string groupName);
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScaleAll(const double factorX,
			const double factorY,
			const double factorZ,
			std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief symétrie d'objets géométriques par rapport à un plan
     *  \param geo      les objets d'origine
     *  \param plane    le plan de symétrie
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
            mirror(std::vector<std::string>& geo, Utils::Math::Plane* plane);
	SET_SWIG_COMPLETABLE_METHOD(mirror)

    /*------------------------------------------------------------------------*/
    /** \brief copie et symétrie d'objets géométriques par rapport à un plan
     *  \param geo      les objets d'origine à copier et symétriser
     *  \param plane    le plan de symétrie
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	copyAndMirror(std::vector<std::string>& geo, Utils::Math::Plane* plane, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndMirror)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un arc de cercle à partir de 3 sommets et de la donnée
     *         d'une direction de création (directe ou indirecte).
     *
     *         Une exception est retournée
     *
     *  \param pc centre de l'arc de cercle
     *  \param pd point de départ de l'arc de cercle
     *  \param pe point de fin de l'arc de cercle
     *  \param direct indique si l'on tourne dans le sens direct ou indirect
     *  \param normal vecteur normal au plan pour lever l'ambiguïté dans le cas des 3 points alignés
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newArcCircle( std::string pc,  std::string pd,  std::string pe,
                              const bool direct,
                              const Vector& normal,
                              std::string groupName="");
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newArcCircle( std::string pc,  std::string pd,  std::string pe,
                              const bool direct,
                              std::string groupName="");

    /** \brief création d'un arc de cercle à partir des angles et du rayon, dans un repère donné
     *
     *  \param angleDep angle de départ / axe OX, dans le plan XOY
     *  \param angleFin angle de fin
     *  \param rayon le rayon du cercle
     *  \param sysCoordName le repère dans lequel est défini le cercle (celui par défaut si pas défini)
     *  \param groupName groupe dans lequel est mis l'arc créé
     */
    virtual Internal::M3DCommandResultIfc*
        newArcCircle(const double& angleDep, const double& angleFin,
                     const double& rayon,
                     std::string sysCoordName,
                     std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cercle à partir de 3 points par lesquels le cercle
     *         passera
     *
     *  \param p1 premier point
     *  \param p2 second point
     *  \param p3 dernier point

     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newCircle( std::string p1,  std::string p2,  std::string p3,
                   std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newCircle)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un arc d'ellipse à partir de 3 sommets et de la donnée
     *         d'une direction de création (directe ou indirecte)
     *
     *  \param pc centre de l'arc de cercle
     *  \param pd point de départ de l'arc de cercle
     *  \param pe point de fin de l'arc de cercle
     *  \param direct indique si l'on tourne dans le sens direct ou indirect
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newArcEllipse( std::string pc,  std::string pd,  std::string pe,
                              const bool direct,
                              std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newArcEllipse)

    /*------------------------------------------------------------------------*/
    /** \brief création d'une bspline à partir de la liste ordonnée de points
     *  \param n1 le premier sommet
     *  \param vp une liste ordonnée de points
     *  \param n2 le second somment
     *  \param deg_min degré minimum pour le polynome de la B-Spline
     *  \param deg_max degré maximum pour le polynome de la B-Spline
     *  \param groupName un nom de groupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    newBSpline(std::string n1,
			std::vector<Utils::Math::Point>& vp,
			std::string n2,
			int deg_min,
			int deg_max,
			std::string groupName);
	//SET_SWIG_COMPLETABLE_METHOD(newBSpline)

    /*------------------------------------------------------------------------*/
    /** \brief création d'une boite parallèle aux axes Ox,Oy et Oz à partir des
     *         points pmin et pmax où pmin est le point de plus petites
     *         coordonnées (x,y,z) et pmax le point de plus grandes coordonnées
     *         (x,y,z)
     *
     *  \param pmin le point min de la boite
     *  \param pmax le point max de la boite
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* newBox(const Point& pmin,
            const Point& pmax,
            std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newBox)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cylindre suivant un cercle,
     *         sa base (un cercle défini par un centre et un rayon)
     *         et son axe
     *
     *  \param pcentre le centre du cercle de base
     *  \param dr le rayon du cercle
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param da l'angle de la portion de cylindre
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newCylinder(const Point& pcentre, const double& dr,
                             const Vector& dv, const double& da,
                             std::string groupName="");
    /*------------------------------------------------------------------------*/
    /** \brief création d'un cylindre suivant un cercle,
     *         sa base (un cercle défini par un centre et un rayon)
     *         et son axe
     *
     *  \param pcentre le centre du cercle de base
     *  \param dr le rayon du cercle
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param dt le type de portion de cylindre que l'on crée
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newCylinder(const Point& pcentre, const double& dr,
                             const Vector& dv, const  Utils::Portion::Type& dt,
                             std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère.
     *
     *  \param  pcentre Le centre de la sphère
     *  \param  radius  Le rayon de la sphère
     *  \param  angle    L'angle pour avoir un quartier de sphère (ex : 90
     *                  fournit un 1/4 de sphère)
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newSphere(const Point& pcentre,  const double& radius,
                           const double& angle,
                           std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère.
     *
     *  \param  pcentre Le centre de la sphère
     *  \param  radius  Le rayon de la sphère
     *  \param  dt      portion de sphere à créer
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newSphere(const Point& pcentre,  const double& radius,
                            const  Utils::Portion::Type& dt,
                            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une aiguille pleine du type partie de sphère.
     *
     *  \param  radius  Le rayon de la sphère
     *  \param  angleY  L'angle d'ouverture autour de Ox dans le plan Oxy
     *  \param  angleZ  L'angle d'ouverture autour de Ox dans le plan Oxz
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		newSpherePart(const double& radius,
                      const double& angleY,
                      const double& angleZ,
                      std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newSpherePart)

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une aiguille creuse du type partie de sphère.
     *
     *  \param dr_int le rayon interne de l'aiguille creuse
     *  \param dr_ext le rayon externe de l'aiguille creuse
     *  \param  angleY  L'angle d'ouverture autour de Ox dans le plan Oxy
     *  \param  angleZ  L'angle d'ouverture autour de Ox dans le plan Oxz
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
     virtual Mgx3D::Internal::M3DCommandResultIfc*
	 	 newHollowSpherePart(const double& dr_int,
	 			 const double& dr_ext,
				 const double& angleY,
				 const double& angleZ,
				 std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newHollowSpherePart)

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cylindre creux
     *
     *  \param pcentre le centre des cercles de base
     *  \param dr_int le rayon du cercle interne
     *  \param dr_ext le rayon du cercle externe >dr_int
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param da l'angle de la portion de cylindre
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
    newHollowCylinder(const Point& pcentre,
            const double& dr_int,
            const double& dr_ext,
            const Vector& dv, const double& da,
            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cylindre creux
     *
     *  \param pcentre le centre des cercles de base
     *  \param dr_int le rayon du cercle interne
     *  \param dr_ext le rayon du cercle externe >dr_int
     *  \param dv le vecteur pour l'axe du cylindre
     *  \param dt le type de portion de cylindre que l'on crée
     *  \param groupName optionnellement un nom de groupe
     *
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newHollowCylinder(const Point& pcentre,
                const double& dr_int,
                const double& dr_ext,
                const Vector& dv, const  Utils::Portion::Type& dt,
                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère creuse
     *
     *  \param  pcentre Le centre de la sphère
     *  \param  radius_int  Le rayon interne de la sphère
     *  \param  radius_ext  Le rayon externe de la sphère
     *  \param  angle    L'angle pour avoir un quartier de sphère (ex : 90
     *                  fournit un 1/4 de sphère)
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newHollowSphere(const Point& pcentre,
                const double& radius_int,
                const double& radius_ext,
                const double& angle,
                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère creuse
     *
     *  \param  pcentre Le centre de la sphère
     *  \param  radius_int  Le rayon interne de la sphère
     *  \param  radius_ext  Le rayon externe de la sphère
     *  \param  dt      portion de sphere à créer
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        newHollowSphere(const Point& pcentre,
                const double& radius_int,
                const double& radius_ext,
                const  Utils::Portion::Type& dt,
                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief translation des entités (identifiée par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param ve nom des entités géométrique à translater
     *  \param dp le vecteur de translation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        translate(std::vector<std::string>& ve, const Vector& dp);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        translateAll(const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief translation d'une copie des entités (identifiée par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param ve nom des entités géométrique à copier et translater
     *  \param dp le vecteur de translation
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont ajoutées les entités copiées
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copyAndTranslate(std::vector<std::string>& ve,
        		const Vector& dp,
				bool withTopo,
				std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndTranslate)

    virtual Mgx3D::Internal::M3DCommandResultIfc*
	    copyAndTranslateAll(const Vector& dp, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndTranslateAll)

    /*------------------------------------------------------------------------*/
    /** \brief joinCurves opération permettant de réunir plusieurs courbes en
     *         une seule
     *
     *  \param entities les courbes à réunir
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        joinCurves( std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(joinCurves)


    /*------------------------------------------------------------------------*/
    /** \brief joinSurfaces opération permettant de réunir plusieurs surfaces
     *         en une seule
     *
     *  \param entities les surfaces à réunir
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        joinSurfaces( std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(joinSurfaces)


    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une ou plusieurs entités géométrique
     *
     *  \param entities les entities dont on fait la rotation
     *  \param rot la rotation
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        rotate( std::vector<std::string>& entities,
        		const Utils::Math::Rotation& rot);
	SET_SWIG_COMPLETABLE_METHOD(rotate)
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        rotateAll( const Utils::Math::Rotation& rot);
	SET_SWIG_COMPLETABLE_METHOD(rotateAll)

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une ou plusieurs copie d'entités géométrique
     *
     *  \param entities les entities dont on fait la rotation
     *  \param rot la rotation
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        copyAndRotate( std::vector<std::string>& entities,
        		const Utils::Math::Rotation& rot, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndRotate)
    virtual Mgx3D::Internal::M3DCommandResultIfc*
		copyAndRotateAll( const Utils::Math::Rotation& rot, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndRotateAll)

    /*------------------------------------------------------------------------*/
    /** \brief Collage d'entités géométriques
     *
     *  \param entities les entités sur lesquelles on travaille
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        glue(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(glue)

    /*------------------------------------------------------------------------*/
    /** \brief Fusion Booléenne de n entités géométriques
     *
     *  \param entities les entités sur lesquelles on travaille
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        fuse(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(fuse)

    /*------------------------------------------------------------------------*/
    /** \brief Intersection Booléenne de n entités géométriques
     *
     *  \param entities les entités sur lesquelles on travaille
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        common(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(common)

    /*------------------------------------------------------------------------*/
    /** \brief Intersection Booléenne de 2 entités géométriques 1D ou 2D, détruit les entités de base
     *
     *  \param entity1 (courbe ou surface)
     *  \param entity2 (comme la précédente)
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	     common2D(std::string entity1, std::string entity2, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(common2D)

    /*------------------------------------------------------------------------*/
    /** \brief Intersection Booléenne de 2 entités géométriques 1D ou 2D, ne détruit pas les entités de base
     *
     *  \param entity1 (courbe ou surface)
     *  \param entity2 (comme la précédente)
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
	     common2DOnCopy(std::string entity1, std::string entity2, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(common2DOnCopy)

    /*------------------------------------------------------------------------*/
    /** \brief Différence Booléenne de n entités géométriques avec la première
     *         entité qui est conservée
     *
     *  \param tokeep l(es) entité(s) que l'on conserve
     *  \param tocut  les entités que l'on retire de tokeep
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc*
        cut(std::string tokeep,std::vector<std::string>& tocut);

    virtual Mgx3D::Internal::M3DCommandResultIfc*
        cut(std::vector<std::string>& tokeep,std::vector<std::string>& tocut);

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format CATIA
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param testVolumicProperties test que les volumes sont fermés
     *  \param splitCompoundCurves décompose les courbes composites en de multiples courbes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importCATIA(std::string n, const bool testVolumicProperties=true, const bool splitCompoundCurves=false);
	SET_SWIG_COMPLETABLE_METHOD(importCATIA)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format STEP
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param testVolumicProperties test que les volumes sont fermés
     *  \param splitCompoundCurves décompose les courbes composites en de multiples courbes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importSTEP(std::string n, const bool testVolumicProperties=true, const bool splitCompoundCurves=false);
	SET_SWIG_COMPLETABLE_METHOD(importSTEP)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format STL
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param splitCompoundCurves décompose les courbes composites en de multiples courbes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importSTL(std::string n);
	SET_SWIG_COMPLETABLE_METHOD(importSTL)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format IGES
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param splitCompoundCurves décompose les courbes composites en de multiples courbes
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importIGES(std::string n, const bool splitCompoundCurves=false);
	SET_SWIG_COMPLETABLE_METHOD(importIGES)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format MDL
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param all à vrai si l'on veut importer toutes les entités, faux si l'on ne prend que ce qui est utile au 2D
     *  \param useAreaName à vrai si l'on souhaite conserver les noms des zones
     *  \param prefixName préfixe pour les noms des groupes des entités importées
     *  \param deg_min degré minimum pour les polynomes des B-Splines
     *  \param deg_max degré maximum pour les polynomes des B-Splines
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importMDL(std::string n, const bool all,
    		const bool useAreaName=false, std::string prefixName="",
			int deg_min=1, int deg_max=2);

    /** \brief Import d'une partie d'un fichier au format MDL
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param groupe le nom du groupe dont on importe les zones et les entités associées
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importMDL(std::string n, std::string groupe);

    /** \brief Import d'une partie d'un fichier au format MDL
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param zones la liste des zones que l'on importe avec les entités associées
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* importMDL(std::string n, std::vector<std::string>& zones);

    /** \brief Import d'un fichier au format MDL avec création des commandes élémentaires
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param withTopo à vrai si l'on veut importer la topologie en plus de la géométrie
     */
    virtual void mdl2CommandesMagix3D(std::string n, const bool withTopo);
	SET_SWIG_COMPLETABLE_METHOD(mdl2CommandesMagix3D)

    /*------------------------------------------------------------------------*/
    /** \brief Export d'une sélection dans un fichier au format MDL
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportMDL(std::vector<std::string>& ge, const std::string& n);
	SET_SWIG_COMPLETABLE_METHOD(exportMDL)

    /*------------------------------------------------------------------------*/
    /** \brief Export dans un fichier au format VTK de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportVTK(const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief Export d'une sélection dans un fichier au format VTK
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportVTK(std::vector<std::string>& ge,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief Export dans un fichier au format STL d'une entité géométrique (une surface)
     *
     *  \param ge le nom de l'entité que l'on exporte
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportSTL(const std::string& ge,
    		const std::string& n);
	SET_SWIG_COMPLETABLE_METHOD(exportSTL)

   /*------------------------------------------------------------------------*/
    /** \brief Export au format Mli de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportMLI(const std::string& n);

    /** \brief Export d'une sélection au format Mli
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportMLI(std::vector<std::string>& ge,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief Export au format STEP de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportSTEP(const std::string& n);

    /** \brief Export d'une sélection au format STEP
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportSTEP(std::vector<std::string>& ge,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief Export au format IGES de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportIGES(const std::string& n);

    /** \brief Export d'une sélection au format IGES
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* exportIGES(std::vector<std::string>& ge,
    		const std::string& n);

    /*------------------------------------------------------------------------*/
    /** \brief Section d'un groupe d'entités géométrique par un outil
     *
     *  \param entities les entités que l'on veut couper
     *  \param tool     l'entité pour découper
     *  \param planeGroupName le nom du groupe dans lequel on place toutes les
     *                        entites sur le plan
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* section(std::vector<std::string>& entities, std::string tool);
	SET_SWIG_COMPLETABLE_METHOD(section)


    virtual Mgx3D::Internal::M3DCommandResultIfc* sectionByPlane(
            std::vector<std::string>& entities,  Utils::Math::Plane* tool,
            std::string planeGroupName);
	SET_SWIG_COMPLETABLE_METHOD(sectionByPlane)

    /*------------------------------------------------------------------------*/
    /** \brief Découpage d'une courbe en un point
     *
     *  \param entity le nom de la courbe
     *  \param pt     la position de la coupe
     */
    virtual Mgx3D::Internal::M3DCommandResultIfc* splitCurve(std::string entity, const Point& pt);
	SET_SWIG_COMPLETABLE_METHOD(splitCurve)

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des volumes gérées par le manager
     */
    virtual std::vector<std::string> getVolumes() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des surfaces gérées par le manager
     */
    virtual std::vector<std::string> getSurfaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des courbes gérées par le manager
     */
    virtual std::vector<std::string> getCurves() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des sommets gérées par le manager
     */
    virtual std::vector<std::string> getVertices() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de volumes gérés
     */
    virtual int getNbVolumes() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de surfaces gérées par le manager
     */
    virtual int getNbSurfaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de courbes gérées par le manager
     */
    virtual int getNbCurves() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de sommets gérées par le manager
     */
    virtual int getNbVertices() const;


#ifndef SWIG
    /** Retourne le Volume suivant le nom en argument */
    virtual Volume* getVolume(const std::string& name, const bool exceptionIfNotFound=true) const;
    /** Retourne la Surface suivant le nom en argument */
    virtual Surface* getSurface(const std::string& name, const bool exceptionIfNotFound=true) const;
    /** Retourne la Curve suivant le nom en argument */
    virtual Curve* getCurve(const std::string& name, const bool exceptionIfNotFound=true) const;
    /** Retourne le Vertex suivant le nom en argument */
    virtual Vertex* getVertex(const std::string& name, const bool exceptionIfNotFound=true) const;
    /** Retourne la GeomEntity suivant le nom en argument */
    virtual GeomEntity* getEntity(const std::string& name, const bool exceptionIfNotFound=true) const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des volumes gérées par le manager
     */
    virtual std::vector<Volume*> getVolumesObj() const;
    /** \brief retourne la liste des surfaces gérées par le manager
     */
    virtual std::vector<Surface*> getSurfacesObj() const;
    /** \brief retourne la liste des courbes gérées par le manager
     */
    virtual std::vector<Curve*> getCurvesObj() const;
    /** \brief retourne la liste des sommets gérées par le manager
     */
    virtual std::vector<Vertex*> getVerticesObj() const;
#endif

    /** Retourne le nom du sommet en fonction d'une position géométrique */
    virtual std::string getVertexAt(const Point& pt1) const;
	SET_SWIG_COMPLETABLE_METHOD(getVertexAt)

    /** Retourne le nom de la courbe en fonction d'un triplet de positions géométriques */
    virtual std::string getCurveAt(const Point& pt1, const Point& pt2, const Point& pt3) const;
	SET_SWIG_COMPLETABLE_METHOD(getCurveAt)

    /** Retourne le nom de la surface en fonction des positions géométriques de ses sommets */
    virtual std::string getSurfaceAt(std::vector<Point>& pts) const;
	//SET_SWIG_COMPLETABLE_METHOD(getSurfaceAt) [EB] pas de complétion possible pour cas vector<...>

    /** Retourne le nom de le volume en fonction des positions géométriques de ses sommets */
    virtual std::string getVolumeAt(std::vector<Point>& pts) const;
	//SET_SWIG_COMPLETABLE_METHOD(getVolumeAt)

    /** Retourne l'indice du sommet v*/
    virtual int getIndexOf(Vertex* v) const;
    /** Retourne l'indice de la courbe c*/
    virtual int getIndexOf(Curve* c) const;
    /** Retourne l'indice de la surface s*/
    virtual int getIndexOf(Surface* s) const;
    /** Retourne l'indice du volume v*/
    virtual int getIndexOf(Volume* v) const;

    /** Ajoute un groupe à un ensemble d'entités géométriques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
	SET_SWIG_COMPLETABLE_METHOD(addToGroup)

    /** Enlève un groupe à un ensemble d'entités géométriques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
	SET_SWIG_COMPLETABLE_METHOD(removeFromGroup)

    /** Défini le groupe pour un ensemble d'entités géométriques, suivant une dimension */
    virtual Internal::M3DCommandResultIfc* setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
	SET_SWIG_COMPLETABLE_METHOD(setGroup)

protected :

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    GeomManagerIfc (const std::string& name, Internal::ContextIfc * c);


	private :

	/*------------------------------------------------------------------------*/
	/**
	 * \brief		Constructeur de copie. Interdit.
	 */
	GeomManagerIfc (const GeomManagerIfc&);

	/*------------------------------------------------------------------------*/
	/**
	 * \brief		Opérateur =. Interdit.
	 */
	GeomManagerIfc& operator = (const GeomManagerIfc&);
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GEOMMANAGERIFC_H_ */
/*----------------------------------------------------------------------------*/
