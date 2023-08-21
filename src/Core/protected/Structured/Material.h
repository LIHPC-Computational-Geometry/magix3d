/**
 * \file		Material.h
 * \author		Charles PIGNEROL
 * \date		23/11/2018
 */

#ifndef MGX3D_MATERIAL_H
#define MGX3D_MATERIAL_H

#include <cstdint>
#include <string>


namespace Mgx3D
{

namespace Structured
{

/**
 * Classe représentant la répartition d'un matériau (milieu) dans un maillage.
 */
class Material
{
	public :

	/**
	 * Constructeur. Effectue les allocations des différents tableaux
	 * (identifiants, densités, fractions de présence) pour <I>cellNum</I>
	 * mailles.
	 * \param	Nom du matériau
	 * \param	Nombre de mailles où ce matériau est présent.
	 */
	Material (const std::string& name, int64_t cellNum);

	/**
	 * Destructeur.
	 * Détruit <U>tous</U> les tableaux.
	 */
	~Material ( );

	/** \return	Le nom du matériau. */
	const std::string& name ( ) const
	{ return _name; }

	/** \return	Le nombre de mailles où ce matériau est présent. */
	int64_t cellNum ( ) const
	{ return _cellNum; }

	/**
	 * \return	Le nombre de mailles mixtes contenant ce matériau.
	 * \see		setMixedCellNum
	 * \see		setPureCellNum
	 */
	int64_t mixedCellNum ( ) const
	{ return _mixedCellNum; }

	/**
	 * \param	Le nombre de mailles mixtes contenant ce matériau.
	 * \see		mixedCellNum
	 * \see		setPureCellNum
	 */
	void setMixedCellNum (int64_t num);

	/**
	 * \return	Le nombre de mailles pures contenant ce matériau.
	 * \see		setPureCellNum
	 * \see		setMixedCellNum
	 */
	int64_t pureCellNum ( ) const
	{ return _pureCellNum; }

	/**
	 * \param	Le nombre de mailles pures contenant ce matériau.
	 * \see		pureCellNum
	 * \see		setPixedCellNum
	 */
	void setPureCellNum (int64_t num);

	/**
	 * \return	Les identifiants des mailles contenant le matériau.
	 * \see		cellDensities
	 * \see		cellPresFrac
	 */
	const int64_t* cellIds ( ) const
	{ return _cellIds; }
	int64_t* cellIds ( )
	{ return _cellIds; }

	/**
	 * \return	Les densités des mailles.
	 * \see		cellIds
	 * \see		cellPresFrac
	 */
	const double* cellDensities ( ) const
	{ return _cellDensities; }
	double* cellDensities ( )
	{ return _cellDensities; }

	/**
	 * \return	Les fractions de présence dans chaque maille.
	 * \see		cellIds
	 * \see		cellDensities
	 */
	const double* cellPresFrac ( ) const
	{ return _cellPresFrac; }
	double* cellPresFrac ( )
	{ return _cellPresFrac; }


	private :

	Material (const Material&);
	Material& operator = (const Material&);

	std::string		_name;
	int64_t			_cellNum;
	int64_t			_mixedCellNum;
	int64_t			_pureCellNum;
	int64_t*			_cellIds;
	double*		_cellDensities;
	double*		_cellPresFrac;
};	// class Material


}	// namespace Structured

}	// namespace Mgx3D


#endif	// MGX3D_MATERIAL_H

