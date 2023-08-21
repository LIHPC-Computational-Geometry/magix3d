/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyDirectional.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYDIRECTIONAL_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYDIRECTIONAL_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/BlockMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'un bloc suivant une direction
 */
class BlockMeshingPropertyDirectional : public BlockMeshingProperty {

protected:
	typedef enum{
		points,
		coedge,
		dir
	} initMethod;

public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec direction définie par 2 points
    BlockMeshingPropertyDirectional(Utils::Math::Point v1, Utils::Math::Point v2)
    : m_method(points), m_dir(dir_undef), m_v1(v1), m_v2(v2), m_coedge_name()
    {}

    /// Constructeur dont on connait la direction
    BlockMeshingPropertyDirectional(meshDirLaw md)
     : m_method(dir), m_dir(md), m_v1(), m_v2(), m_coedge_name()
     {}

    /// Constructeur avec direction définie par une arête
    BlockMeshingPropertyDirectional(std::string coedgeName)
     : m_method(coedge), m_dir(dir_undef), m_v1(), m_v2(), m_coedge_name(coedgeName)
     {}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    virtual BlockMeshingPropertyDirectional* clone()  {return new BlockMeshingPropertyDirectional(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    virtual BlockMeshingProperty::meshLaw getMeshLaw() const {return BlockMeshingProperty::directional;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    virtual std::string getMeshLawName() const
    {
        if (m_dir == dir_i)
            return "DirectionI";
        else if (m_dir == dir_j)
            return "DirectionJ";
        else if (m_dir == dir_k)
            return "DirectionK";
        else
            return "Direction à préciser";
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la direction associée à la CoFace
    virtual uint getDir() const
    {
        if (m_dir == dir_i)
            return 0;
        else if (m_dir == dir_j)
            return 1;
        else if (m_dir == dir_k)
            return 2;
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("BlockMeshingPropertyDirectional::getDir() ne peut se faire sans initialiser la direction", TkUtil::Charset::UTF_8));
    }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
   /// initialisation de la direction en fonction de ce qui a été donné à la construction
    virtual void initDir(Block* bloc)
    {
	   if (m_method == points)
		   m_dir = _computeDir(bloc, m_v1, m_v2);
	   else if (m_method == coedge)
		   m_dir = _computeDir(bloc, m_coedge_name);
    }
#endif

   /*------------------------------------------------------------------------*/
    /** \brief  Indique si la face est structurée
     */
    virtual bool isStructured() const {return true;}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    virtual TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
        if (m_method == points)
        	o << getMgx3DAlias() << ".BlockMeshingPropertyDirectional("
        	<< m_v1.getScriptCommand() << ", "
        	<< m_v2.getScriptCommand() << ")";
        else if (m_method == coedge)
        	o << getMgx3DAlias() << ".BlockMeshingPropertyDirectional(\""
        	<< m_coedge_name << "\")";
        return o;
    }
#endif

protected:
    /*------------------------------------------------------------------------*/
    /// méthode suivant laquelle est initialisée la direction
    initMethod m_method;

    /// Direction suivant laquelle est effectuée la discrétisation (relative au bloc)
    meshDirLaw m_dir;

    /// premier point pour déterminer la direction
    Utils::Math::Point m_v1;

    /// deuxième point pour déterminer la direction
    Utils::Math::Point m_v2;

    /// nom d'une arête du bloc pour déterminer la direction
    std::string m_coedge_name;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYDIRECTIONAL_H_ */
