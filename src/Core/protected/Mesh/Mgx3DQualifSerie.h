/*
 * \file		Mgx3DQualifSerie.h
 * \author		Charles PIGNEROL
 * \date		02/08/2013
 */

#ifndef MGX3D_QUALIF_SERIE_H
#define MGX3D_QUALIF_SERIE_H

#include <GQGMDS/GMDSQualifSerie.h>
#include <GMDS/IG/IG.h>
#include "Mesh/MeshEntity.h"


namespace Mgx3D
{

namespace Mesh
{

/**
 *
 * <P>Adapteur de maillage <I>GMDS</I> au format <I>Qualif</I> pour le logiciel
 * <I>Magix 3D</I>. </P>
 *
 */
class Mgx3DQualifSerie : public GQualif::GMDSQualifSerie
{
	public :

	/**
	 * Constructeur 1. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Surface <I>GMDS</I> représentée.
	 * @param		Eventuel nom de la surface (au sens de l'application). En 
	 * 				son absence c'est le nom du fichier qui sera utilisé.
	 * @param		Nom du fichier d'où est éventuellement issue la surface.
	 * @param		Eventuelle entité Magix3D de rattachement.
	 */
	Mgx3DQualifSerie (gmds::IGMesh::surface&, const std::string& name,
				const std::string& fileName, Mgx3D::Mesh::MeshEntity* entity = 0);

	/**
	 * Constructeur 2. Les données reçues en argument ne seront pas détruites
	 * lors de la destruction de cette série.
	 * @param		Volume <I>GMDS</I> représenté.
	 * @param		Eventuel nom du volume (au sens de l'application). En 
	 * 				son absence c'est le nom du fichier qui sera utilisé.
	 * @param		Nom du fichier d'où est éventuellement issue le volume.
	 * @param		Eventuelle entité Magix3D de rattachement.
	 */
	Mgx3DQualifSerie (gmds::IGMesh::volume&, const std::string& name,
				const std::string& fileName, Mgx3D::Mesh::MeshEntity* entity = 0);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~Mgx3DQualifSerie ( );

	/**
	 * \param		En retour les index GMDS des mailles dont l'index de la 
	 * 				classe est transmis en second argument.
	 */
	virtual void getGMDSCellsIndexes (
						std::vector<gmds::TCellID>& indexes, size_t clas) const;
						
	/**
	 * \return		L'éventuelle entité Magix3D de rattachement.
	 */
	 virtual Mgx3D::Mesh::MeshEntity* getMeshEntity ( );
	 virtual const Mgx3D::Mesh::MeshEntity* getMeshEntity ( ) const;


	protected :


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	Mgx3DQualifSerie (const Mgx3DQualifSerie&);
	Mgx3DQualifSerie& operator = (const Mgx3DQualifSerie&);
	
	Mgx3D::Mesh::MeshEntity*		_meshEntity;
};	// class Mgx3DQualifSerie

}	// namespace Mesh

}	// namespace Mgx3D

#endif	// MGX3D_QUALIF_SERIE_H
