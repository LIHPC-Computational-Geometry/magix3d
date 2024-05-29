/**
 * \file		StructuredMesh.h
 * \author		Charles PIGNEROL
 * \date		23/11/2018
 */


#ifndef MGX3D_STRUCTURED_MESH_H
#define MGX3D_STRUCTURED_MESH_H


#include <cstdint>
#include <string>


namespace Mgx3D
{

namespace Structured
{

/**
 * Classe de maillage structuré de dimensions ni * nj * nk (donc respectivement
 * ni+1, nj+1 et nk+1 noeuds dans chaque dimension).
 */
class StructuredMesh
{
	public :

	/**
	 * Constructeur.
	 * \param	<I>type</I> est le type de maillage structuré au sens 
	 *		<I>HIC</I> du terme, donc par exemple <I>Maillage-3D-S-Ortho</I>.
	 * \param	<I>ni</I>,<I>nj</I> et <I>nk</I> sont les dimensions du maillage.
	 * \param	<I>true</I> s'il faut allouer les tableaux contenant les composantes
	 *		des coordonnées des noeuds, <I>false</I> dans le cas contraire.
	 */
	StructuredMesh (const std::string& type, int64_t ni, int64_t nj, int64_t nk,
	                bool allocate = true);

	/**
	 * Destructeur.Désalloue la mémoire utilisée (<U>tous</U> les tableaux).
	 */
	virtual ~StructuredMesh ( );

	/** \return	Le type de maillage structuré (au sens <I>HIC</I>). */
	virtual const std::string& type ( ) const
	{ return _type; }

	/** \return	Le nombre de noeuds du maillage. */
	int64_t nodeNum ( ) const
	{ return (_ni + 1) * (_nj + 1) * (_nk + 1); }

	/** \return	Les dimensions du maillage. */
	int64_t ni ( ) const
	{ return _ni; }
	int64_t nj ( ) const
	{ return _nj; }
	int64_t nk ( ) const
	{ return _nk; }

	/** \return	Les composantes des noeuds sur les 3 axes. */
	double* x ( )
	{ return _abs; }
	const double* x ( ) const
	{ return _abs; }
	const double* abs ( ) const
	{ return _abs; }
	double* abs ( )
	{ return _abs; }
	double* y ( )
	{ return _ord; }
	const double* y ( ) const
	{ return _ord; }
	const double* ord ( ) const
	{ return _ord; }
	double* ord ( )
	{ return _ord; }
	double* z ( )
	{ return _elev; }
	const double* z ( ) const
	{ return _elev; }
	const double* elev ( ) const
	{ return _elev; }
	double* elev ( )
	{ return _elev; }

	/**
	 * <P><I>Nombre de mailles = nombre de mailles pures + nombre de mailles mixtes.</I></P>
	 * \return	Le nombre de mailles.
	 * \see		mixedCellNum
	 * \see		pureCellNum
	 */
	int64_t cellNum ( ) const
	{ return _cellNum; }

	/**
	 * \return	Le nombre de mailles mixtes.
	 * \see		cellNum
	 * \see		mixedIds
	 */
	int64_t mixedCellNum ( ) const
	{ return _mixedCellNum; }

	/**
	 * \return	Les identifiants des mailles mixtes.
	 * \see		mixedCellNum
	 * \see		pureIds
	 */
	const int64_t* mixedIds ( ) const
	{ return _mixedIds; }
	int64_t* mixedIds ( )
	{ return _mixedIds; }

	/**
	 * \return	Le nombre de mailles pures.
	 * \see		mixedCellNum
	 * \see		setPureCellNum
	 */
	int64_t pureCellNum ( ) const
	{ return _pureCellNum; }

	/**
	 * <P>Mémorise le nombre de mailles pures et alloue les tableaux
	 * contenant les identifiants des mailles pures et des mailles
	 * mixtes.</P>
	 * <P><I>Nombre de mailles = nombre de mailles pures + nombre de mailles mixtes.</I></P>
	 */
	void setPureCellNum (int64_t num);

	/**
	 * \return	Les identifiants des mailles pures.
	 * \see		mixedCellNum
	 */
	const int64_t* pureIds ( ) const
	{ return _pureIds; }
	int64_t* pureIds ( )
	{ return _pureIds; }

	/**
	 * \param	Valeurs extrèmales prises par la densité, pour optimisation en vue
	 *		de ne pas multiplier les recherches.
	 * \see		cellDensities
	 */
	void setDensityRange (double min, double max);
	double densityMin ( ) const
	{ return _densityMin; }
	double densityMax ( ) const
	{ return _densityMax; }

	/**
	 * \return	Les densités des mailles.
	 * \see		setCellDensities
	 */
	const double* cellDensities ( ) const
	{ return _densities; }
	double* cellDensities ( )
	{ return _densities; }

	/**
	 * \param	Les nouvelles densités des mailles.
	 * \warning	<B>Détruit l'ancien tableau de densités et adopte celui reçu en argument.</B>
	 */
	void setCellDensities (double* densities);


	private :

	/**
	 * Constructeur de copie, opérateur = : interdits.
	 */
	StructuredMesh (const StructuredMesh&);
	StructuredMesh& operator = (const StructuredMesh&);

	std::string		_type;
	int64_t			_ni, _nj, _nk;
	int64_t			_cellNum;
	int64_t			_mixedCellNum;
	int64_t			_pureCellNum;
	double*		_abs;
	double*		_ord;
	double*		_elev;
	int64_t*			_pureIds;
	int64_t*			_mixedIds;
	double			_densityMin, _densityMax;
	double*		_densities;
};	// class StructuredMesh

}	// namespace Structured

}	// namespace Mgx3D



#endif	// MGX3D_STRUCTURED_MESH_H

