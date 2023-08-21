/**
 * \file		RenderingManager.h
 * \author		Charles PIGNEROL
 * \date		04/06/2012
 */
#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H


#include "Group/GroupEntity.h"
#include "Internal/Context.h"
#include "Utils/SelectionManager.h"
#include "Utils/GraphicalEntityRepresentation.h"
#include "Internal/CommandInternal.h"
#include "QtComponents/LandmarkAttributes.h"


#include <TkUtil/Timer.h>
/**
 * Macro-commandes pour faire du profiling sur le rendu graphique.
 */
#define DECLARE_RENDERING_TIMER                                               \
static TkUtil::Timer		renderingTimer;

#define RUN_RENDERING_TIMER                                                   \
renderingTimer.reset ( );                                                     \
renderingTimer.start ( );

#define STOP_RENDERING_TIMER(ostream, message)                                \
renderingTimer.stop ( );                                                      \
ostream << message << " Temps mis : "                                         \
	<< (1000.*renderingTimer.cpuDuration( )/TkUtil::Timer::cpuPerSecond( ))   \
	<< " ms" << std::endl;

namespace Mgx3D 
{

namespace QtComponents
{

/**
 * \brief		<P>Classe gestionnaire d'affichages 3D générique.
 *				Cette classe est à dériver pour spécialiser avec une API 3D
 *				(VTK, Paraview).</P>
 *
 *				<P>Cette classe dérive de SelectionManagerObserver afin d'être
 *				en mesure d'actualiser les représentations graphiques en
 *				fonction du caractère sélectionné ou non des entités.</P>
 * 				</P>
 * \author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class RenderingManager : public Utils::SelectionManagerObserver
{
public :

	struct ColorTableDefinition;

	class ColorTable;

	/**
	 */
	RenderingManager();

	/**
	 * RAS.
	 */
	virtual ~RenderingManager();

	/**
	 * Le contexte d'utilisation du gestionnaire de rendu.
	 */
	//@{

	/*
	 * \return		Le contexte d'utilisation du gestionnaire de rendu.
	 * \see			setContext
	 */
	virtual Mgx3D::Internal::Context &getContext();

	virtual const Mgx3D::Internal::Context &getContext() const;

	/*
	 * \param		Nouveau contexte d'utilisation du gestionnaire de rendu.
	 * \see			getContext
	 */
	virtual void setContext(Mgx3D::Internal::Context *context);

	//@}	// Le contexte d'utilisation du gestionnaire de rendu.

	/**
	 * Les entités représentées.
	 */
	//@{

	/**
	 * Ajoute l'instance transmise en argument à la liste des entités
	 * représentées.
	 * \param		Instance à ajouter.
	 */
	virtual void addEntity(Mgx3D::Utils::Entity &entity);

	/**
	 * Ajoute les instances transmises en argument à la liste des entités
	 * représentées.
	 * \param		Instances à ajouter.
	 * \param		<I>true</I> si elles sont affichées, <I>false</I>
	 *				dans le cas contraire.
	 * \param		Type d'entités
	 */
	virtual void addEntities(
			const std::vector<Mgx3D::Utils::Entity *> &entities, bool shown,
			Mgx3D::Utils::DisplayRepresentation::display_type type);

	/**
	 * Enlève l'instance transmise en argument de la liste des entités
	 * représentées.
	 * \param		Instance à enlever.
	 */
	virtual void removeEntity(Mgx3D::Utils::Entity &entity);

	/**
	 * Enlève les instances transmises en argument de la liste des entités
	 * représentées.
	 * \param		Instances à enlever.
	 * \param		Type d'entités
	 */
	virtual void removeEntities(
			const std::vector<Mgx3D::Utils::Entity *> &entities,
			Mgx3D::Utils::DisplayRepresentation::display_type type);

	/**
	 * \return		La représentation associée à l'entité transmise en argument.
	 * \see			createRepresentation
	 * \see			isDisplayed
	 */
	virtual Utils::GraphicalEntityRepresentation *getRepresentation(Mgx3D::Utils::Entity &e);

	virtual const Utils::GraphicalEntityRepresentation *getRepresentation(const Mgx3D::Utils::Entity &e) const;

	/**
	 * Affiche ou masque, selon la valeur de <I>show</I>, la représentation de
	 * l'entité transmise en argument.
	 * \param		Entité à représenter
	 * \param		<I>true</I> s'il faut représenter l'entité, <I>false</I>
	 *				dans le cas contraire.
	 * \param		Masque d'affichage de l'entité (cf. 
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres.
	 * \see			hasDisplayedRepresentation
	 * \see			isDisplayed
	 */
	virtual void displayRepresentation(Mgx3D::Utils::Entity &entity, bool show, unsigned long mask);

	/**
	 * Affiche ou masque, selon la valeur de <I>show</I>, les représentations
	 * des entités transmises en argument.
	 * \param		Entités à représenter
	 * \param		<I>true</I> s'il faut représenter les entités, <I>false</I>
	 *				dans le cas contraire.
	 * \param		Masque d'affichage des entités (cf. 
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres.
	 * \param		Type d'entités
	 * \see			hasDisplayedRepresentation
	 * \see			isDisplayed
	 */
	virtual void displayRepresentations(
			const std::vector<Mgx3D::Utils::Entity *> &entities,
			bool show, unsigned long mask,
			Mgx3D::Utils::DisplayRepresentation::display_type type);

	/**
	 * Affiche ou masque, selon la valeur de <I>show</I>, les représentations
	 * des entités des groupes transmis en argument.
	 * \param		Groupes d'entités à représenter
	 * \param		<I>true</I> s'il faut représenter les entités, <I>false</I>
	 *				dans le cas contraire.
	 * \see			displayTypes
	 * \see			updateDisplayedGroupsEntities
	 */
	virtual void displayRepresentations(
			const std::vector<Mgx3D::Group::GroupEntity *> &groups, bool show);

	/**
	 * Affiche ou masque, selon la valeur de <I>show</I>, les représentations
	 * des entités sélectionnées
	 * \param		<I>true</I> s'il faut représenter les entités, <I>false</I>
	 *				dans le cas contraire.
	 * \see			hasDisplayedRepresentation
	 * \see			isDisplayed
	 */
	virtual void displayRepresentationsSelectedEntities(bool show);

	/**
	 * \return		Les types d'entités affichés
	 * \see			displayTypes
	 */
	 virtual Mgx3D::Utils::FilterEntity::objectType displayedTypes ( ) const;

	/**
	 * Actualise la représentation des groupes de données transmis en arguments
	 * avec le nouveau filtre transmis en argument.
	 * <B>Fonctionne avec la méthode <I>displayRepresentations</I> prenant en
	 * argument des instances de la classe <I>GroupEntity</I>.</B>
	 * \param		Nouveau filtre sur les types d'entités affichés à appliquer.
	 * \see			displayRepresentations
	 * \see			updateDisplayedGroupsEntities
	 * \see			displayedTypes
	 */
	virtual void displayTypes(Mgx3D::Utils::FilterEntity::objectType types);

	/**
	 * Actualise la représentation de l'entité tranmise en argument.
	 * \param		Entité dont on modifie la représentation
	 * \param		Nouveau masque de représentation
	 * \param		true s'il faut recalculer sa représentation quoi qu'il en
	 *				coûte (ex : modification du shrink factor)
	 */
	virtual void updateRepresentation(
			Mgx3D::Utils::Entity &entity, unsigned long mask, bool updateRep);

	/**
	 * Actualise la représentation de l'entité tranmise en argument.
	 * \see			isDisplayed
	 */
	virtual void updateRepresentation(Mgx3D::Utils::Entity &entity);

	/**
	 * Force la réactualisation de toutes les représentations des entités
	 * affichées.
	 */
	virtual void updateRepresentations();

	/**
	 * \return		<I>true</I> si l'entité transmise en argument est affichée,
	 *				<I>false</I> dans le cas contraire.
	 * \see			displayRepresentation
	 */
	virtual bool isDisplayed(const Mgx3D::Utils::Entity &entity) const;

	/**
	 * \return		<I>true</I> si des représentations sont affichées,
	 *				<I>false</I> dans le cas contraire.
	 * \see			displayRepresentation
	 * \see			isDisplayed
	 */
	virtual bool hasDisplayedRepresentation() const;

	/**
	 * \return		La liste des entités représentées.
	 * \warning		<B>Ne fait rien, méthode à surcharger.</B>
	 */
	virtual std::vector<Mgx3D::Utils::Entity *> getDisplayedEntities();

	/**
	 * Recherche parmi les entités représentées celles dont les valeurs aux
	 * noeuds ou aux mailles sont affichées. En retour, la liste des définitions
	 * de table de couleurs rencontrées.
	 * \warning		Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void getDisplayedValues(std::vector <ColorTableDefinition> &definitions);

	/**
	 * Passe ou quitte le mode d'utilisation des propriétés d'affichage
	 * global, partagées par les entités, et utilisées pour initialiser les
	 * propriétés d'affichage individuelles.
	 */
	virtual void useGlobalDisplayProperties(bool global);

	virtual bool useGlobalDisplayProperties() const;

	/**
	 * Passe ou quitte le mode d'utilisation de la couleur des entités
	 * géométriques associées pour représenter les entités topologiques.
	 */
	virtual void topoUseGeomColor(bool useGeomColor);

	virtual bool topoUseGeomColor() const;

	//@}	// Les entités représentées.


	/**
	 * Les représentations annexes.
	 */
	//@{

	typedef void *RepresentationID;

	/**
	 * Affiche/masque la représentation dont l'identifiant est tranmis en
	 * argument.
	 */
	virtual void displayRepresentation(RepresentationID id, bool show);

	/**
	 * Gère le <I>previewBegin</I>/<I>previewEnd</I> d'une commande. Lors de sa
	 * destruction appelle <I>previewEnd</I> de la commande puis affiche les
	 * représentations annexes recensées.
	 */
	class CommandPreviewMgr
	{
	public :

		/**
		 * \param	Commande à annuler lors de la destruction
		 * \param	Gestionnaire de rendu associé
		 * \param	Appelle <I>command.previewBegin ( )</I> si <I>true</I>.
		 */
		CommandPreviewMgr(
				Mgx3D::Internal::CommandInternal &command,
				Mgx3D::QtComponents::RenderingManager &renderingManager,
				bool begin);

		/**
		 * Appelle <I>getCommand ( ).previewEnd ( )</I> puis affiche les
		 * représentations recensées.
		 * \see		registerRepresentationID
		 */
		virtual ~CommandPreviewMgr();

		/**
		 * Enregistre l'identifiant transmis en argument pour affichage lors de
		 * la destruction de ce gestionnaire d'apperçu.
		 */
		virtual void registerRepresentationID(
				Mgx3D::QtComponents::RenderingManager::RepresentationID id);


	protected :

		/**
		 * \return		La commande prise en charge.
		 */
		virtual Mgx3D::Internal::CommandInternal &getCommand();

		/**
		 * \return		Le gestionnaire de rendu utilisé.
		 */
		virtual Mgx3D::QtComponents::RenderingManager &getRenderingManager();

		/**
		 * \return		Les représentations à afficher.
		 */
		virtual std::vector <Mgx3D::QtComponents::RenderingManager::RepresentationID> getRepresentationsIDs() const;


	protected :

		CommandPreviewMgr();


	private :

		CommandPreviewMgr(const CommandPreviewMgr &);

		CommandPreviewMgr &operator=(const CommandPreviewMgr &);

		Mgx3D::Internal::CommandInternal      *_command;
		Mgx3D::QtComponents::RenderingManager *_renderingManager;
		std::vector <Mgx3D::QtComponents::RenderingManager::RepresentationID> _ids;
	};    // class CommandPreviewMgr

	/**
	 * Affiche un segment.
	 * \param		Extrémités du segment.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation du segment créée.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createSegment(
			const Mgx3D::Utils::Math::Point &p1,
			const Mgx3D::Utils::Math::Point &p2,
			const Mgx3D::Utils::DisplayProperties &properties, bool display);

	/**
	 * Affiche un vecteur.
	 * \param		Extrémités du vecteur.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation du vecteur créée.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createVector(
			const Mgx3D::Utils::Math::Point &p1,
			const Mgx3D::Utils::Math::Point &p2,
			const Mgx3D::Utils::DisplayProperties &properties, bool display);

	/**
	 * Affiche un axe de rotation.
	 * \param		Extrémités de l'axe de rotation.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation de l'axe créé.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createRotationAxe(
			const Mgx3D::Utils::Math::Point &p1,
			const Mgx3D::Utils::Math::Point &p2,
			const Mgx3D::Utils::DisplayProperties &properties, bool display);

	/**
	 * Affiche une boite sous forme filaire.
	 * \param		2 points opposés de la boite.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation de la boite créée.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			destroyRepresentation
	 */
	virtual RepresentationID createOutlineBox(
			const Mgx3D::Utils::Math::Point &p1,
			const Mgx3D::Utils::Math::Point &p2,
			const Mgx3D::Utils::DisplayProperties &properties, bool display);

	/**
	 * Affiche une représentation du nuage de points transmis en argument.
	 * \param		Nuage de points à représenter.
	 * \param		Attributs de représentation.
	 * \param		<I>true</I> s'il faut l'ajouter au <I>renderer</I>,
	 * 				<I>false</I> s'il ne faut pas.
	 * \return		Identifiant de la représentation du nuage de points créée.
	 * \warning		Un identifiant nul est retourné en cas d'échec de la
	 *				création de la représentation.
	 * \see			createSegmentsWireRepresentation
	 * \see			destroyRepresentation
	 */
	virtual RenderingManager::RepresentationID createCloudRepresentation(
			const std::vector <Mgx3D::Utils::Math::Point> &points,
			const Mgx3D::Utils::DisplayProperties &properties,
			bool display);

	/**
	 * Affiche une représentation filaire des segments transmis en argument.
	 * \param		Noeuds des segments.
	 * \param		Triangles à représenter.
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
			const std::vector <Mgx3D::Utils::Math::Point> &points,
			const std::vector <size_t> &segments,
			const Mgx3D::Utils::DisplayProperties &properties,
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
	virtual RenderingManager::RepresentationID createTextRepresentation(
			const Mgx3D::Utils::Math::Point &position,
			size_t number, const Mgx3D::Utils::DisplayProperties &properties,
			bool display);

	/**
	 * Détruit la représentation dont l'identifiant est transmis en argument.
	 * \param		<I>true</I> si la représentation ne doit préalablement plus
	 * 				être affichée.
	 */
	virtual void destroyRepresentation(
			RenderingManager::RepresentationID, bool hide);

	//@}	// Les représentations annexes.

	/**
	 * Les tables de couleurs.
	 */
	//@{

	/**
	 * Défini une table de couleur affichée dans la vue graphique.
	 */
	struct ColorTableDefinition
	{
		/**
		 * Constucteur par défaut.
		 * Nom invalide, USHRT_MAX couleurs, domaine max.
		 */
		ColorTableDefinition();

		/**
		 * Constructeur.
		 * \param	Nom de la valeur représentée.
		 * \param	Nombre d'entrées dans la table de couleurs.
		 * \param	Valeurs extrémales de la valeur.
		 */
		ColorTableDefinition(const std::string &name, size_t colorNum, double min, double max);

		/**
		 * Constructeur de copie, opérateur = : RAS.
		 */
		ColorTableDefinition(const ColorTableDefinition &);

		ColorTableDefinition &operator=(const ColorTableDefinition &);

		/**
		 * Destructeur. RAS.
		 */
		~ColorTableDefinition();

		/**
		 * Opérateurs de comparaison. RAS.
		 */
		bool operator==(const ColorTableDefinition &) const;

		bool operator!=(const ColorTableDefinition &) const;

		/**
		 * Actualise la définition de la table avec celle transmise en argument.
		 */
		void extent(const ColorTableDefinition &def);

		/**
		 * Actualise le domaine de définition de la table en y ajoutant celui
		 * transmis en argument.
		 */
		void extentDomain(double min, double max);

		/**
		 * \return		<I>true</I> si la table transmise en argument est
		 * 				comparable (traite de la même valeur).
		 */
		bool comparable(const ColorTableDefinition &);

		/** Le nom de la valeur représentée. */
		std::string valueName;

		/** Le nombre de couleurs. */
		size_t colorNum;

		/** Le domaine de représentation de la valeur. */
		double domainMin, domainMax;
	};    // struct ColorTableDefinition

	/**
	 * Représente une table de couleur affichée dans la vue graphique.
	 * Classe a spécialiser avec une API 3D.
	 */
	class ColorTable
	{
	public :

		/**
		 * Destructeur. RAS.
		 */
		virtual ~ColorTable();

		/**
		 * Affiche/masque la table de couleurs.
		 * \warning	A surcharger, lève une exception par défaut.
		 */
		virtual void display(bool show);

		/**
		 * \return	Le nombre de couleurs de la table de couleurs.
		 * \see		setColorNum
		 */
		virtual size_t getColorNum() const;

		/**
		 * \param	Le nombre de couleurs de la table de couleurs.
		 * \see		getColorNum
		 */
		virtual void setColorNum(size_t num);

		/**
		 * \return	Valeur minimale de la table de couleur.
		 * \see		getMaxValue
		 * \see		setMinValue
		 * \see		getExtrema
		 */
		virtual double getMinValue() const;

		/**
		 * \param	Valeur minimale de la table de couleur.
		 * \see		getMinValue
		 * \see		setMaxValue
		 * \see		setExtrema
		 */
		virtual void setMinValue(double min);

		/**
		 * \return	Valeur maximale de la table de couleur.
		 * \see		getMinValue
		 * \see		setMaxValue
		 * \see		setExtrema
		 */
		virtual double getMaxValue() const;

		/**
		 * \param	Valeur maximale de la table de couleur.
		 * \see		getMaxValue
		 * \see		setMinValue
		 * \see		setExtrema
		 */
		virtual void setMaxValue(double min);

		/**
		 * \param	En retour, les valeurs extremales de la table de couleur.
		 * \see		getMinValue
		 * \see		getMaxValue
		 * \see		setExtrema
		 */
		virtual void getExtrema(double &min, double &max) const;

		/**
		 * \param	Les valeurs extremales de la table de couleur.
		 * \see		setMinValue
		 * \see		setMaxValue
		 * \see		getExtrema
		 */
		virtual void setExtrema(double min, double max);

		/**
		 * \return	L'éventuel nom de valeur représentée.
		 * \see		setValueName
		 */
		virtual std::string getValueName() const;

		/**
		 * \param	L'éventuel nom de valeur représentée.
		 * \see		setValueName
		 */
		virtual void setValueName(const std::string &name);

		/**
		 * \return	La définition de la table de couleurs.
		 */
		virtual RenderingManager::ColorTableDefinition getDefinition() const;

		/**
		 * \param	La nouvelle définition de la table de couleurs.
		 */
		virtual void setDefinition(const RenderingManager::ColorTableDefinition &definition);


	protected :

		/**
		 * Constructeur par défaut.
		 * \param	Gestionnaire d'affichage associé.
		 * \param	Définition de la table de couleur créée.
		 */
		ColorTable(RenderingManager &rm, const ColorTableDefinition &def);

		/**
		 * \return	Le gestionnaire d'affichage associé.
		 */
		virtual const RenderingManager &getRenderingManager() const;

		virtual RenderingManager &getRenderingManager();


	private :

		/**
		 * Constructeur de copie, opérateur = : interdits.
		 */
		ColorTable(const ColorTable &);

		ColorTable &operator=(const ColorTable &);

		RenderingManager *_renderingManager;
		ColorTableDefinition _definition;
	};    // class ColorTable


	/**
	 * \return	Une table de couleur associée à ce gestionnaire de
	 * 			rendu et représentant la valeur dont le nom est transmis en
	 * 			argument. Créé la table si nécessaire.
	 * \warning	Méthode à surcharger, lève une exception par défaut.
	 */
	virtual ColorTable *getColorTable(const ColorTableDefinition &);

	/**
	 * Détruit la table de couleur transmise en argument.
	 * \see		createColorTable
	 * \warning	Méthode à surcharger, lève une exception par défaut.
	 */
	virtual void removeColorTable(ColorTable *table);

	/**
	 * \return	L'ensemble des tables de couleurs gérées par le gestionnaire.
	 * \warning	Méthode à surcharger, lève une exception par défaut.
	 */
	virtual std::vector<ColorTable *> getColorTables() const;

	/**
	 * \param	La nouvelle définition de la table de couleurs transmise en argument.
	 */
	virtual void setTableDefinition (const RenderingManager::ColorTableDefinition &definition);


	//@}	// Les tables de couleurs.

	/**
	 * Les interacteurs
	 */
	//@{

	/**
 	 * Classe de base d'observateur d'interacteur. Définit les callbacks
 	 * appelables, façon <I>Adapter</I>, c.a.d. qu'ici ils ne font rien et
 	 * qu'ils sont à surcharger au cas par cas.
 	 *
 	 * \see		Interactor
 	 */
	class InteractorObserver
	{
	public :

		/**
		 * Destructeur. RAS.
		 */
		virtual ~InteractorObserver();


		/**
		 * Les callbacks possibles.
		 * \warning		Ces méthodes ne font rien (type <I>adapter</I> et sont
		 *				donc à surcharger si nécessaire.
		 */
		virtual void pointModifiedCallback(Mgx3D::Utils::Math::Point point);

		virtual void planeModifiedCallback(
				Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal);


	protected :

		/**
		 * Constructeur. RAS.
		 */
		InteractorObserver();


	private :

		/**
		 * Constructeur de copie et opérateur = : interdit.
		 */
		InteractorObserver(const InteractorObserver &);

		InteractorObserver &operator=(const InteractorObserver &);
	};    // class InteractorObserver


	/**
	 * <P>Classe de base définissant un interacteur. Un interacteur permet à
	 * l'utilisateur de définir interactivement dans la vue 3D un élément
	 * géométrique (plan, boite, sphère, ...).</P>
	 * <P>Des callbacks permettent en retour d'actualiser l'IHM (panneau de
	 * définition du paramétrage de la forme géométrique par exemple). Ces
	 * callbacks sont adressés à des instances de la classe
	 * <I>InteractorObserver</I>.
	 * </P>
	 */
	class Interactor
	{
	public :

		/**
		 * Destructeur. RAS.
		 */
		virtual ~Interactor();


	protected :

		/**
		 * \param		Eventuel observateur des interactions.
		 */
		Interactor(RenderingManager::InteractorObserver *observer);

		/**
		 * \return		Un pointeur sur l'éventuel observateur.
		 */
		virtual RenderingManager::InteractorObserver *getObserver();

		/**
		 * Notifie l'éventuel observateur des changements opérés par
		 * l'utilisateur avec l'interacteur en invoquant la méthode
		 * <I>callback</I> adaptée.
		 * \warning		Cette méthode ne fait rien, elle est à surcharger dans
		 * 				les classes dérivées.
		 */
		virtual void notifyObserverForModifications();


	private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		Interactor(const Interactor &);

		Interactor &operator=(const Interactor &);

		RenderingManager::InteractorObserver *_observer;
	};    // class Interactor

	/**
	 * <P>Interacteur de base permettant la définition d'un point.</P>
	 * <P>Cette classe est à spécialiser. Lors des modifications du
	 * paramétrage du point les classes dérivées doivent informer l'éventuel
	 * observateur des modfications, par exemple en appelant la méthode
	 * <I>notifyObserverForModifications</I>.
	 * </P>
	 */
	class PointInteractor : public Interactor
	{
	public :

		/**
		 * Destructeur. RAS.
		 */
		virtual ~PointInteractor();

		/**
		 * \return		Le point
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 * \see			setPoint
		 */
		virtual Mgx3D::Utils::Math::Point getPoint() const;

		/**
		 * Actualise l'interacteur.
		 * \param		Nouvelle définition du point.
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 */
		virtual void setPoint(Mgx3D::Utils::Math::Point point);

		/**
		 * \param		En retour, les dimensions de la boite englobante.
		 * \see			setBoxDimensions
		 */
		virtual void getBoxDimensions(double &dx, double &dy, double &dz) const;

		/**
		 * \param		Les nouvelles dimensions de la boite englobante.
		 * \see			getBoxDimensions
		 */
		virtual void setBoxDimensions(double dx, double dy, double dz);


	protected :

		/**
		 * \param		Eventuel observateur des interactions.
		 */
		PointInteractor(RenderingManager::InteractorObserver *observer);

		/**
		 * Notifie l'éventuel observateur des changements opérés par
		 * l'utilisateur avec l'interacteur en invoquant sa méthode
		 * <I>pointModifiedCallback</I>.
		 */
		virtual void notifyObserverForModifications();


	private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		PointInteractor(const PointInteractor &);

		PointInteractor &operator=(const PointInteractor &);
	};    // class PointInteractor

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
	virtual RenderingManager::PointInteractor *createPointInteractor(
			Mgx3D::Utils::Math::Point point, double dx, double dy, double dz,
			RenderingManager::InteractorObserver *observer);

	/**
	 * <P>Interacteur de base permettant la définition contrainte d'un point contraint
	 * dans l'espace.</P>
	 * <P>Cette classe est à spécialiser. Lors des modifications du
	 * paramétrage du point les classes dérivées doivent informer l'éventuel
	 * observateur des modfications, par exemple en appelant la méthode
	 * <I>notifyObserverForModifications</I>.
	 * </P>
	 */
	class ConstrainedPointInteractor : public Interactor
	{
	public :

		/**
		 * Destructeur. RAS.
		 */
		virtual ~ConstrainedPointInteractor();

		/**
		 * \return		Le point
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 * \see			setPoint
		 */
		virtual Mgx3D::Utils::Math::Point getPoint() const;

		/**
		 * Actualise l'interacteur.
		 * \param		Nouvelle définition du point.
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 */
		virtual void setPoint(Mgx3D::Utils::Math::Point point);

		/**
		 * \param		La nouvelle contrainte de positionnement.
		 * \param		degré de raffinement attendu (ex : 10 pour x10 par
		 * 			rapport à la représentation par défaut, ...).
		 */
		virtual void setConstraint(Utils::Entity *constraint, size_t factor);


	protected :

		/**
		 * \param		Eventuel observateur des interactions.
		 */
		ConstrainedPointInteractor(RenderingManager::InteractorObserver *observer);

		/**
		 * Notifie l'éventuel observateur des changements opérés par
		 * l'utilisateur avec l'interacteur en invoquant sa méthode
		 * <I>pointModifiedCallback</I>.
		 */
		virtual void notifyObserverForModifications();


	private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		ConstrainedPointInteractor(const ConstrainedPointInteractor &);

		ConstrainedPointInteractor &operator=(const ConstrainedPointInteractor &);
	};    // class ConstrainedPointInteractor

	/**
	 * \param		Point proposé par défaut.
	 * \param		Contrainte spatiale exercée pour définir la position du point.
	 * \param		degré de raffinement attendu (ex : 10 pour x10 par
	 * 			rapport à la représentation par défaut, ...).
	 * \param		Eventuel observateur étant notifié de toute modification
	 *			effectuée à l'aide de l'interacteur.
	 * \return		Pointeur sur l'interacteur permettant de saisir les
	 *			paramètres de définition du point, ou 0 en cas d'échec de
	 *			la création de l'interacteur.
	 * \see			destroyInteractor
	 */
	virtual RenderingManager::ConstrainedPointInteractor *createConstrainedPointInteractor(
			Mgx3D::Utils::Math::Point point, Utils::Entity *constraint, size_t factor,
			RenderingManager::InteractorObserver *observer);

	/**
	 * <P>Interacteur de base permettant la définition d'un plan.</P>
	 * <P>Cette classe est à spécialiser. Lors des modifications du
	 * paramétrage du plan les classes dérivées doivent informer l'éventuel
	 * observateur des modfications, par exemple en appelant la méthode
	 * <I>notifyObserverForModifications</I>.
	 * </P>
	 */
	class PlaneInteractor : public Interactor
	{
	public :

		/**
		 * Destructeur. RAS.
		 */
		virtual ~PlaneInteractor();

		/**
		 * \return		Un point du plan
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 * \see			getNormal
		 * \see			setPlane
		 */
		virtual Mgx3D::Utils::Math::Point getPoint() const;

		/**
		 * \return		La normale au plan
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 * \see			setPlane
		 * \see			getPoint
		 */
		virtual Mgx3D::Utils::Math::Vector getNormal() const;

		/**
		 * Actualise l'interacteur.
		 * \param		Nouvelle définition du plan.
		 * \warning		<B>Lève une exception, méthode à surcharger</B>.
		 */
		virtual void setPlane(
				Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal);


	protected :

		/**
		 * \param		Eventuel observateur des interactions.
		 */
		PlaneInteractor(RenderingManager::InteractorObserver *observer);

		/**
		 * Notifie l'éventuel observateur des changements opérés par
		 * l'utilisateur avec l'interacteur en invoquant sa méthode
		 * <I>planeModifiedCallback</I>.
		 */
		virtual void notifyObserverForModifications();


	private :

		/**
		 * Constructeur de copie et opérateur = : interdits.
		 */
		PlaneInteractor(const PlaneInteractor &);

		PlaneInteractor &operator=(const PlaneInteractor &);
	};    // class PlaneInteractor

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
	virtual RenderingManager::PlaneInteractor *createPlaneInteractor(
			Mgx3D::Utils::Math::Point point, Mgx3D::Utils::Math::Vector normal,
			double xMin, double xMax, double yMin, double yMax,
			double zMin, double zMax,
			RenderingManager::InteractorObserver *observer);

	/**
	 * Détruit l'interacteur transmis en argument.
	 */
	virtual void destroyInteractor(RenderingManager::Interactor *interactor);

	//@}	// Les interacteurs


	/**
	 * La sélection des entités.
	 */
	//@{

	/**
	 * \param		Le gestionnaire de sélection à associer. Se référence
	 *				auprès de lui pour être informé des modifications de la
	 *				sélection.
	 * \see			getSelectionManager
	 * \see			entitiesAddedToSelection
	 * \see			entitiesRemovedFromSelection
	 */
	virtual void setSelectionManager(Utils::SelectionManagerIfc *);

	/**
	 * \return		L'éventuel gestionnaire de sélection associé.
	 * \see			setSelectionManager
	 */
	virtual Utils::SelectionManagerIfc *getSelectionManager();

	virtual const Utils::SelectionManagerIfc *getSelectionManager() const;

	/**
	 * Ajoute les entités tranmises en argument de la sélection.
	 */
	virtual void entitiesAddedToSelection(
			const std::vector<Mgx3D::Utils::Entity *> &entities);

	/**
	 * Enlève les entités tranmises en argument de la sélection.
	 * \param	Entités enlevées de la sélection
	 * \param	<I>true</I> s'il s'agit d'un <I>clearSelection</I>, à des fins
	 * 			d'optimisation.
	 */
	virtual void entitiesRemovedFromSelection(
			const std::vector<Mgx3D::Utils::Entity *> &entities, bool clear);

	/**
	 * Annule la sélection en cours (aspects visuels).
	 */
	virtual void selectionCleared();

	//@}	// La sélection des entités.

	/**
	 * Les opérations graphiques.
	 */
	//@{

	friend class DisplayLocker;

	/**
	 * Classe dont la finalité est de bloquer les opérations d'affichage
	 * d'une fenêtre graphique durant la durée de vie de l'instance, par
	 * exemple durant une méthode callback de créations de nombreuses
	 * entités. Grâce à cette instance toutes les opérations de tracés
	 * graphiques type <I>forceRender</I> sont inhibées.
	 */
	class DisplayLocker
	{
	public :

		/**
		 * @param		Fenêtre dont les opérations graphiques sont inhibées
		 *				durant la durée de vie de l'instance créée.
		 * @param		Si <I>true</I> appelle <I>forceRender</I> sur
		 *				la fenêtre graphique lors de la destruction de
		 *				cette instance.
		 */
		DisplayLocker(RenderingManager &w, bool forceRenderAtEnd = true);

		/**
		 * Appelle éventuellement <I>forceRender</I> sur la fenêtre
		 * graphique associée (cf. constructeur).
		 */
		~DisplayLocker();


	private :

		DisplayLocker(const DisplayLocker &);

		DisplayLocker &operator=(const DisplayLocker &);

		RenderingManager *_3dwidget;
		bool _forceRenderAtEnd;
	};    // class DisplayLocker

	/**
	 * En retour, la boite englobante de ce qui est affiché.
	 * \warning		<B>Ne fait rien par défaut, méthode à surcharger.</B>
	 */
	virtual void getBoundingBox(
			double &xmin, double &xmax, double &ymin, double &ymax,
			double &zmin, double &zmax);

	/**
	 * <P>En retour, des marges à appliquer à une boite englobante d'entité
	 * affiché (ex : <I>highlighting</I>, ...).</P>
	 * <P>Les marges retournées sont un pourcentage des échelles correspondant
	 * aux 3 dimensions de la boite englobante de ce qui est visible. Ex :
	 * si la boite englobante est (0.1, 0.3, 10.2, 45.3, 10, 34), les échelles
	 * sont 1, 100, 100, et c'est percent*échelle pour chaque axe qui est
	 * retourné.
	 * </P>
	 * \see			getBoundingBox
	 * \see			QtMgx3DApplication::_marginBoundingBoxPercentage
	 */
	virtual void getBoundingBoxMargins(
			double &xmargin, double &ymargin, double &zmargin);

	/**
	 * \return	La couleur de fond de la fenêtre graphique.
	 */
	virtual void getBackground(double &red, double &green, double &blue) const;

	virtual TkUtil::Color getBackground() const;

	/**
	 * Nouvelle couleur de fond de la fenêtre graphique.
	 */
	virtual void setBackground(double red, double green, double blue);

	virtual void setBackground(const TkUtil::Color &background);

	/**
	 * Faut-il afficher ou non le trièdre dans la vue graphique ?
	 */
	virtual void setDisplayTrihedron(bool display);

	/**
	 * Faut-il afficher ou non le repère dans la vue graphique ?
	 */
	virtual void setDisplayLandmark(bool display);

	/**
	 * \return	Les propriétés du repère.
	 */
	virtual LandmarkAttributes getAxisProperties();

	/**
	 * \param	Les nouvelles propriétés du repère.
	 */
	virtual void setAxisProperties(const LandmarkAttributes &);

	/**
	 * Faut-il afficher ou non le point focal (matérialisé par une croix,
	 * équivalent de l'option <I>show center</I>) de <I>ParaView</I>.
	 */
	virtual void setDisplayFocalPoint(bool display);

	/**
	 * Modifier les dimensions de la vue graphique.
	 * \param		Largeur, hauteur
	 */
	virtual void resizeView(unsigned long w, unsigned long h);

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
	virtual void getViewDefinition(
			double &x, double &y, double &z, double &fx, double &fy, double &fz,
			double &upx, double &upy, double &upz);

	/**
	 * Définir la vue (position observateur/point focal).
	 * \param		Position de l'observateur (x, y, z)
	 * \param		Position du point focal (x, y, z)
	 * \param		Vecteur définissant la direction "haut" de la caméra
	 */
	virtual void defineView(
			double x, double y, double z, double fx, double fy, double fz,
			double upx, double upy, double upz);

	/**
	 * Recadre la vue. Cette fonction est à surcharger, elle se contente
	 *				d'appeler <I>forceRender</I> si <I>render</I> vaut
	 *				<I>true</I>.
	 * \param		<I>true</I> s'il faut provoquer un réaffichage,
	 *				<I>false</I> dans le cas contraire.
	 */
	virtual void resetView(bool render = true);

	/**
	 * Passage mode de projection perspective <-> parallèle.
	 * @param		<I>true</I> s'il faut passer en mode de projection
	 * 				parallèle, <I>false</I> s'il faut passer en mode de
	 * 				projection perspective.
	 */
	virtual void setParallelProjection(bool parallel);

	/**
	 * Appelé pour ajuster automatiquement la vue à son contenu (ex : le repère
	 * à ce qui est affiché).
	 */
	virtual void updateView();

	/**
	 * Positionne la vue respectivement dans le plan <I>xOy</I>, <I>xOz</I>,
	 * <I>yOz</I>.
	 * \warning		Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void displayxOyViewPlane();

	virtual void displayxOzViewPlane();

	virtual void displayyOzViewPlane();

	/**
	 * Annule le roulis (angle entre la caméra et le point focal).
	 * \warning		Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void resetRoll();

	/**
	 * Force une opération de rendu.
	 * Ne fait rien par défaut.
	 */
	virtual void forceRender();

	/**
	 * \param		Temps de rendu souhaité hors interactions.
	 * \param		Temps de rendu souhaité durant les interactions.
	 */
	virtual void setFrameRates(double stillRF, double desiredFR);

	/**
	 * Mécanisme interne à la classe. Appelé lorsqu'une interaction utilisateur
	 * commence. Propage l'information au serveur pour adapter le
	 * <I>frame rate</I>.
	 * \warning		A surcharger, ne fait rien par défaut.
	 * \see			endInteractionCallback
	 */
	virtual void startInteractionCallback();

	/**
	 * Mécanisme interne à la classe. Appelé lorsqu'une interaction utilisateur
	 * s'achève. Propage l'information au serveur pour adapter le
	 * <I>frame rate</I>.
	 * \warning		A surcharger, ne fait rien par défaut.
	 * \see			startInteractionCallback
	 */
	virtual void endInteractionCallback();

	//@}	// Les opérations graphiques.

	/**
	 * La gestion de la configuration.
	 */
	//@{

	/**
	 * Actualise les préférences utilisateurs utilisées.
	 */
	virtual void updateConfiguration();

	//@}	// La gestion de la configuration.

	/**
	 * Les données structurées
	 */
	//@{

	/**
	 * La valeur de seuillage des données structurées affichées (ne concerne que l'affichage).
	 * Par défaut -NumericServices::doubleMachInfinity ( )
	 */
	virtual double getStructuredDataThresholdValue ( ) const;
	virtual void setStructuredDataThresholdValue (double threshold);
	//@}    // Les données structurées


	protected :

	/**
	 * \return		Une représentation 3D de l'entité transmise en argument.
	 *				A spécialiser dans les classes dérivées selon l'API 3D
	 *				utilisée.
	 * \see			getRepresentation
	 */
	virtual Utils::GraphicalEntityRepresentation* createRepresentation (
													Mgx3D::Utils::Entity& e);

	/**
	 * Affiche ou détruit la représentation 3D transmise en argument.
	 * A spécialiser dans les classes dérivées selon l'API 3D utilisée.
	 * \param		Représentation à afficher ou détruire.
	 * \param		<I>true</I> s'il faut afficher la représentation,
	 *				<I>false</I> s'il faut la détruire.
	 * \param		<I>true</I> s'il faut forcer l'actualisation de la
	 *				représentation graphique.
	 * \param		Masque d'affichage de l'entité (cf. 
	 *				<I>GraphicalEntityRepresentation::CURVES</I> et autres.
	 */
	virtual void displayRepresentation (
				Utils::GraphicalEntityRepresentation& representation, bool show,
				bool forceUpdate, unsigned long mask);

	/**
	 * Affiche/masque les groupes d'entités transmis en arguments.
	 * \params		Groupes d'entités à afficher/masquer.
	 */
	virtual void updateDisplayedGroupsEntities (
				std::vector<Mgx3D::Geom::GeomEntity*>& geomAddedShown,
				std::vector<Mgx3D::Geom::GeomEntity*>& geomAddedHidden,
				std::vector<Mgx3D::Topo::TopoEntity*>& topoAddedShown,
				std::vector<Mgx3D::Topo::TopoEntity*>& topoAddedHidden,
				std::vector<Mgx3D::Mesh::MeshEntity*>& meshAddedShown,
				std::vector<Mgx3D::Mesh::MeshEntity*>& meshAddedHidden,
				std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedShown,
				std::vector<CoordinateSystem::SysCoord*>& sysCoordAddedHidden,
				std::vector<Mgx3D::Structured::StructuredMeshEntity*>& sMeshAddedShown,
				std::vector<Mgx3D::Structured::StructuredMeshEntity*>& sMeshAddedHidden
				);

	/**
	 * \return		<I>true</I> si les opérations d'affichage sont inhibées
	 *				(optimisation).
	 * \see			lockDisplay
	 */
	virtual bool displayLocked ( ) const;

	/**
	 * \param		<I>true</I> si les opérations d'affichage sont à inhiber,
	 *				<I>false</I> dans le cas contraire.
	 * \see			displayLocked
	 */
	virtual void lockDisplay (bool lock);


	private :

	// Opérations interdites :
	RenderingManager (const RenderingManager&);
	RenderingManager& operator = (const RenderingManager&);

	/** Le contexte de session associé. */
	Mgx3D::Internal::Context*						_context;

	/** Si <I>true</I> ne pas effectuer d'opération d'affichage (optimisation).
	 */
	bool											_displayLocked;

	/** Si <I>true</I> l'affichage des entités utilise des propriétés
	 * d'affichage communes. Si <I>false</I> se sont des propriétés
	 * individuelles qui sont utilisées.
	 */
	bool											_useGlobalDisplayProperties;

	/** Les types d'entités affichés. */
	Mgx3D::Utils::FilterEntity::objectType          _displayedTypes;

	/** Si <I>true</I> l'affichage des entités topologiques utilise les
	 * couleurs des entités géométriques associées.
	 */
	bool											_topoUseGeomColor;

	/** La valeur de seuillage des données structurées affichées (ne concerne que l'affichage).
	 * Par défaut -NumericServices::doubleMachInfinity ( ) */
	double                                          _structuredDataThreshold;
};	// class RenderingManager


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// RENDERING_MANAGER_H
