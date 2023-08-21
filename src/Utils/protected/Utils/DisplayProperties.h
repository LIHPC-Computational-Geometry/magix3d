/*----------------------------------------------------------------------------*/
/** \file DisplayProperties.h
 *
 *  \author Charles PIGNEROL
 *
 *  \date 22/12/2010
 */
/*----------------------------------------------------------------------------*/
#ifndef DISPLAY_PROPERTIES_H_
#define DISPLAY_PROPERTIES_H_
/*----------------------------------------------------------------------------*/
#include <TkUtil/PaintAttributes.h>
#include <string>
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D
{
namespace Utils
{
	class Entity;
}	// namespace Mgx3D::Utils
}	// namespace Mgx3D
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Utils {
/*----------------------------------------------------------------------------*/
/** \class DisplayProperties
 *  \brief La classe DisplayProperties contient les paramètres de dessin
 *         utilisés pour représenter graphiquement une entité.
 *
 *         Un tel ensemble de paramètres est attaché à chaque instance d'entité
 *         ce qui permet d'avoir simultanément plusieurs vues de la même entité
 *         utilisant toutes les mêmes propriétés d'affichage. De plus ce
 *         dispositif permet de conserver ces propriétés entre 2 "sessions"
 *         d'affichage.
 *
 * \warning	Depuis décembre 2011, et à des fins d'optimisation, on privilégie
 *			fortement le fait qu'il n'y ait qu'une seule vue graphique, ce
 *			qui permet de rattacher directement à l'entité, via cette classe
 *			certaines infos type isDisplayed => évite les recherches longues
 *			et coûteuses, par exemple dans des instances de <I>vector</I>.
 *
 * \see Context::newDisplayProperties
 */
/*----------------------------------------------------------------------------*/
class DisplayProperties
{
	public:

	/**
	 * Constructeur et destructeurs.
	 */
	//@{
	/**
	 * Constructeur. RAS.
	 */
	DisplayProperties ( );

	/**
	 * Constructeur de copie. RAS.
	 */
	DisplayProperties (const DisplayProperties&);

	/**
	 * Opérateur =. RAS.
	 */
	DisplayProperties& operator = (const DisplayProperties&);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~DisplayProperties ( );
	//@}

	/**
	 * Accesseurs sur les propriétés graphiques.
	 */
	//@{

	/**
	 * \return		La couleur utilisée en mode "nuage".
	 * \see			setCloudColor
	 * \see			getWireColor
	 * \see			getSurfacicColor
	 * \see			getVolumicColor
	 */
	virtual const TkUtil::Color& getCloudColor ( ) const
	{ return _cloudColor; }

	/**
	 * \param		La nouvelle couleur utilisée en mode "nuage".
	 * \warning		<B>Absence de mécanisme permettant d'informer les vues de ce
	 *				changement de couleur.</B>
	 * \see			getCloudColor
	 * \see			getWireColor
	 * \see			setSurfacicColor
	 * \see			getVolumicColor
	 */
	virtual void setCloudColor (const TkUtil::Color& color);

	/**
	 * \return		La couleur utilisée en mode filaire.
	 * \see			setWireColor
	 * \see			getSurfacicColor
	 * \see			getVolumicColor
	 */
	virtual const TkUtil::Color& getWireColor ( ) const
	{ return _wireColor; }

	/**
	 * \param		La nouvelle couleur utilisée en mode filaire.
	 * \warning		<B>Absence de mécanisme permettant d'informer les vues de ce
	 *				changement de couleur.</B>
	 * \see			getWireColor
	 * \see			setSurfacicColor
	 */
	virtual void setWireColor (const TkUtil::Color& color);

	/**
	 * \return		La couleur utilisée en mode surfacique.
	 * \see			setSurfacicColor
	 * \see			getCloudColor
	 * \see			getWireColor
	 * \see			getVolumicColor
	 * \warning		<B>ATTENTION :</B> dans la version actuelle les couleurs
	 *				surfacique et volumique sont portées par le même attribut.
	 */
	virtual const TkUtil::Color& getSurfacicColor ( ) const
	{ return _solidColor; }

	/**
	 * \param		La nouvelle couleur utilisée en mode surfacique.
	 * \warning		<B>Absence de mécanisme permettant d'informer les vues de ce
	 *				changement de couleur.</B>
	 * \see			getSurfacicColor
	 * \see			setCloudColor
	 * \see			setWireColor
	 * \warning		<B>ATTENTION :</B> dans la version actuelle les couleurs
	 *				surfacique et volumique sont portées par le même attribut.
	 */
	virtual void setSurfacicColor (const TkUtil::Color& color);

	/**
	 * \return		La couleur utilisée en mode volumique.
	 * \see			setSurfacicColor
	 * \see			getCloudColor
	 * \see			getWireColor
	 * \see			getSurfacicColor
	 * \see			getVolumicColor
	 * \warning		<B>ATTENTION :</B> dans la version actuelle les couleurs
	 *				surfacique et volumique sont portées par le même attribut.
	 */
	virtual const TkUtil::Color& getVolumicColor ( ) const
	{ return _solidColor; }

	/**
	 * \param		La nouvelle couleur utilisée en mode volumique.
	 * \warning		<B>Absence de mécanisme permettant d'informer les vues de ce
	 *				changement de couleur.</B>
	 * \see			getVolumicColor
	 * \see			setCloudColor
	 * \see			setWireColor
	 * \see			setSurfacicColor
	 * \warning		<B>ATTENTION :</B> dans la version actuelle les couleurs
	 *				surfacique et volumique sont portées par le même attribut.
	 */
	virtual void setVolumicColor (const TkUtil::Color& color);

	/**
	 * \return		La couleur utilisée pour les textes 2D.
	 * \see			setFontColor
	 * \see			getFontProperties
	 */
	virtual const TkUtil::Color& getFontColor ( ) const
	{ return _fontColor; }

	/**
	 * \param		La nouvelle couleur utilisée pour les textes 2D.
	 * \warning		<B>Absence de mécanisme permettant d'informer les vues de ce
	 *				changement de couleur.</B>
	 * \see			setFontProperties
	 */
	virtual void setFontColor (const TkUtil::Color& color);

	/**
	 * \return		Le nom de l'éventuelle valeur (aux noeuds/aux mailles)
	 * 				représentée.
	 * \see			setValueName
	 */
	virtual const std::string& getValueName ( ) const
	{ return _valueName; }

	/**
	 * \param		Le nom de l'éventuelle valeur (aux noeuds/aux mailles)
	 * 				représentée.
	 * \see			getValueName
	 */
	virtual void setValueName (const std::string& name);

	/**
	 * \return		L'opacité (comprise entre 0 et 1) utilisée en mode "nuage".
	 * \see			setCloudOpacity
	 * \see			getWireOpacity
	 * \see			getSurfacicOpacity
	 */
	virtual double getCloudOpacity ( ) const
	{ return _cloudOpacity; }

	/**
	 * \param		La nouvelle opacité utilisée en mode "filaire".
	 * \see			getCloudOpacity
	 * \see			getWireOpacity
	 * \see			setSurfacicOpacity
	 */
	virtual void setCloudOpacity (double opacity);

	/**
	 * \return		L'opacité (comprise entre 0 et 1) utilisée en mode "filaire".
	 * \see			setCloudOpacity
	 * \see			getWireOpacity
	 * \see			getSurfacicOpacity
	 */
	virtual double getWireOpacity ( ) const
	{ return _wireOpacity; }

	/**
	 * \param		La nouvelle opacité utilisée en mode "surfacique".
	 * \see			getWireOpacity
	 * \see			getCloudOpacity
	 * \see			setSurfacicOpacity
	 */
	virtual void setWireOpacity (double opacity);

	/**
	 * \return		L'opacité (comprise entre 0 et 1) utilisée en mode
	 *				"surfacique".
	 * \see			setSurfacicOpacity
	 * \see			getWireOpacity
	 * \see			getCloudOpacity
	 */
	virtual double getSurfacicOpacity ( ) const
	{ return _surfacicOpacity; }

	/**
	 * \param		La nouvelle opacité utilisée en mode "surfacique".
	 * \see			getCloudOpacity
	 * \see			getWireOpacity
	 * \see			setSurfacicOpacity
	 */
	virtual void setSurfacicOpacity (double opacity);

	/**
	 * \return		L'opacité (comprise entre 0 et 1) utilisée en mode
	 *				"volumique".
	 * \see			setVolumicOpacity
	 * \see			getWireOpacity
	 * \see			getCloudOpacity
	 */
	virtual double getVolumicOpacity ( ) const
	{ return _volumicOpacity; }

	/**
	 * \param		Le nouveau facteur de rétrécissement (compris entre 0
	 *			et 1, 1 : affichage normal).
	 * \see			getShrinkFactor
	 */
	virtual void setShrinkFactor (double factor);

	/**
	 * \return		Le facteur de rétrécissement
	 * \see			setShrinkFactor
	 */
	virtual double getShrinkFactor ( ) const
	{ return _shrinkFactor; }

	/**
	 * \param		Le nouveau facteur facteur d'agrandissement des flèches 
	 *			association (compris entre 0.1 et 10., 1 : affichage normal).
	 * \see			getArrowComul
	 */
	virtual void setArrowComul (double comul);

	/**
	 * \return		Le facteur facteur d'agrandissement des flèches association
	 * \see			setArrowComul
	 */
	virtual double getArrowComul ( ) const
	{ return _arrowComul; }

	/**
	 * \param		La nouvelle opacité utilisée en mode "volumique".
	 * \see			getCloudOpacity
	 * \see			getWireOpacity
	 * \see			setVolumicOpacity
	 */
	virtual void setVolumicOpacity (double opacity);

	/**
	 * \return		L'épaisseur des points.
	 * \see			setPointSize
	 */
	virtual float getPointSize ( ) const;

	/**
	 * \param		L'épaisseur des points.
	 * \see			getPointSize
	 */
	virtual void setPointSize (float size);

	/**
	 * \return		L'épaisseur des lignes.
	 * \see			setLineWidth
	 */
	virtual float getLineWidth ( ) const;

	/**
	 * \param		L'épaisseur des lignes.
	 * \see			getLineWidth
	 */
	virtual void setLineWidth (float width);

	/**
	 * \return		La fonte utilisée pour les affichages textuels.
	 * \see			setFontProperties
	 */
	virtual void getFontProperties (
					int& family, int& size, bool& bold, bool& italic,
					TkUtil::Color& color) const;

	/**
	 * \param		La fonte utilisée pour les affichages textuels.
	 * \see			getFontProperties
	 */
	virtual void setFontProperties (
					int family, int size, bool bold, bool italic,
					const TkUtil::Color& color);

    /**
     * \return      La visibilité dans la vue graphique.
     * \see         setDisplayed
     */
	virtual bool isDisplayed ( ) const;

    /**
     * \param       La visibilité dans la vue graphique.
     * \see         isDisplayed
     */
    virtual void setDisplayed (bool disp);

    /**
     * \return      La propriété sur la possibilité d'être visible dans la vue graphique.
     * \see         setDisplayable
     */
    virtual bool isDisplayable ( ) const
    { return _displayable;}

    /**
     * \param       La propriété sur la possibilité d'être visible dans la vue graphique.
     * \see         isDisplayable
     */
    virtual void setDisplayable (bool disp)
    { _displayable = disp;}


	/**
	 * Classe de base de représentation graphique.
	 * Cette classe est là pour stocker un pointeur à des fins
	 * d'optimisation, et gère également le caractère 
	 * <I>sélectionné</I> de l'instance représentée dans la vue
	 * graphique.
	 */
	class GraphicalRepresentation
	{
		public :

		GraphicalRepresentation ( );
		GraphicalRepresentation (const GraphicalRepresentation& gr);
		GraphicalRepresentation& operator = (const GraphicalRepresentation& gr);
		virtual ~GraphicalRepresentation ( );

		virtual void setSelected (bool selected);
		virtual bool isSelected ( ) const;
		virtual void setSelectable (bool selected);
		virtual bool isSelectable ( ) const;
		virtual void setHighlighted (bool selected);
		virtual bool isHighlighted ( ) const;

		/**
		 * \return		Le masque de représentation de l'entité, à préciser
		 *				dans des classes dérivées.
		 * \see			setRepresentationMask
		 * \see			updateRepresentation
		 */
		virtual unsigned long getRepresentationMask ( ) const
		{ return _representationMask; }

		/**
		 * \return		Nouveau masque de représentation de l'entité.
		 * \see			getRepresentationMask
		 * \see			updateRepresentation
		 */
		virtual void setRepresentationMask (unsigned long mask);

		/**
		 * Met à jour la représentation graphique de l'entité
		 * représentée. Créé cette représentation graphique si nécessaire.
		 * Ne fait rien par défaut, si ce n'est mémoriser le masque de
		 * représentation à utiliser.
		 * \param   Nouveau masque de représentation
		 * \param   <I>true</I> s'il faut réactualiser la représentation en
		 *			toutes circonstances, y compris si le masque transmis est
		 *			identique à l'actuel, <I>false</I> dans le cas contraire.
		 *			Cette réactualisation est de A à Z, et comprend donc
		 *			également le maillage.
		 * \see	 	getRepresentationMask
		 * \see		updateRepresentationProperties
		 */
		virtual void updateRepresentation (unsigned long mask, bool force);

		/**
		 * Actualise les attributs de représentation (couleur, épaisseur des
		 * traits, ...). Prend en compte le caractère sélectionné ou non de
		 * l'entité dans la représentation graphique.
		 */
		virtual void updateRepresentationProperties ( );

		/**
		 * Affiche l'entité au premier plan (évite que d'autres entités lui
		 * soient superposées). Permet, par exemple, de faire ressortir une
		 * entité sélectionnée.
		 * \warning		Ne fait rien par défaut, méthode à surcharger.
		 */
		virtual void bringToTop ( );

		/**
		 * Créé une "représentation raffinée". Ne fait rien par défaut.
		 * \param		degré de raffinement attendu (ex : 10 pour x10 par
		 * 			rapport à la représentation par défaut, ...).
		 * \see destroyRefinedRepresentation
		 */
		virtual void createRefinedRepresentation (size_t factor);

		/**
		 * Détruit la "représentation raffinée".
		 * \see createRefinedRepresentation
		 */
		virtual void destroyRefinedRepresentation ( );


		private :

		bool						_selected;
		bool						_selectable;
		bool						_highlighted;
		unsigned long				_representationMask;
	};	// class GraphicalRepresentation

	/**
	 * \return		La représentation graphique, s'il y en a une.
	 * \see			setGraphicalRepresentation
	 */
	virtual const GraphicalRepresentation* getGraphicalRepresentation ( ) const;
	virtual GraphicalRepresentation* getGraphicalRepresentation ( );

	/**
	 * \param		La représentation graphique, ou 0.
	 * \see			getGraphicalRepresentation
	 */
	virtual void setGraphicalRepresentation (GraphicalRepresentation*);

	//@}


	/**
	 * Les vues "non graphiques".
	 */
	//@{

	/**
	 * Cette classe représente des vues "non graphiques" sur des entités
	 * (items de liste, panneaux spécialisés, ...). Elles sont destinées à
	 * être directement enregistrées auprès de l'entité qu'elles représentent
	 * afin d'être tenues averties, sans recherche particulière, de toute
	 * modification.
	 */
	class EntityView
	{
		public :

		virtual ~EntityView ( );
		virtual const Mgx3D::Utils::Entity& getViewEntity ( ) const;
		virtual Mgx3D::Utils::Entity& getViewEntity ( );

		/**
		 * A surcharger : appelé lorsque l'entité est affichée (<I>true</I>) ou
		 * masquée (<I>false</I>).
		 */
		virtual void entityDisplayed (bool displayed);


		protected :

		EntityView (Mgx3D::Utils::Entity* entity);


		private :

		EntityView (const EntityView&);
		EntityView& operator = (const EntityView&);
		Mgx3D::Utils::Entity*		_entity;
	};	// class EntityView

	/**
	 * Enregistre une vue de l'entité à tenir informée lorsqu'elle est
	 * affichée/masquée.
	 * \see		notifyViews
	 * \see		unregisterEntityView
	 */
	virtual void registerEntityView (EntityView& view);

	/**
	 * Annule l'enregistrement de la vue de l'entité transmise en argument.
	 * \see		notifyViews
	 * \see		registerEntityView
	 */
	virtual void unregisterEntityView (EntityView& view);

	/**
	 * Informe les vues que l'entité est affichée (<I>true</I> ou masquée
	 * <I>false</I>) en invoquant <I>entityDisplayed</I>.
	 * \see		registerEntityView
	 */
	virtual void notifyViews (bool displayed);

	//@}	// Les vues "non graphiques".
	

	/**
	 * Opérations de comparaison.
	 */
	//@{
	/**
	 * Opérateurs == et != : RAS.
	 */
	virtual bool operator == (const DisplayProperties& dp) const;
	virtual bool operator != (const DisplayProperties& dp) const;
	//@}


	/**
	 * Quelques valeurs par défaut.
	 */
	//@{

	/** Fonte 2D pour les affichages graphiques. */
	static unsigned short	_defaultFontFamily;
	static unsigned short	_defaultFontSize;
	static bool				_defaultFontBold;
	static bool				_defaultFontItalic;

	//@}	// Quelques valeurs par défaut.


	private :

	/** Couleur d'affichage en mode "nuage". */
	TkUtil::Color				_cloudColor;

	/** Couleur d'affichage en mode filaire. */
	TkUtil::Color				_wireColor;

	/** Couleur d'affichage en mode surfacique et volumique. */
	TkUtil::Color				_solidColor;

	/** L'opacité dans les différents modes. */
	double					_cloudOpacity, _wireOpacity, _surfacicOpacity,
						_volumicOpacity;

	/** L'éventuel facteur de rétrécissement, compris entre 0 et 1.
	 * 1 : affichage normal. */
	double					_shrinkFactor;

	/** L'éventuel facteur d'agrandissement des flèches association,
	 * compris entre 0.1 et 10., 1. : affichage normal. */
	double					_arrowComul;

	/** L'épaisseur des points. */
	float					_pointSize;

	/** L'épaisseur des lignes. */
	float					_lineWidth;

	/** Le texte 2D. */
	unsigned short				_fontFamily, _fontSize;
	bool					_bold, _italic;
	TkUtil::Color				_fontColor;

	/** Le nom de l'éventuelle valeur (aux noeuds/aux mailles) représentée. */
	std::string				_valueName;

	/** Visibilité de l'entité dans la vue graphique */
	bool					_displayed;

	/** L'éventuelle représentation graphique. */
	GraphicalRepresentation*		_graphicalRepresentation;

	/** Propriété d'une entité pour le fait d'être affichable/représentable ou non */
	bool                      	  	_displayable;

	/** Les représentations de l'entité. */
	std::vector<EntityView*>		_views;
};	// class DisplayProperties
/*----------------------------------------------------------------------------*/

/**
 * Interface définissant les services d'une fabrication de représentations
 * graphiques d'entités.
 */
class GraphicalRepresentationFactoryIfc
{
	public :

	/**
	 * Constructeur. RAS.
	 */
	GraphicalRepresentationFactoryIfc ( );

	/**
	 * Destructeur. RAS.
	 */
	virtual ~GraphicalRepresentationFactoryIfc ( );

	/**
	 * \return		<I>true</I> s'il existe une instance (a priori) unique à la
	 *				classe, <I>false</I> dans le cas contraire.
	 * \see			hasInstance
	 * \see			setInstance
	 */
	static bool hasInstance ( );

	/**
	 * \return		Une référence sur l'instance (a priori) unique à la classe.
	 * \exception	Une exception est levée en l'absence d'instance unique.
	 * \see			hasInstance
	 * \see			setInstance
	 */
	static GraphicalRepresentationFactoryIfc& getInstance ( );

	/**
	 * Affecter une instance (a priori) unique à la classe. Détruit l'éventuelle
	 * instance déjà affectée.
	 * \see			hasInstance
	 * \see			getInstance
	 */
	static void setInstance (GraphicalRepresentationFactoryIfc* instance);

	/**
	 * \return	 	Des propriétés d'affichage pour l'entité transmise en
	 *				argument.
	 */
	virtual Mgx3D::Utils::DisplayProperties::GraphicalRepresentation* create (
													Mgx3D::Utils::Entity&) = 0;


	protected :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	GraphicalRepresentationFactoryIfc (const GraphicalRepresentationFactoryIfc&);
	GraphicalRepresentationFactoryIfc& operator = (
									const GraphicalRepresentationFactoryIfc&);

	/** L'instance (a priori) unique de la classe. */
	static GraphicalRepresentationFactoryIfc*			_instance;
};	// class GraphicalRepresentationFactoryIfc
/*----------------------------------------------------------------------------*/

} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* DISPLAY_PROPERTIES_H_ */
/*----------------------------------------------------------------------------*/
