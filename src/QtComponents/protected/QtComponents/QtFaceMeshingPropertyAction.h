/**
 * \file		QtFaceMeshingPropertyAction.h
 * \author		Charles PIGNEROL
 * \date		05/11/2013
 */
#ifndef QT_FACE_MESHING_PROPERTY_ACTION_H
#define QT_FACE_MESHING_PROPERTY_ACTION_H

#include "Internal/Context.h"

#include "QtComponents/QtMgx3DEntityPanel.h"
#include "QtComponents/QtMgx3DEdgePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"
#include "QtComponents/RenderingManager.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"
#include "Topo/FaceMeshingPropertyDelaunayGMSH.h"
#include "Topo/FaceMeshingPropertyQuadPairing.h"

#include <QtUtil/QtIntTextField.h>

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>


namespace Mgx3D 
{

namespace QtComponents
{

/**
 * Le paramétrage d'un maillage transfini.
 */
class QtFaceTransfinitePanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtFaceTransfinitePanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtFaceTransfinitePanel ( );

	/**
	 * \return		La propriété de maillage de la face conforme au panneau.
	 */
	virtual Mgx3D::Topo::FaceMeshingPropertyTransfinite*
											getMeshingProperty ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtFaceTransfinitePanel (const QtFaceTransfinitePanel&);
	QtFaceTransfinitePanel& operator = (const QtFaceTransfinitePanel&);
};	// class QtFaceTransfinitePanel


/**
 * Le paramétrage d'un maillage Delaunay via <I>GMSH</I>.
 */
class QtFaceDelaunayGMSHPanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
	 *				utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
	 */
	QtFaceDelaunayGMSHPanel (
				QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtFaceDelaunayGMSHPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

	/**
	 * \return		La propriété de maillage de la face conforme au panneau.
	 */
	virtual Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH*
											getMeshingProperty ( ) const;

	/**
	 * \return		Le paramètre <I>min</I> pour <I>GMSH</I>.
	 * \see			getMax
	 */
	virtual double getMin ( ) const;

	/**
	 * \return		Le paramètre <I>max</I> pour <I>GMSH</I>.
	 * \see			getMin
	 */
	virtual double getMax ( ) const;


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtFaceDelaunayGMSHPanel (const QtFaceDelaunayGMSHPanel&);
	QtFaceDelaunayGMSHPanel& operator = (const QtFaceDelaunayGMSHPanel&);

	/** Les valeurs min et max pour <I>GMSH</I>. */
	QtDoubleTextField			*_minTextField, *_maxTextField;
};	// class QtFaceDelaunayGMSHPanel


/**
 * Le paramétrage d'un maillage surfacique par appariement de triangles.
 */
class QtFaceQuadPairingPanel : public QtMgx3DOperationsSubPanel
{
    public :

    /**
     * \param       Widget parent
     * \param       Fenêtre principale <I>Magix 3D</I> de rattachement,
     *              utilisée notamment pour récupérer le contexte.
	 * \param		Eventuel panneau de rattachement.
     */
    QtFaceQuadPairingPanel (
                QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow& mw,
				QtMgx3DOperationPanel* mainPanel);

    /**
     * Destructeur. RAS.
     */
    virtual ~QtFaceQuadPairingPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * \return      La propriété de maillage de la face conforme au panneau.
     */
    virtual Mgx3D::Topo::FaceMeshingPropertyQuadPairing*
                                            getMeshingProperty ( ) const;

    /**
     * \return      Le paramètre <I>min</I> pour <I>GMSH</I>.
     * \see         getMax
     */
    virtual double getMin ( ) const;

    /**
     * \return      Le paramètre <I>max</I> pour <I>GMSH</I>.
     * \see         getMin
     */
    virtual double getMax ( ) const;


    private :

    /**
     * Constructeur de copie et opérateur = : interdits.
     */
    QtFaceQuadPairingPanel (const QtFaceQuadPairingPanel&);
    QtFaceQuadPairingPanel& operator = (const QtFaceQuadPairingPanel&);

    /** Les valeurs min et max pour <I>GMSH</I>. */
    QtDoubleTextField           *_minTextField, *_maxTextField;
};  // class QtFaceQuadPairingPanel



/**
 * Panneau d'édition des paramètres de maillage de faces topologiques.
 */
class QtFaceMeshingPropertyPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de maillage de la face.
	 * <OL>
	 * <LI>Maillage structuré transfini,
	 * <LI>Maillage non structuré Delaunay généré par <I>GMSH</I>
	 * </OL>
	 */
	enum OPERATION_METHOD { TRANSFINITE, DELAUNAY_GMSH };

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtFaceMeshingPropertyPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtFaceMeshingPropertyPanel ( );

	/**
	 * \return		La méthode de maillage de la face.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * \return		La propriété de maillage de la face.
	 * \warning		C'est à l'appelant de détruire l'instance retournée.
	 */
	virtual Mgx3D::Topo::CoFaceMeshingProperty* getMeshingProperty ( ) const;

	/**
	 * \return		La liste des faces sélectionnées.
	 */
	virtual std::vector<std::string> getFacesNames ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée pour vérifier les paramètres saisis par l'utilisateur.
     * En cas de paramètrage invalide cette méthode doit leve une exception de
     * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
     * rencontrées.
     */
    virtual void validate ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
     * Invoque <I>preview (false)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour prévisualiser le paramétrage surfacique du
	 *				maillage, <I>false</I> pour arrêter la prévisualisation.
	 * \param		inutilisé
	 */
	virtual void preview (bool show, bool destroyInteractor);


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque la méthode change. Actualise le panneau de saisie des
	 * paramètres.
	 */
	virtual void operationMethodCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = opérations interdites.
	 */
	QtFaceMeshingPropertyPanel (const QtFaceMeshingPropertyPanel&);
	QtFaceMeshingPropertyPanel& operator = (const QtFaceMeshingPropertyPanel&);

	/** La méthode de maillage de la face. */
	QComboBox*								_operationMethodComboBox;

	/**
	 * Les panneaux de définition de la discrétisation des faces.
	 */
	//{@

	/** Parent du panneau courant. */
	QWidget*									_currentParentWidget;

	/** Panneau courant. */
	QtMgx3DOperationsSubPanel*					_currentPanel;

	/** Maillage transfini. */
	QtFaceTransfinitePanel*						_transfinitePanel;

	/** Maillage non structuré Delaunay généré avec <I>GMSH</I>. */
	QtFaceDelaunayGMSHPanel*					_delaunayGMSHPanel;

    /** Maillage non structuré par appariement de quads. */
    QtFaceQuadPairingPanel*                  	_quadPairingPanel;

	//@}	// Panneaux de saisie des paramètres de définition du vertex

	/** Les faces à discrétiser. */
	QtMgx3DEntityPanel*							_facesPanel;
};	// class QtFaceMeshingPropertyPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtFaceMeshingPropertyPanel</I> de modification des paramètres de maillage
 * d'une face.
 */
class QtFaceMeshingPropertyAction : public QtMgx3DTopoOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtFaceMeshingPropertyPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtFaceMeshingPropertyAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtFaceMeshingPropertyAction ( );

	/**
	 * \return		Le panneau d'édition de discrétisation de faces associé.
	 */
	virtual QtFaceMeshingPropertyPanel* getMeshingPropertyPanel ( );

	/**
	 * Actualise la discrétisation des faces avec le paramétrage de son
	 * panneau associé. Invoque préalablement
	 * <I>QtMgx3DTopoOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtFaceMeshingPropertyAction (const QtFaceMeshingPropertyAction&);
	QtFaceMeshingPropertyAction& operator = (
									const QtFaceMeshingPropertyAction&);
};  // class QtFaceMeshingPropertyAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_FACE_MESHING_PROPERTY_ACTION_H
