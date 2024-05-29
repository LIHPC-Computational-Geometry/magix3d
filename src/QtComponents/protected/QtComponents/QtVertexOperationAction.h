/**
 * \file		QtVertexOperationAction.h
 * \author		Charles PIGNEROL
 * \date		09/09/2013
 */
#ifndef QT_VERTEX_OPERATION_ACTION_H
#define QT_VERTEX_OPERATION_ACTION_H

#include "Internal/ContextIfc.h"

#include "QtComponents/QtMgx3DGeomOperationAction.h"
#include "QtComponents/QtMgx3DGroupNamePanel.h"
#include "QtComponents/QtMgx3DPointPanel.h"
#include "QtComponents/QtCurvilinearAbscissaPanel.h"

#include <QtUtil/Qt3DDataPanel.h>
#include <QtUtil/QtDoubleTextField.h>

#include <QComboBox>


namespace Mgx3D 
{

namespace QtComponents
{


/**
 * Panneau d'édition des paramètres d'une opération géométrique touchant
 * un vertex.
 */
class QtVertexOperationPanel : public QtMgx3DOperationPanel
{
	Q_OBJECT

	public :

	/** La méthode de création/modification du vertex.
	 * <OL>
	 * <LI>Renseignement des coordonnées,
	 * <LI>Projection d'un point sur une courbe,
	 * <LI>Projection d'un point sur une surface,
	 * <LI>Point sur une courbe renseigné par son abscisse curvigne
	 * </OL>
	 * \warning		Dans le cas de la projection du point sur la courbe, la
	 * 				nature de la projection n'est pas complétement établie
	 * 				à ce jour (09/09/13) : a priori orthogonale à la courbe,
	 * 				et point le plus près retenu.
	 */
	enum OPERATION_METHOD {
			COORDINATES, VERTEX_PROJECTED_ON_CURVE, VERTEX_PROJECTED_ON_SURFACE, CURVILINEAR_ABSCISSA, RATE_WITH_2_POINTS};

	/**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Politique de création/modification du groupe
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtVertexOperationPanel (
			QWidget* parent, const std::string& panelName,
			QtMgx3DGroupNamePanel::POLICY creationPolicy,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtVertexOperationPanel ( );

	/**
	 * \return		Le nom du groupe créé/modifié.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La méthode de création/modification du vertex.
	 */
	virtual OPERATION_METHOD getOperationMethod ( ) const;

	/**
	 * Les coordonnées du vertex (si <I>getOperationMethod ( )</I>
	 * retourne <I>COORDINATES</I>).
	 */
	virtual Mgx3D::Utils::Math::Point getPoint ( ) const;

	/**
	 * Le nom du premier point (si <I>getOperationMethod ( )</I>
	 * retourne <I>RATE_WITH_2_POINTS</I>).
	 */
	virtual std::string getFirstPointName ( ) const;

	/**
	 * Le nom du second point (si <I>getOperationMethod ( )</I>
	 * retourne <I>RATE_WITH_2_POINTS</I>).
	 */
	virtual std::string getSecondPointName ( ) const;

	/**
	 * Le nom de la courbe (si <I>getOperationMethod ( )</I>
	 * retourne <I>VERTEX_PROJECTED_ON_CURVE</I> ou
	 * <I>CURVILINEAR_ABSCISSA</I>).
	 */
	virtual std::string getCurveName ( ) const;

	/**
	 * Le nom de la surface (si <I>getOperationMethod ( )</I>
	 * retourne <I>VERTEX_PROJECTED_ON_SURFACE</I> ).
	 */
	virtual std::string getSurfaceName ( ) const;

	/**
	 * Le nom du vertex à projeter (si <I>getOperationMethod ( )</I>
	 * retourne <I>VERTEX_PROJECTED_ON_CURVE</I>.
	 */
	virtual std::string getProjectedVertexName ( ) const;

	/**
	 * \return		L'abscisse curviligne du vertex (
	 * si <I>getOperationMethod ( )</I> retourne <I>CURVILINEAR_ABSCISSA</I>).
	 */
	virtual double getCurvilinearAbscissa ( ) const;

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
	 * \see		autoUpdate
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * \see		cancel
	 */
	virtual void autoUpdate ( );

	/**
	 * \param		<I>true</I> pour prévisualiser les entités créées,
	 *              <I>false</I> pour arrêter la prévisualisation.
	 * \param		<I>true</I> s'il faut détruire l'éventuel interacteur dans
	 * 				le cas d'un arrêt de la prévisualisation.
	 */
	virtual void preview (bool show, bool destroyInteractor);


	protected :

	class QtVertexCurveProjectionPanel : public QtMgx3DOperationsSubPanel
	{
		public :

		/**
		 * \param		Widget parent
		 * \param		Le nom de l'application, pour les éventuels messages
		 *				d'erreur.
		 * \param		Contexte d'utilisation du panneau.
		 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
		 *				utilisée notamment pour récupérer le contexte.
		 */
		QtVertexCurveProjectionPanel (
								QWidget* parent, const std::string& appTitle,
								Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~QtVertexCurveProjectionPanel ( );

		/**
		 * Réinitialise le panneau.
		 */
		virtual void reset ( );

		/**
		 * Méthode appelée lorsque l'utilisateur suspend l'édition de
		 * l'opération.
		 * cf. spécifications de la classe de base.
		 */
		virtual void cancel ( );

		/**
		 * Actualise le panneau en fonction du contexte.
		 * cf. spécifications de la classe de base.
		 */
		virtual void autoUpdate ( );

		/**
		 * \return		Le point sélectionné à projeter.
		 */
		virtual std::string getVertexUniqueName ( ) const;

		/**
		 * \return		La courbe de projection sélectionnée.
		 */
		virtual std::string getCurveUniqueName ( ) const;

		/**
		 * Quitte le mode sélection interactive.
		 */
		 virtual void stopSelection ( );

		 /**
		  * \return		Le champ de saisie de la courbe.
		  */
		 virtual QtMgx3DEntityPanel& getCurvePanel ( );

		 /**
		  * \return		Le champ de saisie de du sommet.
		  */
		 virtual QtMgx3DEntityPanel& getPointPanel ( );

		protected :

		/**
		 * \return		La liste des entités impliquées dans l'opération.
		 */
		virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );


		private :

		/**
		 * Constructeur de copie, opérateur =. Interdits.
		 */
		QtVertexCurveProjectionPanel (const QtVertexCurveProjectionPanel&);
		QtVertexCurveProjectionPanel& operator = (
										const QtVertexCurveProjectionPanel&);

		/** Le point projeté. */
		QtMgx3DEntityPanel*				_pointPanel;

		/** La courbe sur laquelle est effectuée la projection. */
		QtMgx3DEntityPanel*				_curvePanel;
	};	// class QtVertexCurveProjectionPanel

	class QtVertexSurfaceProjectionPanel : public QtMgx3DOperationsSubPanel
	{
		public :

		/**
		 * \param		Widget parent
		 * \param		Le nom de l'application, pour les éventuels messages
		 *				d'erreur.
		 * \param		Contexte d'utilisation du panneau.
		 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
		 *				utilisée notamment pour récupérer le contexte.
		 */
		QtVertexSurfaceProjectionPanel (
								QWidget* parent, const std::string& appTitle,
								Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~QtVertexSurfaceProjectionPanel ( );

		/**
		 * Réinitialise le panneau.
		 */
		virtual void reset ( );

		/**
		 * Méthode appelée lorsque l'utilisateur suspend l'édition de
		 * l'opération.
		 * cf. spécifications de la classe de base.
		 */
		virtual void cancel ( );

		/**
		 * Actualise le panneau en fonction du contexte.
		 * cf. spécifications de la classe de base.
		 */
		virtual void autoUpdate ( );

		/**
		 * \return		Le point sélectionné à projeter.
		 */
		virtual std::string getVertexUniqueName ( ) const;

		/**
		 * \return		La surface de projection sélectionnée.
		 */
		virtual std::string getSurfaceUniqueName ( ) const;

		/**
		 * Quitte le mode sélection interactive.
		 */
		 virtual void stopSelection ( );

		 /**
		  * \return		Le champ de saisie de la surface.
		  */
		 virtual QtMgx3DEntityPanel& getSurfacePanel ( );

		 /**
		  * \return		Le champ de saisie de du sommet.
		  */
		 virtual QtMgx3DEntityPanel& getPointPanel ( );

		protected :

		/**
		 * \return		La liste des entités impliquées dans l'opération.
		 */
		virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );


		private :

		/**
		 * Constructeur de copie, opérateur =. Interdits.
		 */
		QtVertexSurfaceProjectionPanel (const QtVertexSurfaceProjectionPanel&);
		QtVertexSurfaceProjectionPanel& operator = (
										const QtVertexSurfaceProjectionPanel&);

		/** Le point projeté. */
		QtMgx3DEntityPanel*				_pointPanel;

		/** La surface sur laquelle est effectuée la projection. */
		QtMgx3DEntityPanel*				_surfacePanel;
	};	// class QtVertexSurfaceProjectionPanel

    class QtVerticesRatioPanel : public QtMgx3DOperationsSubPanel
    {
    public :

        /**
         * \param		Widget parent
         * \param		Le nom de l'application, pour les éventuels messages
         *				d'erreur.
         * \param		Contexte d'utilisation du panneau.
         * \param		Fenêtre principale <I>Magix 3D</I> de rattachement,
         *				utilisée notamment pour récupérer le contexte.
         */
        QtVerticesRatioPanel (
                QWidget* parent, const std::string& appTitle,
                Mgx3D::QtComponents::QtMgx3DMainWindow& mw);

        /**
         * Destructeur. RAS.
         */
        virtual ~QtVerticesRatioPanel ( );

        /**
         * Réinitialise le panneau.
         */
        virtual void reset ( );

        /**
         * Méthode appelée lorsque l'utilisateur suspend l'édition de
         * l'opération.
         * cf. spécifications de la classe de base.
         */
        virtual void cancel ( );

        /**
         * Actualise le panneau en fonction du contexte.
         * cf. spécifications de la classe de base.
         */
        virtual void autoUpdate ( );

        /**
         * \return		Le premier point sélectionné.
         */
        virtual std::string getFirstVertexUniqueName ( ) const;

        /**
         * \return		Le second point sélectionné.
         */
        virtual std::string getSecondVertexUniqueName ( ) const;

         /**
         * \return		Le ratio.
         */
        virtual double getRate ( ) const;


        /**
         * Quitte le mode sélection interactive.
         */
        virtual void stopSelection ( );

        /**
         * \return		Le champ de saisie du premier point.
         */
        virtual QtMgx3DEntityPanel& getFirstPointPanel ( );

        /**
         * \return		Le champ de saisie du second point.
         */
        virtual QtMgx3DEntityPanel& getSecondPointPanel ( );

        /**
         * \return		Le champ de saisie du ratio.
         */
        virtual QtDoubleTextField& getRatePanel ( );

    protected :

        /**
         * \return		La liste des entités impliquées dans l'opération.
         */
        virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );


    private :

        /**
         * Constructeur de copie, opérateur =. Interdits.
         */
        QtVerticesRatioPanel (const QtVerticesRatioPanel&);
        QtVerticesRatioPanel& operator = (
                const QtVerticesRatioPanel&);

        /** Le premier point sélectionné. */
        QtMgx3DEntityPanel*				_firstPointPanel;

        /** Le second point sélectionné. */
        QtMgx3DEntityPanel*				_secondPointPanel;

        /** Le ratio. */
        QtDoubleTextField*              _ratePanel;

    };	// class QtVerticesRatioPanel


protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération
	 * 				(<B>hors panneaux autonomes dans le highlighting</B>).
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
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
	QtVertexOperationPanel (const QtVertexOperationPanel&);
	QtVertexOperationPanel& operator = (const QtVertexOperationPanel&);

	/** Le nom de la boite. */
	QtMgx3DGroupNamePanel*			_namePanel;

	/** La méthode de création/modification du cylindre. */
	QComboBox*						_operationMethodComboBox;

	/**
	 * Les panneaux de saisie des paramètres de définition du vertex
	 */
	//{@
	/** Parent du panneau courant. */
	QWidget*						_currentParentWidget;

	/** Panneau courant. */
	QWidget*						_currentPanel;

	/** Coordonnées du vertex. */
	QtMgx3DPointPanel*				_pointPanel;

	/** Projection d'un point sur une courbe. */
	QtVertexCurveProjectionPanel*	_vertexCurveProjectionPanel;

	/** Projection d'un point sur une surface. */
	QtVertexSurfaceProjectionPanel*	_vertexSurfaceProjectionPanel;

	/** Point défini par son abscisse curviligne sur une courbe. */
	QtCurvilinearAbscissaPanel*		_curvilinearAbscissaPanel;

	/** Point défini par son ratio de distance entre 2 autres points. */
    QtVerticesRatioPanel*           _verticesRatioPanel;

	//@}	// Panneaux de saisie des paramètres de définition du vertex
};	// class QtVertexOperationPanel


/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtVertexOperationPanel</I> de création/modification d'un vertex.
 */
class QtVertexOperationAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtVertexOperationPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 * \param		Politique de création/modification du groupe
	 */
	QtVertexOperationAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip, QtMgx3DGroupNamePanel::POLICY creationPolicy);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtVertexOperationAction ( );

	/**
	 * \return		Le panneau d'édition de vertex associé.
	 */
	virtual QtVertexOperationPanel* getVertexPanel ( );

	/**
	 * Créé/modifie le vertex avec le paramétrage de son panneau associé.
	 * Invoque préalablement
	 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
	 */
	virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtVertexOperationAction (const QtVertexOperationAction&);
	QtVertexOperationAction& operator = (const QtVertexOperationAction&);
};  // class QtVertexOperationAction



}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_VERTEX_OPERATION_ACTION_H
