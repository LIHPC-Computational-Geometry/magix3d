/**
 * \file		QtMgx3DApplication.h
 * \author		Charles PIGNEROL
 * \date		29/11/2010
 */
#ifndef QT_MGX3D_APPLICATION_H
#define QT_MGX3D_APPLICATION_H

#include "Internal/Resources.h"
#include <QApplication>

#include <TkUtil/Version.h>

#include <string>


namespace Mgx3D 
{

/*!
 * \namespace Mgx3D::QtComponents
 *
 * \brief Espace de nom des classes pour l'affichage via Qt.
 *
 *
 */
namespace QtComponents
{

class QtMgx3DMainWindow;

/**
 * \brief		Classe <I>application de base Magix 3D</I>.
 *
 * \warning		Après construction appeler <B>init</B>. Permet de profiter
 *				du polymorphisme via la surcharge.
 *
 */
class QtMgx3DApplication : public QApplication
{
	public :

	/**
	 * <P>Instancie l'application</P>.
	 * \param		arguments du point d'entrée (<I>main</I>) de l'application.
	 * \warning		<B>Appeler <I>init</I> juste après construction</B>.
	 * \warning		<B>argc doit être passé par référence pour
	 *				<I>QApplication</I>, sinon risque de plantage.</B>
	 * \see			init
	 */
	QtMgx3DApplication (int& argc, char* argv[], char* envp[]);

	/**
	 * RAS.
	 */
	virtual ~QtMgx3DApplication ( );

	/**
	 * <P>Procède au différentes initialisations requises.
	 * </P>
	 * <P>Invoque entre-autres <I>parseArgs</I>.
	 * </P>
	 * \param		arguments du point d'entrée (<I>main</I>) de l'application.
	 * \see			getArgs
	 * \see			getEnv
	 */
	virtual void init (int argc, char* argv[], char* envp[]);
	
	/**
	 * \return		Les arguments <I>argc</I> et <I>argv</I> de l'application. Requiert qu'ils aient été 
	 * 				transmis via <I>init</I>.
	 * \see			init
	 * \see			getEnv
	 */
	 static void getArgs (int* argc, char*** argv);
	 
	 	/**
	 * \return		L'environnement <I>argc</I> et <I>argv</I> de l'application. Requiert qu'ils aient été 
	 * 				transmis via <I>init</I>.
	 * \see			init
	 * \see			getArgs
	 */
	 static char** getEnv ( );

	/**
	 * \return		Une référence sur l'instance unique de cette classe.
	 * \exception	Une exception est levée en l'absence d'instance.
	 */	
	static QtMgx3DApplication& instance ( );
	
	/**
	 * \return		La fenêtre principale de l'application.
	 * \exception	Lève une exception s'il n'y a pas une unique instance de QtMgx3DMainWindow.
	 */
	 static QtMgx3DMainWindow& getMainWindow ( );

	/**
	 * @return		Le titre de l'application.
	 */
	static std::string getAppTitle ( );

	/**
	 * @return		<I>true</I> si l'application doit afficher des boites de
	 *				dialogue d'erreur lors des exceptions remontées lors de
	 *				l'actualisation d'informations sur des entités, <I>false</I>
	 *				dans le cas contraire. Concerne avant tout les modes
	 *				<I>client/serveur</I> en fonctionnement <I>asynchrone</I> :
	 *				un client peut recevoir un évènement de modification d'une
	 *				entité alors que celle-ci est en cours de destruction côté
	 *				serveur ...
	 */
	static bool displayUpdatesErrors ( );

	/**
	 * La boucle évènementielle.
	 */
	//@{


	/**
	 * Traite les évènements en cours dans la boucle évènementielle et pour une
	 * durée inférieure à <I>maxtime</I> millisecondes.
	 * \see		flushEventQueue
	 */
	static void processEvents (unsigned long maxtime);

	/**
	 * Traite les évènements en cours dans la boucle évènementielle.
	 * Permet par exemple le raffraichissement de l'IHM lors de tâches non
	 * graphiques longues.
	 * \see		processEvents
	 */
	static void flushEventQueue ( );

	//@}	// La boucle évènementielle.

	/**
	 * Les options de fonctionnement de l'application.
	 */
	//@{

	/**
	 * <P>Analyse les arguments reçus et initialise certaines variables
	 * en fonctions des arguments reçus.
	 * </P>
	 * \param		Nombre d'arguments.
	 * \param		Arguments.
	 * \see			syntax
	 */
	virtual void parseArgs (int& argc, char* argv []);

	/**
	 * Affiche la ligne de commande de lancement et ses options.
	 * \param		exécutable de l'application
	 * \see			parseArgs
	 */
	virtual void syntax (const std::string& exe) const;

	//@}	// Les options de fonctionnement de l'application.

	/**
	 * La gestion de la configuration.
	 */
	//@{

	/**
	 * Applique la configuration reçue en argument.
	 * \param		Section racine de la configuration à appliquer.
	 */
	virtual void applyConfiguration (const Preferences::Section& mainSection);

	/**
	 * Affiche une boite de dialogue permettant d'éditer la configuration
	 * (préférences utilisateur) de l'application.
	 * \param		Widget parent de la boite de dialogue.
	 * \param		Titre de la boite de dialogue.
	 */
	static void editConfiguration (QWidget* parent, const TkUtil::UTF8String& title);

	/**
	 * \return		Les préférences utilisateur issues du moyen de stockage
	 *				(préférences par défaut surchargées des éventuelles
	 *				préférences personnelles). Données à détruire par
	 *				l'appelant.
	 * @exception	En cas d'erreur au chargement
	 * \see			setPersistantConfiguration
	 */
	static Preferences::Section* getPersistantConfiguration ( );

	/**
	 * \param		Préférences utilisateur enregistrées dans le fichier
	 * 				de préférences de l'utilisateur.
	 * \exception	En cas d'erreur lors de l'écriture
	 * \see			getPersistantConfiguration
	 */
	static void setPersistantConfiguration (const Preferences::Section& mainSection);

	/**
	 * Actualise la configuration transmise en premier argument avec l'élément
	 * de configuration transmis en second argument.
	 * \exception	Une exception est levée en cas d'erreur.
	 */
	static void updateConfiguration (Preferences::Section& section, Preferences::NamedValue& value);

	/**
	 * Ajoute à la section tranmise en argument la configuration actuelle.
	 * \param		Section principale de la configuration de l'application.
	 * \warning		N'effectue aucune action persistante.
	 * \see			setPersistantConfiguration
	 */
	virtual void saveConfiguration (Preferences::Section& mainSection);

	/**
	 * \return		Un entier identifiant la police de caractères transmise en
	 *				argument.
	 * \warning		Retourne 0 par défaut, méthode à surcharger.
	 */
	virtual unsigned short fontNameToInt (const std::string& name);

	/**
	 * \return		Le masque de logs à appliquer.
	 */
	static TkUtil::Log::TYPE getLogsMask ( );


	/**
	 * \return		Le numéro de la dernière version utilisée par l'utilisateur.
	 */
	static TkUtil::Version getLastVersion ( );

	/**
	 *	Enregistre le numéro de la dernière version utilisée par l'utilisateur dans le fichier.
	 */
	static void setLastVersion (const TkUtil::Version& version);

	/**
	 * \return		Le numéro de la version actuelle.
	 */
	static TkUtil::Version getVersion ( );


	//@}	// La gestion de la configuration.


	/**
	 * Le système d'aide contextuelle.
	 */
	//@{

	class HelpSystem
	{
		public :

		/** Le manuel utilisateur en entier (format PDF, EPS, ...). */
		std::string					userManual;

		/** L'outil de visualisation du manuel utilisateur. */
		std::string					userManualViewer;

		/** L'URL racine du système d'aide. Nulle par défaut.*/
		std::string					rootURL;

		/** L'index de l'aide (manuel utilisateur) (chemin relatif). */
		std::string					indexURL;

		/** L'index de l'historique (chemin relatif). */
		std::string					historiqueURL;

		/** L'URL du Wiki Magix 3D. */
		std::string					wikiURL;
		
		/** L'URL du Tutorial Magix 3D. */
		std::string					tutorialURL;

		/** L'URL de l'API Python Magix 3D. Nulle par défaut.*/
		std::string					pythonAPIURL;

	 	/** Le manuel utilisateur et l'étude bibliographique de Qualif. */
		std::string					qualifURL;

		/** L'aide pour tout ce qui est raccourcis clavier */
		std::string       			shortKeyURL,
									shortKeyTag;

		/** L'aide pour tout ce qui est sélection d'entités */
		std::string       			selectURL,
									selectTag;

		/** La page web du panneau d'édition des paramètres de mesure de
		 * distance, et sa balise dans la page. */
		std::string       			distanceMeasurementURL,
									distanceMeasurementTag;

		/** La page web du panneau d'édition des paramètres de mesure
		 * d'angle, et sa balise dans la page. */
		std::string       			angleMeasurementURL,
									angleMeasurementTag;

		/** La page web du panneau de mesure des bras aux extrémités d'une arête. */
		std::string       			extremaMeshingEdgeLengthURL,
		                            extremaMeshingEdgeLengthTag;

		/** La page web du panneau d'information sur le maillage. */
		std::string       			meshInformationURL,
									meshInformationTag;

		/** La page web du panneau d'information sur la topologie. */
		std::string       			topoInformationURL,
									topoInformationTag;

		/** La page web du panneau d'édition des paramètres d'exploration de
		 *  de maillages le long de feuillets, et sa balise dans la page. */
		std::string       			sheetExplorationURL, sheetExplorationTag;

		/** La page web du panneau d'édition des paramètres des opérations et
		  * sa balise dans la page. */
		std::string       			operationsURL, operationsTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  vertex, et sa balise dans la page. */
		std::string       			vertexOperationURL, vertexOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  segment, et sa balise dans la page. */
		std::string       			segmentOperationURL, segmentOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  l'arc de cercle, et sa balise dans la page. */
		std::string       			arcCircleOperationURL, arcCircleOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  le cercle, et sa balise dans la page. */
		std::string       			circleOperationURL, circleOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  d'une courbe par projection d'une courbe sur une surface, et sa
		 *  balise dans la page. */
		std::string       			curveByProjectionOperationURL,
									curveByProjectionOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  boite, et sa balise dans la page. */
		std::string       			boxOperationURL, boxOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  sphère, et sa balise dans la page. */
		std::string       			sphereOperationURL, sphereOperationTag;

		/** La page web du panneau d'édition des paramètres de création d'une
		 *  aiguille, et sa balise dans la page. */
		std::string       			aiguilleOperationURL, aiguilleOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  cylindre, et sa balise dans la page. */
		std::string       			cylinderOperationURL, cylinderOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  cone, et sa balise dans la page. */
		std::string       			coneOperationURL, coneOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  prisme, et sa balise dans la page. */
		std::string       			prismOperationURL, prismOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  surface, et sa balise dans la page. */
		std::string       			surfaceCreationURL, surfaceCreationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  volume, et sa balise dans la page. */
		std::string       			volumeCreationURL, volumeCreationTag;

		/** La page web du panneau d'édition des paramètres de création 
		 *  d'entité par révolution, et sa balise dans la page. */
		std::string       			geomByRevolutionOperationURL,
									geomByRevolutionOperationTag;

		/** La page web des panneaux d'édition des paramètres de copie,
		 * copie avec décallage, destruction d'entités géométriques, et leurs balises dans ces pages.
		 * */
		std::string       			geomCopyOperationURL,
									geomCopyOperationTag,
									geomCopyByOffsetURL,
									geomCopyByOffsetTag,
									geomDestructionOperationURL,
									geomDestructionOperationTag;

		/** La page web des panneaux d'édition des paramètres de translation, 
		 * rotation, homothétie d'entités géométriques, et leurs balises dans ces pages. */
		std::string       			geomTranslationOperationURL,
									geomTranslationOperationTag,
									geomRotationOperationURL,
									geomRotationOperationTag,
									geomHomothetyOperationURL,
									geomHomothetyOperationTag,
									geomMirrorOperationURL,
									geomMirrorOperationTag,
									geomGlueCurvesOperationURL,
									geomGlueCurvesOperationTag,
									geomGlueSurfacesOperationURL,
									geomGlueSurfacesOperationTag;

		/** La page web du panneau d'édition des paramètres de coupe par un
		 * plan d'entités géométriques. */
		std::string       			geomPlaneCutOperationURL, geomPlaneCutOperationTag;

		/** La page web du panneau d'union booléenne de volumes  */
		std::string       			fuseOperationURL, fuseOperationTag;

		/** La page web du panneau d'intersection de 2 surfaces et celle pour 2 courbes  */
		std::string       			geomCommon2DOperationURL,
		                            geomCommon2DOperationSurfacesTag, geomCommon2DOperationCourbesTag;

		/** La page web du panneau de modification des groupes  */
		std::string       			geomAddToGroupOperationURL, geomAddToGroupOperationTag;


		/** La page web du panneau d'édition des paramètres de création de
		 * sommet à partir d'un point, et sa balise dans la page. */
		std::string       			vertexCreationURL, vertexCreationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 * arête à partir d'une courbe, et sa balise dans la page. */
		std::string       			edgeCreationURL, edgeCreationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 * face à partir d'une surface, et sa balise dans la page. */
		std::string       			faceCreationURL, faceCreationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 * bloc à partir d'un volume, et sa balise dans la page. */
		std::string       			blockCreationURL, blockCreationTag;

		/** La page web du panneau d'édition des paramètres de positionnement 
		 * d'un sommet topologique, et sa balise dans la page. */
		std::string       			placeVertexOperationURL,
									placeVertexOperationTag;

		/** La page web du panneau d'édition des paramètres de projection de
		 * sommets topologiques, et sa balise dans la page. */
		std::string       			projectVerticesOperationURL,
									projectVerticesOperationTag;

		/** La page web du panneau d'édition des paramètres de projection de
		 * sommets topologiques, et sa balise dans la page. */
		std::string       			alignVerticesOperationURL,
									alignVerticesOperationTag;

		/** La page web du panneau d'édition des paramètres de projection de
		 * sommets topologiques sur leur géométrie associée, et sa balise dans
		 * la page. */
		std::string       			snapVerticesOperationURL,
									snapVerticesOperationTag;

		/** La page web du panneau d'édition des paramètres d'association
		 * entre entités topologiques vers une géométrie associée, et sa balise dans
		 * la page. */
		std::string       			associateOperationURL,
		                            associateOperationTag;

		/** La page web du panneau d'édition des paramètres de collage de 2 
		 * vertices topologiques (dégénérescence de blocs), et sa balise dans la page. */
		std::string       			degenerateBlockOperationURL,
									degenerateBlockOperationTag;

		/** La page web du panneau d'édition des paramètres de collage de 2
		 * vertices topologiques, et sa balise dans la page. */
		std::string       			fuse2verticesOperationURL,
									fuse2verticesOperationTag;

		/** La page web du panneau d'édition des paramètres de collage d'arêtes 
		 * topologiques, et sa balise dans la page. */
		std::string       			fuseEdgesOperationURL,
									fuseEdgesOperationTag;

		/** La page web du panneau d'édition des paramètres de fusion de 2 arêtes
		 * topologiques, et sa balise dans la page. */
		std::string       			fuse2EdgesOperationURL,
									fuse2EdgesOperationTag;

		/** La page web du panneau d'édition des paramètres de collage de 2 
		 * faces topologiques, et sa balise dans la page. */
		std::string       			fuse2facesOperationURL,
									fuse2facesOperationTag;

		/** La page web du panneau d'édition des paramètres de fusion de 2 
		 * blocs topologiques, et sa balise dans la page. */
		std::string       			fuse2blocksOperationURL,
									fuse2blocksOperationTag;

		/** La page web du panneau d'édition des paramètres de collage de 2 
		 * blocs topologiques, et sa balise dans la page. */
		std::string       			glue2blocksOperationURL,
									glue2blocksOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 * bloc par révolution d'une arête, et sa balise dans la page. */
		std::string       			blocksByRevolutionCreationURL,
									blocksByRevolutionCreationTag;

		/** La page web du panneau de copie d'entités topologiques
		 * et sa balise dans la page. */
		std::string       			topoCopyOperationURL, topoCopyOperationTag;

		/** La page web du panneau d'extraction de blocs topologiques
		 * et sa balise dans la page. */
		std::string       			topoBlockExtractionOperationURL,
									topoBlockExtractionOperationTag;

		/** La page web du panneau de modification des groupes  */
		std::string       			topoAddToGroupOperationURL,
									topoAddToGroupOperationTag;

		/** La page web du panneau d'édition des paramètres de découpe d'une
		 * face topologique, et sa balise dans la page. */
		std::string       			splitFaceOperationURL,
									splitFaceOperationTag;

		/** La page web du panneau d'édition des paramètres de découpe de
		 * faces topologiques, et sa balise dans la page. */
		std::string       			splitFacesOperationURL,
									splitFacesOperationTag;

		/** La page web du panneau d'édition des paramètres de découpe de
		 * faces topologiques avec o-grid, et sa balise dans la page. */
		std::string       			splitFaceWithOGridOperationURL,
									splitFaceWithOGridOperationTag;

		/** La page web du panneau de prolongation du découpage de
		 * faces topologiques, et sa balise dans la page. */
		std::string       			extendSplitFacesOperationURL,
		                            extendSplitFacesOperationTag;

		/** La page web du panneau d'édition des paramètres de découpe de
		 * bloc topologique, et sa balise dans la page. */
		std::string       			splitBlockOperationURL,
									splitBlockOperationTag;

		/** La page web du panneau d'édition des paramètres de découpe de
		 * bloc topologique avec o-grid, et sa balise dans la page. */
		std::string       			splitBlockWithOGridOperationURL,
									splitBlockWithOGridOperationTag;

		/** La page web du panneau d'édition des paramètres de prolongation de
		 * découpe de bloc topologique, et sa balise dans la page. */
		std::string       			extendSplitBlockOperationURL,
									extendSplitBlockOperationTag;

		/** La page web du panneau d'un trou dans la topologie, et sa balise dans la page. */
		std::string       			insertHoleOperationURL,
		                            insertHoleOperationTag;

		/** La page web du panneau d'édition des paramètres de destruction
		 * d'entités topologique, et sa balise dans la page. */
		std::string       			topoDestructionOperationURL,
		                            topoDestructionOperationTag;

		/** La page web des panneaux d'édition des paramètres de translation, 
		 * rotation, homothétie d'entités topologiques, et leurs balises dans ces pages. */
		std::string       			topoTranslationOperationURL,
									topoTranslationOperationTag,
									topoRotationOperationURL,
									topoRotationOperationTag,
									topoHomothetyOperationURL,
									topoHomothetyOperationTag,
									topoMirrorOperationURL,
									topoMirrorOperationTag;

		 /** La page web du panneau de paramétrage de la découpe d'une arête,
		  * et sa balise dans la page. */
		std::string       			edgeCutOperationURL, edgeCutOperationTag;

		 /** La page web du panneau de paramétrage de l'orientation d'une arête,
		  * et sa balise dans la page. */
		std::string       			edgeDirectionOperationURL,
									edgeDirectionOperationTag;

		 /** La page web du panneau de paramétrage de la discrétisation
		  * d'arêtes, et sa balise dans la page. */
		std::string       			edgesRefinementOperationURL, edgesRefinementOperationTag;

		 /** La page web du panneau de paramétrage de la discrétisation
		  * de faces, et sa balise dans la page. */
		std::string       			facesMeshingPropertyOperationURL,
		                            facesMeshingPropertyOperationTag;

		/** La page web du panneau de paramétrage de la discrétisation
		  * de blocs, et sa balise dans la page. */
		std::string       			blocsMeshingPropertyOperationURL,
		                            blocsMeshingPropertyOperationTag;

		/** La page web du panneau d'édition des paramètres de maillage des
		 *  blocs et surfaces, et leurs balises dans la page. */
		std::string       			meshAllSurfacesOperationURL,
									meshAllSurfacesOperationTag,
									meshAllBlocksOperationURL,
									meshAllBlocksOperationTag,
									meshSheetsVolumeOperationURL,
									meshSheetsVolumeOperationTag;

		/** La page web du panneau de projection de surfaces sur un plan. */
		std::string       			groupProjectionOperationURL,
									groupProjectionOperationTag;

		/** La page web du panneau d'édition de lissage d'une surface,
		 *  et sa balise dans la page. */
		std::string       			meshSmoothSurfaceOperationURL,
									meshSmoothSurfaceOperationTag;

		/** La page web du panneau d'édition de lissage d'un volume,
		 *  et sa balise dans la page. */
		std::string       			meshSmoothVolumeOperationURL,
									meshSmoothVolumeOperationTag;

		/** La page web du panneau d'édition de déraffinement de blocs
		 * topologiques , et sa balise dans la page. */
		std::string       			unrefineBlockOperationURL,
									unrefineBlockOperationTag;

		/** La page web du panneau d'édition des paramètres de création de
		 *  feuillets, et sa balise dans la page. */
		std::string       			sheetOperationURL, sheetOperationTag;

		/** La page web du panneau d'édition des paramètres d'opérations de
		 *  qualité de maillage, et sa balise dans la page. */
		std::string       			meshQualityOperationURL, meshQualityOperationTag;

		/** La page web du panneau de modification de la représentation des entités
		 */
		std::string       			representationURL, representationTag;

		/** La page weg du panneau moniteur de commandes
		 */
		std::string       			monitorPanelURL, monitorPanelTag;

		/** Les pages web des panneaux pour les pointages laser */
		std::string       			rayURL, rayTag, rayIOsTag, rayBoundingBoxTag,
		 	 	 	 	 	 	 	raysParametersTag, raysGraphicalParametersTag,
									raysTransform;

		/** Les pages web des panneaux pour les opérations sur les repères */
		std::string       			syscoordOperationURL, syscoordCreationTag,
									syscoordRotationTag, syscoordTranslationTag;

		static HelpSystem& instance ( );
		
		
		private :

		/** Constructeurs et destructeur interdits. */
		HelpSystem ( );
		HelpSystem (const HelpSystem&);
		HelpSystem& operator = (const HelpSystem&);
		~HelpSystem ( );
		static HelpSystem*		_instance;
	};	// class HelpSystem

	//@}	// Le système d'aide contextuelle.


	private :

	// Opérations interdites :
#ifdef QT_4
	QtMgx3DApplication (const QtMgx3DApplication&);
#endif	// QT_4
	QtMgx3DApplication& operator = (const QtMgx3DApplication&);

	static QtMgx3DApplication*				_instance;
	/** argc, argv etenvp du main. */
	static int								_argc;
	static char								**_argv, **_envp;
};	// class QtMgx3DApplication


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_APPLICATION_H
