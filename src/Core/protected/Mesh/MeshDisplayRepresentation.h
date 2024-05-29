/*----------------------------------------------------------------------------*/
/** \file MeshDisplayRepresentation.h
 *
 *  \author Charles Pignerol
 *
 *  \date 22/05/2012
 */

#ifndef MESH_DISPLAY_REPRESENTATION_H
#define MESH_DISPLAY_REPRESENTATION_H

#include <map>

#include "Utils/DisplayRepresentation.h"

namespace Mgx3D
{

namespace Mesh
{

/** \class MeshDisplayRepresentation
 *  \brief La classe MeshDisplayRepresention décrit une représentation
 *         de ce quoi doit être affichée pour représenter graphiquement une
 *         entité maillage.
 */
class MeshDisplayRepresentation: public Utils::DisplayRepresentation
{
	public:

    /**
	 * \brief		Constructeur
	 * \param		Type de représentation
	 * \param		Facteur de décimation (1 noeud sur <I>decimationStep</I> est
	 *				représenté).
     */
	MeshDisplayRepresentation (
			DisplayRepresentation::type t = Utils::DisplayRepresentation::SOLID,
			unsigned long decimationStep = 1);

    /**
	 * \brief		Destructeur
     */
	virtual ~MeshDisplayRepresentation ( );

	/**
	 * \param		Les noeuds des mailles (pour chaque maille : nombre de
	 *				noeuds qui la constitue, puis ids des noeuds).
	 * \param		<I>true</I> si c'est la peau du volume qui est représentée,
	 * 				<I>false</I> s'il s'agit des mailles pleines.
	 * \warning		Ce vecteur est adopté
	 * \see			getCells
	 */
	virtual void setCells (std::vector<size_t>* cells, bool skin);

	/**
	 * \return		Les noeuds des mailles.
	 * \see			setCells
	 */
	virtual const std::vector<size_t>& getCells ( ) const;

	/**
	 * \param		<I>true</I> si c'est la peau du volume qui doit être
	 *				représentée, <I>false</I> s'il s'agit des mailles pleines.
	 * \see			setCells
	 * \see			skinDisplayed
	 */
	virtual void setSkinDisplayed (bool display);

	/**
	 * \return		<I>true</I> si c'est la peau du volume qui est représentée,
	 * 				<I>false</I> s'il s'agit des mailles pleines.
	 * \see			setCells
	 * \see			setCells
	 */
	virtual bool skinDisplayed ( ) const;

    /**
	 * \param		Nouveau facteur de décimation de la représentation.
     */
	virtual void setDecimationStep (unsigned long decimationStep);

    /**
	 * \return		Facteur de décimation de la représentation.
     */
	virtual unsigned long getDecimationStep ( ) const;

    /**
	 * \param		Correspondance entre points et indice des noeuds gmds
     */
	virtual void setPoints2nodesID (std::map<int,int> points2GMDSID);

	/**
	 * \return		Correspondance entre points et indice des noeuds gmds
	 */
	virtual std::map<int,int> getPoints2nodesID () const;


	private :

	/** Correspondance entre points et noeuds gmds. */
	std::map<int,int>					_points2nodesID;

	/** Les mailles (nombre de noeuds puis indices des noeuds). */
	std::vector<size_t>*				_cells;

	/** <I>true</I> si c'est la peau du volume qui est représentée,
	 * <I>false</I> s'il s'agit des mailles pleines. */
	bool								_skin;

    /** Facteur de décimation de la représentation. */
	unsigned long	 					_decimationStep;



	public :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 * A n'utilisation que pour la sérialisation (mode client/serveur).
	 * \warning		<B>Ne pas utiliser directement ces constructeurs, utiliser
	 *				ceux prévus à cet effet.</B>.
	 */
	MeshDisplayRepresentation (const MeshDisplayRepresentation&);
	MeshDisplayRepresentation& operator = (const MeshDisplayRepresentation&);
};	// class MeshDisplayRepresentation

}	// namespace Mesh

}	// namespace Mgx3D

#endif	// MESH_DISPLAY_REPRESENTATION_H

