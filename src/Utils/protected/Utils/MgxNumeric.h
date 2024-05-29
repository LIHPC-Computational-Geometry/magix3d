/**
 * \file		MgxNumeric.h
 * \author		Charles PIGNEROL
 * \date		18/10/2013
 */


#ifndef MGX_NUMERIC_H
#define MGX_NUMERIC_H

#include <numeric>
#include <cmath>
#include <string>
#include <iostream>
#include <TkUtil/Timer.h>


namespace Mgx3D
{

namespace Utils
{

namespace Math
{

	/**
	 * \brief	Gestion des constantes numériques du projet, aussi bien pour
	 * 			les aspects calculs que pour ceux liés à l'IHM. A noter que
	 * 			les constantes numériques non spécifiques au projet, propres
	 * 			à la machine, sont accessibles via la classe
	 * 			<I>TkUtil::NumericServices</I>.
	 */
	class MgxNumeric
	{
		public :


		/**
		 * Les <I>double</I>
		 */
		//@{

		/**
		 * Epsilon <I>Magix 3D</I>.
		 */
#ifndef SWIG
		static double				mgxGeomDoubleEpsilon;
		static double 				mgxParameterEpsilon;
		static double               mgxGeomDoubleEpsilonSquare;
		static double               mgxDoubleEpsilon;
		static double               mgxTopoDoubleEpsilon;
#endif	// SWIG

		/**
		 * Le nombre maximum de caractères d'un double en notation scientifique
		 * au sein du projet.
		 */
#ifndef SWIG
		static size_t				mgxDoubleScientificNotationCharMax;
#endif  // SWIG

		/**
		 * Le nombre maximum de caractères d'un double en notation fixe
		 * au sein du projet.
		 */
#ifndef SWIG
		static size_t				mgxDoubleFixedNotationCharMax;	
#endif	// SWIG

		static inline bool isNearlyZero (const double & u)
		{
			return std::fabs(u) < Mgx3D::Utils::Math::MgxNumeric::mgxDoubleEpsilon;
		}	// isNearlyZero

        static inline bool isNearlyZero (const double & u, const double & epsilon)
        {
#ifdef _DEBUG
            double epsilon10 = epsilon*10;
            bool infEps    = std::fabs(u) < epsilon;
            bool infEps10 = std::fabs(u) < epsilon10;
            if(infEps10 && !infEps){
                std::cerr<<"Warning: "<<u<<" inferieur a "<<epsilon10<<" et superieur a "<<epsilon<<std::endl;
            }
#endif
            return std::fabs(u) < epsilon;
        }   // isNearlyZero



		//@}	//  Les double


		/**
		 * Les <I>float</I>
		 */
		//@{

		/**
		 * Epsilon <I>Magix 3D</I>.
		 */
#ifndef SWIG
		static float				mgxFloatEpsilon;
#endif	// SWIG

		/**
		 * Le nombre maximum de caractères d'un float en notation scientifique
		 * au sein du projet.
		 */
#ifndef SWIG
		static size_t				mgxFloatScientificNotationCharMax;	
#endif	// SWIG

		//@}	//  Les float

		/**
		 * Les angles.
		 */
		//@{

		/**
		 * Précision numérique sur les angles (via l'IHM) : nombre de décimales
		 * en notation standard.
		 */
#ifndef SWIG
		static unsigned short	mgxAngleDecimalNum;
#endif	// SWIG

		//@}	// Les angles.


		/**
		 * \return		Une représentation <I>user friendly</I> du nombre
		 * 				transmis en argument. La <I>precision</I> demandée
		 * 				est <I>mgxDoubleScientificNotationCharMax</I>. La
		 * 				bascule notation <I>fixed</I> vers notation
		 * 				<I>scientific</I> est effectuée au delà de
		 * 				<I>mgxDoubleFixedNotationCharMax</I>.
		 * 				Utilise NumericConversions::userRepresentation.
		 */
		static std::string userRepresentation (double precision);

		/**
		 * \return		Une représentation <I>user friendly</I> du temps
		 * 				transmis en argument. La chaine retournée est du type
		 * 				"3,2 secondes" ou "567 microsecondes" selon les valeurs
		 * 				de timer.duration ( ) et timer.microduration ( ).
		 */
		static std::string userRepresentation (const TkUtil::Timer& timer);


		private :

		/**
		 * Constructeurs, destructeurs : interdits.
		 */
		MgxNumeric ( );
		MgxNumeric (const MgxNumeric&);
		MgxNumeric& operator = (const MgxNumeric&);
		~MgxNumeric ( );
	};	// class MgxNumeric

}	// namespace Math

}	// namespace Utils

}	// namespace Mgx3D

#endif	// MGX_NUMERIC_H
