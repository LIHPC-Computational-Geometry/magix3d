/**
 * \file		MgxText.h
 * \author		Charles PIGNEROL
 * \date		12/01/2014
 */


#ifndef MGX_TEXT_H
#define MGX_TEXT_H

#include <string>
#include <iostream>


namespace Mgx3D
{

namespace Utils
{

	/**
	 * \brief	Utilitaires de gestion du texte du projet.
	 */
	class MgxText
	{
		public :

		/**
		 * Transforme la chaine transmise en argument en chaine multiligne.
		 * Un saut de ligne est ajouté derrièer chaque caractère <I>end</I>.
		 * \return		La chaine multiligne créée
		 */
		static std::string stringToMultilineString (
										const std::string& str, char end);


		private :

		/**
		 * Constructeurs, destructeurs : interdits.
		 */
		MgxText ( );
		MgxText (const MgxText&);
		MgxText& operator = (const MgxText&);
		~MgxText ( );
	};	// class MgxText

}	// namespace Utils

}	// namespace Mgx3D

#endif	// MGX_TEXT_H
