/*----------------------------------------------------------------------------*/
/*
 * CommandCut.h
 *
 *  Created on: 14 févr. 2012
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCUT_H_
#define COMMANDCUT_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandCut
 *  \brief Commande permettant d'effectuer une opération de différence
 *         ensembliste
 */
/*----------------------------------------------------------------------------*/
class CommandCut: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e l'entité à conserver
     *  \param es les entités à retirer de e
     */
    CommandCut(Internal::Context& c, GeomEntity* e,
                std::vector<GeomEntity*>& es);
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param es     les entités à conserver
     *  \param es_cut les entités à retirer de es
     */
    CommandCut(Internal::Context& c,  std::vector<GeomEntity*>& es,
                std::vector<GeomEntity*>& es_cut);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandCut();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();


    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();


protected:

    void validate();

    /* entités à conserver */
    GeomEntity* m_entity;

    /* entités à retirer */
    std::vector<GeomEntity*> m_cutting_entities;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCUT_H_ */
/*----------------------------------------------------------------------------*/


