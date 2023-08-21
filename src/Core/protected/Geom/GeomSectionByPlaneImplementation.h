/*----------------------------------------------------------------------------*/
/*
 * GeomSectionByPlaneImplementation.h
 *
 *  Created on: 24 juin 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMSECTIONIMPLEMENTATION_H_
#define GEOMSECTIONIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Plane.h"
#include "Geom/GeomEntity.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D
{
/*----------------------------------------------------------------------------*/
namespace Geom
{
/*----------------------------------------------------------------------------*/
class Volume;
class Surface;
class Curve;
class Vertex;
/*----------------------------------------------------------------------------*/
/**
 * \class GeomSectionImplementation
 * \brief Classe réalisant la section d'une collection d'entités géométriques
 *        par un plan
 *
 */
class GeomSectionByPlaneImplementation: public GeomModificationBaseClass
{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param e les entités à traiter
     *  \param p le plan de coupe

     */
    GeomSectionByPlaneImplementation(Internal::Context& c, std::vector<GeomEntity*>& e,
            Utils::Math::Plane* p);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomSectionByPlaneImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la découpe. Les entités créées sont stockées dans
     *          res
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  retourne la liste des entites qui sont sur le plan de coupe
     */
    std::vector<GeomEntity*> getEntitiesOnPlane();

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:

    /*------------------------------------------------------------------------*/
        /** \brief  réalisation de la découpe.
         *  Les entités créées sont stockées dans res
         */
    void splitEntities(std::vector<GeomEntity*>& res);

    /**
     * Methodes surchargée pour mettre à jour la shape OCC d'entités M3D adjacentes
     * à celles passées en paramètre de la section.
     *
     * @param entities un vecteur d'entités géométriques M3D
     */
//    virtual void manageAdjacentEntities(std::vector<GeomEntity*>& entities);

    bool isOnPlane(GeomEntity *e);
    bool isOnPlane(Vertex *v);
    bool isOnPlane(Curve *c);
    bool isOnPlane(Utils::Math::Point& p);
    Utils::Math::Plane* m_plane;
private:

    /** Entités qui vont être modifiées par l'algorithme. Purement technique pour
     * faire le lien entre initialize() et finalize()
     *
     */
    std::vector<GeomEntity*> entities_to_update;
    std::vector<GeomEntity*> entities_new;


    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;

    TopoDS_Shape m_restricted_section_tool;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMSECTIONIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/

