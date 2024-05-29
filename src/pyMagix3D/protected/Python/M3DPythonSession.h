#ifndef M3D_PYTHON_SESSION_H
#define M3D_PYTHON_SESSION_H

#include <PythonUtil/PythonSession.h>
#include <TkUtil/Version.h>


/**
 * \return		une instance de la classe <I>PythonSession</I> permettant
 *				d'utiliser le binding <I>Python</I> de <I>Magix 3D</I> de
 *				cette composante. Cette instance est à la charge de l'appelant,
 *				et il lui revient donc de la détruire en fin d'utilisation.
 */
TkUtil::PythonSession* createMgx3DPythonSession ( );


namespace Mgx3D {

/*!
 * \namespace Mgx3D::Python
 *
 * \brief Espace de nom de l'utilitaire M3DPythonSession
 *
 *
 */
namespace Python {


/**
 * <P>
 * Classe gérant l'ouverture et la fermeture de sessions de commandes et scripts
 * <I>python</I> pour <I>Magix 3D</I>.
 * </P>
 * <P>
 * Cette classe permet l'exécution simultanée de plusieurs sessions.
 * La session <I>Magix 3D</I> du programme n'est fermée que lorsque toutes
 * les instances de cette classe sont détruites.
 * </P>
 *
 * @author		Charles PIGNEROL, CEA/DAM/DSSI
 */
class M3DPythonSession : public IN_UTIL PythonSession
{
	public :

	/**
	 * Initialise les sessions <I>python</I>, <I>TkUtil</I> et <I>Magix 3D</I>
	 * si nécessaire.
	 */
	M3DPythonSession ( );

	/**
	 * Finalise les sessions <I>python</I>, <I>TkUtil</I> et <I>Magix 3D</I> si
	 * c'est la dernière instance
	 * chargée de cette classe.
	 */
	virtual ~M3DPythonSession ( );

	/**
	 * Initialise l'environnement d'exécution de scripts et commandes
	 * <I>python</I> pour <I>Magix 3D</I>.
	 * @param		Liste des chemins donnant accès aux librairies
	 *				dynamiques utilisées par <I>Magix 3D</I>.
	 * @warning		A invoquer avant toute instanciation de la classe.
	 */
	static void init (const IN_STD vector<IN_STD string>& sysPaths);

	/**
	 * @return		Le version de cette classe.
	 */
	static const IN_UTIL Version& getVersion ( )
	{ return _version; }


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	M3DPythonSession (const M3DPythonSession&);
	M3DPythonSession& operator = (const M3DPythonSession&);

	/**
	 * Le nombre d'instances de cette classe.
	 */
	static size_t					_instanceCount;

	/**
	 * La version de cette bibliothèque.
	 */
	static const IN_UTIL Version	_version;
};	// class M3DPythonSession

}	// namespace Python

}	// namespace Mgx3D


#endif	// M3D_PYTHON_SESSION_H
