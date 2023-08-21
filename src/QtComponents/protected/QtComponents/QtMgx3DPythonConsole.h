#ifndef QT_MGX3D_PYTHON_CONSOLE_H
#define QT_MGX3D_PYTHON_CONSOLE_H


#include "Internal/ScriptingManager.h"
#include "Utils/CommandManagerIfc.h"
#if PY_MAJOR_VERSION >= 3
#	include <QtPython3/QtPythonConsole.h>
#else	// PY_MAJOR_VERSION >= 3
#	include <QtPython/QtPythonConsole.h>
#endif	// PY_MAJOR_VERSION >= 3

namespace Mgx3D
{

namespace QtComponents
{


class Qt3DGraphicalWidget;
class QtMgx3DMainWindow;

/**
 * <P>
 * Console Qt permettant d'exécuter des commandes Python en environnement
 * <I>Magix 3D</I>.
 *
 * @warning		<B>Ne pas utiliser la méthode setEnabled, mais lui préférer la
 *				méthode <I>setUsabled</I> qui est <I>thread safe</I>.</B>
 */
class QtMgx3DPythonConsole : public QtPythonConsole 
{
	public :

	/** Constructeur
	 * @param	widget parent. Ne doit pas être nul.
	 * @param	Fenêtre principale <I>Magix 3D</I>, dont les actions sont
	 * 			inhibées en mode debug.
	 * @param	titre du panneau
	 */
	QtMgx3DPythonConsole (
		QWidget* parent, Mgx3D::QtComponents::QtMgx3DMainWindow* mainWindow,
		const IN_STD string& title);

	/** Destructeur. */
	virtual ~QtMgx3DPythonConsole ( );

	/**
	 * Le mode de fonctionnement courant.
	 * En mode "debug" inhibe les actions <I>Magix 3D</I>.
	 */
	virtual void setRunningMode (QtPythonConsole::RUNNING_MODE mode);

	/**
	 * Surcharges des méthodes en vue de s'assurer d'une exécution séquentielle
	 * dans l'interpréteur python.
	 */
	virtual void run ( );
	virtual void stop ( );
	virtual void cont ( );
	virtual void next ( );

	/**
	 * Execute la commande transmise en argument.
	 * \param	Commande à exécuter.
	 */
	virtual void executeCommand (const IN_STD string& command);

	/**
	 * Execute le fichier script transmis en argument.
	 * \param	Script à exécuter.
	 */
	virtual void executeFile (const IN_STD string& fileName);

	/**
	 * Ajoute la commande transmise en argument, si il (le panneau) n'en est pas
	 * à l'origine, à l'historique des commandes exécutées, mais ne l'exécute
	 * pas. Présente l'intérêt d'intercaller des commandes effectuées par
	 * ailleurs.
	 * @param	Commande à ajouter
	 * @param	Commentaires associés à la commande.
	 * @param	Sortie de la commande à ajouter.
	 * @param      Status en erreur (<I>true</I>) ou non de la commande
	 * @param	<I>true</I> si la commande vient du noyau, <I>false</I>
	 * 			si elle vient d'ailleurs (par exemple de la console python).
	 * @see		setPythonOutputStream
	 */
	virtual void addToHistoric (
		const IN_UTIL UTF8String& command, const IN_UTIL UTF8String& comments,
		const IN_UTIL UTF8String& commandOutput,
		bool statusErr, bool fromKernel = false);

	/**
	 * @return	L'éventuelle sortie <I>script python utilisateur</I>
	 * 			utilisée.
	 *			Ajoute les commandes saisies depuis ce panneau au script python.
	 */
	virtual Mgx3D::Internal::ScriptingManager* getMgxUserScriptingManager ( );
	virtual const Mgx3D::Internal::ScriptingManager*
											getMgxUserScriptingManager( ) const;

	/**
	 * @param	Sortie <I>script python utilisateur</I> à utiliser.
	 * 			Ajoute les commandes saisies depuis ce panneau au script python.
	 */
	virtual void setMgxUserScriptingManager (
									Mgx3D::Internal::ScriptingManager* scrmng);

	/**
	 * @return	L'éventuelle fenêtre <I>Magix 3D</I> associée.
	 */
	virtual Mgx3D::QtComponents::QtMgx3DMainWindow* getMainWindow ( );

	/**
	 * @param	Fenêtre graphique à inhiber lors de l'exécution des
	 *			commandes (optimisation graphique : on effectue qu'un
	 *			tracé en fin de commande).
	 */
	virtual void setGraphicalWidget (Qt3DGraphicalWidget* widget);


	protected :

	/**
 	 * Récupère/réaffecte la politique de fonctionnement du gestionnaire de
 	 * commandes.
 	 */
	virtual void storePolicy ( );
	virtual void restorePolicy ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DPythonConsole (const QtMgx3DPythonConsole&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DPythonConsole& operator = (const QtMgx3DPythonConsole&);

	/** L'éventuel gestionnaire de scripts en sortie <I>script python</I>
	 * utilisé et contenant les commandes saisies par l'utilisateur (et non
	 * celles générées en toutes circonstances par <I>Magix 3D</I>).
	 */
	Mgx3D::Internal::ScriptingManager*				_mgxUserScriptingManager;

	/** Fenêtre Magix 3D, dont les actions sont inhibées en mode debug. */
	Mgx3D::QtComponents::QtMgx3DMainWindow*			_mainWindow;

	/** Eventuel widget dont on inhibe les tracés graphiques durant l'exécution
	 * d'une commande. */
	Mgx3D::QtComponents::Qt3DGraphicalWidget*		_graphicalWidget;

	/** Police du gestionnaire de commandes à restaurer lorsque la console
	 * aura fini de fonctionner.
	 */
	Mgx3D::Utils::CommandManagerIfc::POLICY			_cmdMgrPolicy;
};	// class QtMgx3DPythonConsole

}	// namespace QtComponents

}	// namespace Mgx3D


#endif	// QT_MGX3D_PYTHON_CONSOLE_H
