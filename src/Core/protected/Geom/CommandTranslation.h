/*----------------------------------------------------------------------------*/
/** \file CommandTranslation.h
 *
 *  \author Franck Ledoux
 *
 *  \date 04/11/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDTRANSLATION_H_
#define COMMANDTRANSLATION_H_
/*----------------------------------------------------------------------------*/
#include <vector>
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
/** \class CommandTranslation
 *  \brief Commande permettant de translater une entité géométrique
 */
/*----------------------------------------------------------------------------*/
class CommandTranslation: public CommandEditGeom{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param e les entités à translater
     *  \param dv le vecteur de translation
     */
    CommandTranslation(Internal::Context& c,
            std::vector<GeomEntity*>& e,
            const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param cmd la commande de construction des entités à déplacer
     *  \param dv le vecteur de translation
     */
    CommandTranslation(Internal::Context& c,
    		Geom::CommandGeomCopy* cmd,
            const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param c le contexte
     *  \param dv le vecteur de translation
     */
    CommandTranslation(Internal::Context& c,
            const Utils::Math::Vector& dv);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandTranslation();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalSpecificExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  initialisation de la commande
     */
    void internalSpecificPreExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  Traitement spécifique délégué aux classes filles
     */
    void internalSpecificUndo();
    void internalSpecificRedo();

protected:

    /// valide les paramètres
    void validate();

    /* vecteur de translation */
    Utils::Math::Vector m_dv;

};
/*----------------------------------------------------------------------------*/
} // end namespace Geom
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDTRANSLATION_H_ */
/*----------------------------------------------------------------------------*/



