/**
 * \file		ValidatedField.h
 * \author		Charles PIGNEROL
 * \date		11/12/2012
 */


#ifndef VALIDATED_FIELD_H
#define VALIDATED_FIELD_H

#include <TkUtil/Exception.h>
#include <TkUtil/UTF8String.h>

#include <stdexcept>


/**
 * Macrodéfinition de réalisation d'une expression et récupération du message
 * d'une éventuelle exception et transfert dans une chaine de caractères
 * (TkUtil::UTF8String) 
 */
#define VALIDATED_FIELD_EXPRESSION_EVALUATION(expression,prefix,error)      \
try                                                                         \
{                                                                           \
	expression;                                                             \
}                                                                           \
catch (const TkUtil::Exception& exc)                                        \
{                                                                           \
	if (0 != error.length ( ))                                              \
		error << "\n";                                                  \
	error << prefix << exc.getFullMessage ( );                              \
}                                                                           \
catch (const std::exception& e)                                             \
{                                                                           \
	if (0 != error.length ( ))                                              \
		error << "\n";                                                  \
	error << prefix << e.what ( );                                          \
}                                                                           \
catch (...)                                                                 \
{                                                                           \
	if (0 != error.length ( ))                                              \
		error << "\n";                                                  \
	error << prefix << "Erreur non documentée.";                            \
}


namespace Mgx3D
{

namespace Utils
{

/**
 * Classe abstraite à vocation de représenter un champ (de saisie) à valider.
 */
class ValidatedField
{
	public :

	/**
	 * Destructeur. RAS.
	 */
	virtual ~ValidatedField ( );

	/**
	 * Méthode de validation, à surcharger.
	 * En cas de non validité du champ de saisie, doit lever une exception de
	 * type <I>TkUtil::Exception</I> contenant un descriptif des erreurs
	 * rencontrées.
	 * Ne fait rien par défaut.
	 */
	virtual void validate ( );


	protected :

	/**
	 * Constructeurs et opérateur = : RAS.
	 */
	ValidatedField ( );
	ValidatedField (const ValidatedField&);
	ValidatedField& operator = (const ValidatedField&);
};	// class ValidatedField


}	// namespace Utils

}	// namespace Mgx3D


#endif	// QT_ERROR_MANAGEMENT_H
