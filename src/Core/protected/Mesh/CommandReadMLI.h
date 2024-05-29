/*----------------------------------------------------------------------------*/
/*
 * \file CommandReadMLI.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 9/2/2015
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDREADMLI_H_
#define COMMANDREADMLI_H_
/*----------------------------------------------------------------------------*/
#include "Internal/CommandInternal.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/*----------------------------------------------------------------------------*/
class SubVolume;
/*----------------------------------------------------------------------------*/
class CommandReadMLI: public Internal::CommandInternal {

public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur pour la lecture d'un maillage dans un fichier
     *
     *  \param c le contexte
     *  \param nom le fichier pour lequel on effectue la lecture
     *  \param prefix qui sera ajouté aux noms de groupes
     */
	CommandReadMLI(Internal::Context& c, std::string& nom, std::string& prefix);

	/// Destructeur, destruction des entités crées de maillage
	virtual ~CommandReadMLI();

	/*------------------------------------------------------------------------*/
	/** \brief  exécute la commande
	 */
	virtual void internalExecute();

	/*------------------------------------------------------------------------*/
	/** \brief  annule la commande
	 */
	void internalUndo();

	/** \brief  rejoue la commande
	 */
	void internalRedo();

	/*------------------------------------------------------------------------*/
private:
	/// destruction des entités crées
	void deleteCreatedEntities();

private:
	/// nom du fichier pour la lecture
	std::string m_file_name;

	/// prefix pour les groupes
	std::string m_prefix;
};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* COMMANDREADMLI_H_ */
