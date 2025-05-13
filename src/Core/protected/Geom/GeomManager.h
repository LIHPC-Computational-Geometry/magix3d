#ifndef MGX3D_GEOM_GEOMMANAGER_H_
#define MGX3D_GEOM_GEOMMANAGER_H_
/*----------------------------------------------------------------------------*/
#include <string>
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Internal/CommandCreator.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Geom/GeomInfo.h"
#include "Utils/Constants.h"
#include "Utils/Vector.h"
#include "Utils/Plane.h"
#include "Utils/SwigCompletion.h"
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
namespace CoordinateSystem {
class SysCoord;
}

/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
class GeomEntity;
class GeomInfo;

/*----------------------------------------------------------------------------*/
/**
 * \class GeomManager
 * \brief Gestionnaire des opérations effectuées au niveau du module
 *        géométrique. Le gestionnaire géométrique est l'objet qui stocke toutes
 *        les entités géométriques pour une session donnée. La responsabilité
 *        de créer les entités est déléguée à une instance de la classe
 *        Geom::EntityFactory. Par contre, la mort des entités géométriques est
 *        liée à celle du GeomManager.
 *
 *        Ce gestionnaire joue le rôle de fabrique de commande, mais aussi d'
 *        entités géométriques. La raison est technique et non pas conceptuelle.
 *        Lorsque l'on crée une entité géométrique via une commande, les entités
 *        incidentes de dimension inférieure doivent aussi être crées puis
 *        stockées au niveau du GeomManager. Il est donc le plus à même de remplir
 *        ce rôle.
 */
class GeomManager final : public Internal::CommandCreator {
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param		Nom unique de l'instance (utile en environnement distribué).
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    GeomManager(const std::string& name, Internal::Context* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    ~GeomManager() override;

    /*------------------------------------------------------------------------*/
#ifndef SWIG
	/** Réinitialisation     */
	void clear();
#endif

	/*------------------------------------------------------------------------*/
    /** \brief récupère des informations sur les entités géométriques.
     *
     *  \param e l'entité géométrique dont on veut des informations.
     */
    GeomInfo getInfos(std::string e, int dim);
	SET_SWIG_COMPLETABLE_METHOD(getInfos)

#ifndef SWIG
    GeomInfo getInfos(const GeomEntity* e);
#endif

    /** retourne les coordonnés d'un sommet */
	Point getCoord(const std::string& name) const;
	SET_SWIG_COMPLETABLE_METHOD(getCoord)

    /*------------------------------------------------------------------------*/
    /** \brief création d'entités géométrique par copie
     *
     *  \param e les entités géométriques que l'on veut copier
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::vector<std::string>& e, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copy)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        copy(std::vector<GeomEntity*>& e, bool withTopo, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'un point p(x,y,z)
     *         et d'une courbe ou surface. Le sommet créé est le projet de p sur la
     *         courbe ou la surface.
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        newVertex(std::string vertexName, std::string curveName,
            std::string groupName="");
#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        newVertex(const Vertex* ref, Curve* curve, std::string groupName="");
    Mgx3D::Internal::M3DCommandResultIfc*
        newVertex(const Vertex* ref, Surface* surface, std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'une courbe et d'un
     *         parametrage dans [0,1] indiquant la position du point sur la
     *         courbe.
     */
    Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(std::string curveName, const double& param,
                           std::string groupName="");
#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(Curve* curve, const double& param, std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique à partir de deux points
     *         et d'un ratio positionnant ce point sur la droite passant par
     *         ces 2 points.
     */
    Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(std::string vertex1Name, std::string vertex2Name,
                           const double& param,
                           std::string groupName="");

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
                 newVertex(const Vertex* ref1, const Vertex* ref2,
                           const double& param,
                           std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'un point (x,y,z)
     *
     *  \param p le point géométrique
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        newVertex(const Point& p, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique  à partir d'un point (x,y,z)
     *
     *  \param x coordonnées en x
     *  \param y coordonnées en y
     *  \param z coordonnées en z
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        newVertex(const double& x, const double& y, const double& z,
            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un sommet géométrique à partir d'un sommet topologique
     *
	 *  \param vertexName	le sommet topologique
     *  \param asso			faut-il associer le sommet topologique au sommet géométrique créé
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newVertexFromTopo(std::string vertexName, bool asso = true,
            std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un segment à partir des sommets v1, v2
     *
     *  \param v1 le premier sommet
     *  \param v2 le second somment
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newSegment(std::string n1,std::string n2, std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newSegment)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		newSegment( Geom::Vertex* v1,  Geom::Vertex* v2, std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cercle à partir de 3 points par lesquels le cercle
     *         passera
     *
     *  \param p1 premier point
     *  \param p2 second point
     *  \param p3 dernier point
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newCircle( std::string p1, std::string p2, std::string p3,
				   std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newCircle)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		newCircle( Vertex* p1, Vertex* p2, Vertex* p3,
                   std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'une ellipse centrée sur le point center, plan défini
     *         par center/p1/p2, grand axe défini par center/p1,
     *         grand rayon défini par la distance center-p1,
     *         petit rayon défini par la distance p2-axe principal.
     *
     *  \param p1 premier point
     *  \param p2 second point
     *  \param center centre
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newEllipse( std::string p1, std::string p2, std::string center,
				   std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newEllipse)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		newEllipse( Vertex* p1, Vertex* p2, Vertex* center,
                   std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un arc de cercle à partir de 3 points et de la donnée
     *         d'une direction de création (directe ou indirecte)
     *
     *  \param pc centre de l'arc de cercle
     *  \param pd point de départ de l'arc de cercle
     *  \param pe point de fin de l'arc de cercle
     *  \param direct indique si l'on tourne dans le sens direct ou indirect
     *  \param normal vecteur normal au plan pour lever l'ambiguïté dans le cas des 3 points alignés
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	newArcCircle( std::string pc,
			std::string pd,
			std::string pe,
			const bool direct,
			const Vector& normal,
			std::string groupName="");

	Mgx3D::Internal::M3DCommandResultIfc*
	newArcCircle( std::string pc,
			std::string pd,
			std::string pe,
			const bool direct,
			std::string groupName="");

	/*------------------------------------------------------------------------*/
    /** \brief création d'un arc de cercle à partir de 3 sommets inscrits.
     *
     *         Une exception est retournée
     *
     *  \param pc le point intermédiaire de l'arc de cercle
     *  \param pd point de départ de l'arc de cercle
     *  \param pe point de fin de l'arc de cercle
     */
    Mgx3D::Internal::M3DCommandResultIfc*
    newArcCircle( std::string pc,
                  std::string pd,
                  std::string pe,
                  std::string groupName="");

    /** \brief création d'un arc de cercle à partir des angles et du rayon, dans un repère donné
     *
     *  \param angleDep angle de départ / axe OX, dans le plan XOY
     *  \param angleFin angle de fin
     *  \param rayon le rayon du cercle
     *  \param sysCoordName le repère dans lequel est défini le cercle (celui par défaut si pas défini)
     *  \param groupName groupe dans lequel est mis l'arc créé
     */
    Internal::M3DCommandResultIfc*
    newArcCircle(const double& angleDep,
                 const double& angleFin,
                 const double& rayon,
                 std::string sysCoordName,
                 std::string groupName="");

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	newArcCircle( Vertex* pc,  Vertex* pd,  Vertex* pe,
			const bool direct,
			const Vector& normal,
			std::string groupName="");
    Mgx3D::Internal::M3DCommandResultIfc*
	newArcCircle( Vertex* pc,  Vertex* pd,  Vertex* pe,
			const bool direct,
			std::string groupName="");
    Mgx3D::Internal::M3DCommandResultIfc*
    newArcCircle( Vertex* pc,  Vertex* pd,  Vertex* pe,
                  std::string groupName="");
    Internal::M3DCommandResultIfc*
    newArcCircle(const double& angleDep,
                 const double& angleFin,
                 const double& rayon,
                 CoordinateSystem::SysCoord* sysCoord,
                 std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'un arc d'ellipse à partir de 3 points et de la donnée
     *         d'une direction de création (directe ou indirecte)
     *
     *  \param pc centre de l'arc de cercle
     *  \param pd point de départ de l'arc de cercle
     *  \param pe point de fin de l'arc de cercle
     *  \param direct indique si l'on tourne dans le sens direct ou indirect
     */
    Mgx3D::Internal::M3DCommandResultIfc*
    newArcEllipse( std::string pc,  std::string pd,  std::string pe,
    		const bool direct,
    		std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newArcEllipse)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
    newArcEllipse( Vertex* pc,  Vertex* pd,  Vertex* pe,
    		const bool direct,
    		std::string groupName="");
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'une bspline à partir de la liste ordonnée de points
     *  \param n1 le premier sommet
     *  \param vp une liste ordonnée de points
     *  \param n2 le second somment
     *  \param deg_min degré minimum pour le polynome de la B-Spline
     *  \param deg_max degré maximum pour le polynome de la B-Spline
     *  \param groupName un nom de groupe
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newBSpline(std::string n1, std::vector<Point>& vp, std::string n2,
				int deg_min, int deg_max, std::string groupName);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		newBSpline(Vertex* vtx1, std::vector<Point>& vp, Vertex* vtx2,
				int deg_min, int deg_max, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'une surface à partir d'une autre à une distance donnée
     *
     *  \param base le nom de la surface à copier
     *  \param offset la distance
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newSurfaceByOffset(std::string name, const double& offset, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newSurfaceByOffset)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		newSurfaceByOffset(Surface* base, const double& offset, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création d'une surface planaire à partir d'un ensemble de courbes
     *
     *  \param curves le nom des courbes définissant le contour
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        newPlanarSurface(std::vector<std::string>& curves, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(newPlanarSurface)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        newPlanarSurface(const std::vector<Geom::Curve* >& curves, std::string groupName );
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Création d'une surface planaire à partir d'un ensemble de points
     *  Création des sommets géométriques et des segments en même temps
     */
    Mgx3D::Internal::M3DCommandResultIfc*
    	newVerticesCurvesAndPlanarSurface(std::vector<Point>& points, std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief création d'une courbe par projection orthogonale d'une courbe sur une surface
     */
    Mgx3D::Internal::M3DCommandResultIfc*
    newCurveByCurveProjectionOnSurface(const std::string& curveName, const std::string& surfaceName, std::string groupName="");
	SET_SWIG_COMPLETABLE_METHOD(newCurveByCurveProjectionOnSurface)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
    newCurveByCurveProjectionOnSurface(Geom::Curve* curve, Geom::Surface* surface, std::string groupName="");
#endif

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
    Mgx3D::Internal::M3DCommandResultIfc*
        destroy(std::vector<std::string>& es, bool propagateDown);
	SET_SWIG_COMPLETABLE_METHOD(destroy)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        destroy(std::vector<GeomEntity*>& es, bool propagateDown);
#endif

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
    Mgx3D::Internal::M3DCommandResultIfc*
        destroyWithTopo(std::vector<std::string>& es, bool propagateDown);
	SET_SWIG_COMPLETABLE_METHOD(destroyWithTopo)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        destroyWithTopo(std::vector<GeomEntity*>& es, bool propagateDown);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief joinCurves opération permettant de réunir plusieurs courbes en
     *         une seule
     *
     *  \param entities les courbes à réunir
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        joinCurves(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(joinCurves)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        joinCurves(std::vector<GeomEntity*>& entities);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief joinSurfaces opération permettant de réunir plusieurs surfaces
     *         en une seule
     *
     *  \param entities les surfaces à réunir
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        joinSurfaces(std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(joinSurfaces)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        joinSurfaces(std::vector<GeomEntity*>& entities);
#endif

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
     *  \see getLastVolume pour obtenir le volume résultant
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newBox(const Point& pmin, const Point& pmax, std::string groupName="");
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
     *  \see getLastVolume pour obtenir le volume résultant
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newCylinder(const Point& pcentre, const double& dr,
                    const Vector& dv, const double& da, std::string groupName="");

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
     *  \see getLastVolume pour obtenir le volume résultant
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		newCylinder(const Point& pcentre, const double& dr,
                    const Vector& dv, const  Utils::Portion::Type& dt, std::string groupName="");

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
    Mgx3D::Internal::M3DCommandResultIfc*
		newSphere(const Point& pcentre,  const double& radius,
                  const double& angle, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère.
     *
     *  \param  pcentre Le centre de la sphère
     *  \param  radius  Le rayon de la sphère
     *  \param  dt      portion de sphere à créer
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    Mgx3D::Internal::M3DCommandResultIfc*
		newSphere(const Point& pcentre,  const double& radius,
                  const  Utils::Portion::Type& dt, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une aiguille du type partie de sphère.
     *
     *  \param  radius  Le rayon de la sphère
     *  \param  angleY  L'angle d'ouverture autour de Ox dans le plan Oxy
     *  \param  angleZ  L'angle d'ouverture autour de Ox dans le plan Oxz
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    Mgx3D::Internal::M3DCommandResultIfc*
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
	Mgx3D::Internal::M3DCommandResultIfc*
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
    Mgx3D::Internal::M3DCommandResultIfc*
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
    Mgx3D::Internal::M3DCommandResultIfc*
        newHollowCylinder(const Point& pcentre,
                const double& dr_int,
                const double& dr_ext,
                const Vector& dv, const  Utils::Portion::Type& dt,
                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère creuse
     *
     *  \param pcentre Le centre de la sphère
     *  \param radius_int  Le rayon interne de la sphère
     *  \param radius_ext  Le rayon externe de la sphère
     *  \param angle    L'angle pour avoir un quartier de sphère (ex : 90
     *                  fournit un 1/4 de sphère)
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
    Mgx3D::Internal::M3DCommandResultIfc*
        newHollowSphere(const Point& pcentre,
                const double& radius_int,
                const double& radius_ext,
                const double& angle,
                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Création d'une sphère creuse
     *
     *  \param pcentre Le centre de la sphère
     *  \param radius_int  Le rayon interne de la sphère
     *  \param radius_ext  Le rayon externe de la sphère
     *  \param dt      portion de sphere à créer
     *  \param groupName optionnellement un nom de groupe
     */
    /*------------------------------------------------------------------------*/
     Mgx3D::Internal::M3DCommandResultIfc*
        newHollowSphere(const Point& pcentre,
                const double& radius_int,
                const double& radius_ext,
                const  Utils::Portion::Type& dt,
                std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'un cone suivant un axe, avec deux rayons
     *         et une longueur
     *
     *  \param dr1 le premier rayon du cone (à l'origine)
     *  \param dr2 le deuxième rayon du cone
     *  \param dv le vecteur pour l'axe et la longueur
     *  \param da l'angle de la portion de cone
     *  \param groupName optionnellement un nom de groupe
     *
     */
    Mgx3D::Internal::M3DCommandResultIfc*
    newCone(const double& dr1, const double& dr2,
    		const Vector& dv, const double& da, std::string groupName="");

	/*------------------------------------------------------------------------*/
	/** \brief création d'un cone suivant un axe, avec deux rayons
	 *         et une longueur
	 *
	 *  \param dr1 le premier rayon du cone (à l'origine)
	 *  \param dr2 le deuxième rayon du cone
	 *  \param dv le vecteur pour l'axe et la longueur
	 *  \param dt portion de cone à créer
	 *  \param groupName optionnellement un nom de groupe
	 *
	 */
	Mgx3D::Internal::M3DCommandResultIfc*
    newCone(const double& dr1, const double& dr2,
    		const Vector& dv, const  Utils::Portion::Type& dt, std::string groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief création d'une entité surfacique ou volumique par révolution
     *         d'une entité géométrique de dimension 1 ou 2 respectivement.
     *
     *  \param entities les entities dont on fait la révolution
     *  \param rot      la rotation
     *  \param keep     indique si l'on conserve (true) ou pas (false) les
     *                  entités de départ
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		makeRevol( std::vector<std::string>& entities,
		            const Utils::Math::Rotation& rot, const bool keep);
	SET_SWIG_COMPLETABLE_METHOD(makeRevol)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		makeRevol(  std::vector<GeomEntity*>& entities,
                    const Utils::Math::Rotation& rot, const bool keep);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création de prismes à partir de surfaces et d'un vecteur de
     *          direction (extrusion dans une direction)
     *
     *  \param entities les entites dont on fait l'extrusion
     *  \param dp   le vecteur d'extrusion
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		makeExtrude(std::vector<std::string>& entities,
					const Vector& dp, const bool keep);
	SET_SWIG_COMPLETABLE_METHOD(makeExtrude)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		makeExtrude(  std::vector<GeomEntity*>& entities,
					const Vector& dp, const bool keep);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief création de prismes et de blocs à partir de surfaces et d'un vecteur de
     *          direction (extrusion dans une direction)
     *
     *  \param entities les entites dont on fait l'extrusion
     *  \param dp   le vecteur d'extrusion
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		makeBlocksByExtrude(std::vector<std::string>& entities,
				const Vector& dp, const bool keep);
	SET_SWIG_COMPLETABLE_METHOD(makeBlocksByExtrude)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		makeBlocksByExtrude(  std::vector<GeomEntity*>& entities,
					const Vector& dp, const bool keep);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief translation des entités (identifiée par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param ve nom des entités géométrique à translater
     *  \param dp le vecteur de translation
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	    translate(std::vector<std::string>& ve, const Vector& dp);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		translate(std::vector<GeomEntity*>& ve, const Vector& dp);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
        translateAll(const Vector& dp);

    /*------------------------------------------------------------------------*/
    /** \brief translation d'une copie des entités (identifiée par un nom unique pour python)
     *  suivant le vecteur de translation défini par dp
     *
     *  \param ve nom des entités géométrique à copier et translater
     *  \param dp le vecteur de translation
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        copyAndTranslate(std::vector<std::string>& ve, const Vector& dp, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndTranslate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	    copyAndTranslate(std::vector<GeomEntity*>& ve, const Vector& dp, bool withTopo, std::string groupName);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
	    copyAndTranslateAll(const Vector& dp, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndTranslateAll)

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une ou plusieurs entités géométrique
     *
     *  \param entities les entities dont on fait la rotation
     *  \param rot      la rotation
     */
    Mgx3D::Internal::M3DCommandResultIfc*
		rotate( std::vector<std::string>& entities,
				const Utils::Math::Rotation& rot);
	SET_SWIG_COMPLETABLE_METHOD(rotate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
		rotate( std::vector<GeomEntity*>& entities,
				const Utils::Math::Rotation& rot);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
        rotateAll( const Utils::Math::Rotation& rot);
    SET_SWIG_COMPLETABLE_METHOD(rotateAll)

    /*------------------------------------------------------------------------*/
    /** \brief rotation d'une ou plusieurs copie d'entités géométrique
     *
     *  \param entities les entities dont on fait une copie et la rotation
     *  \param rot la rotation
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndRotate( std::vector<std::string>& entities,
				const Utils::Math::Rotation& rot, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndRotate)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        copyAndRotate( std::vector<GeomEntity*>& entities,
        		const Utils::Math::Rotation& rot, bool withTopo, std::string groupName);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
		copyAndRotateAll( const Utils::Math::Rotation& rot, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndRotateAll)

    /*------------------------------------------------------------------------*/
    /** \brief homothétie d'objets géométriques
     *
     *  \param geo      les objets d'origine
     *  \param factor   le facteur d'homothétie
     *  \param center   le centre (optionnel)
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo, const double factor);

    Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo, const double factor, const Point& pcentre);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<Geom::GeomEntity*>& geo, const double factor, const Point& pcentre);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
        scaleAll(const double factor, const Point& pcentre = Point(0,0,0));

    /** \brief homothétie d'un objet géométrique
     *
     *  \param geo       les objets d'origine
     *  \param factorX   le facteur d'homothétie suivant l'axe des x
     *  \param factorY   le facteur d'homothétie suivant l'axe des y
     *  \param factorZ   le facteur d'homothétie suivant l'axe des z
     *  \param center   le centre (optionnel)
     */
    Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo,
        		const double factorX,
				const double factorY,
				const double factorZ);

    Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<std::string>& geo,
				const double factorX,
				const double factorY,
				const double factorZ,
				const Point& pcentre);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
        scale(std::vector<Geom::GeomEntity*>& geo,
        		const double factorX,
				const double factorY,
				const double factorZ,
                const Point& pcentre);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
		scaleAll(const double factorX,
			const double factorY,
			const double factorZ,
			const Point& pcentre = Point(0,0,0));

	/*------------------------------------------------------------------------*/
    /** \brief homothétie d'une copie d'objets géométriques
     *
     *  \param geo       les objets d'origine
     *  \param factor    le facteur d'homothétie
     *  \param pcenter    le centre (optionnel)
     *  \param withTopo  a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mises les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo, const double factor, const Point& pcenter, bool withTopo, std::string groupName);

	Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo, const double factor, bool withTopo, std::string groupName);


#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<Geom::GeomEntity*>& geo, const double factor, const Point& pcenter, bool withTopo, std::string groupName);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScaleAll(const double factor, const Point& pcenter, std::string groupName);

    /** \brief homothétie d'une copie d'un objet géométrique
     *
     *  \param geo       les objets d'origine
     *  \param factorX   le facteur d'homothétie suivant l'axe des x
     *  \param factorY   le facteur d'homothétie suivant l'axe des y
     *  \param factorZ   le facteur d'homothétie suivant l'axe des z
     *  \param pcenter    le centre (optionnel)
     *  \param withTopo  a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mises les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo,
				const double factorX,
				const double factorY,
				const double factorZ,
				const Point& pcenter,
				bool withTopo,
				std::string groupName);

	Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<std::string>& geo,
        		const double factorX,
				const double factorY,
				const double factorZ,
				bool withTopo,
				std::string groupName);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScale(std::vector<Geom::GeomEntity*>& geo,
				const double factorX,
				const double factorY,
				const double factorZ,
				const Point& pcenter,
				bool withTopo,
				std::string groupName);
#endif

    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndScaleAll(const double factorX,
			const double factorY,
			const double factorZ,
			const Point& pcenter,
			std::string groupName);

    /*------------------------------------------------------------------------*/
    /** \brief symétrie d'objets géométriques par rapport à un plan
     *  \param geo      les objets d'origine
     *  \param plane    le plan de symétrie
     */
    Mgx3D::Internal::M3DCommandResultIfc*
            mirror(std::vector<std::string>& geo, Utils::Math::Plane* plane);
    SET_SWIG_COMPLETABLE_METHOD(mirror)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
            mirror(std::vector<Geom::GeomEntity*>& geo, Utils::Math::Plane* plane);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief copie et symétrie d'objets géométriques par rapport à un plan
     *  \param geo      les objets d'origine à copier et symétriser
     *  \param plane    le plan de symétrie
     *  \param withTopo a vrai si l'on doit copier la topologie avec la géométrie
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndMirror(std::vector<std::string>& geo, Utils::Math::Plane* plane, bool withTopo, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(copyAndMirror)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	copyAndMirror(std::vector<Geom::GeomEntity*>& geo, Utils::Math::Plane* plane, bool withTopo, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format BREP
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param testVolumicProperties test que les volumes sont fermés
     */
    Mgx3D::Internal::M3DCommandResultIfc* importBREP(std::string n,
        const bool testVolumicProperties=true);
    SET_SWIG_COMPLETABLE_METHOD(importBREP)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format CATIA
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param testVolumicProperties test que les volumes sont fermés
     */
    Mgx3D::Internal::M3DCommandResultIfc* importCATIA(std::string n,
        const bool testVolumicProperties=true);
    SET_SWIG_COMPLETABLE_METHOD(importCATIA)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format STEP
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param testVolumicProperties test que les volumes sont fermés
     */
    Mgx3D::Internal::M3DCommandResultIfc* importSTEP(std::string n,
        const bool testVolumicProperties=true);
    SET_SWIG_COMPLETABLE_METHOD(importSTEP)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format STL
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     */
    Mgx3D::Internal::M3DCommandResultIfc* importSTL(std::string n);
    SET_SWIG_COMPLETABLE_METHOD(importSTL)

    /*------------------------------------------------------------------------*/
    /** \brief Import d'un fichier au format IGES
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     */
    Mgx3D::Internal::M3DCommandResultIfc* importIGES(std::string n);
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
    Mgx3D::Internal::M3DCommandResultIfc* importMDL(
			std::string n, const bool all, const bool useAreaName=false, std::string prefixName="",
			int deg_min=1, int deg_max=2);

    /** \brief Import d'une partie d'un fichier au format MDL
     *  avec mise à l'échelle en mètre)
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param groupe le nom du groupe dont on importe les zones et les entités associées
     */
    Mgx3D::Internal::M3DCommandResultIfc* importMDL(
			std::string n, std::string groupe);

    /** \brief Import d'une partie d'un fichier au format MDL
     *  (avec mise à l'échelle en mètre)
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param zones la liste des zones que l'on importe avec les entités associées
     */
    Mgx3D::Internal::M3DCommandResultIfc* importMDL(
			std::string n, std::vector<std::string>& zones);

    /** \brief Import d'un fichier au format MDL avec création des commandes élémentaires
     *
     *  \param n le nom du ficher dont le contenu doit etre importe
     *  \param withTopo à vrai si l'on veut importer la topologie en plus de la géométrie
     */
    void mdl2CommandesMagix3D(std::string n, const bool withTopo);
    SET_SWIG_COMPLETABLE_METHOD(mdl2CommandesMagix3D)

   /*------------------------------------------------------------------------*/
    /** \brief Export d'une sélection dans un fichier au format MDL
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportMDL(std::vector<std::string>& ge,
            const std::string& n);
    SET_SWIG_COMPLETABLE_METHOD(exportMDL)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* exportMDL(std::vector<Geom::GeomEntity*>& ge,
            const std::string& n);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Export dans un fichier au format VTK de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportVTK(const std::string& n);

    /** \brief Export d'une sélection dans un fichier au format VTK
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportVTK(std::vector<std::string>& ge,
    		const std::string& n);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* exportVTK(std::vector<Geom::GeomEntity*>& ge,
    		const std::string& n);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Export dans un fichier au format STL d'une entité géométrique (une surface)
     *
     *  \param ge le nom de l'entité que l'on exporte
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportSTL(const std::string& ge,
    		const std::string& n);
	SET_SWIG_COMPLETABLE_METHOD(exportSTL)

#ifndef SWIG
	Mgx3D::Internal::M3DCommandResultIfc* exportSTL(Geom::GeomEntity* geomEntity,
    		const std::string& n);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Export au format Mli de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportMLI(const std::string& n);

    /** \brief Export d'une sélection au format Mli
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportMLI(std::vector<std::string>& ge,
    		const std::string& n);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* exportMLI(std::vector<Geom::GeomEntity*>& ge,
    		const std::string& n);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Export au format BREP de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportBREP(const std::string& n);

    /** \brief Export d'une sélection au format BREP
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportBREP(std::vector<std::string>& ge,
            const std::string& n);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* exportBREP(std::vector<Geom::GeomEntity*>& ge,
            const std::string& n);
#endif
    /*------------------------------------------------------------------------*/
    /** \brief Export au format STEP de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportSTEP(const std::string& n);

    /** \brief Export d'une sélection au format STEP
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportSTEP(std::vector<std::string>& ge,
            const std::string& n);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* exportSTEP(std::vector<Geom::GeomEntity*>& ge,
            const std::string& n);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Export au format IGES de toute la géométrie
     *
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportIGES(const std::string& n);

    /** \brief Export d'une sélection au format IGES
     *
     *  \param ge la liste des noms des entités à exporter
     *  \param n le nom du ficher dans lequel on exporte
     */
    Mgx3D::Internal::M3DCommandResultIfc* exportIGES(std::vector<std::string>& ge,
    		const std::string& n);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* exportIGES(std::vector<Geom::GeomEntity*>& ge,
            const std::string& n);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Fusion Booléenne de n entités géométriques
     *
     *  \param entities les entités sur lesquelles on travaille
     */
    Mgx3D::Internal::M3DCommandResultIfc* fuse(
			std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(fuse)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* fuse(
			std::vector<Geom::GeomEntity*>& entities);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Intersection Booléenne de n entités géométriques
     *
     *  \param entities les entités sur lesquelles on travaille
     */
    Mgx3D::Internal::M3DCommandResultIfc* common(
			std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(common)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* common(
			std::vector<Geom::GeomEntity*>& entities);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Intersection Booléenne de 2 entités géométriques 1D ou 2D, détruit les entités de base
     *
     *  \param entity1 (courbe ou surface)
     *  \param entity2 (comme la précédente)
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	     common2D(std::string entity1, std::string entity2, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(common2D)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	     common2D(Geom::GeomEntity* entity1, Geom::GeomEntity* entity2, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Intersection Booléenne de 2 entités géométriques 1D ou 2D, ne détruit pas les entités de base
     *
     *  \param entity1 (courbe ou surface)
     *  \param entity2 (comme la précédente)
     *  \param groupName groupe dans lequel sont mise les nouvelles entités
     */
    Mgx3D::Internal::M3DCommandResultIfc*
	     common2DOnCopy(std::string entity1, std::string entity2, std::string groupName);
	SET_SWIG_COMPLETABLE_METHOD(common2DOnCopy)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc*
	     common2DOnCopy(Geom::GeomEntity* entity1, Geom::GeomEntity* entity2, std::string groupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Différence Booléenne de n entités géométriques avec la première
     *         entité qui est conservée
     *
     *  \param tokeep l(es) entité(s) que l'on conserve
     *  \param tocut  les entités que l'on retire de tokeep
     */
    Mgx3D::Internal::M3DCommandResultIfc* cut(
            std::string tokeep,std::vector<std::string>& tocut);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* cut(
            Geom::GeomEntity* tokeep, std::vector<Geom::GeomEntity*>& tocut);
#endif

    Mgx3D::Internal::M3DCommandResultIfc* cut(
            std::vector<std::string>& tokeep,std::vector<std::string>& tocut);

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* cut(
            std::vector<Geom::GeomEntity*>& tokeep, std::vector<Geom::GeomEntity*>& tocut);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Collage d'entités géométriques
     *
     *  \param entities les entités sur lesquelles on travaille
     */
    Mgx3D::Internal::M3DCommandResultIfc* glue(
			std::vector<std::string>& entities);
	SET_SWIG_COMPLETABLE_METHOD(glue)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* glue(
			std::vector<Geom::GeomEntity*>& entities);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief Section d'un groupe d'entités géométrique par un outil
     *
     *  \param entities les entités que l'on veut couper
     *  \param tool     l'entité pour découper
     */
    Mgx3D::Internal::M3DCommandResultIfc* section(
            std::vector<std::string>& entities, std::string tool);
    SET_SWIG_COMPLETABLE_METHOD(section)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* section(
            std::vector<Geom::GeomEntity*>& entities, Geom::GeomEntity* tool);
#endif

    Mgx3D::Internal::M3DCommandResultIfc* sectionByPlane(
            std::vector<std::string>& entities,  Utils::Math::Plane* tool,
            std::string planeGroupName);
    SET_SWIG_COMPLETABLE_METHOD(sectionByPlane)

#ifndef SWIG
    Mgx3D::Internal::M3DCommandResultIfc* sectionByPlane(
            std::vector<Geom::GeomEntity*>& entities, Utils::Math::Plane* tool,
            std::string planeGroupName);
#endif

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des volumes (non détruits) gérées par le manager
     */
    std::vector<std::string> getVolumes() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des surfaces (non détruits) gérées par le manager
     */
    std::vector<std::string> getSurfaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des courbes (non détruits) gérées par le manager
     */
    std::vector<std::string> getCurves() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne la liste des sommets (non détruits) gérées par le manager
     */
    std::vector<std::string> getVertices() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de volumes gérés
     */
    int getNbVolumes() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de surfaces gérées par le manager
     */
    int getNbSurfaces() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de courbes gérées par le manager
     */
    int getNbCurves() const;

    /*------------------------------------------------------------------------*/
    /** \brief retourne le nombre de sommets gérées par le manager
     */
    int getNbVertices() const;

    /** Ajoute un volume au gestionnaire */
    void add (Volume* v) {m_volumes.push_back(v);}
    /** Ajoute une surface au gestionnaire */
    void add (Surface* s) {m_surfaces.push_back(s);}
    /** Ajoute une courbe au gestionnaire */
    void add (Curve* c) {m_curves.push_back(c);}
    /** Ajoute un sommet au gestionnaire */
    void add (Vertex* v) {m_vertices.push_back(v);}
    /** Ajoute une entité au gestionnaire */
    void addEntity (GeomEntity* ge);

    /** Enlève un volume du gestionnaire */
    void remove (Volume* v);
    /** Enlève une surface au gestionnaire */
    void remove (Surface* s);
    /** Enlève une courbe au gestionnaire */
    void remove (Curve* c);
    /** Enlève un sommet au gestionnaire */
    void remove (Vertex* v);
    /** Enlève une entité au gestionnaire */
    void removeEntity (GeomEntity* ge);

#ifndef SWIG
	/** Retourne le Volume suivant le nom en argument */
	Volume* getVolume(const std::string& name, const bool exceptionIfNotFound=true) const;
	/** Retourne la Surface suivant le nom en argument */
	Surface* getSurface(const std::string& name, const bool exceptionIfNotFound=true) const;
	/** Retourne la Curve suivant le nom en argument */
	Curve* getCurve(const std::string& name, const bool exceptionIfNotFound=true) const;
	/** Retourne le Vertex suivant le nom en argument */
	Vertex* getVertex(const std::string& name, const bool exceptionIfNotFound=true) const;
	/** Retourne la GeomEntity suivant le nom en argument */
	GeomEntity* getEntity(const std::string& name, const bool exceptionIfNotFound=true) const;

	/*------------------------------------------------------------------------*/
	/** \brief retourne la liste des volumes gérées par le manager
	 */
	std::vector<Volume*> getVolumesObj() const;
	/** \brief retourne la liste des surfaces gérées par le manager
	 */
	std::vector<Surface*> getSurfacesObj() const;
	/** \brief retourne la liste des courbes gérées par le manager
	 */
	std::vector<Curve*> getCurvesObj() const;
	/** \brief retourne la liste des sommets gérées par le manager
	 */
	std::vector<Vertex*> getVerticesObj() const;
#endif

    /** Retourne le nom du sommet en fonction d'une position géométrique */
    std::string getVertexAt(const Point& pt1) const;

    /** Retourne le nom de la courbe en fonction d'un triplet de positions géométriques */
    std::string getCurveAt(const Point& pt1, const Point& pt2, const Point& pt3) const;

    /** Retourne le nom de la surface en fonction des positions géométriques de ses sommets */
    std::string getSurfaceAt(std::vector<Point>& pts) const;

    /** Retourne le nom de le volume en fonction des positions géométriques de ses sommets */
    std::string getVolumeAt(std::vector<Point>& pts) const;

    /** Retourne le nom du dernier Volume */
    std::string getLastVolume() const;
    /** Retourne le nom de la dernière Surface */
    std::string getLastSurface() const;
    /** Retourne le nom de la dernière Curve */
    std::string getLastCurve() const;
    /** Retourne le nom du dernier Vertex */
    std::string getLastVertex() const;

    /** Retourne l'indice du sommet v*/
    int getIndexOf(Vertex* v) const;
    /** Retourne l'indice de la courbe c*/
    int getIndexOf(Curve* c) const;
    /** Retourne l'indice de la surface s*/
    int getIndexOf(Surface* s) const;
    /** Retourne l'indice du volume v*/
    int getIndexOf(Volume* v) const;

    /** Ajoute un groupe à un ensemble d'entités géométriques, suivant une dimension */
    Internal::M3DCommandResultIfc* addToGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
    SET_SWIG_COMPLETABLE_METHOD(addToGroup)

    /** Enlève un groupe à un ensemble d'entités géométriques, suivant une dimension */
    Internal::M3DCommandResultIfc* removeFromGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
    SET_SWIG_COMPLETABLE_METHOD(removeFromGroup)

    /** Définit le groupe pour un ensemble d'entités géométriques, suivant une dimension */
    Internal::M3DCommandResultIfc* setGroup(std::vector<std::string>& ve, int dim, const std::string& groupName);
    SET_SWIG_COMPLETABLE_METHOD(setGroup)

#ifndef SWIG
    /// recherche les entitées à partir des noms
    void convert(std::vector<std::string>& names, std::vector<GeomEntity*>& entities);
#endif

private:
    /** volumes gérés par le manager */
    std::vector<Volume*>  m_volumes;
    /** surfaces gérées par le manager */
    std::vector<Surface*> m_surfaces;
    /** courbes gérées par le manager */
    std::vector<Curve*>   m_curves;
    /** sommets gérés par le manager */
    std::vector<Vertex*>  m_vertices;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GEOMMANAGER_H_ */
/*----------------------------------------------------------------------------*/
