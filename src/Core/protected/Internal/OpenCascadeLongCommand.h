/*----------------------------------------------------------------------------*/
/*
 * \file OpenCascadeLongCommand.h
 *
 *  \author Charles PIGNEROL
 *
 *  \date	21/01/2011
 */
/*----------------------------------------------------------------------------*/
#ifndef OPEN_CASCADE_LONG_COMMAND_H
#define OPEN_CASCADE_LONG_COMMAND_H

#include "Internal/CommandInternal.h"

namespace Mgx3D
{

namespace Internal 
{

/*----------------------------------------------------------------------------*/
/** \class OpenCascadeLongCommand
 *  \brief Commande très longue et non interruptible pour test avec les threads.
 *
 */
/*----------------------------------------------------------------------------*/
class OpenCascadeLongCommand : public Internal::CommandInternal 
{
	public:

	/**
	 * \param le nom de la commande
	 */
	OpenCascadeLongCommand  (Internal::Context& c, std::string name);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~OpenCascadeLongCommand ( );

	/**
	 * \return		<I>true</I>, mais juste pour être lancée dans un autre
	 *				thread.
	 */
	virtual bool cancelable  ( ) const
	{ return false; }

    /**
     * la commande a vocation à être lancée dans un autre <I>thread</I>.
     */
	virtual bool threadable ( ) const
	{ return true; }

	/**
	 * Le temps estimé pour exécuter une commande.
	 */
	virtual unsigned long getEstimatedDuration  (PLAY_TYPE playType = DO)
	{ return  (unsigned long)20; }

	/**
	 * \brief  exécute la commande
	 */
	virtual void internalExecute ( );

    /** \brief  annule la commande
     */
    void internalUndo();

    /** \brief  rejoue la commande
     */
    void internalRedo();
};	// class OpenCascadeLongCommand

} // end namespace Internal

} // end namespace Mgx3D

#endif /* OPEN_CASCADE_LONG_COMMAND_H */
