/*----------------------------------------------------------------------------*/
/*
 * CommandSection.h
 *
 *  Created on: 22 janv. 2013
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDSECTION_H_
#define COMMANDSECTION_H_
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Utils/Vector.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandSection
 *  \brief Commande permettant de découper des géométries par une autre
 */
/*----------------------------------------------------------------------------*/
class CommandSection: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur
     *
     *  \param c  le contexte
     *  \param es les entités à découper
     *  \param t  l'objet servant à couper
     */
    CommandSection(Internal::Context& c, std::vector<GeomEntity*>& es,
                    GeomEntity* t);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandSection();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();


    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();



protected:

    /// valide les paramètres
    void validate();

    /* entités à découper*/
    std::vector<GeomEntity*> m_entities;

    /* outil de découpe */
    GeomEntity* m_tool;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDSECTION_H_ */
/*----------------------------------------------------------------------------*/


