/*----------------------------------------------------------------------------*/
/*
 * GeomCommon2DImplementation.h
 *
 *  \author Eric B
 *
 *  \date 7/9/2018
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMCOMMON2DIMPLEMENTATION_H_
#define GEOMCOMMON2DIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
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
class CommandGeomCopy;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomCommon2DImplementation
 * \brief Classe réalisant l'intersection de deux entités
 *        géométriques
 *
 */
class GeomCommon2DImplementation: public GeomModificationBaseClass {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à traiter
     */
    GeomCommon2DImplementation(Internal::Context& c, GeomEntity* e1, GeomEntity* e2);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param geom_cmd la commande de création de la copie des entités
     */
    GeomCommon2DImplementation(Internal::Context& c, Geom::CommandGeomCopy* geom_cmd);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomCommon2DImplementation();

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


    GeomEntity* getEntity1() const {return m_entity1;}
    GeomEntity* getEntity2() const {return m_entity2;}

protected:

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de l'intersection de 2 surfaces */
    void commonSurfaces();
    /** \brief  réalisation de l'intersection de 2 courbes */
    void commonCurves();

    /// la commande de copie qui permet d'accéder aux copies géométriques
    Geom::CommandGeomCopy* m_geomCmd;

    /* entités passées en argument à la commande. */
    GeomEntity* m_entity1;
    GeomEntity* m_entity2;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMCOMMON2DIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/



