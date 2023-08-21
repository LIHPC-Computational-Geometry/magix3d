/**
 * \file		VTKRenderingManager.h
 * \author		Charles PIGNEROL
 * \date		04/06/2012
 */
#ifndef VTK_RENDERING_MANAGER_H
#define VTK_RENDERING_MANAGER_H


#include <QtVtkComponents/vtkMgx3DInteractorStyle.h>
#include "QtVtkComponents/VTKMgx3DPicker.h"
#include "QtVtkComponents/VTKMgx3DPickerCommand.h"
#include "QtVtkComponents/VTKMgx3DSelectionManager.h"
//#include "QtVtkComponents/QtVtkWidget.h"
#include <QtComponents/Qt3DGraphicalWidget.h>
#include <QtVtk/QtVtkGraphicWidget.h>
#include <VtkContrib/vtkConstrainedPointWidget.h>
#include <VtkContrib/vtkECMAxesActor.h>
#include <VtkContrib/vtkLandmarkActor.h>
#include <VtkContrib/vtkTrihedronCommand.h>

#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkImplicitPlaneWidget.h>
#include <vtkPointWidget.h>
#include <vtkLightKit.h>
#include <vtkLookupTable.h>
#include <vtkPlane.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarWidget.h>

#include <vector>


namespace Mgx3D 
{

namespace QtVtkComponents
{

class VTKMgx3DSelectionManager;


/**
 * \brief		Classe de gestionnaire d'entités et rendu pour VTK.
 *
 */
class VTKRenderingManager : public QtComponents::RenderingManager
{
	public :

	/**
	 * \param		Eventuel widget associé.
	 * \param		<I>true</I> si le rendu doit être fait dans
	 *				une fenêtre <I>offscreen</I>.
	 */
//	VTKRenderingManager (QtVtkWidget* w, bool offScreen);
	VTKRenderingManager (QtVtkGraphicWidget* w, bool offScreen);

	/**
	 * RAS.
	 */
	virtual ~VTKRenderingManager ( );

	/**
	 * Les entités représentées.
	 */
	//@{

	/**
	 * \return		<I>true</I> si des représentations sont affichées,
	 *				<I>false</I> dans le cas contraire.
	 * \see			displayRepresentation
	 * \see			isDisplayed
	 */
	virtual bool hasDisplayedRepresentation ( ) const;

	/**
	 * \return		La liste des entités représentées.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getDisplayedEntities ( );

	/**
	 * Recherche parmi les entités représentées celles dont les valeurs aux
	 * noeuds ou aux mailles sont affichées. En retour, la liste des définitions
	 * de table de couleurs rencontrées.
	 */
	virtual void getDisplayedValues (std::vector<RenderingManager::ColorTableDefinition>& definitions);

	/**
	 * Passe ou quitte le mode d'utilisation des propriétés d'affichage
	 * global, partagées par les entités, et utilisées pour initialiser les
	 * propriétés d'affichage individuelles.
	 */
	virtual void useGlobalDisplayProperties (bool global);

	//@}	// Les entités représentées.


	/**
	 * Les représentations annexes.
	 */
	//@{

	/**
	 * Affiche/masque la représentation dont l'identifiant est tranmis en
	 * argument.
	 */
	virtual void displayRepresentation (RepresentationID id, bool show);

	/**
	 * Créé un vecteur orthogonal à celui transmis en argument.
	 * \param		Coordonnées du vecteur dont on cherche un vecteur orthogonal
	 * \param		Coordonnées du vecteur orthogonal (ou (0., 0., 0.) en cas de
	 *				vecteur nul en entrée).
	 */
	static void getOrthogonalVector (const double coords [3], double orth [3]);

	/**
	 * Affiche un segment.
	 * \param		Extrémités du segment.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation du segment créée.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createSegment (
			const Mgx3D::Utils::Math::Point& p1,
			const Mgx3D::Utils::Math::Point& p2,
			const Mgx3D::Utils::DisplayProperties& properties, bool display);

	/**
	 * Affiche un vecteur.
	 * \param		Extrémités du vecteur.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation du vecteur créée.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createVector (
			const Mgx3D::Utils::Math::Point& p1,
			const Mgx3D::Utils::Math::Point& p2,
			const Mgx3D::Utils::DisplayProperties& properties, bool display);

	/**
	 * Affiche un axe de rotation.
	 * \param		Extrémités de l'axe de rotation.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation de l'axe créé.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createRotationAxe (
			const Mgx3D::Utils::Math::Point& p1,
			const Mgx3D::Utils::Math::Point& p2,
			const Mgx3D::Utils::DisplayProperties& properties, bool display);

	/**
	 * Affiche une boite sous forme filaire.
	 * \param		2 points opposés de la boite.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation de la boite créée.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createOutlineBox (
			const Mgx3D::Utils::Math::Point& p1,
			const Mgx3D::Utils::Math::Point& p2,
			const Mgx3D::Utils::DisplayProperties& properties, bool display);

	/**
	 * Affiche une représentation du nuage de points transmis en argument.
	 * \param		Nuage de points à représenter.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation du nuage de point créée.
	 * \see			createSegmentsWireRepresentation
	 * \see			destroyRepresentation
	 */
	virtual RenderingManager::RepresentationID createCloudRepresentation (
				const std::vector<Mgx3D::Utils::Math::Point>& points,
				const Mgx3D::Utils::DisplayProperties& properties,
				bool display);

	/**
	 * Affiche une représentation filaire des segments transmis en argument.
	 * \param		Noeuds des segments.
	 * \param		Segments à représenter.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation filaire créée.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			createCloudRepresentation
	 * \see			destroyRepresentation
	 */
	virtual RenderingManager::RepresentationID createSegmentsWireRepresentation(
				const std::vector<Mgx3D::Utils::Math::Point>& points,
				const std::vector<size_t>& segments,
				const Mgx3D::Utils::DisplayProperties& properties,
				bool display);

	/**
	 * Affiche le texte transmis en argument.
	 * \param		Emplacement du texte à afficher
	 * \param		Nombre à afficher textuellement
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation filaire créée.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			destroyRepresentation
	 */
	virtual RenderingManager::RepresentationID createTextRepresentation (
				const Mgx3D::Utils::Math::Point& position, size_t number,
				const Mgx3D::Utils::DisplayProperties& properties,
				bool display);

	/**
	 * Détruit la représentation dont l'ientifiant est transmis en argument.
	 * \param		<I>true</I> si la représentation ne doit préalablement plus
	 * 				être affichée.
	 */
	virtual void destroyRepresentation (RenderingManager::RepresentationID, bool hide);

	//@}	// Les représentations annexes.

	/**
	 * Les tables de couleurs.
	 */
	//@{

	/**
	 * Spécialisation <I>VTK</I> de la classe <I>ColorTable</I>.
	 */
	class VTKColorTable : public RenderingManager::ColorTable
	{
		public :

		/**
		 * Constructeur par défaut.
		 * \param	Gestionnaire d'affichage associé.
		 * \param	Définition de la table de couleur créée.
		 */
		VTKColorTable (VTKRenderingManager& rm, const RenderingManager::ColorTableDefinition& def);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~VTKColorTable ( );

		/**
		 * Affiche/masque la table de couleurs.
		 */
		virtual void display (bool show);

		/**
		 * \param	Le nombre de couleurs de la table de couleurs.
		 * \see		getColorNum
		 */
		virtual void setColorNum (size_t num);

		/**
		 * \param	Les valeurs extremales de la table de couleur.
		 * \see		setMinValue
		 * \see		setMaxValue
		 * \see		getExtrema
		 */
		virtual void setExtrema (double min, double max);

		/**
		 * \param	La nouvelle définition de la table de couleurs.
		 */
		virtual void setDefinition (const RenderingManager::ColorTableDefinition& definition);



		/**
		 * \return	Le gestionnaire <I>VTK</I> de la table de couleur.
		 */
		virtual const vtkScalarBarWidget& getScalarBarWidget ( ) const;
		virtual vtkScalarBarWidget& getScalarBarWidget ( );

		/**
		 * \return	La table de couleurs associée.
		 */
		virtual const vtkLookupTable& getLookupTable ( ) const;
		virtual vtkLookupTable& getLookupTable ( );

		/**
		 * \return	Le gestionnaire de rendu associé.
		 */
		virtual const VTKRenderingManager& getRenderingManager ( ) const;
		virtual VTKRenderingManager& getRenderingManager ( );


		private :

		/**
		 * Constructeur de copie, opérateur = : interdits.
		 */
		VTKColorTable (const VTKColorTable&);
		VTKColorTable& operator = (const VTKColorTable&);

		/** La table de couleur. */
		vtkScalarBarWidget*		_scalarBarWidget;

		/** Le gestionnaire de rendu associé. */
		VTKRenderingManager*	_renderingManager;
	};	// class VTKColorTable

	/**
	 * \return	Une table de couleur associée à ce gestionnaire de
	 * 			rendu et représentant la valeur dont le nom est transmis en
	 * 			argument. Créé la table si nécessaire.
	 */
	virtual RenderingManager::ColorTable* getColorTable (const RenderingManager::ColorTableDefinition&);

	/**
	 * \return	L'ensemble des tables de couleurs gérées par le gestionnaire.
	 */
	virtual std::vector<ColorTable*> getColorTables ( ) const;

	/**
	 * Détruit la table de couleur transmise en argument.
	 * \see		createColorTable
	 */
	virtual void removeColorTable (RenderingManager::ColorTable* table);

	/**
	 * \param	La nouvelle définition de la table de couleurs transmise en argument.
 	*/
	virtual void setTableDefinition(const RenderingManager::ColorTableDefinition &definition);


	//@}	// Les tables de couleurs.


	/**
	 * Les interacteurs
	 */
	//@{

	/**
	 * Classe <I>callback VTK</I> sur une interaction et template sur une
	 * classe ayant en fonction membre publique la méthode
	 * <I>vtkInteractorModified</I>. Cette méthode est invoquée lorsque
	 * l'utilisateur modifie interactivement l'interacteur.
	 */
	template <class Observer> class VTKInteractorCallback : public vtkCommand
	{
		public :

		VTKInteractorCallback (Observer* observer, vtkInteractorObserver* io)
			: vtkCommand ( ), _observer (observer)
		{
			if ((0 != _observer) && (0 != io))
				io->AddObserver (vtkCommand::InteractionEvent, this);
		}

		virtual void Execute (vtkObject*, unsigned long, void*)	
		{
			if (0 != _observer)
				_observer->vtkInteractorModified ( );
		}	// VTKInteractorCallback::Execute


		private :

		VTKInteractorCallback (const VTKInteractorCallback&)
			: vtkCommand ( ), _observer (0)
		{ }
		VTKInteractorCallback& operator = (const VTKInteractorCallback&)
		{ return *this; }

		Observer*		_observer;
	};	// class VTKInteractorCallback


	/**
	 * <P>Interacteur <I>VTK</I> permettant la définition d'un point.</P>
	 */
	class VTKPointInteractor : public PointInteractor
	{
		public :

		/**
		 * \param		Un point
	 	 * \param		Dimensions de la boite englobante de l'interacteur
		 * \param		Gestionnaire de rendu où l'interacteur sera représenté
		 * \param		Eventuel observateur des interactions.
		 */
		VTKPointInteractor (Mgx3D::Utils::Math::Point point, double dx, double dy, double dz, VTKRenderingManager& renderingManager, RenderingManager::InteractorObserver* observer);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~VTKPointInteractor ( );

		/**
		 * \return		Le point
		 * \see			setPoint
		 */
		virtual Mgx3D::Utils::Math::Point getPoint ( ) const;

		/**
		 * Actualise l'interacteur.
		 * \param		Nouvelle définition du point.
		 */
		virtual void setPoint (Mgx3D::Utils::Math::Point point);

		/**
		 * \param		En retour, les dimensions de la boite englobante.
		 */
		virtual void getBoxDimensions(double& dx, double& dy, double& dz) const;

		/**
		 * \param		Les dimensions de la boite englobante.
		 */
		virtual void setBoxDimensions(double dx, double dy, double dz);

		/**
		 * Méthode appelée lorsque l'interacteur <I>VTK</I> est modifié par
		 * l'utilisateur. Invoque <I>notifyObserverForModifications</I>.
		 */
		virtual void vtkInteractorModified ( );


		private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		VTKPointInteractor (const VTKPointInteractor&);
		VTKPointInteractor& operator = (const VTKPointInteractor&);

		/** Le widget VTK */
		vtkPointWidget*									_pointWidget;

		/** Le callback sur la modification de l'interacteur. */
		VTKInteractorCallback<VTKPointInteractor>*		_callback;

		/** Les dimensions de la boite englobante. */
		double											_dx, _dy, _dz;

		/** Le point initial. */
		double											_x, _y, _z;

		/** La boite englobante. */
		double											_boundingBox [6];
	};	// class VTKPointInteractor

	/**
	 * \param		Point proposé par défaut.
	 * \param		Dimensions de la boite englobante de l'interacteur
	 * \param		Eventuel observateur étant notifié de toute modification
	 *				effectuée à l'aide de l'interacteur.
	 * \return		Pointeur sur l'interacteur permettant de saisir les
	 *				paramètres de définition du point, ou 0 en cas d'échec de
	 *				la création de l'interacteur.
	 * \see			destroyInteractor
	 */
	virtual RenderingManager::PointInteractor* createPointInteractor (
			Mgx3D::Utils::Math::Point point, double dx, double dy, double dz,
			RenderingManager::InteractorObserver* observer);

	/**
	 * <P>Interacteur <I>VTK</I> permettant la définition d'un point contraint
	 * dans l'espace (le point sera l'un de ceux d'une grille VTK représentant une
	 * entité).</P>
	 */
	class VTKConstrainedPointInteractor : public ConstrainedPointInteractor
	{
		public :

		/**
		 * \param		Un point
		 * \param		La contrainte spatiale
		 * \param		degré de raffinement attendu (ex : 10 pour x10 par
		 * 			rapport à la représentation par défaut, ...).
		 * \param		Gestionnaire de rendu où l'interacteur sera représenté
		 * \param		Eventuel observateur des interactions.
		 */
		VTKConstrainedPointInteractor (Mgx3D::Utils::Math::Point point,
				Utils::Entity* constraint, size_t factor,
				VTKRenderingManager& renderingManager,
				RenderingManager::InteractorObserver* observer);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~VTKConstrainedPointInteractor ( );

		/**
		 * \return		Le point
		 * \see			setPoint
		 */
		virtual Mgx3D::Utils::Math::Point getPoint ( ) const;

		/**
		 * Actualise l'interacteur.
		 * \param		Nouvelle définition du point.
		 */
		virtual void setPoint (Mgx3D::Utils::Math::Point point);

		/**
		 * \param		La nouvelle contrainte de positionnement.
		 * \param		degré de raffinement attendu (ex : 10 pour x10 par
		 * 			rapport à la représentation par défaut, ...).
		 */
		virtual void setConstraint (Utils::Entity* constraint, size_t factor);

		/**
		 * Méthode appelée lorsque l'interacteur <I>VTK</I> est modifié par
		 * l'utilisateur. Invoque <I>notifyObserverForModifications</I>.
		 */
		virtual void vtkInteractorModified ( );


		private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		VTKConstrainedPointInteractor (const VTKConstrainedPointInteractor&);
		VTKConstrainedPointInteractor& operator = (const VTKConstrainedPointInteractor&);

		/** Le widget VTK */
		vtkConstrainedPointWidget*					_pointWidget;

		/** La contrainte. */
		vtkActor*							_constraintActor;

		/** Le callback sur la modification de l'interacteur. */
		VTKInteractorCallback<VTKConstrainedPointInteractor>*		_callback;

		/** Le point initial. */
		double								_x, _y, _z;
	};	// class VTKConstrainedPointInteractor

	/**
	 * \param		Point proposé par défaut.
	 * \param		La contrainte spatiale (de type vtkUnstructuedGrid*)
	 * \param		degré de raffinement attendu (ex : 10 pour x10 par
	 * 			rapport à la représentation par défaut, ...).
	 * \param		Eventuel observateur étant notifié de toute modification
	 *			effectuée à l'aide de l'interacteur.
	 * \return		Pointeur sur l'interacteur permettant de saisir les
	 *			paramètres de définition du point, ou 0 en cas d'échec de
	 *			la création de l'interacteur.
	 * \see			destroyInteractor
	 */
	virtual RenderingManager::ConstrainedPointInteractor* createConstrainedPointInteractor (
			Mgx3D::Utils::Math::Point point, Utils::Entity* constraint, size_t factor,
			RenderingManager::InteractorObserver* observer);

	/**
	 * <P>Interacteur <I>VTK</I> permettant la définition d'un plan.</P>
	 */
	class VTKPlaneInteractor : public PlaneInteractor
	{
		public :

		/**
		 * \param		Un point du plan
		 * \param		La normale au plan
	 	 * \param		Boite englobante de l'interacteur
		 * \param		Gestionnaire de rendu où l'interacteur sera représenté
		 * \param		Eventuel observateur des interactions.
		 */
		VTKPlaneInteractor (Mgx3D::Utils::Math::Point point,
		                    Mgx3D::Utils::Math::Vector normal,
							double xMin, double xMax, double yMin, double yMax,
							double zMin, double zaMx,
		                    VTKRenderingManager& renderingManager,
		                    RenderingManager::InteractorObserver* observer);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~VTKPlaneInteractor ( );

		/**
		 * \return		Un point du plan
		 * \see			getNormal
		 * \see			setPlane
		 */
		virtual Mgx3D::Utils::Math::Point getPoint ( ) const;

		/**
		 * \return		La normale au plan
		 * \see			setPlane
		 * \see			getPoint
		 */
		virtual Mgx3D::Utils::Math::Vector getNormal ( ) const;

		/**
		 * Actualise l'interacteur.
		 * \param		Nouvelle définition du plan.
		 */
		virtual void setPlane (
			Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal);

		/**
		 * Méthode appelée lorsque l'interacteur <I>VTK</I> est modifié par
		 * l'utilisateur. Invoque <I>notifyObserverForModifications</I>.
		 */
		virtual void vtkInteractorModified ( );


		private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		VTKPlaneInteractor (const VTKPlaneInteractor&);
		VTKPlaneInteractor& operator = (const VTKPlaneInteractor&);

		/** Le widget VTK */
		vtkImplicitPlaneWidget*							_planeWidget;

		/** Le support des données à récupérer auprès de l'interacteur. */
		vtkPlane*										_plane;

		/** Le callback sur la modification de l'interacteur. */
		VTKInteractorCallback<VTKPlaneInteractor>*		_callback;
	};	// class VTKPlaneInteractor

	/**
	 * \param		Point du plan et vecteur normal au plan proposés par défaut.
	 * \param		Boite englobante de l'interacteur
	 * \param		Eventuel observateur étant notifié de toute modification
	 *				effectuée à l'aide de l'interacteur.
	 * \return		Pointeur sur l'interacteur permettant de saisir les
	 *				paramètres de définition d'un plan, ou 0 en cas d'échec de
	 *				la création de l'interacteur.
	 * \see			destroyInteractor
	 */
	virtual RenderingManager::PlaneInteractor* createPlaneInteractor (
			Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal,
			double xMin, double xMax, double yMin, double yMax,
			double zMin, double zMax,
			RenderingManager::InteractorObserver* observer);

	/**
	 * Détruit l'interacteur transmis en argument.
	 */
	virtual void destroyInteractor (RenderingManager::Interactor* interactor);

	//@}	// Les interacteurs

	/**
	 * La sélection des entités.
	 */
	//@{

	/**
	 * \param		Le gestionnaire de sélection à associer.
	 * \see			getSelectionManager
	 */
	virtual void setSelectionManager (QtVtkComponents::VTKMgx3DSelectionManager*);

	/**
	 * Le gestionnaire de sélection à la souris (<I>picking</I>).
	 *
	 * \exception		Lève une exception en l'absence de picker associé.
	 */
	virtual VTKMgx3DPicker& getPicker ( );


	/**
	 * Le commande du gestionnaire de sélection à la souris (<I>picking</I>).
	 *
	 * \exception		Lève une exception en l'absence de commande associée.
	 */
	virtual VTKMgx3DPickerCommand& getPickerCommand ( );
	
	/**
	 * Se déréférence des outils de sélection en cours.
	 * Enregistre la commande de sélection auprès du gestionnaire de sélection à la souris (<I>picking</I>).
	 * @param			Nouveaux outils de sélection à utiliser.
	 */
	 virtual void setPickingTools (QtVtkComponents::VTKMgx3DPicker*, QtVtkComponents::VTKMgx3DPickerCommand*);
	 

	//@}	// La sélection des entités.

	/**
	 * Les opérations graphiques.
	 */
	//@{

	/**
	 * En retour, la boite englobante de ce qui est affiché.
	 * \warning		<B>Ne fait rien par défaut, méthode à surcharger.</B>
	 */
	virtual void getBoundingBox (double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax);

	/**
	 * \return	La couleur de fond de la fenêtre graphique.
	 */
	virtual void getBackground (double& red, double& green, double& blue) const;
	virtual TkUtil::Color getBackground ( ) const;
	/**
	 * Nouvelle couleur de fond de la fenêtre graphique.
	 */
	virtual void setBackground (double red, double green, double blue);
	virtual void setBackground (const TkUtil::Color& background);

	/**
	 * Faut-il afficher ou non le trièdre dans la vue graphique ?
	 */
	virtual void setDisplayTrihedron (bool display);

	/**
	 * Faut-il afficher ou non le repere dans la vue graphique ?
	 */
	virtual void setDisplayLandmark (bool display);

	/**
	 * \return	Les propriétés du repère.
	 */
	 virtual LandmarkAttributes getAxisProperties ( );

	/**
	 * \param	Les nouvelles propriétés du repère.
	 */
	 virtual void setAxisProperties (const LandmarkAttributes&);

	/**
	 * Faut-il afficher ou non le point focal (matérialisé par une croix,
	 * équivalent de l'option <I>show center</I>) de <I>ParaView</I>.
	 */
	virtual void setDisplayFocalPoint (bool display);

	/**
	 * Utiliser ou non un éclairage multiple.
	 */
	virtual void enableMultipleLighting (bool enable);

	/**
	 * Modifier les dimensions de la vue graphique.
	 * \param		Largeur, hauteur
	 */
	virtual void resizeView (unsigned long w, unsigned long h);

	/**
	 * Zoom du facteur (réduit l'angle entre les yeux) transmis en argument la vue.
	 * \warning		Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void zoomView (double factor);

	/**
	 * Récupérer les paramètres de la vue (position observateur/point focal).
	 * \param		Position de l'observateur (x, y, z)
	 * \param		Position du point focal (x, y, z)
	 * \param		Vecteur définissant la direction "haut" de la caméra
	 */
	virtual void getViewDefinition (
				double& x, double& y, double& z, double& fx, double& fy, double& fz,
				double& upx, double& upy, double& upz);

	/**
	 * Définir la vue (position observateur/point focal).
	 * \param		Position de l'observateur (x, y, z)
	 * \param		Position du point focal (x, y, z)
	 * \param		Vecteur définissant la direction "haut" de la caméra
	 */
	virtual void defineView (
				double x, double y, double z, double fx, double fy, double fz,
				double upx, double upy, double upz);

	/**
	 * Recadre la vue.
	 * \param		<I>true</I> s'il faut provoquer un réafffichage,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual void resetView (bool render = true);

	/**
	 * Passage mode de projection perspective <-> parallèle.
	 * @param		<I>true</I> s'il faut passer en mode de projection
	 * 				parallèle, <I>false</I> s'il faut passer en mode de
	 * 				projection perspective.
	 */
	virtual void setParallelProjection (bool parallel);

	/**
	 * Appelé pour ajuster automatiquement la vue à son contenu (ex : le repère
	 * à ce qui est affiché).
	 */
	virtual void updateView ( );

	/**
	 * Positionne la vue respectivement dans le plan <I>xOy</I>, <I>xOz</I>,
	 * <I>yOz</I>.
	 */
	virtual void displayxOyViewPlane ( );
	virtual void displayxOzViewPlane ( );
	virtual void displayyOzViewPlane ( );

	/**
	 * Annule le roulis (angle entre la caméra et le point focal).
	 */
	virtual void resetRoll ( );

	/**
	 * Force une opération de rendu.
	 */
	virtual void forceRender ( );

	/**
	 * \param		Temps de rendu souhaité hors interactions.
	 * \param		Temps de rendu souhaité durant les interactions.
	 */
	virtual void setFrameRates (double stillRF, double desiredFR);

	//@}	// Les opérations graphiques.

	/**
	 * Les objets <I>VTK</I>.
	 */
	//@{

	virtual bool hasRenderer ( ) const;
	virtual bool hasRenderWindow ( ) const;
	virtual vtkRenderer& getRenderer ( ) const;
	virtual vtkRenderer& getRenderer ( );
	virtual vtkCamera& getCamera ( );
	virtual vtkRenderWindow& getRenderWindow ( );
	virtual vtkMgx3DInteractorStyle& getMgx3DInteractorStyle ( );

	//@}	// Les objets <I>VTK</I>.

	/**
	 * Divers services.
	 */
	//@{

	/**
	 * Initialisations de cette API, notamment des aspects <I>VTK</I>.
	 * \param		<I>true</I> si le compositing sera utilisé (serveur/MPI).
	 * \see			finalize
	 */
	static void initialize (bool compositing = false);

	/**
	 * Libération des ressources de l'API.
	 * \see			initialize
	 */
	static void finalize ( );

	//@}	// Divers services.

	/**
	 * Callbacks.
	 */
	//@{

	/**
	 * Appelé lorsque le point focal est modifié. Actualise la croix
	 * matérialisant le point focal.
	 */
	virtual void focalPointModifiedCallback ( );

	//@}	// Callbacks


	protected :

	/**
	 * Constructeur "vide". Ne créé ni gestionnaire de rendu, ni fenêtre
	 * de rendu <I>VTK</I>.
	 */
	VTKRenderingManager ( );

	/**
	 * \return		Le widget VTK utilisé.
	 */
//	virtual QtVtkWidget& getVTKWidget ( );
	virtual QtVtkGraphicWidget& getVTKWidget ( );

	/**
	 * \return		Une représentation <I>VTK</I> de l'entité transmise en
	 *				argument.
	 * \warning		Pour les entités de type maillage cette fonction est
	 *				optimisée (lien direct GMDS -> VTK).
	 * \see			getRepresentation
	 */
	virtual Utils::GraphicalEntityRepresentation* createRepresentation (Mgx3D::Utils::Entity& e);

	/**
	 * Affiche ou détruit la représentation 3D transmise en argument.
	 * \param		Représentation à afficher ou détruire.
	 * \param		<I>true</I> s'il faut afficher la représentation,
	 *				<I>false</I> s'il faut la détruire.
	 * \param		<I>true</I> s'il faut forcer l'actualisation de la
	 *				représentation graphique.
	 * \param		Masque d'affichage de l'entité (cf. 
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres.
	 */
	virtual void displayRepresentation (
				Utils::GraphicalEntityRepresentation& representation,
				bool show, bool forceUpdate, unsigned long mask);

	/**
	 * Classe actualisant la vue graphique lors de certains évènements
	 * <I>VTK</I>.
	 */
	class VTKRenderingManagerInteractionUpdater : public vtkCommand
	{
		public :

		VTKRenderingManagerInteractionUpdater (VTKRenderingManager& renderingManager);
		virtual ~VTKRenderingManagerInteractionUpdater ( );

		/**
		 * Actualise la croix représentant le point focal de la vue 3D.
		 */
		virtual void Execute (vtkObject*, unsigned long, void*);


		private :

		// Opérations interdites :
		VTKRenderingManagerInteractionUpdater (const VTKRenderingManagerInteractionUpdater&);
		VTKRenderingManagerInteractionUpdater& operator = (const VTKRenderingManagerInteractionUpdater&);

		VTKRenderingManager*			_renderingManager;
	};	// class VTKRenderingManagerInteractionUpdater

	private :

	// Opérations interdites :
	VTKRenderingManager (const VTKRenderingManager&);
	VTKRenderingManager& operator = (const VTKRenderingManager&);

	/** Eventuel widget <I>Qt/VTK</I> associé. */
//	QtVtkWidget*									_vtkWidget;
	QtVtkGraphicWidget*						_vtkWidget;

	/** L'afficheur utilisé. */
	mutable vtkRenderer*							_renderer;

	/** La fenêtre de rendu. */
	vtkRenderWindow*								_renderWindow;

	/** Le nombre de couches, les identifiants de chaque couche. */
	int												_layerNum;
	int												_theaterLayer,
													_trihedronLayer;

	/** La caméra utilisée pour définir la vue. */
	vtkCamera*										_camera;

	/** Les éventuelles sources lumineuses additionnelles. */
	vtkLightKit*									_lightKit;

	/** Le picker utilisé. */
	VTKMgx3DPicker*									_magix3dPicker;

	/** La commande utilisée éventuellement pour le picking. */
	VTKMgx3DPickerCommand*							_pickerCommand;
	unsigned long									_pickerCommandTag;

	/** L'éventuel croix montrant le point focal dans la vue 3D. */
	vtkECMAxesActor*								_focalPointAxesActor;

	/** La command actualisant la croix représentant le point focal. */
	unsigned long									_focalPointAxesTag;

	/** L'éventuel trièdre régulier donnant l'orientation de la vue 3D. */
	vtkTrihedron*									_trihedron;
	unsigned long									_trihedronCommandTag;

	/** Le renderer de l'éventuel triédron. */
	vtkRenderer*									_trihedronRenderer;

	/** Le repère et ses propriétés. */
	vtkLandmarkActor*								_axisActor;
	LandmarkAttributes								_axisProperties;


	/** Les éventuelles tables de couleur affichées. */
	std::vector<VTKColorTable*>						_colorTables;
};	// class VTKRenderingManager


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_RENDERING_MANAGER_H
