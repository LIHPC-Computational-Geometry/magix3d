/*----------------------------------------------------------------------------*/
/*
 * \file CommandChangeLandmark.h
 *
 *  \author Charles Pignerol
 *
 *  \date 1 sept. 2016
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCHANGELANDMARK_H_
#define COMMANDCHANGELANDMARK_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
#include "Internal/Context.h"
#include "Utils/Landmark.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
/*----------------------------------------------------------------------------*/
/** \class CommandChangeLandmark
 *  \brief Commande permettant de changer de repère (maillage/chambre d'expérience)
 */
/*----------------------------------------------------------------------------*/
class CommandChangeLandmark: public Internal::CommandInternal{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
     *  \param context  le contexte
     *  \param l le nouveau repère
     */
    CommandChangeLandmark(Internal::Context& context, Utils::Landmark::kind l);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandChangeLandmark();

    /*------------------------------------------------------------------------*/
    /** \brief  exécute la commande
     */
    void internalExecute();

    /** Ce qui est fait de propre à la commande pour annuler une commande */
    virtual void internalUndo();

    /** Ce qui est fait de propre à la commande pour rejouer une commande */
    virtual void internalRedo();


protected:

    /* nouveau repère */
    Utils::Landmark::kind	m_new_landmark;

    /* ancien repère */
    Utils::Landmark::kind	m_old_landmark;


private :

	CommandChangeLandmark (const CommandChangeLandmark&);
	CommandChangeLandmark& operator = (const CommandChangeLandmark&);
};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCHANGELANDMARK_H_ */
/*----------------------------------------------------------------------------*/
