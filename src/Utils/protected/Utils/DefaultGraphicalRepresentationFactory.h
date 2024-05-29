/*----------------------------------------------------------------------------*/
/** \file DefaultGraphicalRepresentationFactory.h
 *
 *  \author Charles PIGNEROL
 *
 *  \date 23/07/2010
 */
/*----------------------------------------------------------------------------*/

#ifndef DEFAULT_GRAPHICAL_REPRESENTATION_FACTORY_H
#define DEFAULT_GRAPHICAL_REPRESENTATION_FACTORY_H

#include "Utils/DisplayProperties.h"


namespace Mgx3D
{

namespace Utils
{

/**
 * <P>
 * Classe de base définissant les services d'une fabrication de représentations
 * graphiques d'entités. Cette classe n'est pas spécialisée pour une <I>API</I>
 * graphique type <I>VTK</I>, et les services qu'elle propose ne font donc
 * rien ! Si ce n'est offrir une implémentation.
 * <P>
 * <P>Cette classe est utile à la génération d'exécutables <I>Magix 3D</I>
 * dépourvus de représentations graphiques type <I>IHM</I> : tests unsitaires,
 * scripts, ...
 * </P>
 */
class DefaultGraphicalRepresentationFactory :
						public Mgx3D::Utils::GraphicalRepresentationFactoryIfc
{
	public :

	/**
	 * Constructeur. RAS.
	 */
	DefaultGraphicalRepresentationFactory ( );

	/**
	 * Destructeur. RAS.
	 */
	virtual ~DefaultGraphicalRepresentationFactory ( );

	/**
	 * \return	 	Des propriétés d'affichage pour l'entité transmise en
	 *				argument.
	 */
	virtual Mgx3D::Utils::DisplayProperties::GraphicalRepresentation* create (
													Mgx3D::Utils::Entity&);


	protected :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	DefaultGraphicalRepresentationFactory (
								const DefaultGraphicalRepresentationFactory&);
	DefaultGraphicalRepresentationFactory& operator = (
								const DefaultGraphicalRepresentationFactory&);
};	// class DefaultGraphicalRepresentationFactory

} // end namespace Utils

} // end namespace Mgx3D

#endif // DEFAULT_GRAPHICAL_REPRESENTATION_FACTORY_H
