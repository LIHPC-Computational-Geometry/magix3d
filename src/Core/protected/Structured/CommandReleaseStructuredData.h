/*
 * \file	CommandReleaseStructuredData.h
 * \author	Charles PIGNEROL
 * \date	05/12/2018
 */


#ifndef COMMAND_RELEASE_STRUCTURED_DATA_H
#define COMMAND_RELEASE_STRUCTURED_DATA_H

#include "Internal/CommandInternal.h"


namespace Mgx3D 
{

namespace Structured
{

/*----------------------------------------------------------------------------*/
/**
 * \class	CommandReleaseStructuredData
 * \brief	Libération des données structurées.
 * \see		CommandLoadHICStructuredData
 */
class CommandReleaseStructuredData : public Mgx3D::Internal::CommandInternal
{
	public:

	/** Constructeur
	 *
	 * \param	Contexte d'utilisation
	 * \param	Nom unique de la commande.
	 */
	CommandReleaseStructuredData (Mgx3D::Internal::Context& c, const std::string& name);

	/** 
	 * Destructeur
	 */
	virtual ~CommandReleaseStructuredData ( );

	/**
	 * Exécute la commande.
	 */
	virtual void internalExecute ( );

	/**
	 * Annulation de la commande.
	 * \see		<B>Lève une exception dans cette implémentation.</B>
	 */
	virtual void internalUndo ( );

	/**
	 * Rejoue la commande.
	 */
	virtual void internalRedo ( );


	protected :


	private:

	/**
	 * Constructeur de copie. Interdit.
	 */
	CommandReleaseStructuredData (const CommandReleaseStructuredData&);

	/**
	 * Opérateur =. Interdit.
	 */
	CommandReleaseStructuredData& operator = (const CommandReleaseStructuredData&);
};	// class CommandReleaseStructuredData


}	// namespace Structured


} // namespace Mgx3D


#endif	// COMMAND_RELEASE__STRUCTURED_DATA_H
