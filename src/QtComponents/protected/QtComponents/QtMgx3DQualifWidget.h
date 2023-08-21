/**
 * \file		QtMgx3DQualifWidget.h
 * \author		Charles PIGNEROL
 * \date		02/08/2013
 */

#ifndef QT_MGX3D_QUALIF_WIDGET_H
#define QT_MGX3D_QUALIF_WIDGET_H

#include "Internal/ContextIfc.h"

#include <QtQualif/QtQualifWidget.h>
#include <QMenu>

#include "Mesh/MeshItf.h"
#include "Internal/Context.h"

#include <GMDS/Utils/CommonTypes.h>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * <P>Classe de Widget <I>Qt</I> permettant d'évaluer la qualité d'un
 * maillage <I>Magix 3D</I> à l'aide de la bibliothèque <I>Qualif</I>.
 * L'accès aux données est effectué à l'aide d'une instance de la classe
 * <I>AbstractQualifSerie</I>, indépendante du type de maillage analysé.
 * </P>
 *
 * <P>Les méthodes de cette classe sont susceptibles de lever des exceptions
 * de type <I>TkUtil::Exception</I>.
 * </P>
 *
 * \see		AbstractQualifSerie
 */
class QtMgx3DQualifWidget : public GQualif::QtQualifWidget
{
	Q_OBJECT

	public :

	/**
	 * Constructeur.
	 * \param		Widget parent.
	 * \param		Context d'utilisation du widget
	 */
	QtMgx3DQualifWidget (QWidget* parent, Mgx3D::Internal::Context* context);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~QtMgx3DQualifWidget ( );

	/**
	 * Actualise l'histogramme conformément aux paramètres renseignés dans
	 * l'IHM.
	 */
	virtual void updateHistogram ( );

	/**
	 * \return		La liste des surfaces correspondant aux classes
	 *				sélectionnées.
	 * \warning		<B>Ces surfaces sont à détruire par l'appelant.</B>
	 */
	virtual std::vector<gmds::IGMesh::surface*> getSelectedClassesSurfaces ( );

	/**
	 * \return		La liste des volumes correspondant aux classes
	 *				sélectionnées.
	 * \warning		<B>Ces volumes sont à détruire par l'appelant.</B>
	 */
	virtual std::vector<gmds::IGMesh::volume*> getSelectedClassesVolumes ( );


	protected :

	/**
	 * \return		Le context d'utilisation du widget.
	 */
	virtual Mgx3D::Internal::Context& getContext ( );
	virtual const Mgx3D::Internal::Context& getContext ( ) const;

	virtual void 	focusInEvent (QFocusEvent*);


	protected slots :

	/**
	 * Affiche le menu contextuel.
	 */
	virtual void displayPopupMenu (const QPoint&);

	/**
	 * Affiche l'éditeur de graphiques.
	 */
	virtual void editGraphicCallback ( );


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	QtMgx3DQualifWidget (const QtMgx3DQualifWidget&);
	QtMgx3DQualifWidget& operator = (const QtMgx3DQualifWidget&);

	/** Le context Magix 3D d'utilisation du panneau. */
	Mgx3D::Internal::Context*									_context;

	/** Le menu contextuel. */
	QMenu*				_popupMenu;
};	// class QtMgx3DQualifWidget


}	// namespace QtComponents

}	// namespace Mgx3D

#endif	// QT_MGX3D_QUALIF_WIDGET_H
