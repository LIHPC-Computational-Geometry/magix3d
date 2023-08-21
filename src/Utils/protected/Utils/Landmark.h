/*----------------------------------------------------------------------------*/
/*
 * \file Landmark.h
 *
 *  \author Charles Pignerol
 *
 *  \date 2 sept. 2016
 */
/*----------------------------------------------------------------------------*/
#ifndef MGX3D_UTILS_LANDMARK_H_
#define MGX3D_UTILS_LANDMARK_H_

#include <string>


/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/

class Landmark
{
public:

	/// les différentes unités de longueur supportées
	enum kind {
		maillage,
		chambre,
		undefined
	};

	/// conversion en une string ("maillage" ...)
	static std::string toString(const kind& l);

	/// retourne la chaine pour génération d'une ligne de commande python
	static std::string  getScriptCommand(const kind& lu);

	/// convertion d'une string en un repère
	static kind toLandmark(const std::string& name);


	private :

	Landmark ( );
	Landmark (const Landmark&);
	Landmark& operator = (const Landmark&);
	~Landmark ( );
}; // class Landmark
/*----------------------------------------------------------------------------*/
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_UTILS_LANDMARK_H_ */
/*----------------------------------------------------------------------------*/
