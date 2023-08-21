#ifndef QT_MGX3D_PREFERENCES_DIALOG_H
#define QT_MGX3D_PREFERENCES_DIALOG_H

#include <PrefsQt/QtPreferencesDialog.h>

namespace Mgx3D
{

namespace QtComponents
{

/**
 * Boite de dialogue d'édition des paramètres de configuration de MGX3D.
 * Permet de modifier les paramètres de configuration et d'appliquer les
 * modifications.
 */
class QtMgx3DPreferencesDialog : public QtPreferencesDialog 
{
	Q_OBJECT

	public :

	/** Constructeur.
	 * @param		widget parent. Ne doit pas être nul.
	 * @param		Section à éditer. Cette section sera détruite
	 *				par le destructeur si releaseSection n'a pas été
	 *				appelé.
	 * @param		Fichier d'où est issue la section.
	 * @param		true si le panneau est éditable, sinon false.
	 * @param		true si le menu doit avoir un item "Appliquer", sinon false.
	 * @param		true si la boite de dialogue est modale, sinon false.
	 * @param		titre de la boite de dialogue.
	 * @param		URL de l'aide
	 * @param		Balise de l'aide
	 */
	QtMgx3DPreferencesDialog (QWidget* parent, Preferences::Section* section, 
	                        const std::string& fromFile, bool editable,
	                        bool withApply, bool modal, 
	                        const TkUtil::UTF8String& dlgTitle,
	                        const TkUtil::UTF8String& appTitle,
	                        const std::string& helpURL ="",
	                        const std::string& helpTag = "");



	/** Destructeur. */
	virtual ~QtMgx3DPreferencesDialog ( );


	protected :

	/**
	 * Met en application, au niveau de l'application courante, la configuration
	 * en cours.
	 */
	virtual void applyCallback ( );


	protected slots :

	/**
	 * Affiche l'aide de cette boîte de dialogue.
	 */
	virtual void helpCallback ( );


	private :

	/** Constructeur de copie. Interdit. */
	QtMgx3DPreferencesDialog (const QtMgx3DPreferencesDialog&);

	/** Opérateur de copie. Interdit. */
	QtMgx3DPreferencesDialog& operator = (const QtMgx3DPreferencesDialog&);

	/** L'URL et la balise de l'aide. */
	std::string		_helpURL, _helpTag;
};	// class QtMgx3DPreferencesDialog

}	// namespace Mgx3D

}	// namespace QtComponents


#endif	// QT_MGX3D_PREFERENCES_DIALOG_H
