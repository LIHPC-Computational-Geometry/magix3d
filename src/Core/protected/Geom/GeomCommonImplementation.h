/*----------------------------------------------------------------------------*/
/*
 * GeomCommonImplementation.h
 *
 *  Created on: 14 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMCOMMONIMPLEMENTATION_H_
#define GEOMCOMMONIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
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
 * \class GeomCommonImplementation
 * \brief Classe réalisant l'intersection de deux entités
 *        géométriques 3D
 *
 */
class GeomCommonImplementation: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à traiter
     */
    GeomCommonImplementation(Internal::Context& c,std::vector<GeomEntity*> e);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomCommonImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'intersection. Les entités créées sont stockées
     *          dans res
     */
    void perform(std::vector<GeomEntity*>& res);


    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'intersection de volumes
     */
    void commonVolumes(std::vector<GeomEntity*>& res);

    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMCOMMONIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



