/**
 * \file		EntitySeizureManager.h
 * \author		Charles PIGNEROL
 * \date		01/03/2013
 */

#ifndef ENTITY_SEIZURE_MANAGER_H
#define ENTITY_SEIZURE_MANAGER_H


#include <Utils/SelectionManager.h>
#include <Internal/ContextIfc.h>

#include <QColor>


namespace Mgx3D
{

namespace QtComponents
{

class QtMgx3DMainWindow;

/**
 * <P>Classe proposant un support de base pour saisir de manière interactive
 * une entité d'une dimension prédéfinie. La méthode <I>criteriaMatch</I>
 * permet de tester si des critères donnés sont acceptables pour retenir une
 * entité donnée.
 * </P>
 *
 * <P>Les instances de cette classe sont en relation avec une instance de la
 * classe <I>QtMgx3DMainWindow</I> qui permet :<BR>
 * <OL>
 * <LI>- de s'assurer qu'à tout instant seule une instance de
 * <I>EntitySeizureManager</I> est en train d'effectuer la saisie interactive
 * d'une entité,
 * <LI>- d'accéder au gestionnaire de sélection de la session.
 * </OL>
 * </P>
 *
 * <P>Les classes dérivées doivent surcharger la méthode
 * <I>setSelectedEntity</I>. </P>
 * </P>
 */
class EntitySeizureManager
{
	public :

	/** Constructeur
	 * \param		Fenêtre principale associée, pour la saisie de l'entité.
	 * \param		Dimension(s) de l'entité à saisir.
	 * \param		Masque sur le type d'entité à saisir.
	 * \see			criteriaMatch
	 */
	EntitySeizureManager (Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
	                      Mgx3D::Utils::SelectionManagerIfc::DIM dimensions,
	                      Mgx3D::Utils::FilterEntity::objectType type);

	/**
	 * Destructeur. Quitte le mode de sélection interactive si nécessaire.
	 */
	virtual ~EntitySeizureManager ( );

	/**
	 * Réinitialise l'instance.
	 */
	virtual void reset ( );

	/** En mode de saisie interactive annule toute sélection.
	 */
	virtual void selectionCleared ( );

	/**
	 * Actualisation de la représentation de la sélection.
	 * \return	<I>true</I> en cas de modification, <I>false</I> dans le cas
	 * 			contraire. Retourne <I>false</I> par défaut.
	 */
	virtual bool actualizeGui (bool removeDestroyed);

	/**
	 * Appelé lorsque des entités sont (interactivement) ajoutées à la sélection.
	 * Si l'instance est en mode "sélection interactive" et que l'entité est de
	 * la dimension souhaitée appelle alors <I>setSelectedEntity</I> avec
	 * l'entité sélectionnée.
	 * \see			setSelectedEntity
	 * \see			setInteractiveMode
	 * \see			criteriaMatch
	 */
	virtual void entitiesAddedToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);
	
	/**
	 * Appelé lorsque des entités sont (interactivement) ajoutées à la sélection.
	 * Ne fait rien par défaut.
	 */
	virtual void entitiesAddedToSelection (const std::vector<std::string>& uniqueNames);

    /**
     * Appelé lorsque des entités sont (interactivement) enlevées de la sélection.
     * Si l'instance est en mode "sélection interactive" et que l'entité est de
     * la dimension souhaitée appelle alors <I>removeFromSelection</I> avec
     * les entités désélectionnées.
	 * @param		Entités enlevées de la sélection
	 * @param		<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des 
	 * 				fins d'optimisation.
     * \see         setSelectedEntity
     * \see         setInteractiveMode
     * \see         criteriaMatch
     */
    virtual void entitiesRemovedFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities, bool clear);
    
	/**
	 * Appelé lorsque des entités sont (interactivement) enlevées de la sélection.
	 * Ne fait rien par défaut.
	 */
    virtual void entitiesRemovedFromSelection (const std::vector<std::string>& uniqueNames);

	/**
	 * \return		<I>true</I> si l'on est mode de sélection interactive,
	 *				<I>false</I> dans le cas contraire.
	 * \see			setInteractiveMode
	 */
	virtual bool inInteractiveMode ( ) const;

	/**
	 * Passage (<I>true</I>) ou arrêt (<I>false</I>) du mode de saisie
	 * interactive. Invoque préalablement <I>updateGui</I> qui actualise le
	 * panneau selon <I>enable</I>.
	 * En cas de passage en mode de saisie interactive, notifie la main window
	 * associée afin que celle-ci arrête une éventuelle autre saisie.
	 * Dans le cas contraire remet le gestionnaire de sélection dans son état
	 * initial (dimensions et types filtrés).
	 * interactive.
	 * \see			inInteractiveMode
	 * \see			updateGui
	 */
	virtual void setInteractiveMode (bool enable);

	/**
	 * \return		<I>true</I> si la dimension et le type transmis en
	 *				arguments correspondent à des critères acceptables
	 * 				d'entité à sélectionner, <I>false</I> dans le cas contraire.
	 */
	virtual bool criteriaMatch (int dimension, Mgx3D::Utils::FilterEntity::objectType type) const;

	/**
	 * \return		Une référence sur le contexte associé.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;
	virtual Mgx3D::Internal::ContextIfc& getContext ( );

	/**
	 * \return		Une référence sur la fenêtre principale associée.
	 */
	virtual const Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( ) const;
	virtual Mgx3D::QtComponents::QtMgx3DMainWindow& getMainWindow ( );

	/**
	 * \return		Un pointeur sur l'éventuel gestionnaire de sélection
	 *				associé.
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc* getSelectionManager ( );

	/**
	 * \return		Les types filtrés
	 * \see			getObjectType
	 * \see			getFilteredDimensions
	 */
	virtual Mgx3D::Utils::FilterEntity::objectType getFilteredTypes ( ) const;

	/**
	 * \return		Le type d'objet sélectionné, ou <I>Entity::undefined</I>
	 *				si la sélection est vide ou multiple et de types différents.
	 * \see			getFilteredTypes
	 * \see			getFilteredDimensions
	 * \see			getObjectsCount
	 */
	virtual Mgx3D::Utils::Entity::objectType getObjectType ( ) const;

	/**
	 * \return		Le nombre d'objets de la sélection courante
	 * \see			getObjectType
	 */
	virtual size_t getObjectsCount ( ) const;

	/**
	 * \return		<I>true</I> si on est en mode de saisie multiple.
	 * \see			setMultiSelectMode
	 */
	virtual bool isMultiSelectMode ( ) const;

	/**
	 * \param		<I>true</I> pour passer en mode de saisie multiple.
	 * \see			isMultiSelectMode
	 */
	virtual void setMultiSelectMode (bool multi);

	/**
	 * Change les dimensions possibles des entités à sélectionner
	 * \see			getFilteredDimensions
	 */
	virtual void setFilteredDimensions (Mgx3D::Utils::SelectionManagerIfc::DIM dimensions);

	/**
	 * \return		Les dimensions filtrées.
	 * \see			setFilteredDimensions
	 * \see			getFilteredTypes
	 * \see			getObjectType
	 */
	virtual Mgx3D::Utils::SelectionManagerIfc::DIM getFilteredDimensions ( ) const;

    /**
     * Change le type des entités à sélectionner
     */
    virtual void setFilteredTypes (Utils::FilterEntity::objectType types);

	/**
	 * La couleur de fond des widgets en cours de saisie interactive.
	 */
	static QColor interactiveModeBackground;

	/**
	 * Fluidification de la saisie interactive : passer d'un gestionnaire de
	 * saisie au suivant de manière interactive.
	 */
	//@{

	/**
 	 * \return	Gestionnaire de saisie précédent.
 	 * \see		getNextSeizureManager
 	 * \see		setLinkedSeizureManagers
 	 */
	virtual EntitySeizureManager* getPreviousSeizureManager ( ) const;

	/**
 	 * \return	Gestionnaire de saisie suivant.
 	 * \see		getPreviousSeizureManager
 	 * \see		setLinkedSeizureManagers
 	 */
	virtual EntitySeizureManager* getNextSeizureManager ( ) const;

	/**
 	 * \param	Gestionnaire de saisie précédent.
 	 * \param	Gestionnaire de saisie suivant.
 	 * \see		getPreviousSeizureManager
 	 * \see		getNextSeizureManager
 	 */
	virtual void setLinkedSeizureManagers (
					EntitySeizureManager* prev, EntitySeizureManager* next);

	//@}	// Fluidification de la saisie interactive


	protected :

	/**
 	 * Actualise le panneau selon la valeur de <I>enable</I> (<I>true</I> : mode
 	 * de saisie interactif activé, <I>false</I> : mode de saisie interactif 
 	 * inactivé).
 	 * \see			setInteractiveMode
 	 * \warning		<B>Cette méthode ne fait rien par défaut. A surcharger.</B>
 	 */
	virtual void updateGui (bool enable);

	/**
	 * <B>A surcharger dans les classes dérivées.</B> Lorsque
	 * l'utilisateur a sélectionné de manière interactive une entité de la
	 * dimension souhaitée, cette méthode est appelée avec en argument 
	 * une référence sur l'entité.
	 * \warning		<B>Ne fait rien par défaut, à surcharger.</B>
	 */
	virtual void setSelectedEntity (const Mgx3D::Utils::Entity& entity);

    /**
     * <B>A surcharger dans les classes dérivées.</B> Lorsque
     * l'utilisateur a sélectionné de manière interactive une liste d'entités de la
     * dimension souhaitée, cette méthode est appelée avec en argument
     * un vecteur d'entités.
     * \warning     <B>Ne fait rien par défaut, à surcharger.</B>
     */
    virtual void setSelectedEntities (const std::vector<Mgx3D::Utils::Entity*>);

	/**
	 * \return		La liste des entités sélectionnées via cette instance.
	 */
	const std::vector<Mgx3D::Utils::Entity*> getSelectedEntities ( ) const;

	/**
	 * Appelé lorsque l'utilisateur ajoute des entités à la sélection.
	 * \param		Entités ajoutées à la sélection
	 * \see			removeFromSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void addToSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);

	/**
	 * Appelé lorsque l'utilisateur enlève des entités de la sélection.
	 * \param		Entités enlevées de la sélection
	 * \see			addToSelection
	 * \see			setSelectedEntity
	 * \see			setSelectedEntities
	 */
	virtual void removeFromSelection (const std::vector<Mgx3D::Utils::Entity*>& entities);


	protected :

	/**
	 * Vide la liste des entités en cours de sélection.
	 * Ne fait rien d'autre.
	 */
	virtual void clearSeizuredEntities ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	EntitySeizureManager (const EntitySeizureManager&);
	EntitySeizureManager& operator = (const EntitySeizureManager&);

	/** La fenêtre principale associée. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*		_mainWindow;

	/** Les dimensions possibles de l'entité à sélectionner. */
	Mgx3D::Utils::SelectionManagerIfc::DIM		_dimensions;

	/** Le type d'entité à sélectionner. */
	Mgx3D::Utils::FilterEntity::objectType		_types;

	/** Est on en mode de sélection interactive ? */
	bool										_interactiveMode;

	/** Est-on en mode sélection multiple ? */
	bool										_multiSelectMode;

	/** Les entités que l'on sélectionne actuellement */
	std::vector<unsigned long>					_seizuredEntities;

	/** L'ancien mode de sélection. */
	Mgx3D::Utils::SelectionManagerIfc::DIM		_selectionDims;
	Mgx3D::Utils::FilterEntity::objectType		_selectionTypes;

	/** Le gestionnaires de saisie précédent et suivant. */
	Mgx3D::QtComponents::EntitySeizureManager	*_previousSeizureManager,
												*_nextSeizureManager;
};	// class EntitySeizureManager


}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// ENTITY_SEIZURE_MANAGER_H
