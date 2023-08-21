/**
 * \file		VTKGraphicalRepresentationFactory.h
 * \author		Charles PIGNEROL
 * \date		02/07/2012
 */
#ifndef VTK_GRAPHICAL_REPRESENTATION_FACTORY_H
#define VTK_GRAPHICAL_REPRESENTATION_FACTORY_H

#include "Utils/DisplayProperties.h"


namespace Mgx3D
{

namespace QtVtkComponents
{

/**
 * Implémente l'interface <I>GraphicalRepresentationFactoryIfc</I> pour un
 * fonctionnement en environnement graphique <I>VTK</I> de <I>Magix 3D</I>.
 */
class VTKGraphicalRepresentationFactory :
						public Mgx3D::Utils::GraphicalRepresentationFactoryIfc
{
	public :

	/**
	 * Constructeur : RAS.
	 */
	VTKGraphicalRepresentationFactory ( );

	/**
	 * Destructeur. RAS.
	 */
	virtual ~VTKGraphicalRepresentationFactory ( );

	/**
	 * \return		Des propriétés d'affichage en environnement <I>VTK</I>
	 *				pour l'entité transmise en argument.
	 */
	virtual Mgx3D::Utils::DisplayProperties::GraphicalRepresentation* create (
														Mgx3D::Utils::Entity&);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
    VTKGraphicalRepresentationFactory (const VTKGraphicalRepresentationFactory&);
    VTKGraphicalRepresentationFactory& operator = (
									const VTKGraphicalRepresentationFactory&);
};	// class VTKGraphicalRepresentationFactory


}	// namespace Mgx3D

}	// namespace QtVtkComponents

#endif	// VTK_GRAPHICAL_REPRESENTATION_FACTORY_H

