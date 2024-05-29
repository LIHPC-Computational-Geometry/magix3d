/*----------------------------------------------------------------------------*/
/*
 * \file CommandNewSysCoord.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 22 mai 2018
 */
/*----------------------------------------------------------------------------*/
#ifndef PROTECTED_SYSCOORD_COMMANDNEWSYSCOORD_H_
#define PROTECTED_SYSCOORD_COMMANDNEWSYSCOORD_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace CoordinateSystem {
/*----------------------------------------------------------------------------*/
class CommandNewSysCoord: public Internal::CommandInternal {

	enum method {parDefaut=0, unCentre, unCentreEt2Points};

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     * \param c le contexte (permet l'accès au manager)
     */
	CommandNewSysCoord(Internal::Context& c,
			const std::string& groupName="");

	/** \brief  Constructeur
	 * \param c le contexte (permet l'accès au manager)
	 * \param p le centre de ce repère
	 */
	CommandNewSysCoord(Internal::Context& c,
			const Utils::Math::Point& p,
			const std::string& groupName="");

	/** \brief  Constructeur
	 * \param c le contexte (permet l'accès au manager)
	 * \param pCentre le centre de ce repère
	 * \param pX le point dans la direction des X
	 * \param pY le point dans la direction des Y
	 */
	CommandNewSysCoord(Internal::Context& c,
			const Utils::Math::Point& pCentre,
			const Utils::Math::Point& pX,
			const Utils::Math::Point& pY,
			const std::string& groupName="");

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur, destruction du repère
     */
    virtual ~CommandNewSysCoord();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    virtual void internalExecute();

    /*------------------------------------------------------------------------*/
    /** \brief  annule la commande
     */
    virtual void internalUndo();

    /*------------------------------------------------------------------------*/
    /** \brief  rejoue la commande
     */
    virtual void internalRedo();

    /** \brief  ajoute à un groupe un repère
     */
    virtual void addToGroup(SysCoord* rep, bool use_default_name = false);

    /*------------------------------------------------------------------------*/
    /** \brief  exécute et annule la commande, retourne dans DisplayRepresentation
     *  une représentation des entités créées
     *
     *  La commande doit ensuite être détruite
     */
    virtual void getPreviewRepresentation(Utils::DisplayRepresentation& dr);

private:

    /// méthode de création
    method m_method;

    /// centre du repère local
    Utils::Math::Point m_center;
    /// le point dans la direction des X
    Utils::Math::Point m_pX;
    /// le point dans la direction des Y
    Utils::Math::Point m_pY;

    /// le nom d'un groupe dans lequel on met les nouvelles entités (peut être vide)
    std::string m_group_name;
};
/*----------------------------------------------------------------------------*/
} // end namespace CoordinateSystem
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* PROTECTED_SYSCOORD_COMMANDNEWSYSCOORD_H_ */
/*----------------------------------------------------------------------------*/
