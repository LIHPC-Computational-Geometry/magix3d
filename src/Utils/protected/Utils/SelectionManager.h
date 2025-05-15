#ifndef SELECTION_MANAGER_H
#define SELECTION_MANAGER_H


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
class SelectionManagerObserver
{
	public :

	/** Constructeur. Se référence auprès du gérant de sélection.
	 */
	SelectionManagerObserver (Mgx3D::Utils::SelectionManager* selectionManager);

	/**
	 * Destructeur. Se désenregistre auprès de son gestionnaire de 
	 * sélection.
	 */
	~SelectionManagerObserver ( );

	/** Se déréférence auprès de son éventuel gestionnaire de
	 * sélection, puis se référence auprès de celui reçu en 
	 * argument.
	 * @param		Nouveau gestionnaire de sélection. Peut être nul.
	 */
	void setSelectionManager (Mgx3D::Utils::SelectionManager* selectionManager);

	/**
	 * @return		le gestionnaire de sélection
	 */
	Mgx3D::Utils::SelectionManager* getSelectionManager ( )
	{ return _selectionManager; }
	const Mgx3D::Utils::SelectionManager* getSelectionManager ( ) const
	{ return _selectionManager; }

	/**
	 * Appelé lorsque le gérant de la sélection est détruit.
	 * Perd sa référence sur son gestionnaire de sélection.
	 */
	void selectionManagerDeleted ( );

	/** 
	 * Appelé lorsque la sélection change.
	 * Ne fait rien par défaut.
	 */
	void selectionModified ( );

	/** 
	 * Appelé lorsque la sélection est annulée.
	 * Ne fait rien par défaut.
	 */
	void selectionCleared ( );

	/**
	 * Appelé lorsque les entités reçues en arguments sont ajoutées à
	 * la sélection.
	 * Ne fait rien par défaut.
	 */
	void entitiesAddedToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);
	void entitiesAddedToSelection (const std::vector<std::string>& uniqueNames);

	/**
	 * Appelé lorsque les entités reçues en arguments sont supprimées de
	 * la sélection.
	 * Ne fait rien par défaut.
	 * @param	Entités enlevées de la sélection
	 * @param	<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des fins
	 * 			d'optimisation.
	 */
	void entitiesRemovedFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);
	void entitiesRemovedFromSelection (const std::vector<std::string>& uniqueNames);

	/**
	 * Appelé lorsque la politique de sélection du gestionnaire de sélection
	 * est modifiée. La politique de sélection dépend du type de gestionnaire.
	 * \param	Eventuel pointeur sur des données décrivant ce changement de
	 * 			politique de sélection. Informations optionnelles et spécifiques
	 * 			au gestionnaire de sélection.
	 * Ne fait rien par défaut.
	 * \see		isSelectionPolicyAdaptationEnabled
	 * \see		enableSelectionPolicyAdaptation
	 */
	virtual void selectionPolicyModified (void* smp);

	/**
	 * \return  <I>true</I> si le panneau s'adapte à la politique de sélection
	 *          courrante du gestionnaire de sélection, <I>false</I> dans le
	 *          cas contraire.
	 * Ne fait rien par défaut.
	 */
	bool isSelectionPolicyAdaptationEnabled ( ) const;

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
	void enableSelectionPolicyAdaptation (bool enable);


	protected :

	/**
	 * Le mutex qui protège l'instance.
	 */
	TkUtil::Mutex* getMutex ( ) const;


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
	Mgx3D::Utils::SelectionManager*			_selectionManager;

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
class SelectionManager
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
	virtual std::vector<unsigned long> getEntitiesIds ( ) const;

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
	
	/**
	 * Annule la dernière opération de sélection.
	 * \see		redo
	 * \see		resetUndoStack
	 */
	virtual void undo ( );

	/**
	 * Rejoue la dernière opération de sélection annulée.
	 * \see		undo
	 * \see		resetUndoStack
	 */
	virtual void redo ( );

	/**
	 * Vide la pile de undo/redo.
	 * \see		undo
	 * \see		redo
	 */
	virtual void resetUndoStack ( );
	
	/**
	 * \return	true si il y a au moins une sélection annulable, sinon false.
	 */
	 virtual bool isUndoable ( ) const;
	 
	/**
	 * \return	true si il y a au moins une sélection rejouable, sinon false.
	 */
	 virtual bool isRedoable ( ) const;

	//@}	// Les actions directes sur la sélection

	/**
	 * Politique évènementielle et évènements pouvant arriver au gestionnaire de
	 * sélection.
	 * Ce comportement sur évènements, la politique évènementielle du
	 * gestionnaire, est à définir dans les classes dérivées. Ces méthodes ne
	 * font rien par défaut.
	 */
	//@{

	/** Les dimensions possibles des entités sélectionnables. Ces dimensions
	 * sont encodées à l'aide de bits décalables à gauche => masque possible
	 * avec les opérateurs & et |.
	 */
	enum DIM { NO_DIM = 0, D0 = 1, D1 = 1<<1, D2 = 1<<2, D3 = 1<<3 };

	static DIM dimensionToDimensions (int dimension);
	static int dimensionsToDimension (DIM dimensions);

	/** La combinaison de toutes les dimensions. */
	static const DIM ALL_DIMENSIONS;

	/** Pour des opérations bit à bit, les compléments à 2. */
	static const DIM	NOT_NO_DIM, NOT_D0, NOT_D1, NOT_D2, NOT_D3;

	/**
	 * \return		<I>true</I> si la sélection est activée pour l'entité
	 *				transmise en argument, sinon <I>false</I>. Le choix
	 *				repose sur la dimension et le type de l'entité.
	 * \warning		Méthode à surcharger, retourne <I>false</I> par défaut.
	 * \see			is0DSelectionActivated
	 * \see			is1DSelectionActivated
	 * \see			is2DSelectionActivated
	 * \see			is3DSelectionActivated
	 * \see			getFilteredTypes
	 */
	virtual bool isSelectionActivated (const Mgx3D::Utils::Entity&) const;

	/**
	 * \return		Le masque de sélection sur le type d'entité sélectionnable.
	 * \see			getFilteredDimensions
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getFilteredTypes ( ) const;

	/**
	 * \return		Le masque de sélection sur les dimensions sélectionnables.
	 * \see			getFilteredTypes
	 */
	virtual DIM getFilteredDimensions ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 0D est activée, sinon
	 *				<I>false</I>.
	 * \warning		Méthode à surcharger, retourne <I>false</I> par défaut.
	 * \see			activateSelection
	 */
	virtual bool is0DSelectionActivated ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 1D est activée, sinon
	 *				<I>false</I>.
	 * \warning		Méthode à surcharger, retourne <I>false</I> par défaut.
	 * \see			activateSelection
	 */
	virtual bool is1DSelectionActivated ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 2D est activée, sinon
	 *				<I>false</I>.
	 * \warning		Méthode à surcharger, retourne <I>false</I> par défaut.
	 * \see			activateSelection
	 */
	virtual bool is2DSelectionActivated ( ) const;

	/**
	 * \return		<I>true</I> si la sélection d'entités 3D est activée, sinon
	 *				<I>false</I>.
	 * \warning		Méthode à surcharger, retourne <I>false</I> par défaut.
	 * \see			activateSelection
	 */
	virtual bool is3DSelectionActivated ( ) const;

	/**
	 * Activation de la sélection d'entités de dimensions et de types
	 * correspondant au masque.
	 * \see		is3DSelectionActivated
	 */
	virtual void activateSelection (
			DIM dimensions, Mgx3D::Utils::FilterEntity::objectType mask = Mgx3D::Utils::FilterEntity::All);

	//@}	// Politique évènementielle.

	/**
	 * Les observateurs de sélection.
	 */
	//@{

	/** Recense l'observateur de sélection reçu en argument. Cet
	 * observateur sera informé de toutes les modifications de la
	 * sélection à venir.
	 * @param		Observateur à recencer, non nul.
	 */
	virtual void addSelectionObserver (Mgx3D::Utils::SelectionManagerObserver& observer);

	/** Désenregistre l'observateur de sélection reçu en argument. Cet
	 * observateur ne sera plus informé des modifications de la
	 * sélection à venir.
	 * @param		Observateur à désenregistrer, non nul.
	 */
	virtual void removeSelectionObserver (Mgx3D::Utils::SelectionManagerObserver& observer);

	//@}	// Les observateurs de sélection.


	protected :

	/**
	 * @return		La liste des observateurs.
	 */
	virtual std::vector<Mgx3D::Utils::SelectionManagerObserver*> getObservers ( );

	/**
	 * Le mutex qui protège l'instance.
	 */
	virtual TkUtil::Mutex* getMutex ( ) const;

	/**
	 * Envoie le log transmis en argument dans le flux de logs associés.
	 */
	virtual  void log (const TkUtil::Log& log);

	/**
	 * Informe les observateurs d'une modification de la politique de sélection.
	 * \param	Eventuel pointeur sur des données décrivant ce changement de
	 * 			politique de sélection. Informations optionnelles et spécifiques
	 * 			au gestionnaire de sélection.
	 * \see		SelectionManager::selectionPolicyModified
	 */
	virtual void notifyObserversForNewPolicy (void* smp = 0);

	/**
     * Les actions directes sur la sélection
     */	
	//@{
	enum STACK_ACTION { ADD, REMOVE, CLEAR };
	
	/**
	 * Informe ses observateurs que les entités sont ajoutées à la sélection.
	 * Se référence auprès des entités.
	 * Si undoable vaut true alors l'opération de sélection est ajoutée au gestionnaire de undo/redo.
	 * @see			pushAction
	 * @warning		Ne tient pas compte du mode d'extension de la sélection.
	 */
	virtual void addToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool undoable);

	/**
	 * Informe ses observateurs que les entités sont otées de la sélection.
	 * Se déréférence auprès des entités.
	 * Si undoable vaut true alors l'opération de sélection est ajoutée au gestionnaire de undo/redo.
	 * @see			pushAction
	 * @exception		Une exception est levée si une entité ne peut pas être enlevée de la sélection.
	 */
	virtual void removeFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool undoable);

	/**
	 * Informe ses observateurs de la désélection des éléments qu'il est oté de la sélection.
	 * Si undoable vaut true alors l'opération de sélection est ajoutée au gestionnaire de undo/redo.
	 * @see			pushAction
	 */
	virtual void clearSelection (bool undoable);
	
	/**
	 * Actualise la pile d'actions de sélection en ajoutant - si nécessaire - celle reçue en argument.
	 */
	virtual void pushAction (STACK_ACTION action, const std::vector<Mgx3D::Utils::Entity*>& entities);
	
	/**
	 * \return	L'action de sélection courante, sous forme de chaine de caractères, pour aide à la mise au point.
	 */
	virtual std::string getCurrentAction ( ) const;

	/**
	 * \return	La i-ème action de sélection courante, sous forme de chaine de caractères, pour aide à la mise au point.
	 */
	virtual std::string getAction (size_t i) const;
		
	/**
	 * Affiche la pile d'actions dans le flux transmis en argument à raison d'une ligne par action. L'action de sélection
	 * courante, si il y en a, est précédée de "=>". Pour aide à la mise au point.
	 */
	virtual void printActionsStack (std::ostream& stream) const;
	//@}	// Les actions directes sur la sélection


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
	std::vector<Mgx3D::Utils::SelectionManagerObserver*>	_observers;

	/** Mutex pour protéger certaines opérations. */
	mutable TkUtil::Mutex*									_mutex;
	
	/** La pile d'opération de sélection pour undo/redo. */
	std::vector < std::pair <STACK_ACTION, std::vector<Mgx3D::Utils::Entity*> > >				_undoStack;
	
	/** L'opération courante de sélection. */
	size_t																						_currentAction;

	/** le flux pour les messages */
	TkUtil::LogOutputStream* m_logOutputStream;

};	// class SelectionManager

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D
/*----------------------------------------------------------------------------*/
}	// end namespace Utils
/*----------------------------------------------------------------------------*/

#endif	// SELECTION_MANAGER_H
