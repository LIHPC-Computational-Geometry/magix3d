/**
 * \file		QtVtkMgx3DApplication.h
 * \author		Charles PIGNEROL
 * \date		24/11/2011
 */
#ifndef QT_VTK_MGX3D_APPLICATION_H
#define QT_VTK_MGX3D_APPLICATION_H

#include <PrefsCore/BoolNamedValue.h>
#include <PrefsCore/DoubleNamedValue.h>
#include <PrefsCore/UnsignedLongNamedValue.h>
#include <TkUtil/Log.h>

#include <QtComponents/QtMgx3DApplication.h>


namespace Mgx3D 
{

/*!
 * \namespace Mgx3D::QtVtkComponents
 *
 * \brief Espace de nom des classes pour l'affichage via Qt.
 *
 *
 */
namespace QtVtkComponents
{

/**
 * \brief		Classe <I>application Magix 3D</I> spécialisée pour un
 *				affichage 3D reposant sur <I>VTK</I>.
 *
 */
class QtVtkMgx3DApplication : public QtComponents::QtMgx3DApplication
{
	public :

	/**
	 * Instancie l'application
	 * \param		arguments du point d'entrée (<I>main</I>) de l'application.
	 * \warning		<B>argc doit être passé par référence pour
	 *				<I>QApplication</I>, sinon risque de plantage.</B>
	 */
	QtVtkMgx3DApplication (int& argc, char* argv[], char* envp[]);

	/**
	 * RAS.
	 */
	virtual ~QtVtkMgx3DApplication ( );
	
	/**
	 * \return		Une référence sur l'instance de cette application.
	 */
	static QtVtkMgx3DApplication& instance ( );
	
	/**
	 * <P>Procède au différentes initialisations requises, notamment celles
	 * de <I>VTK</I>.
	 * </P>
	 * <P>Appelle au préalalble QtMgx3DApplication::init.
	 * </P>
	 * \param		arguments du point d'entrée (<I>main</I>) de l'application.
	 */
	virtual void init (int argc, char* argv[], char* envp[]);

	/**
	 * La gestion de la configuration.
	 */
	//@{

	/**
	 * Applique la configuration reçue en argument.
	 * \param		Section racine de la configuration à appliquer.
	 */
	virtual void applyConfiguration (const Preferences::Section& mainSection);

	/**
	 * Ajoute à la section tranmise en argument la configuration actuelle.
	 * \param		Section principale de la configuration de l'application.
	 * \warning		N'effectue aucune action persistante.
	 */
	virtual void saveConfiguration (Preferences::Section& mainSection);

	/**
	 * \return		Un entier identifiant la police de caractères transmise en
	 *				argument.
	 * \warning		Retourne 0 par défaut, méthode à surcharger.
	 */
	virtual unsigned short fontNameToInt (const std::string& name);

	/** Faut-il restituer la mémoire inutilisée pour les pointages lasers et spots (calculs intermédiaires) ? */
	Preferences::BoolNamedValue				_raysReleaseIntermediateMemory;

	/** Epaisseur des traits des pointages laser. */
	Preferences::DoubleNamedValue			_raysLineWidth;

	/** Opacité des traits des pointages laser. */
	Preferences::DoubleNamedValue			_raysOpacity, _raysSpotOpacity;

	/** Tolérance lors des intersections rai/maille. */
	Preferences::DoubleNamedValue			_spotTolerance;

	/** Limitation des moirés ? */
	Preferences::BoolNamedValue				_spotRepresentationCheat;

	/** Paramètres de limitation des moirés. */
	Preferences::DoubleNamedValue			_spotRepresentationCheatFactor;
	
	/** Elimination des spots cachés ? */
	Preferences::BoolNamedValue				_hiddenCellsElimination;

	/** Fenêtre off-screen pour l'élimination des spots cachés. */
	Preferences::UnsignedLongNamedValue		_offScreenWindowWidth, _offScreenWindowHeight;

	/** Le jeu de caractère utilisé pour l'encodage des fichiers lasers/diagnostics. */
	Preferences::StringNamedValue			_raysFileCharset;
	//@}	// La gestion de la configuration.


	private :

	// Opérations interdites :
#ifdef QT_4
	QtVtkMgx3DApplication (const QtVtkMgx3DApplication&);
#endif	// QT_4
	QtVtkMgx3DApplication& operator = (const QtVtkMgx3DApplication&);
};	// class QtVtkMgx3DApplication


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// QT_VTK_MGX3D_APPLICATION_H
