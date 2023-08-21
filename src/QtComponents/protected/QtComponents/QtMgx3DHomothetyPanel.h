/**
 * \file		QtMgx3DHomothetyPanel.h
 * \author		Charles PIGNEROL
 * \date		05/05/2017
 */
#ifndef QT_MGX3D_HOMOTHETY_PANEL_H
#define QT_MGX3D_HOMOTHETY_PANEL_H

#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtMgx3DTopoOperationAction.h"

#include <QtUtil/QtDoubleTextField.h>


namespace Mgx3D 
{

namespace QtComponents
{

// ===========================================================================
//                   LA CLASSE QtMgx3DHomogeneousHomothetyPanel
// ===========================================================================

/**
 * Panneau permettant la saisie de paramètres définissant une homothétie 
 * homogène : son centre et son facteur d'agrandissement.
 */
class QtMgx3DHomogeneousHomothetyPanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Le nom de l'application, pour les éventuels messages
	 *				d'erreur.
	 * \param		Fenêtre principale associée, pour la saisie du centre.
	 */
	QtMgx3DHomogeneousHomothetyPanel (
					QWidget* parent, const std::string& appTitle,
					Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DHomogeneousHomothetyPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de
     * l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
   	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * \return		Le rapport d'homothétie.
	 * \see			getTopoEntitiesNames
	 * \see			getCenter
	 */
	virtual double getHomothetyFactor ( ) const;

	/**
	 * \see			Le centre de l'homothétie
	 * \see			getHomothetyFactor
	 */
	virtual Mgx3D::Utils::Math::Point getCenter ( ) const;

	/**
	 * \return		Le widget de saisie du rapport d'homothétie.
	 */
	virtual QtDoubleTextField& getFactorTextField ( );

	/**
	 * \return		Le widget de saisie du centre de l'homothétie.
	 */
	virtual QtMgx3DPointPanel& getCenterPanel ( );


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */	
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;


	private :

	/**
	 * Constructeur de copie, opérateur =. Interdits.
	 */
	QtMgx3DHomogeneousHomothetyPanel (const QtMgx3DHomogeneousHomothetyPanel&);
	QtMgx3DHomogeneousHomothetyPanel& operator = (
									const QtMgx3DHomogeneousHomothetyPanel&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Le rapport d'homothétie. */
	QtDoubleTextField*								_homothetyFactorTextField;

	/** Le centre de l'homothétie. */
	QtMgx3DPointPanel*								_centerPanel;
};	// class QtMgx3DHomogeneousHomothetyPanel



// ===========================================================================
//                   LA CLASSE QtMgx3DHeterogeneousHomothetyPanel
// ===========================================================================

/**
 * Panneau permettant la saisie de paramètres définissant une homothétie 
 * hétérogène : ses facteurs d'agrandissement selon les 3 axes.
 */
class QtMgx3DHeterogeneousHomothetyPanel : public QtMgx3DOperationsSubPanel
{
	public :

	/**
	 * \param		Widget parent
	 * \param		Le nom de l'application, pour les éventuels messages
	 *				d'erreur.
	 * \param		Fenêtre principale associée, pour la saisie du centre.
	 */
	QtMgx3DHeterogeneousHomothetyPanel (
					QWidget* parent, const std::string& appTitle,
					Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DHeterogeneousHomothetyPanel ( );

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );

    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de
     * l'opération.
     * Restitue l'environnement dans son état initial.
	 * \see		autoUpdate
   	 */
	virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * Quitte le mode sélection interactive.
	 */
	virtual void stopSelection ( );

	/**
	 * \return		Le rapport d'homothétie selon les différents axes.
	 * \see			getTopoEntitiesNames
	 */
	virtual double getHomothetyXFactor ( ) const;
	virtual double getHomothetyYFactor ( ) const;
	virtual double getHomothetyZFactor ( ) const;

	/**
	 * \return		Les widgets de saisie des rapports d'homothétie selon les
	 * 				différents axes.
	 */
	virtual QtDoubleTextField& getXFactorTextField ( );
	virtual QtDoubleTextField& getYFactorTextField ( );
	virtual QtDoubleTextField& getZFactorTextField ( );


	protected :

	/**
	 * \return	La liste des entités impliquées dans l'opération.
	 */	
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * \return	Une référence sur le contexte <I>Magix 3D</I> associé à
	 *			l'opération.
	 */
	virtual const Mgx3D::Internal::ContextIfc& getContext ( ) const;


	private :

	/**
	 * Constructeur de copie, opérateur =. Interdits.
	 */
	QtMgx3DHeterogeneousHomothetyPanel (
									const QtMgx3DHeterogeneousHomothetyPanel&);
	QtMgx3DHeterogeneousHomothetyPanel& operator = (
									const QtMgx3DHeterogeneousHomothetyPanel&);

	/** Le contexte <I>Magix 3D</I> de l'opération. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Le rapport d'homothétie. */
	QtDoubleTextField								*_homothetyXFactorTextField,
													*_homothetyYFactorTextField,
													*_homothetyZFactorTextField;
};	// class QtMgx3DHeterogeneousHomothetyPanel

}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_HOMOTHETY_PANEL_H
