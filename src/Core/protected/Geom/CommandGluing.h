/*----------------------------------------------------------------------------*/
/*
 * CommandSewing.h
 *
 *  Created on: 1 déc. 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDGLUING_H_
#define COMMANDGLUING_H_
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
/** \class CommandGluing
 *  \brief Commande permettant de coller 2 géométries
 */
/*----------------------------------------------------------------------------*/
class CommandGluing: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur
     *
     *  \param c le contexte
     *  \param es les entités à coller
     */
    CommandGluing(Internal::Context& c, std::vector<GeomEntity*>& es);

    /*------------------------------------------------------------------------*/
    /** \brief Constructeur
     *
     *  \param c le contexte
     *  \param ccg les entités à coller dont on ne dispose que de la commande de création
     */
    CommandGluing(Internal::Context& c, std::vector<CommandCreateGeom*>& ccg);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandGluing();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  Préparation en début de commande
     */
    void internalSpecificPreExecute();

protected:

    /// valide les paramètres
    void validate();

    /** entités à unir */
    std::vector<GeomEntity*> m_entities;

    /** les commandes de création des entités à unir */
    std::vector<CommandCreateGeom*> m_command_create_entities;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDGLUING_H_ */
/*----------------------------------------------------------------------------*/

