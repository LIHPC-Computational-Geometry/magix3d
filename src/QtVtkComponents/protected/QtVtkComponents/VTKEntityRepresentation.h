/**
 * \file		VTKEntityRepresentation.h
 * \author		Charles PIGNEROL
 * \date		24/11/2011
 */
#ifndef VTK_ENTITY_REPRESENTATION_H
#define VTK_ENTITY_REPRESENTATION_H

#include "QtVtkComponents/VTKMgx3DActor.h"
#include "QtVtkComponents/VTKRenderingManager.h"

#include <QtComponents/RenderedEntityRepresentation.h>

#include <vtkActor2D.h>
#include <vtkArrowSource.h>
#include <vtkDataSetMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkOutlineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkSelectVisiblePoints.h>
#include <vtkUnstructuredGrid.h>
#include <VtkComponents/vtkUnstructuredGridRefinementFilter.h>

#include <vector>


namespace Mgx3D 
{

/*!
 * \namespace Mgx3D::QtVtkComponents
 *
 * \brief Espace de nom des classes pour l'affichage via Qt.
 *
 *
 */
namespace QtVtkComponents
{

/**
 * \brief		Classe <I>représentant graphique d'entité</I> spécialisée pour
 *				un affichage 3D reposant sur <I>VTK</I>.
 *
 */
class VTKEntityRepresentation :
						public QtComponents::RenderedEntityRepresentation
{
	public :

	/**
	 * \param		Entité représentée
	 */
	VTKEntityRepresentation (Mgx3D::Utils::Entity& entity);

	/**
	 * Destructeur. RAS.
	 * \see		destroyRepresentations
	 */
	virtual ~VTKEntityRepresentation ( );

	/**
	 * Affichage/masquage de l'entité représentée.
	 * \param	Gestionnaire de rendu utilisé.
	 * \param	<I>true</I> s'il faut afficher l'entité, <I>false</I> s'il
	 *			faut l'enlever.
	 */
	virtual void show (
			Mgx3D::QtVtkComponents::VTKRenderingManager& renderer, bool show);

	/**
	 * Modifie si nécessaire les attributs de représentation selon
	 * <I>selected</I>.
	 */
	virtual void setSelected (bool selected);

	/**
	 * Modifie si nécessaire les attributs de représentation selon
	 * <I>highlighted</I>.
	 *
	 * \warning		Force l'affichage du type de discrétisation afin de donner
	 *				éventuellement un complément d'information visuel à
	 *				l'utilisateur.
	 */
	virtual void setHighlighted (bool highlighted);

	/**
	 * Met à jour la représentation graphique VTK de l'entité
	 * représentée. Créé cette représentation graphique si nécessaire.
	 * Ne fait rien par défaut, si ce n'est mémoriser le masque de
	 * représentation à utiliser.
	 * \param	Nouveau masque de représentation
	 * \param	<I>true</I> s'il faut réactualiser la représentation en toutes
	 * 			circonstances, y compris si le masque transmis est identique
	 * 			à l'actuel, <I>false</I> dans le cas contraire.
	 * \see		destroyRepresentations
	 * \see		createCloudRepresentation
	 * \see		createSurfacicRepresentation
	 * \see		createWireRepresentation
	 */
	virtual void updateRepresentation (unsigned long mask, bool force);

	/**
	 * Affiche l'entité au premier plan (évite que d'autres entités lui
	 * soient superposées). Permet, par exemple, de faire ressortir une
	 * entité sélectionnée.
	 */
	virtual void bringToTop ( );

	/**
	 * \param		Gestionnaire de rendus 3D.
	 */
	virtual void setRenderingManager(Mgx3D::QtComponents::RenderingManager* rm);

	/**
	 * Les ressources <I>VTK</I>.
	 */
	//@{

	/**
	 * \return	La représentation pleine (surfacique).
	 */
	virtual vtkUnstructuredGrid* getSurfacicGrid ( );
	virtual vtkActor* getSurfacicActor ( );
	virtual vtkDataSetMapper* getSurfacicMapper ( );

	/**
 * \return	La représentation pleine (volumique).
 */
	virtual vtkUnstructuredGrid* getVolumicGrid ( );
	virtual vtkActor* getVolumicActor ( );
	virtual vtkDataSetMapper* getVolumicMapper ( );


	/**
	 * \return	La représentation raffinée (ex : positionnement d'un point).
	 */
	virtual vtkUnstructuredGrid* getRefinedGrid ( );
	virtual vtkActor* getRefinedActor ( );
	virtual vtkDataSetMapper* getRefinedMapper ( );

	//@}	// Les ressources <I>VTK</I>.


	protected :

	/**
	 * Créé la représentation "nuage" de l'entité représentée.
	 * \see createSurfacicRepresentation
	 * \see createWireRepresentation
	 */
	virtual void createCloudRepresentation ( ) {}

	/**
	 * Créé la représentation surfacique de l'entité représentée.
	 * \see createCloudRepresentation
	 * \see createWireRepresentation
	 * \see	createAssociationVectorRepresentation
	 */
	virtual void createSurfacicRepresentation ( ) {}

    /**
     * Créé la représentation volumique de l'entité représentée.
     * \see createSurfacicRepresentation
     */
    virtual void createVolumicRepresentation ( ) {}

	/**
	 * Créé la représentation filaire de l'entité représentée.
	 * \see createCloudRepresentation
	 * \see createSurfacicRepresentation
	 * \see	createAssociationVectorRepresentation
	 */
	virtual void createWireRepresentation ( ) {}

    /**
     * Créé la représentation "nuage" de l'entité représentée avec projection sur la modélisation.
     * \see createCloudRepresentation
     */
    virtual void createMeshShapeCloudRepresentation ( ) {}

    /**
     * Créé la représentation filaire de l'entité représentée avec projection sur la modélisation.
     * \see createWireRepresentation
     */
    virtual void createMeshShapeWireRepresentation ( ) {}

	/**
     * Créé la représentation de la discrétisation.
     * \see createWireRepresentation
     */
    virtual void createDiscretisationRepresentation ( ) {}

	/**
	 * Créé la représentation filaire complémentaire "ISO" de l'entité
	 * représentée.
	 * \see createWireRepresentation
	 */
	virtual void createIsoWireRepresentation ( ) {}

	/**
	 * Créé la représentation textuelle de l'entité représentée.
	 * \see	getRepresentationText
	 * \see	getRepresentationTextPosition
	 */
	virtual void createTextualRepresentation ( );

	/**
	 * Créé la représentation "association vectorielle" entre 2 entités
	 * (ex : association topologie vers géométrie).
	 * \see	createWireRepresentation
	 * \see	createSurfacicRepresentation
	 */
	virtual void createAssociationVectorRepresentation ( ) {}

	/**
	 * Créé la représentation "trièdre"
	 */
	virtual void createTrihedronRepresentation ( ) {}

	/**
	 * Retourne une "représentation raffinée" de l'entité : points et
	 * éventuellement structuration segmentaire/triangulaire.
	 * En paramètre le degré de raffinement attendu (ex : 10 pour x10 par
	 * rapport à la représentation par défaut, ...).
	 * \return	true si la discrétisation a été faite selon les instructions,
	 *		false si elle doit être faite par ailleurs (par exemple par VTK).
	 */
	virtual bool getRefinedRepresentation (
		std::vector<Utils::Math::Point>& points, std::vector<size_t>& discretization,
		size_t factor) {return false;}

	/**
	 * Détruit la "représentation raffinée".
	 * \see createRefinedRepresentation
	 */
	virtual void destroyRefinedRepresentation ( );

	/**
	 * Détruit les représentations graphiques actuelles (hors raffinée).
	 */
	virtual void destroyRepresentations (bool realyDestroy);

	/**
	 * Actualise les attributs de représentation (couleur, épaisseur des traits,
	 * ...). Prend en compte le caractère sélectionné ou non de l'entité dans
	 * la représentation graphique.
	 * \warning	N'actualise pas la mise en évidence (<I>highlight</I>).
	 * \see		isSelected
	 * \see		updateHighlightRepresentation
	 */
	virtual void updateRepresentationProperties ( );

	/**
	 * Actualise la représentation <I>highlight</I>.
	 */
	virtual void updateHighlightRepresentation (bool highlight);

	/**
	 * <P>Créé un <I>pattern</I> de ligne pointillée sous VTK à transmettre à la
	 * fonction <I>vtkProperty::SetLineStipplePattern</I>.
	 * Repose sur un masque de bits à 0 ou 1 sur un entier.</P>
	 *
	 * <P>Davantage d'infos : en VTK 5.1.0 les lignes pointillées ne sont actives
	 * qu'avec <I>OpenGL</I>. Elles utilisent la fonction <I>glLineStipple</I>
	 * d'<I>OpenGL</I> => man glLineStipple.
	 * </P>
	 */
	virtual int createLineStipplePattern (int on, int off);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	VTKEntityRepresentation (VTKEntityRepresentation&);
	VTKEntityRepresentation& operator = (const VTKEntityRepresentation&);


	protected :

	/** L'éventuel <I>renderer</I> utilisé. */
	vtkRenderer*				_renderer;

	/**
	 * La représentation pleine (surfacique).
	 */
	//@{
	/** Le maillage représentant l'entité en mode plein. */
	vtkUnstructuredGrid*		_surfacicGrid;

	/** Correspondance entre points vtk et noeuds gmds. */
	std::map<int,int>			_surfacicPointsVTK2GMDSID;

	/** L'acteur associé en mode plein. */
	VTKMgx3DActor*				_surfacicActor;

	/** Le mapper associé en mode plein. */
	vtkDataSetMapper*		   _surfacicMapper;
	//@}

    /**
     * La représentation pleine (volumique).
     */
    //@{
    /** Le maillage représentant l'entité en mode plein. */
    vtkUnstructuredGrid*        _volumicGrid;

    /** Correspondance entre points vtk et noeuds gmds. */
    std::map<int,int>			_volumicPointsVTK2GMDSID;

    /** L'acteur associé en mode plein. */
    VTKMgx3DActor*              _volumicActor;

    /** Le mapper associé en mode plein. */
    vtkDataSetMapper*          _volumicMapper;
    //@}

	/**
	 * La représentation filaire.
	 */
	//@{
	/** Le maillage représentant l'entité en mode filaire. */
	vtkUnstructuredGrid*		_wireGrid;

	/** L'acteur associé en mode filaire. */
	VTKMgx3DActor*				_wireActor;

	/** Le mapper associé en mode filaire. */
	vtkDataSetMapper*		   	_wireMapper;

	/** Le maillage complémentaire ISO représentant l'entité en mode filaire. */
	vtkUnstructuredGrid*		_isoWireGrid;

	/** L'acteur complémentaire ISO associé en mode filaire. */
	VTKMgx3DActor*				_isoWireActor;

	/** Le mapper complémentaire ISO associé en mode filaire. */
	vtkDataSetMapper*		   _isoWireMapper;

	/** Le maillage représentant une représentation de la discrétisation de l'entité. */
	vtkUnstructuredGrid*		_discGrid;

	/** L'acteur associé en mode filaire. */
	VTKMgx3DActor*				_discActor;

	/** Le mapper associé en mode filaire. */
	vtkDataSetMapper*		   	_discMapper;

	//@}

	/**
	 * La représentation "nuage".
	 */
	//@{
	/** Le maillage représentant les noeuds de l'entité. */
	vtkUnstructuredGrid*		_cloudGrid;

	/** L'acteur associé en mode "nuage". */
	VTKMgx3DActor*				_cloudActor;

	/** Le mapper associé en mode "nuage". */
	vtkDataSetMapper*			_cloudMapper;
	//@}

	/**
	 * Informations textuelles complémentaires.
	 */
	//@{

	/** L'acteur associé aux informations textuelles affichées. */
	vtkActor2D*					_textualInfosActor;

	/** Le mapper associé aux informations textuelles affichées. */
	vtkTextMapper*				_textualInfosMapper;

	//@}

	/**
	 * La représentation "association vectorielle" entre 2 entités.
	 */
	//@{
	/** Le vecteur. */
	vtkArrowSource*				_vectAssArrow;

	/** L'acteur associé. */
	VTKMgx3DActor*				_vectAssActor;

	/** Le mapper associé. */
	vtkPolyDataMapper*			_vectAssMapper;
	//@}	// La représentation "association vectorielle" entre 2 entités.


	/**
	 * La représentation "repère local".
	 */
	//@{
	/** Le trièdre avec 3 flêches */
	vtkArrowSource*				_trihedronArrowX;
	vtkArrowSource*				_trihedronArrowY;
	vtkArrowSource*				_trihedronArrowZ;

	/** Les 3 acteurs associés. */
	VTKMgx3DActor*				_triedronXActor;
	VTKMgx3DActor*				_triedronYActor;
	VTKMgx3DActor*				_triedronZActor;

	/** Les mappers associés. */
	vtkPolyDataMapper*			_triedronXMapper;
	vtkPolyDataMapper*			_triedronYMapper;
	vtkPolyDataMapper*			_triedronZMapper;

	//@}	// La représentation "repère local"


	/**
	 * La représentation "raffinée".
	 */
	//@{
	/** Le maillage représentant l'entité en mode "raffiné". */
	vtkUnstructuredGrid*        _refinedGrid;

	/** La discretisation du maillage raffiné. */
	vtkUnstructuredGridRefinementFilter*          _refineFilter;

	/** L'acteur associé en mode plein. */
	VTKMgx3DActor*              _refinedActor;

	/** Le mapper associé en mode plein. */
	vtkDataSetMapper*          _refinedMapper;
	//@}	// La représentation "raffinée".

	/**
	 * La mise en évidence (<I>highlighting</I>).
	 */
	//@{
	vtkOutlineSource*			_outlineSource;
	vtkActor*				_highlightActor;
	vtkPolyDataMapper*			_highlightMapper;
	//@}	// La mise en évidence

	/** La mise en évidence de l'entité force-t-elle l'affichage de son type de
	  *discrétisation ? */
	bool						_highlightForceDiscretisationType;
};	// class VTKEntityRepresentation


}	// namespace QtVtkComponents

}	// namespace Mgx3D

#endif	// VTK_ENTITY_REPRESENTATION_H
