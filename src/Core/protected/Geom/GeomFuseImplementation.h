/*----------------------------------------------------------------------------*/
/** \file GeomFuseImplementation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 04/02/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMFUSEIMPLEMENTATION_H_
#define GEOMFUSEIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomFuseImplementation
 * \brief Classe réalisant la fusion (union Booléenne) de deux entités
 *        géométriques volumiques (dim=3)
 *
 */
class GeomFuseImplementation: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à traiter

     */
    GeomFuseImplementation( Internal::Context& c, std::vector<GeomEntity*> e);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomFuseImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'union. Les entités créées sont stockées dans
     *          res
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'union de volumes
     */
    void fuseVolumes(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'union de surfaces
     */
    void fuseSurfaces(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'union de courbes
     */
    void fuseCurves(std::vector<GeomEntity*>& res);

    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMFUSEIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/


