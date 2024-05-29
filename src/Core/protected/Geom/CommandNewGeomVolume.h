/*----------------------------------------------------------------------------*/
/*
 * CommandNewGeomVolume.h
 *
 *  Created on: 3 nov. 2014
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDNEWGEOMVOLUME_H_
#define COMMANDNEWGEOMVOLUME_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Geom/CommandEditGeom.h"
#include "Geom/GeomEntity.h"
#include "Internal/Context.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Geom {
/*----------------------------------------------------------------------------*/
class GeomManager;
/*----------------------------------------------------------------------------*/
/** \class CommandNewGeomVolume
 *  \brief Commande permettant de créer un volume à partir de surfaces
 */
/*----------------------------------------------------------------------------*/
class CommandNewGeomVolume: public CommandEditGeom
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param surfs les entités à réunir
     */
    CommandNewGeomVolume( Internal::Context& c,
                          std::vector<Surface*>& surfs,
                          const std::string& groupName);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandNewGeomVolume();

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

    /* entité surfaciques à unir */
    std::vector<Surface*> m_surfaces;
};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDNEWGEOMVOLUME_H_ */
/*----------------------------------------------------------------------------*/
