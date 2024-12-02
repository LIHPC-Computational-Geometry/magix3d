/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingProperty.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 7/12/2011
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTY_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTY_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <sys/types.h>
#include "Utils/SerializedRepresentation.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {

class Block;
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'un bloc
 */
class BlockMeshingProperty {
public:

    /** Les méthodes de maillage pour un bloc
     */
    typedef enum{
        transfinite,
        delaunayTetgen
    } meshLaw;

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual BlockMeshingProperty* clone() =0;
#endif
    /*------------------------------------------------------------------------*/
    /// Destructeur
    virtual ~BlockMeshingProperty() {}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual meshLaw getMeshLaw() const =0;

    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const =0;

    /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    virtual bool isStructured() const =0;

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const =0;
#endif

    /*------------------------------------------------------------------------*/
#ifndef SWIG
   /// initialisation de la direction en fonction de ce qui a été donné à la construction pour certaines méthodes
    virtual void initDir(Block* bl)
    {}
#endif

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la direction associée à la CoFace
    virtual uint getDir() const
    {
        throw TkUtil::Exception(TkUtil::UTF8String ("BlockMeshingProperty::getDir() ne peut se faire que pour une méthode avec une direction", TkUtil::Charset::UTF_8));
    }

    /*------------------------------------------------------------------------*/
    /// ajoute la description des propriétés spécifiques
#ifndef SWIG
    virtual void addProperties(Utils::SerializedRepresentation& ppt) const {};
#endif


protected:
    /*------------------------------------------------------------------------*/
    /// Constructeur par défaut
    BlockMeshingProperty() {}

    /*------------------------------------------------------------------------*/
    /// Constructeur par copie
    BlockMeshingProperty(const BlockMeshingProperty& pm) {}

};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTY_H_ */
