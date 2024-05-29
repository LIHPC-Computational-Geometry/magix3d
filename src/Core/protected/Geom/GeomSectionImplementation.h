/*----------------------------------------------------------------------------*/
/*
 * GeomSectionImplementation.h
 *
 *  Created on: 7 oct. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GEOMSECTIONIMPLEMENTATION_H_
#define GEOMSECTIONIMPLEMENTATION_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Vector.h"
#include "Geom/GeomModificationBaseClass.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS_Shape.hxx>
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
 * \class GeomSectionImplementation
 * \brief Classe réalisant le splitting de deux entités géométriques
 *
 */
class GeomSectionImplementation: public GeomModificationBaseClass{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur.
     *
     *  \param es les entités sur lesquelles appliquer le splitting

     */
    GeomSectionImplementation(Internal::Context& c, std::vector<GeomEntity*>& es,
            GeomEntity* t);

    /*------------------------------------------------------------------------*/
    /** \brief   Destructeur
     */
    virtual ~GeomSectionImplementation();

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation du collage
     *          Les entités créées sont stockées dans res
     */
    void perform(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  intialisation de données avant le perform pour les mementos des
     *          commandes appelantes
     */
    void prePerform();


protected:

    /**
     * Methodes surchargée pour mettre à jour la shape OCC d'entités M3D adjacentes
     * à celles passées en paramètre de la section.
     *
     * @param entities un vecteur d'entités géométriques M3D
     */
//    virtual void manageAdjacentEntities(std::vector<GeomEntity*>& entities);

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la coupe par m_tool
     */
    void sectionVolumes(std::vector<GeomEntity*>& res);

    /*------------------------------------------------------------------------*/
    /** \brief  réalisation de la coupe par m_tool
     */
    void sectionSurfaces(std::vector<GeomEntity*>& res);

private:

    /** Entités qui vont être modifiées par l'algorithme. Purement technique pour
     * faire le lien entre initialize() et finalize()
     *
     */
    std::vector<GeomEntity*> entities_to_update;
    std::vector<GeomEntity*> entities_new;

    /* entités passées en argument à la commande. */
    std::vector<GeomEntity*> m_entities_param;
    /* outil de découpe */
    GeomEntity* m_tool;


    TopoDS_Shape m_restricted_section_tool;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GEOMSECTIONIMPLEMENTATION_H_ */
/*----------------------------------------------------------------------------*/
