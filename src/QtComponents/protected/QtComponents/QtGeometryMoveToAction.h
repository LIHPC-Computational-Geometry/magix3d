#ifndef QT_GEOMETRY_MOVETO_ACTION_H
#define QT_GEOMETRY_MOVETO_ACTION_H

#include "Internal/Context.h"

#include "QtMgx3DPointPanel.h"
#include "QtEntityByDimensionSelectorPanel.h"
#include "QtMgx3DGeomOperationAction.h"

#include "Utils/Vector.h"

#include <QCheckBox>




namespace Mgx3D
{

namespace QtComponents
{

class QtMgx3DGroupNamePanel;

/**
* Panneau d'édition des paramètres de translation d'entités géomtriques.
*/
class QtGeometryMoveToPanel : public QtMgx3DOperationPanel {

    Q_OBJECT

    public:

    /**
	 * Créé l'ihm.
	 * \param	Widget parent.
	 * \param	Nom du panneau.
	 * \param	Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *			notamment pour récupérer le contexte.
	 * \param	Eventuelle action associée à ce panneau.
	 */
	QtGeometryMoveToPanel (
			QWidget* parent, const std::string& panelName,
			Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
			Mgx3D::QtComponents::QtMgx3DOperationAction* action);

	/**
	 * RAS.
	 */
	virtual ~QtGeometryMoveToPanel ( );

	/**
	 * \param		Nouvelle dimension de saisie des entités géométriques.
	 */
	virtual void setDimension (Mgx3D::Internal::SelectionManager::DIM dim);

	/**
	 * \return		<I>true</I> s'il faut copier les entités avant la transformation
	 */
	virtual bool copyEntities ( ) const;

	/**
	 * @return		Le nom du groupe créé en cas de copie.
	 */
	virtual std::string getGroupName ( ) const;

	/**
	 * \return		La liste des entités géométriques devant effectuer une
	 *				translation.
	 * \see			getTranslation
	 * \see			doPropagate
	 * \see			processAllGeomEntities
	 */
	virtual std::vector<std::string> getGeomEntitiesNames ( ) const;

	/**
	 * \return		La translation à effectuer.
	 * \see			getGeomEntitiesNames
	 */
	virtual Utils::Math::Point getPoint ( ) const;

	/**
	 * Réinitialise le panneau.
	 */
	virtual void reset ( );


    /**
     * Méthode appelée lorsque l'utilisateur suspend l'édition de l'opération.
     * Restitue l'environnement dans son état initial.
     * Invoque <I>preview (false, true)</I>.
	 * \see		autoUpdate
	 * \see		preview
     */
    virtual void cancel ( );

	/**
	 * Actualise le panneau en fonction du contexte.
	 * Invoque <I>preview (true, true)</I>.
	 * \see		cancel
	 * \see		preview
	 */
	virtual void autoUpdate ( );


	protected :

	/**
	 * \return		La liste des entités impliquées dans l'opération.
	 */
	virtual std::vector<Mgx3D::Utils::Entity*> getInvolvedEntities ( );

	/**
	 * Invoqué lorsque l'opération a été exécutée. Actualise le panneau.
	 * cf. spécifications dans la classe de base.
	 */
	virtual void operationCompleted ( );


	protected slots :

	/**
	 * Appelé lorsque l'on active ou non la copie avant transformation.
	 * (In)active la zone de sélection du nom d'un groupe ainsi que
	 * l'option de copie de la topologie associée.
	 */
	virtual void copyCallback();


	private :

	/**
	* Constructeur de copie et opérateur = opérations interdites.
	*/
	QtGeometryMoveToPanel (const QtGeometryMoveToPanel&);
	QtGeometryMoveToPanel& operator = (
										const QtGeometryMoveToPanel&);

	/** Les dimensions possibles pour les entités géométriques suivant la
	 * translation. */
	QtEntityByDimensionSelectorPanel*			_geomEntitiesPanel;

	/** Coordonnées du vertex. */
	QtMgx3DPointPanel*							_pointPanel;

	/** Case à cocher copier. */
	QCheckBox*									_copyCheckBox;

	/** Le nom du groupe créé (en cas de copie). */
	QtMgx3DGroupNamePanel*						_namePanel;
};	// class QtGeometryMoveToPanel


	/**
 * Classe d'action type <I>check box</I> associée à un panneau type
 * <I>QtGeometryMoveToPanel</I> de translation d'entités géométriques.
 */
class QtGeometryMoveToAction : public QtMgx3DGeomOperationAction
{
	public :

	/**
	 * Créé et s'associe une instance de la classe
	 * <I>QtGeometryMoveToPanel</I>.
	 * \param		Icône représentant l'action.
	 * \param		Texte représentant l'action.
	 * \param		Fenêtre principale <I>Magix 3D</I> de rattachement, utilisée
	 *				notamment pour récupérer le contexte et le panneau contenant
	 *				les icônes.
	 * \param		Tooltip décrivant l'action.
	 */
	QtGeometryMoveToAction (
		const QIcon& icon, const QString& text,
		Mgx3D::QtComponents::QtMgx3DMainWindow& mainWindow,
		const QString& tooltip);

		/**
		 * Destructeur. RAS.
		 */
		virtual ~QtGeometryMoveToAction ( );

		/**
		 * \return		Le panneau d'édition des paramètres de translation d'entités
		 *				géométriques.
		 */
		virtual QtGeometryMoveToPanel* getMoveToPanel ( );

		/**
		 * Actualise le paramétrage des entités géométriques avec le paramétrage de
		 * son panneau associé. Invoque préalablement
		 * <I>QtMgx3DGeomOperationAction::executeOperation</I>.
		 */
		virtual void executeOperation ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtGeometryMoveToAction (const QtGeometryMoveToAction&);
		QtGeometryMoveToAction& operator = (
										const QtGeometryMoveToAction&);
};

}

}

#endif //QT_GEOMETRY_MOVETO_ACTION_H