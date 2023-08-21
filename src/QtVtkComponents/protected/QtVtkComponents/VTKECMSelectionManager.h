#ifndef VTK_ECM_SELECTION_MANAGER_H
#define VTK_ECM_SELECTION_MANAGER_H


#include <TkUtil/LogOutputStream.h>
#include <TkUtil/Mutex.h>

#include <vtkActor.h>

#include <string>
#include <vector>


/**
 * <P>Classe définissant un comportement de base de gestionnaire de sélection
 * en environnement <I>VTK</I>.
 * </P>
 *
 * <P>Cette classe repose uniquement sur des classes
 * <I>VTK</I> afin de pouvoir être utilisé dans n'importe quelle application.
 * </P>
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class VTKECMSelectionManager
{
	public :

	/**
	 * Constructeur.
	 *
     *  \param	Flux utilisé pour les messages.
	 */
	VTKECMSelectionManager (TkUtil::LogOutputStream* los);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~VTKECMSelectionManager ( );

	/**
	 * Les informations sur la sélection en cours.
	 */
	 //@{

	/**
	 * @param		la boite englobante de la sélection (valeur de retour).
	 */
	virtual void GetBounds (double bounds [6]);

	/**
	 * @return		true si la sélection est vide, ou false. 
	 */
	virtual bool IsSelectionEmpty ( ) const;

	/**
	 * @return		les objets de la sélection.
	 */
	virtual std::vector<vtkActor*> GetActors ( ) const;

    /** \return le nombre d'objets sélectionnés */
    virtual size_t GetSelectedObjectsNum ( )
	{return _actors.size();}

	//@}	// Les informations sur la sélection en cours.

	/**
     * Les actions directes sur la sélection
     */
    //@{

	/**
	 * Ajoute l'acteur transmis en argument à la sélection.
	 * \except	Lève une exception en cas d'action impossible à réaliser.
	 * \see		removeFromSelection
	 */
	virtual void AddToSelection (vtkActor& actor);

	/**
	 * Enlève l'acteur transmis en argument de la sélection.
	 * \except	Lève une exception en cas d'action impossible à réaliser.
	 * \see		addToSelection
	 */
	virtual void RemoveFromSelection (vtkActor& actor);

	/**
	 * @return		true si l'acteur reçu en argument fait parti de la
	 *				sélection, ou false.
	 */
	virtual bool IsSelected (const vtkActor& actor) const;

	/**
	 * Annule la sélection en cours.
	 */
	virtual void ClearSelection ( );

	//@}	// Les actions directes sur la sélection


	/**
	 * La gestion des messages.
	 */
	//@{

	/**
	 * \return		Le nom de l'objet sélectionné associé à l'acteur.
	 * \warning		<B>Ne fait rien par défaut, à surcharger.</B>
	 */
	virtual std::string GetName (const vtkActor& actor) const;

	/**
	 * Ecrit si possible le message transmis en argument dans le flux de
	 * messages.
	 */
	virtual void Log (const TkUtil::Log& log);

	//@}	//  La gestion des messages.


	protected :

	/**
	 * \return		Le mutex qui protège l'instance.
	 */
	virtual TkUtil::Mutex& GetMutex ( ) const;


	private :

	/**
	 * Constructeur de copie : interdit.
	 */
	VTKECMSelectionManager (const VTKECMSelectionManager&);

	/**
	 * Opérateur = : interdit.
	 */
	VTKECMSelectionManager& operator = (const VTKECMSelectionManager&);

	/** Les acteurs représentant les objets de la sélection. */
	std::vector<vtkActor*>								_actors;

	/** Mutex pour protéger certaines opérations. */
	mutable TkUtil::Mutex								_mutex;

	/** Le flux pour les messages */
	TkUtil::LogOutputStream*							_logOutputStream;

};	// class VTKECMSelectionManager


#endif	// VTK_ECM_SELECTION_MANAGER_H
