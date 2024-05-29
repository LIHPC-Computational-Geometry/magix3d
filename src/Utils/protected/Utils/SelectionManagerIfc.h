#ifndef SELECTION_MANAGER_IFC_H
#define SELECTION_MANAGER_IFC_H


#include "Entity.h"

#include <TkUtil/Mutex.h>

#include <string>
#include <vector>
#include <sys/types.h>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {


class SelectionManagerIfc;
class SelectionManagerObserverIfc;


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
class SelectionManagerObserverIfc
{
	public :

	/** Constructeur. Se référence auprès du gérant de sélection.
	 */
	SelectionManagerObserverIfc (Mgx3D::Utils::SelectionManagerIfc* selectionManager);

	/**
	 * Destructeur. Se désenregistre auprès de son gestionnaire de 
	 * sélection.
	 */
	virtual ~SelectionManagerObserverIfc ( );

	/** Se déréférence auprès de son éventuel gestionnaire de
	 * sélection, puis se référence auprès de celui reçu en 
	 * argument.
	 * @param		Nouveau gestionnaire de sélection. Peut être nul.
	 */
	virtual void setSelectionManager (Mgx3D::Utils::SelectionManagerIfc* selectionManager);

	/**
	 * @return		le gestionnaire de sélection
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc*	getSelectionManager ( );
	virtual const Mgx3D::Utils::SelectionManagerIfc*getSelectionManager ( ) const;

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
	 * Appelé lorsque les entités reçues en arguments sont ajoutées à la sélection.
	 * Ne fait rien par défaut.
	 */
	virtual void entitiesAddedToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);
	virtual void entitiesAddedToSelection (const std::vector<std::string>& uniqueNames);

	/**
	 * Appelé lorsque les entités reçues en arguments sont supprimées de la sélection.
	 * Ne fait rien par défaut.
	 * @param	Entités enlevées de la sélection
	 * @param	<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des fins d'optimisation.
	 */
	virtual void entitiesRemovedFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);
	virtual void entitiesRemovedFromSelection (const std::vector<std::string>& uniqueNames);
	
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



	private :

	/**
	 * Constructeur de copie. Interdit.
	 */
	SelectionManagerObserverIfc (const SelectionManagerObserverIfc&);

	/**
	 * Opérateur =. Interdit.
	 */
	SelectionManagerObserverIfc& operator = (const SelectionManagerObserverIfc&);
};	// class SelectionManagerObserver


/**
 * <P>Interface décrivant les services offerts par un gestionnaire
 * de sélection d'entités Magix 3D. Parmi les services figurent des
 * <B>évènements</B> auquels le gestionnaire peut éventuellement être sensible.
 * Le comportement du gestionnaire sur évènements, sa <B>politique</B>, n'est
 * pas définie, et reste à préciser dans les classes implémentants cette
 * interface.
 * </P>
 * 
 * <P>
 * Des <B>observateurs</B> sont informés des modifications de cette sélection.
 * </P>
 * <P>
 * </P>
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class SelectionManagerIfc
{
	public :

	/**
	 * Constructeur.
	 */
	SelectionManagerIfc ( );

	/**
	 * Invoque selectionManagerDeleted de chacun de ses observateurs.
	 */
	virtual ~SelectionManagerIfc ( );

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
     * @return      les noms des entités de la sélection filtrées suivant le type d'objet
     * \param type le type d'entité que l'on souhaite avoir
     */
    virtual std::vector<std::string> getEntitiesNames (Entity::objectType type) const;

    /**
     * @return      les noms des entités de la sélection correspondant au filtre
     * 				transmis en argument.
     */
    virtual std::vector<std::string> getEntitiesNames (FilterEntity::objectType mask) const;


    /** \return le nombre d'entités sélectionnées */
    virtual uint getNbEntities();

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
	 * @return		true si l'entité reçue en argument fait parti de la
	 *				sélection, ou false.
	 */
	virtual bool isSelected (const Mgx3D::Utils::Entity& entity) const;

	/**
	 * Informe ses observateurs de la désélection des éléments.
	 */
	virtual void clearSelection ( );

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
	virtual void addSelectionObserver (Mgx3D::Utils::SelectionManagerObserverIfc& observer);

	/** Désenregistre l'observateur de sélection reçu en argument. Cet
	 * observateur ne sera plus informé des modifications de la
	 * sélection à venir.
	 * @param		Observateur à désenregistrer, non nul.
	 */
	virtual void removeSelectionObserver (Mgx3D::Utils::SelectionManagerObserverIfc& observer);

	//@}	// Les observateurs de sélection.


	protected :


	private :

	/**
	 * Constructeur de copie : interdit.
	 */
	SelectionManagerIfc (const SelectionManagerIfc&);

	/**
	 * Opérateur = : interdit.
	 */
	SelectionManagerIfc& operator = (const SelectionManagerIfc&);
};	// class SelectionManagerIfc

/*----------------------------------------------------------------------------*/
}	// end namespace Mgx3D
/*----------------------------------------------------------------------------*/
}	// end namespace Utils
/*----------------------------------------------------------------------------*/

#endif	// SELECTION_MANAGER_IFC_H
