#ifndef ERROR_MANAGEMENT_H
#define ERROR_MANAGEMENT_H

#include <TkUtil/Exception.h>

/**
 * \brief	Macro définissant le début d'un block C++ try/catch avec
 *			définition de deux variables, <I>hasError</I> et
 * 			<I>errorString</I> simplifiant le traitement d'erreur en
 *			sortie du bloc try/catch. Cette macro est à utiliser
 *			conjointement avec la macro <I>COMPLETE_TRY_CATCH_BLOCK</I>.
 * \see		COMPLETE_TRY_CATCH_BLOCK
 */
#define BEGIN_TRY_CATCH_BLOCK                                              \
	bool            hasError    = false;                                   \
	std::string     errorString;                                           \
	try {

/**
 * \brief	Macro définissant la fin d'un block C++ try/catch.
 *			En cas d'exception attrapée, cette macro affecte <I>true</I> à
 *			la variable <I>hasError</I> et l'éventuel message d'erreur à la
 *			variable <I>errorString</I>.
 * \see		BEGIN_TRY_CATCH_BLOCK
 */
#define COMPLETE_TRY_CATCH_BLOCK                                           \
	}                                                                      \
	catch (const TkUtil::Exception& exc)                                   \
	{                                                                      \
		errorString = exc.getFullMessage ( );                              \
		std::cout << "Erreur : " << errorString << std::endl;              \
		hasError    = true;                                                \
	}                                                                      \
	catch (const std::exception& stdExc)                                   \
	{                                                                      \
		errorString = stdExc.what ( );                                     \
		std::cout << "Erreur : " << errorString << std::endl;              \
		hasError    = true;                                                \
	}                                                                      \
	catch (...)                                                            \
	{                                                                      \
		errorString = "Erreur non documentée.";                            \
		std::cout << "Erreur interne non documentée." << std::endl;        \
		hasError    = true;                                                \
	}


#endif	// ERROR_MANAGEMENT_H
