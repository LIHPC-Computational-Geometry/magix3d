/*----------------------------------------------------------------------------*/
/** \file CommandCreator.h
 *
 *  \author Franck Ledoux, Eric Brière de l'Isle
 *
 *  \date 14/10/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef COMMANDCREATOR_H_
#define COMMANDCREATOR_H_


#include <string>


/*----------------------------------------------------------------------------*/
namespace TkUtil {
class LogOutputStream;
class Log;
}
/*----------------------------------------------------------------------------*/
namespace Mgx3D {

namespace Utils {
class CommandManagerIfc;
}
/*----------------------------------------------------------------------------*/
namespace Internal {

class ContextIfc;
class Context;
/*----------------------------------------------------------------------------*/
/** \class CommandCreator
 *  \brief Toute classe créant des commandes doit hériter de CommandCreator
 *         afin de faire le lien avec le manager de commandes associé
 *         via le contexte.
 *
 *         Gestion du logStream commun à tous les managers
 *
 *         Les managers n'ont pas de raison d'être multi-threads,
 *         seule l'exécution des commandes l'est.
 */
/*----------------------------------------------------------------------------*/
class CommandCreator {

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     *
	 *  \param	Nom à vocation unique de l'instance. Ce nom unique permet, dans
	 *			un contexte distribué, de retrouver la bonne instance sur un
	 *			serveur.
     *  \param c le contexte ce qui permet d'accéder entre autre au CommandManager
     */
    CommandCreator(const std::string& name, Internal::ContextIfc* c);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~CommandCreator();

	/**
	 * \return		Le nom de l'instance
	 */
	virtual const std::string& getName ( ) const;

    /**
     * Envoit le <
     * Envoit le <I>log</I> transmis en argument dans le flux de messages
     * associé a l'instance.
     * \see     TkUtil::Log
     */
    virtual void log (const TkUtil::Log& log);

    /** retourne le CommandManager */
    virtual Utils::CommandManagerIfc& getCommandManager();

    /** retourne le contexte */
    virtual const Internal::ContextIfc& getContext() const;
    virtual Internal::ContextIfc& getContext();

    /** retourne le contexte local (même zone mémoire). */
    virtual const Internal::Context& getLocalContext() const;
    virtual Internal::Context& getLocalContext();

    /// retourne l'afficheur (qui peut être nul)
    virtual TkUtil::LogOutputStream* getLogStream ( );


	private :

	/** Le nom de l'instance. */
	std::string				m_name;

   /** le contexte */
	Internal::ContextIfc*	m_context;

};
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* COMMANDCREATOR_H_ */
/*----------------------------------------------------------------------------*/

