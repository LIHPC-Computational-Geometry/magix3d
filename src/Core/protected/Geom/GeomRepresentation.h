/*----------------------------------------------------------------------------*/
/** \file GeomRepresentation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 19/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_GEOM_GEOMREPRESENTATION_H_
#define MGX3D_GEOM_GEOMREPRESENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Internal/ContextIfc.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Utils/DisplayRepresentation.h"
#include "Utils/Vector.h"
#include "Utils/Plane.h"
/*----------------------------------------------------------------------------*/

#include <GMDS/Math/Triangle.h>

// OCC
#include <gp_Pln.hxx>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Curve;
class Volume;
class Surface;
class Vertex;
class GeomEntity;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomRepresentation
 *
 * \brief Définit une interface que devront implémenter les propriétés de
 *        calcul géométrique lié à un objet géométrique.
 */
class GeomRepresentation{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GeomRepresentation(){;}

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GeomRepresentation()
    {;}

    /*------------------------------------------------------------------------*/
    /** \brief  Calcule l'aire d'une entité:  Pour une courbe, c'est la
     *          longueur, pour une surface, l'aire, pour un volume le volume.
     */
    virtual double computeVolumeArea()=0;
    virtual double computeSurfaceArea()=0;
    virtual double computeCurveArea()=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Calcul de la boite englobante orientée selon les axes Ox,Oy,Oz
     *
     *  \param pmin Les coordonnées min de la boite englobante
     *  \param pmax Les coordonnées max de la boite englobante
     */
    virtual void computeBoundingBox(Utils::Math::Point& pmin,Utils::Math::Point& pmax,
                                    double tol = 0.00) const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité en ses sous-entités. Elle ne les crée pas
     *          directement mais demande au manager passé en argument de les
     *          créer.
     *
     *  \param owner l'entité qui demande le découpage
     */
    virtual void split( std::vector<Surface*>& surf,
                        std::vector<Curve*  >& curv,
                        std::vector<Vertex* >&  vert,
                        Volume* owner)=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité surfacique en ses sous-entités.
     */
    virtual void split(std::vector<Curve*>& curv,
               std::vector<Vertex* >&  vert,
               Surface* owner)=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Décompose l'entité linéaire en ses sous-entités.
     */
    virtual void split( std::vector<Vertex* >&  vert,
                Curve* owner)=0;
    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur l'entité géométrique associée.
     *
     *  \param P le point à projeter qui sera modifié
     *  \param C la courbe sur laquelle on projette
     */
    virtual void project(Utils::Math::Point& P, const Curve* C) =0;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P sur l'entité géométrique associée.
     *
     *  \param P le point à projeter qui sera modifié
     *  \param S la surface sur laquelle on projette
     */
    virtual void project(Utils::Math::Point& P, const Surface* S) =0;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur l'entité géométrique associée.
     *         Le résultat est le point P2.
     *
     *  \param P1 le point à projeter
     *  \param P2 le résultat de la projection
     *  \param S la surface sur laquelle on projete
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2,
                         const Surface* S) =0 ;

    /*------------------------------------------------------------------------*/
    /** \brief Projete le point P1 sur l'entité géométrique associée.
     *         Le résultat est le point P2.
     *
     *  \param P1 le point à projeter
     *  \param P2 le résultat de la projection
     *  \param C la courbe sur laquelle on projete
     */
    virtual void project(const Utils::Math::Point& P1, Utils::Math::Point& P2,
                         const Curve* C) =0 ;

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la normale à une surface en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la normale recherchée suivant le projeté du point
     *  \param S la surface sur laquelle on projete
     */
    virtual void normal(const Utils::Math::Point& P1, Utils::Math::Vector& V2, const Surface* S) =0;

    /*------------------------------------------------------------------------*/
    /** \brief Calcul la tangente à une courbe en un point
     *
     *  \param P1 le point à projeter
     *  \param V2 la tangente recherchée suivant le projeté du point
     *  \param C la courbe sur laquelle on projete
     */
    virtual void tangent(const Utils::Math::Point& P1, Utils::Math::Vector& V2) =0;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point en fonction du paramètre sur la courbe
     * \param p le paramètre curviligne
     * \param Pt le résultat
     */
    virtual void getPoint(const double& p, Utils::Math::Point& Pt,
            const bool in01=false) =0 ;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le point a l'intersection avec le plan
     * \param plan_cut le plan
     * \param Pt le résultat
     */
    virtual void getIntersection(gp_Pln& plan_cut, Utils::Math::Point& Pt) =0 ;

    /*------------------------------------------------------------------------*/
    /** \brief Donne le paramètre curviligne du point sur la courbe
     * \param Pt le point sur la courbe
     */
    virtual void getParameter(const Utils::Math::Point& Pt, double& p,
            const Curve* C) =0 ;

    /*------------------------------------------------------------------------*/
    /** \brief Donne les paramètres curviligne aux extrémités de la courbe
     */
    virtual void getParameters(double& first, double& last) =0 ;

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la translation de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param V le vecteur de translation à appliquer
     */
    virtual void translate(const Utils::Math::Vector& V) =0;

    /*------------------------------------------------------------------------*/
    /** \brief Effectue l'homothétié de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param F le facteur d'homothétie
     *  \param center le centre d'homothétie
     */
    virtual void scale(const double F, const Utils::Math::Point& center) =0;
    virtual void scale(const double factorX,
            const double factorY,
            const double factorZ) =0;

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la rotation de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param P1    premier point definissant l'axe de rotation
     *  \param P2    second point definissant l'axe de rotation
     *  \param Angle    angle de rotation en degré (compris entre ]0,360])
     */
    virtual void rotate(const Utils::Math::Point& P1,
            const Utils::Math::Point& P2, double Angle)=0;

    /*------------------------------------------------------------------------*/
    /** \brief Effectue la symétrie de l'entité géométrique associée de
     *         manière isolée. C'est-à-dire que aucune des entités incidentes
     *         n'est impactée par ce traitement
     *
     *  \param plane le plan de symétrie
     */
    virtual void mirror(const Utils::Math::Plane& plane) =0;


//    /*------------------------------------------------------------------------*/
//    /** \brief Met à jour la représentation géométrique. Pour cela on doit avoir
//     *         accès aux entités géométriques M3D impliquées, d'où le paramètre
//     *         de type GeomEntity
//     *
//     *  \param entity l'entité géométrique que l'on met à jour
//     */
//    virtual void updateShape(GeomEntity* entity) =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation affichable de l'entité en se basant
     *          sur la représentation passée en paramètre
     *
     *  \param dr la représentation que l'on demande à afficher
     *  \param caller un pointeur sur la géométrie appelant cette méthode. Il
     *         est nécessaire à cause de la discrétisation de courbes qui
     *         peuvent nécessiter d'accéder à des entités de dimension
     *         supérieure.
     */
    virtual void buildDisplayRepresentation(Utils::DisplayRepresentation& dr,
                                   const GeomEntity* caller) const=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit une représentation facétisée (point, segments, polygones) de l'entité
     *
     */
    virtual void getFacetedRepresentation(
            std::vector<gmds::math::Triangle >& AVec,
            const GeomEntity* caller) const=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Renvoie si la surface AEntityOrientation est bien orientée
     * 			par rapport au volume ACaller.
     *
     */
    virtual void facetedRepresentationForwardOrient(
    		const GeomEntity* ACaller,
    		const GeomEntity* AEntityOrientation,
    		std::vector<gmds::math::Triangle >*) const=0;

    /*------------------------------------------------------------------------*/
    /** \brief  Fournit un clone de l'instance de représentation géométrique.
     *          Pour cela, un appel à "new" est effectué dans cette commande.
     *
     *  \return un pointeur vers une représentation géométrique identique
     */
    virtual GeomRepresentation* clone() const=0;


    /*------------------------------------------------------------------------*/
    /** \brief  Fournit la précision
     *
     *  \return un double (en général de l'ordre de e-5)
     */
   virtual double getPrecision() =0;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GEOM_GEOMREPRESENTATION_H_ */
/*----------------------------------------------------------------------------*/



