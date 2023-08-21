/*----------------------------------------------------------------------------*/
/*
 * \file CommandRotateSysCoord.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 6 Juin 2018
 */
/*----------------------------------------------------------------------------*/
#ifndef PROTECTED_SYSCOORD_COMMANDROTATESYSCOORD_H_
#define PROTECTED_SYSCOORD_COMMANDROTATESYSCOORD_H_
/*----------------------------------------------------------------------------*/
#include "SysCoord/CommandEditSysCoord.h"
#include "Utils/Rotation.h"
#include "gp_Trsf.hxx"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {

class CommandDuplicateSysCoord;

/*----------------------------------------------------------------------------*/
class CommandRotateSysCoord: public CommandEditSysCoord {

public:

	/*------------------------------------------------------------------------*/
	/** \brief  Constructeur
	 * \param c le contexte (permet l'accès au manager)
	 * \param syscoord le repère à modifier
	 * \param rot la rotation
	 */
	CommandRotateSysCoord(Internal::Context& c,
			SysCoord* syscoord, const Utils::Math::Rotation& rot);

	/** \brief  Constructeur
	 * \param c le contexte (permet l'accès au manager)
	 * \param cmd la commande de création du repère à modifier
	 * \param rot la rotation
	 */
	CommandRotateSysCoord(Internal::Context& c,
			CommandDuplicateSysCoord* cmd, const Utils::Math::Rotation& rot);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandRotateSysCoord();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();


private:

    /// transformation d'un point
    Point transform(gp_Trsf& tr, Point& pt);

    //// repère initial, copié
    SysCoord* m_syscoord;

    /// commande de création du repère
    CommandDuplicateSysCoord* m_command;

    /// objet définissant la rotation
    Rotation m_rotation;
};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROTECTED_SYSCOORD_COMMANDROTATESYSCOORD_H_ */
/*----------------------------------------------------------------------------*/
