#ifndef SELECTION_MANAGER_H
#define SELECTION_MANAGER_H


#include "SelectionManagerIfc.h"
#include "Entity.h"

#include <TkUtil/Mutex.h>

#include <string>
#include <vector>
#include <sys/types.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {


class SelectionManager;
class SelectionManagerObserver;


/**
 * <P>Classe dont les instances observent une sélection d'entités Magix 3D.
 * Les instances de cette classe sont averties à chaque modification de la
 * sélection.</P>
 *
 * <P>Dans la conception de Magix 3D une entité n'a pas de statut <I>sélectionné
 * </I>ou <I>non sélectionné</I> : la sélection est effectuée au niveau du
 * contexte.
 * </P>
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class SelectionManagerObserver : public Mgx3D::Utils::SelectionManagerObserverIfc
{
	public :

	/** Constructeur. Se référence auprès du gérant de sélection.
	 */
	SelectionManagerObserver (Mgx3D::Utils::SelectionManagerIfc* selectionManager);

	/**
	 * Destructeur. Se désenregistre auprès de son gestionnaire de 
	 * sélection.
	 */
	virtual ~SelectionManagerObserver ( );

	/** Se déréférence auprès de son éventuel gestionnaire de
	 * sélection, puis se référence auprès de celui reçu en 
	 * argument.
	 * @param		Nouveau gestionnaire de sélection. Peut être nul.
	 */
	virtual void setSelectionManager (Mgx3D::Utils::SelectionManagerIfc* selectionManager);

	/**
	 * @return		le gestionnaire de sélection
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc* getSelectionManager ( )
	{ return _selectionManager; }
	virtual const Mgx3D::Utils::SelectionManagerIfc* getSelectionManager ( ) const
	{ return _selectionManager; }

	/**
	 * Appelé lorsque le gérant de la sélection est détruit.
	 * Perd sa référence sur son gestionnaire de sélection.
	 */
	virtual void selectionManagerDeleted ( );

	/** 
	 * Appelé lorsque la sélection change.
	 * Ne fait rien par défaut.
	 */
	virtual void selectionModified ( );

	/** 
	 * Appelé lorsque la sélection est annulée.
	 * Ne fait rien par défaut.
	 */
	virtual void selectionCleared ( );

	/**
	 * Appelé lorsque les entités reçues en arguments sont ajoutées à
	 * la sélection.
	 * Ne fait rien par défaut.
	 */
	virtual void entitiesAddedToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * Appelé lorsque les entités reçues en arguments sont supprimées de
	 * la sélection.
	 * Ne fait rien par défaut.
	 * @param	Entités enlevées de la sélection
	 * @param	<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des fins
	 * 			d'optimisation.
	 */
	virtual void entitiesRemovedFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);

	/**
	 * \return  <I>true</I> si le panneau s'adapte à la politique de sélection
	 *          courrante du gestionnaire de sélection, <I>false</I> dans le
	 *          cas contraire.
	 * Ne fait rien par défaut.
	 */
	virtual bool isSelectionPolicyAdaptationEnabled ( ) const;

	/**
	 * Si <I>true</I> est transmis en argument (défaut), le panneau s'adapte
	 * à la politique de sélection courrante du gestionnaire de sélection.
	 * Si <I>false</I> est transmis en argument, le panneau ne s'adapte pas
	 * à la politique de sélection courrante du gestionnaire de sélection
	 * et <I>selectionPolicyModified</I> ne fait rien.
	 * Ne fait rien par défaut.
	 * \see		selectionPolicyModified
	 * \see		isSelectionPolicyAdaptationEnabled
	 */
	virtual void enableSelectionPolicyAdaptation (bool enable);


	protected :

	/**
	 * Le mutex qui protège l'instance.
	 */
	virtual TkUtil::Mutex* getMutex ( ) const;


	private :

	/**
	 * Constructeur de copie. Interdit.
	 */
	SelectionManagerObserver (const SelectionManagerObserver&);

	/**
	 * Opérateur =. Interdit.
	 */
	SelectionManagerObserver& operator = (const SelectionManagerObserver&);

	/** Le gestionnaire de sélection. */
	Mgx3D::Utils::SelectionManagerIfc*			_selectionManager;

	/** Mutext pour protéger certaines opérations. */
	mutable TkUtil::Mutex*						_mutex;

	bool										_selectionPolicyAdaptation;
};	// class SelectionManagerObserver


/**
 * <P>Classe de base gérant une sélection d'entités Magix 3D.
 * Des observateurs sont informés des modifications de cette sélection.
 * </P>
 *
 * <P>Cette classe n'offre pas de service de politique évènementielle de
 * gestion des évènements.
 * </P>
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class SelectionManager : public Mgx3D::Utils::SelectionManagerIfc
{
	public :

	/**
	 * Constructeur.
	 *
	 * \param	Nom unique du gestionnaire de sélection
     *  \param los le LogOutputStream
	 */
	SelectionManager (const std::string& name, TkUtil::LogOutputStream* los);

	/**
	 * Invoque selectionManagerDeleted de chacun de ses observateurs.
	 */
	virtual ~SelectionManager ( );

	/**
	 * \return		Le nom unique du gestionnaire de sélection.
	 */
	virtual const std::string& getName ( ) const;

	/**
	 * Les informations sur la sélection en cours.
	 */
	 //@{

	/**
	 * @param		la boite englobante de la sélection (valeur de retour).
	 */
	virtual void getBounds (double bounds [6]);

	/**
	 * @return		true si la sélection est vide, ou false. 
	 */
	virtual bool isSelectionEmpty ( ) const;

	/**
	 * @return		les entités de la sélection.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getEntities ( ) const;

    /**
     * @return      les noms des entités de la sélection.
     */
    virtual std::vector<std::string> getEntitiesNames ( ) const;

    /**
     * @return      les entités de la sélection filtrées suivant le type d'objet
     * \param 		type le type d'entité que l'on souhaite avoir
     */
    virtual std::vector<Mgx3D::Utils::Entity*> getEntities (Entity::objectType type) const;

    /**
     * @return      les noms des entités de la sélection correspondant au filtre transmis en argument.
     */
    virtual std::vector<std::string> getEntitiesNames (FilterEntity::objectType mask) const;

    /**
     * @return      les noms des entités de la sélection filtrées suivant le type d'objet
     * \param 		type le type d'entité que l'on souhaite avoir
     */
    virtual std::vector<std::string> getEntitiesNames (Entity::objectType type) const;

    /** \return le nombre d'entités sélectionnées */
    virtual uint getNbEntities()
	{return _entities.size();}

	//@}	// Les informations sur la sélection en cours.

	/**
     * Les actions directes sur la sélection
     */
    //@{

	/**
	 * Informe ses observateurs que les entités sont ajoutées à la sélection.
	 * Se référence auprès des entités.
	 * @warning		Ne tient pas compte du mode d'extension de la sélection.
	 */
	virtual void addToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * Informe ses observateurs que les entités sont otées de la sélection.
	 * Se déréférence auprès des entités.
	 * @exception		Une exception est levée si une entité ne peut pas
	 *					être enlevée de la sélection.
	 */
	virtual void removeFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * @return		true si l'entité reçue en argument fait parti de la sélection, ou false.
	 */
	virtual bool isSelected (const Mgx3D::Utils::Entity& entity) const;

	/**
	 * Informe ses observateurs de la désélection des éléments.
	 * qu'il est oté de la sélection.
	 */
	virtual void clearSelection ( );

	//@}	// Les actions directes sur la sélection

	/**
	 * Les observateurs de sélection.
	 */
	//@{

	/** Recense l'observateur de sélection reçu en argument. Cet
	 * observateur sera informé de toutes les modifications de la
	 * sélection à venir.
	 * @param		Observateur à recencer, non nul.
	 */
	virtual void addSelectionObserver (Mgx3D::Utils::SelectionManagerObserverIfc& observer);

	/** Désenregistre l'observateur de sélection reçu en argument. Cet
	 * observateur ne sera plus informé des modifications de la
	 * sélection à venir.
	 * @param		Observateur à désenregistrer, non nul.
	 */
	virtual void removeSelectionObserver (Mgx3D::Utils::SelectionManagerObserverIfc& observer);

	//@}	// Les observateurs de sélection.


	protected :

	/**
	 * @return		La liste des observateurs.
	 */
	virtual std::vector<Mgx3D::Utils::SelectionManagerObserverIfc*> getObservers ( );

	/**
	 * Le mutex qui protège l'instance.
	 */
	virtual TkUtil::Mutex* getMutex ( ) const;

	/**
	 * Envoie le log transmis en argument dans le flux de logs associés.
	 */
	 virtual void log (const TkUtil::Log& log);

	/**
	 * Informe les observateurs d'une modification de la politique de sélection.
	 * \param	Eventuel pointeur sur des données décrivant ce changement de
	 * 			politique de sélection. Informations optionnelles et spécifiques
	 * 			au gestionnaire de sélection.
	 * \see		SelectionManagerObserverIfc::selectionPolicyModified
	 */
	virtual void notifyObserversForNewPolicy (void* smp = 0);


	private :

	/**
	 * Constructeur de copie : interdit.
	 */
	SelectionManager (const SelectionManager&);

	/**
	 * Opérateur = : interdit.
	 */
	SelectionManager& operator = (const SelectionManager&);

	/** Le nom unique du gestionnaire de sélection. */
	std::string												_name;

	/** Les entités de la sélection. */
	std::vector<Mgx3D::Utils::Entity*>						_entities;

	/** Les observateurs de la sélection. */
	std::vector<Mgx3D::Utils::SelectionManagerObserverIfc*>	_observers;

	/** Mutex pour protéger certaines opérations. */
	mutable TkUtil::Mutex*									_mutex;

	/** le flux pour les messages */
	TkUtil::LogOutputStream* m_logOutputStream;

};	// class SelectionManager

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D
/*----------------------------------------------------------------------------*/
}	// end namespace Utils
/*----------------------------------------------------------------------------*/

#endif	// SELECTION_MANAGER_H
