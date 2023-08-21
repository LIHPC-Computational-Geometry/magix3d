/*----------------------------------------------------------------------------*/
/*
 * GMDSGeomAdapterAbstractFactory.h
 *
 *  Created on: 9 nov. 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GMDSGEOMADAPTERABSTRACTFACTORY_H_
#define GMDSGEOMADAPTERABSTRACTFACTORY_H_
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Mesh {
/**
 * \class GMDSGeomAdapterAbstractFactory
 * \brief Fabrique permettant de créer des adaptateurs qui adaptent des entités
 *        géométriques de Magix3D a une utilisation par GMDS. Plus précisément,
 *        pour une entité géométrique de dimension N, GMDS définit une
 *        (ou plusieurs pour certains algorithmes) interface(s) de services que
 *        doivent fournit les entités géométriques de dimension N. L'adapateur
 *        de dimension N sera une classe concrète immplémentant cette interface
 *        en composant un objet géométrique de dimension N de Magix3D.
 *
 */
/*----------------------------------------------------------------------------*/
class GMDSGeomAdapterAbstractFactory
{
public:
    /*------------------------------------------------------------------------*/
    /** \brief  Constructeur
     */
    GMDSGeomAdapterAbstractFactory();

    /*------------------------------------------------------------------------*/
    /** \brief  Destructeur
     */
    virtual ~GMDSGeomAdapterAbstractFactory();

};
/*----------------------------------------------------------------------------*/
} // end namespace Mesh
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/
#endif /* GMDSGEOMADAPTERABSTRACTFACTORY_H_ */
/*----------------------------------------------------------------------------*/
