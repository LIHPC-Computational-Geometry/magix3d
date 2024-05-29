#ifndef VTK_ECM_PICKER_H
#define VTK_ECM_PICKER_H


#include <TkUtil/Mutex.h>
#include <VtkContrib/vtkFloatingPointType.h>

#include <vtkAssemblyPath.h>
#include <vtkRenderer.h>

#include <vtkPicker.h>
#include <vtkCellPicker.h>

#include <iostream>
#include <map>
#include <vector>


/**
 * <P>Classe de picker <I>VTK</I> offrant une <I>API</I> générique et
 * fonctionnant avec les classes <I>VTKECMSelectionManager</I> et
 * <I>VTKECMPickerCommand</I>.
 * </P>
 * <P>L'<I>API</I> propose 2 modes de sélection : par clic dans la boite
 * englobante de l'acteur, par clic à proximité de l'acteur (voir méthodes
 * <I>setMode</I> et <I>getMode</I>).
 * </P>
 * <P>Ces classes sont à spécialiser au sein de chaque application.</P>
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class VTKECMPicker : public vtkAbstractPropPicker
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~VTKECMPicker ( );

	/**
	 * Instanciateur VTK. RAS.
	 */
	static VTKECMPicker* New ( );

	/**
	 * Les types de pointage :<BR>
	 * <UL>
	 * <LI>BOUNDING_BOX : un objet est sélectionné si on clique dans sa boite
	 * englobante,
	 * <LI>DISTANCE : un objet est sélectionné si on clique a proximité,
	 * </UL>
	 */
	enum MODE {BOUNDING_BOX, DISTANCE};

	/**
	 * \param		Nouveau mode de fonctionnement du pointage.
	 * \see			GetMode
	 * \see			SetTolerance
	 */
	virtual void SetMode (MODE mode);

	/**
	 * \return		Mode de fonctionnement courrant du pointage.
	 * \see			SetMode
	 * \see			GetTolerance
	 */
	virtual MODE GetMode ( ) const;

	virtual void PrintSelf (std::ostream& os, vtkIndent indent);

	/**
	 * \param		Tolérance appliquée lors de la recherche d'objet pointé
	 *				par boite englobante.
	 *				La tolérance est spécifiée comme une fraction de la
	 *				diagonale de la fenêtre de rendu.
	 * \see			GetTolerance
	 * \see			SetCellTolerance
	 */
	virtual void SetTolerance (double tolerance);

	/**
	 * \return		La tolérance appliquée lors de la recherche d'objet
	 *				pointé par boite englobante.
	 * \see			SetTolerance
	 * \see			GetCellTolerance
	 */
	virtual double GetTolerance ( ) const;

	/**
	 * \param		Tolérance appliquée lors de la recherche d'objet pointé
	 *				par proximité directe.
	 *				La tolérance est spécifiée comme une fraction de la
	 *				diagonale de la fenêtre de rendu.
	 * \see			GetTolerance
	 * \see			GetCellTolerance
	 */
	virtual void SetCellTolerance (double tolerance);

	/**
	 * \return		La tolérance appliquée lors de la recherche d'objet
	 *				pointé par proximité directe.
	 * \see			SetCellTolerance
	 * \see			GetTolerance
	 */
	virtual double GetCellTolerance ( ) const;

	/**
	 * Effectue l'opération de picking et ordonne (selon GetMode) les objets
	 * pointés. Invoque <I>CompletePicking</I> avant d'appeler 
	 * <I>InvokeEvent (vtkCommand::EndPickEvent, 0)</I>, ce qui permet de
	 * spécialiser le traitement de picking dans une classe dérivée avant que
	 * <I>VTKECMPickerCommand::Execute</I> ne soit lui-même appelé.
	 * \param		coordonnées x, y et z du point pické, en pixels (z est a
	 *				priori nul).
	 * \param		renderer siège de l'opération de picking.
	 * \return		1 en cas de succès du picking, sinon 0.
	 * @see			CompletePicking
	 */
	virtual int Pick (vtkFloatingPointType x, vtkFloatingPointType y, vtkFloatingPointType z, vtkRenderer* renderer);

	/**
	 * \return		Le point sélectionné en coordonnées écran (pixels). La
	 *				troisième valeur est la profondeur dans le z-buffer
	 *				(normallement 0).
	 */
	virtual double* GetSelectionPoint ( );
	virtual void GetSelectionPoint (double pt [3]);

	/**
	 * \return		La position pointée, en cordonnées globales.
	 *
	 */
	virtual double* GetPickPosition ( );
	virtual void GetPickPosition (double pt[3]);

	/**
	 * \return		Les propriétés 3D (non triées) pointées par la souris.
	 * \see			GetPickedPositions
	 */
	virtual vtkProp3DCollection* GetProp3Ds ( );

	/**
	 * \return		Les points d'intersection des propriétés 3D pointées par la
	 *				souris.
	 * \see			GetProp3Ds
	 */
	virtual vtkPoints* GetPickedPositions ( );

	/**
	 * \return		Les objets pointés et classés du plus proche au plus
	 *				éloigné. L'algorithme de calcul de la distance est fonction
	 *				du mode de fonctionnement courrant.
	 * \see			GetMode
	 * \see			SetPickedActors
	 */
	const std::vector<vtkActor*>& GetPickedActors ( ) const;


	protected :

	/**
	 * Constructeur par défaut. RAS.
	 */
	VTKECMPicker ( );

	/**
	 * \return		Le <I>picker</I> courrant, selon le mode en cours.
	 * \see			GetMode
	 */
	virtual vtkPicker* GetPicker ( );

	/**
	 * \return		Le mutex qui protège l'instance.
	 */
	virtual TkUtil::Mutex& GetMutex ( ) const;

	/**
	 * Appelé par <I>Pick</I> en fin de traitement. Permet de spécialiser
	 * l'opération de pointage effectuée avant qu'une commande type
	 * <I>VTKECMPickerCommand::Execute</I> ne soit appelée.
	 * \warning		Ne fait rien par défaut.
	 */
	virtual void CompletePicking ( );

	/**
	 * Ordonne les objets pickés transmis en arguments et les affecte à
	 * au vecteur et au path transmis en arguments.
	 * @param		acteurs pointés classés selon la distance (entrée)
	 * @param		acteurs pointés triés selon la distance (entrée) et
	 * 				l'ancienneté (entité la plus récente à la plus ancienne)
	 * @param		acteurs pointés triés selon la distance (entrée) et
	 * 				l'ancienneté (entité la plus récente à la plus ancienne)
	 * @see			SetPath
	 */
	virtual void RegisterPickedEntities (
						const std::multimap<double,vtkActor*>& objects,
						std::vector<vtkActor*>& actors,
						vtkAssemblyPath& path) const;
	/**
	 * \param		Les objets pointés. Ne sont pas reclassés.
	 * \see			GetMode
	 * \see			GetPickedActors
	 */
	virtual void SetPickedActors (const std::vector<vtkActor*>&);


	private :

	VTKECMPicker (const VTKECMPicker&);
	VTKECMPicker& operator = (const VTKECMPicker&);

	/** Le mutex qui protège l'instance. */
	mutable TkUtil::Mutex				_mutex;

	/** La tolérance appliquée lors de la recherche d'objet pointé. */
	double								_tolerance;

	/** Le picker utilisé pour rechercher les objets pointés. */
	vtkPicker*							_picker;
	vtkCellPicker*						_cellPicker;

	/** Le mode courrant de fonctionnement du pointage. */
	MODE								_mode;

	/** Les objets pointés. */
	std::vector<vtkActor*>				_actors;
};	// class VTKECMPicker


/**
 * Classe de picker a priori plus précise dans le pointage sur boite englobante
 * que vtkPicker. Cette dernière pose des problèmes avec VTK 5.10.0 au niveau
 * des distances retournées.
 */
class VTKECMBoundingBoxPicker : public vtkPicker
{
	public :

	VTKECMBoundingBoxPicker ( );
	virtual ~VTKECMBoundingBoxPicker ( );
	virtual int Pick (double selectionX, double selectionY, double selectionZ, vtkRenderer *renderer);


	private :

	VTKECMBoundingBoxPicker (const VTKECMBoundingBoxPicker&);
	VTKECMBoundingBoxPicker& operator = (const VTKECMBoundingBoxPicker&);
};	// class VTKECMBoundingBoxPicker


#endif	// VTK_ECM_PICKER_H

