#ifndef QT_MGX3D_LOGS_VIEW_H
#define QT_MGX3D_LOGS_VIEW_H

#include <TkUtil/util_config.h>
#include <QtUtil/QtLogsView.h>

#include <QSettings>


namespace Mgx3D
{

namespace QtComponents
{

/**
 * Classe "vue de logs" spécialisée pour le projet <I>Magix 3D</I>.
 */
class QtMgx3DLogsView : public QtLogsView 
{
	public :

	/** Constructeur
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		masque à appliquer initialement pour les logs à afficher
	 * @param		<I>true</I> si les dates doivent être affichées
	 * @param		<I>true</I> si les heures doivent être affichées
	 */
	QtMgx3DLogsView (QWidget *parent, IN_UTIL Log::TYPE mask,
	                 bool enableDate=false, bool enableTime=true);

	/** Destructeur. */
	virtual ~QtMgx3DLogsView ( );

	/**
	 * Divers IHM.
	 */
	//@{

	/**
	 * Enregistre les paramètres d'affichage (taille, position, ...) de cette
	 * fenêtre.
	 * \see		readSettings
	 */
	virtual void writeSettings (QSettings& settings);

	/**
	 * Lit et s'applique les paramètres d'affichage (taille, position, ...) de
	 * cette fenêtre.
	 * \see		writeSettings
	 */
	virtual void readSettings (QSettings& settings);

	//@}	// Divers IHM

	/**
	 * Affiche le log transmis en argument en environnement multithread.
	 * \param		Log à afficher.
	 */
	virtual void log (const IN_UTIL Log& log);


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DLogsView (const QtMgx3DLogsView&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DLogsView& operator = (const QtMgx3DLogsView&);
};	// class QtMgx3DLogsView

}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_MGX3D_LOGS_VIEW_H
