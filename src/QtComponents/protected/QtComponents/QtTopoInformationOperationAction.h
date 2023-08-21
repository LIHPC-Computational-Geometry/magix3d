/**
 * \file		QtTopoInformationOperationAction.h
 * \author		Eric Brière de l'Isle
 * \date		25/5/2016
 */
#ifndef QT_TOPO_INFORMATION_OPERATION_ACTION_H
#define QT_TOPO_INFORMATION_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "Utils/Entity.h"
#include "QtComponents/QtMgx3DGeomOperationAction.h"

#include <TkUtil/Mutex.h>

#include <QLabel>

#include <memory>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'information sur la topologie.
 */
class QtTopoInformationOperationPanel :
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
	QtTopoInformationOperationPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action,
			const std::string& helpURL, const std::string& helpTag);

	/**
	 * RAS.
	 */
	virtual ~QtTopoInformationOperationPanel ( );

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
	QtTopoInformationOperationPanel (
								const QtTopoInformationOperationPanel&);
	QtTopoInformationOperationPanel& operator = (
								const QtTopoInformationOperationPanel&);

	/** Nombre de blocs. */
	QLabel*									_nbBlocksLabel;
	/** Nombre de blocs structurés suivant direction. */
	QLabel*									_nb_dom_str_dirLabel;
	/** Nombre de blocs structurés suivant rotation. */
	QLabel*									_nb_dom_str_rotLabel;
	/** Nombre de blocs structurés avec transfini. */
	QLabel*									_nb_dom_str_transLabel;
	/** Nombre de blocs non-structurés. */
	QLabel*									_nb_dom_unstrLabel;

	/** Nombre de mailles au total. */
	QLabel*									_nb_regions_totLabel;
	/** Nombre de mailles structurés suivant direction. */
	QLabel*									_nb_regions_str_dirLabel;
	/** Nombre de mailles structurés suivant rotation. */
	QLabel*									_nb_regions_str_rotLabel;
	/** Nombre de mailles structurés avec transfini. */
	QLabel*									_nb_regions_str_transLabel;
	/** Nombre de mailles non-structurés. */
	QLabel*									_nb_regions_unstrLabel;

	/** Nombre de faces communes. */
	QLabel*									_nb_cofacesLabel;
	/** Nombre de faces structurées suivant direction. */
	QLabel*									_nb_fac_str_dirLabel;
	/** Nombre de faces structurées suivant rotation. */
	QLabel*									_nb_fac_str_rotLabel;
	/** Nombre de faces structurées avec transfini. */
	QLabel*									_nb_fac_str_transLabel;
	/** Nombre de faces non-structurées. */
	QLabel*									_nb_fac_unstrLabel;


	/** Nombre de polygones au total. */
	QLabel*									_nb_faces_totLabel;
	/** Nombre de polygones structurés suivant direction. */
	QLabel*									_nb_faces_str_dirLabel;
	/** Nombre de polygones structurés suivant rotation. */
	QLabel*									_nb_faces_str_rotLabel;
	/** Nombre de polygones structurés avec transfini. */
	QLabel*									_nb_faces_str_transLabel;
	/** Nombre de polygones non-structurés. */
	QLabel*									_nb_faces_unstrLabel;

	/** Le nom du fichier contenant l'aide contextuelle associée à ce panneau
	 * (chemin relatif par rapport à la racine de l'aide contextuelle du
	 * logiciel).
	 */
	std::string										_helpURL;

	/** La balise identifiant le paragraphe d'aide contextuelle associée à ce
	 * panneau dans le fichier d'aide contextuelle référencé par <I>_helpURL</I>
	 * .*/
	std::string										_helpTag;

};	// class QtTopoInformationOperationPanel


/**
 * Classe d'action associée à un panneau type
 * <I>QtTopoInformationOperationPanel</I> d'informations sur le maillage
 */
class QtTopoInformationOperationAction :
					public Mgx3D::QtComponents::QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtTopoInformationOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte.
	 * \param		Tooltip décrivant l'action.
	 */
	QtTopoInformationOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtTopoInformationOperationAction ( );

	/**
	 * \return		Le panneau d'information
	 */
	virtual QtTopoInformationOperationPanel* getTopoInformationPanel ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtTopoInformationOperationAction (
								const QtTopoInformationOperationAction&);
	QtTopoInformationOperationAction& operator = (
								const QtTopoInformationOperationAction&);

	/** Un mutex non récursif pour empêcher toute réentrance.
	 * cf. executeOperation pour davantage d'explications. */
	TkUtil::Mutex							_mutex;
};  // class QtTopoInformationOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_TOPO_INFORMATION_OPERATION_ACTION_H
