#ifndef VTK_ECM_PICKER_COMMAND_H
#define VTK_ECM_PICKER_COMMAND_H

#include "VTKECMPicker.h"
#include "QtVtkComponents/VTKECMSelectionManager.h"
#include <TkUtil/LogDispatcher.h>
#include <TkUtil/Mutex.h>

#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vector>


/** Commande reliant un picker, utilisé pour effectuer des sélections à
 * la souris, au théatre. Cette classe collabore avec un interacteur afin
 * d'avoir davantage de renseignements quant aux évènements survenus
 * (ex : une touche est-elle pressée).
 * Attention : cette classe est faite pour fonctionner avec une instance de la
 * classe VTKECMPicker.
 * \author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class VTKECMPickerCommand : public vtkCommand
{
	public :

	VTKECMPickerCommand (vtkRenderer* renderer, 
	                     vtkRenderWindowInteractor* interactor);

	virtual ~VTKECMPickerCommand ( );

	/** Notifie le gestionnaire de sélection du théatre du changement
	 * de la sélection.
	 */
	void Execute (vtkObject* caller, unsigned long eventId, void* callData);

	/**
	 * Appelé par l'interacteur lorsque l'utilisateur demande de sélectionner
	 * l'entité suivante ou précédante.
	 */
	virtual void NextSelection (bool next);

	/**
	 * Affecte à cette instance un gestionnaire de sélection associé.
	 * @warning		Il convient d'invoquer setSelectionManager (0) avant la
	 *				destruction du gestionnaire.
	 */
	virtual void SetSelectionManager (VTKECMSelectionManager* manager);

	/**
	 * \return		Eventuel gestionnaire de sélection associé.	
	 */	
	virtual const VTKECMSelectionManager* GetSelectionManager ( ) const;
	virtual VTKECMSelectionManager* GetSelectionManager ( );

	/**
	 * Affecte à cette instance un gestionnaire de logs dans lequel seront
	 * écrit les résultats des opérations de picking.
	 * @warning		Il convient d'invoquer setLogDispatcher (0) avant la
	 *				destruction du dispatcher.
	 * @see			getLogDispatcher
	 */
	virtual void SetLogDispatcher (TkUtil::LogDispatcher* dispatcher);

	/**
	 * @return		Le nom de l'objet associé à l'acteur transmis en argument
	 * 				(par exemple pour les messages).
	 * @warning		<B>Ne fait rien par défaut, méthode à surcharger.</B>
	 */
	virtual std::string GetName (const vtkActor& actor) const;


	protected :

	/**
	 * \return		L'interacteur associé.
	 */
	 virtual vtkRenderWindowInteractor& getInteractor ( );
	 
	 	/**
	 * \return		Le gestionnaire de rendu associé.
	 */
	 virtual vtkRenderer& getRenderer ( );
	  
	/**
	 * @return		le gestionnaire de logs, s'il y en a un.
	 */
	virtual TkUtil::LogDispatcher* GetLogDispatcher ( ) const
	{ return _logDispatcher; }

	/**
	 * Ecrit les informations tranmises dans le gestionnaire de logs associé
	 * à cette instance, s'il y en a un.
	 */
	virtual void Log (const TkUtil::Log& log);

	/**
	 * \return		 Le mutex qui protège l'instance.
	 */
	virtual TkUtil::Mutex& GetMutex ( ) const;


	private :

	VTKECMPickerCommand (const VTKECMPickerCommand&);
	VTKECMPickerCommand& operator = (const VTKECMPickerCommand&);

	vtkRenderWindowInteractor*				_interactor;
	vtkRenderer*							_renderer;

	/** Le gestionnaire de sélection associé. */
	VTKECMSelectionManager*					_selectionManager;

	/** Le gestionnaire de log où sont écrits les résultats des 
	 * opérations de picking.
	 */
	TkUtil::LogDispatcher*					_logDispatcher;

	/** Mutex pour protéger certaines opérations. */
	mutable TkUtil::Mutex					_mutex;

	/** Les objets pointés à la souris. */
	std::vector<vtkActor*>					_pointedObjects;
	size_t									_pointedObject;
};	// class VTKECMPickerCommand


#endif	// VTK_ECM_PICKER_COMMAND_H
