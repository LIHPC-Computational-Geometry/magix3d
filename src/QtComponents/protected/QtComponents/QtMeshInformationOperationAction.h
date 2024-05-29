/**
 * \file		QtMeshInformationOperationAction.h
 * \author		Eric Brière de l'Isle
 * \date		4/3/2016
 */
#ifndef QT_MESH_INFORMATION_OPERATION_ACTION_H
#define QT_MESH_INFORMATION_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "Utils/Entity.h"
#include "Geom/GeomEntity.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"

#include <TkUtil/Mutex.h>

#include <QLabel>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'information sur le maillage.
 */
class QtMeshInformationOperationPanel :
							public Mgx3D::QtComponents::QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/**
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 * \param	Fichier d'aide contextuelle associé à ce panneau.
	 * \param	Balise dans le fichier d'aide contextuelle associé à ce panneau
	 *			identifiant le paragraphe sur ce panneau.
	 */
	QtMeshInformationOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtMeshInformationOperationPanel ( );

	/**
	 * Ne fait pas QtMgx3DOperationPanel::setVisible qui est invoque cancel
	 * ou autoUpdate (API opérations). Ici on reste à l'aspect visible ou non
	 * de la fenêtre et de son contenu.
	 * Cette surcharge est due au fait que dans un système d'onglets, quand on
	 * affiche un onglet celui qui était au premier plan devient non visible ...
	 * => appel de cancel, clear, ...
	 */
	virtual void setVisible (bool visible);

	/**
	 * Si une commande a lieu, on met à jour.
	 */
	virtual void commandModifiedCallback (Mgx3D::Internal::InfoCommand&);

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );


	protected :



	protected slots :

	/**
	 * Ferme cette fenêtre.
	 */
	virtual void closeCallback ( );

	/**
	 * Affiche l'aide associée au panneau.
	 */
	virtual void helpCallback ( );

	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtMeshInformationOperationPanel (
								const QtMeshInformationOperationPanel&);
	QtMeshInformationOperationPanel& operator = (
								const QtMeshInformationOperationPanel&);

	/** Le nombre de blocs. */
	QLabel*									_nbBlocksLabel;
	/** Le nombre de blocs maillés. */
	QLabel*									_nbMeshedBlocksLabel;
	/** Le nombre de polyèdres créés. */
	QLabel*									_nbCreatedRegionLabel;
	/** Le nombre de polyèdres prévisibles. */
	QLabel*									_nbProvideRegionLabel;

	/** Le nombre de cofaces. */
	QLabel*									_nbFacesLabel;
	/** Le nombre de cofaces maillées. */
	QLabel*									_nbMeshedFacesLabel;
	/** Le nombre de polygones créés. */
	QLabel*									_nbCreatedFaceLabel;
	/** Le nombre de polygones prévisibles. */
	QLabel*									_nbProvideFaceLabel;

	/** Le nom du fichier contenant l'aide contextuelle associée à ce panneau
	 * (chemin relatif par rapport à la racine de l'aide contextuelle du
	 * logiciel).
	 */
	std::string										_helpURL;

	/** La balise identifiant le paragraphe d'aide contextuelle associée à ce
	 * panneau dans le fichier d'aide contextuelle référencé par <I>_helpURL</I>
	 * .*/
	std::string										_helpTag;

};	// class QtMeshInformationOperationPanel


/**
 * Classe d'action associée à un panneau type
 * <I>QtMeshInformationOperationPanel</I> d'informations sur le maillage
 */
class QtMeshInformationOperationAction :
					public Mgx3D::QtComponents::QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtMeshInformationOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 */
	QtMeshInformationOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMeshInformationOperationAction ( );

	/**
	 * \return		Le panneau d'information
	 */
	virtual QtMeshInformationOperationPanel* getMeshInformationPanel ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMeshInformationOperationAction (
								const QtMeshInformationOperationAction&);
	QtMeshInformationOperationAction& operator = (
								const QtMeshInformationOperationAction&);

	/** Un mutex non récursif pour empêcher toute réentrance.
	 * cf. executeOperation pour davantage d'explications. */
	TkUtil::Mutex							_mutex;
};  // class QtMeshInformationOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MESH_INFORMATION_OPERATION_ACTION_H
