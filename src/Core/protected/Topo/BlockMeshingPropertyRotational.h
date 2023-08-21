/*----------------------------------------------------------------------------*/
/*
 * \file BlockMeshingPropertyRotational.h
 *
 *  \author Eric Brière de l'Isle
 *
 *  \date 17/10/13
 */
/*----------------------------------------------------------------------------*/


#ifndef MGX3D_TOPO_BLOCKMESHINGPROPERTYROTATIONAL_H_
#define MGX3D_TOPO_BLOCKMESHINGPROPERTYROTATIONAL_H_
/*----------------------------------------------------------------------------*/
#include "Utils/Point.h"
#include "Topo/BlockMeshingProperty.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Topo {
/*----------------------------------------------------------------------------*/
/**
   @brief Propriété de la discrétisation d'un bloc suivant une direction par rotation
 */
class BlockMeshingPropertyRotational : public BlockMeshingProperty {

	typedef enum{
		points,
		coedge,
		dir
	} initMethod;

public:

    /*------------------------------------------------------------------------*/
    /// Constructeur avec 2 points qui définissent la direction et 2 autres points pour l'axe de rotation
    BlockMeshingPropertyRotational(Utils::Math::Point v1, Utils::Math::Point v2,
            Utils::Math::Point axis1, Utils::Math::Point axis2)
    : m_method(points), m_dir(dir_undef), m_v1(v1), m_v2(v2), m_axis1(axis1), m_axis2(axis2), m_coedge_name()
    {}

    /// Constructeur une direction définie et 2 points pour l'axe de rotation
    BlockMeshingPropertyRotational(meshDirLaw md,
            Utils::Math::Point axis1, Utils::Math::Point axis2)
    : m_method(dir), m_dir(md), m_v1(), m_v2(), m_axis1(axis1), m_axis2(axis2), m_coedge_name()
    {}

    /// Constructeur une arête et 2 points pour l'axe de rotation
    BlockMeshingPropertyRotational(std::string coedgeName,
    		Utils::Math::Point axis1, Utils::Math::Point axis2)
    : m_method(coedge), m_dir(dir_undef), m_v1(), m_v2(), m_axis1(axis1), m_axis2(axis2), m_coedge_name(coedgeName)
    {}

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /** Création d'un clone, on copie toutes les informations */
    BlockMeshingPropertyRotational* clone()  {return new BlockMeshingPropertyRotational(*this);}
#endif
    /*------------------------------------------------------------------------*/
    /// Accesseur sur la méthode de maillage
    BlockMeshingProperty::meshLaw getMeshLaw() const {return BlockMeshingProperty::rotational;}

    /*------------------------------------------------------------------------*/
    /// Accesseur sur le nom de la méthode de maillage
    std::string getMeshLawName() const
    {
        if (m_dir == dir_i)
            return "RotationI";
        else if (m_dir == dir_j)
            return "RotationJ";
        else if (m_dir == dir_k)
            return "RotationK";
        else
            return "Rotation à préciser";
    }

    /*------------------------------------------------------------------------*/
    /// Accesseur sur la direction associée à la CoFace
    uint getDir() const
    {
        if (m_dir == dir_i)
            return 0;
        else if (m_dir == dir_j)
            return 1;
        else if (m_dir == dir_k)
            return 2;
        else
            throw TkUtil::Exception(TkUtil::UTF8String ("BlockMeshingPropertyRotational::getDir() ne peut se faire sans initialiser la direction", TkUtil::Charset::UTF_8));
    }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
   /// initialisation de la direction en fonction de ce qui a été donné à la construction
   void initDir(Block* bloc)
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
    bool isStructured() const {return true;}

    /*------------------------------------------------------------------------*/
    /**  Retourne l'axe de rotation */
    void getAxis(Utils::Math::Point & axis1, Utils::Math::Point & axis2)
    {
        axis1 = m_axis1;
        axis2 = m_axis2;
    }
    /*------------------------------------------------------------------------*/
    /**  Change l'axe de rotation */
    void setAxis(Utils::Math::Point & axis1, Utils::Math::Point & axis2)
    {
        m_axis1 = axis1;
        m_axis2 = axis2;
    }

    /*------------------------------------------------------------------------*/
#ifndef SWIG
    /// Script pour la commande de création Python
    TkUtil::UTF8String getScriptCommand() const
    {
        TkUtil::UTF8String o (TkUtil::Charset::UTF_8);
    	if (m_method == points)
    		o << getMgx3DAlias() << ".BlockMeshingPropertyRotational("
    		<< m_v1.getScriptCommand() << ", "
    		<< m_v2.getScriptCommand() << ", "
    		<< m_axis1.getScriptCommand() << ", "
    		<< m_axis2.getScriptCommand() << ")";
    	else if (m_method == coedge)
    		o << getMgx3DAlias() << ".BlockMeshingPropertyRotational(\""
    		<< m_coedge_name <<"\", "
    		<< m_axis1.getScriptCommand() << ", "
    		<< m_axis2.getScriptCommand() << ")";
    	return o;
    }
#endif

private:
    /*------------------------------------------------------------------------*/
    /// méthode suivant laquelle est initialisée la direction
    initMethod m_method;

    /// Direction suivant laquelle est effectuée la discrétisation (relative au bloc)
    meshDirLaw m_dir;

    /// premier point pour déterminer la direction de la rotation
    Utils::Math::Point m_v1;

    /// deuxième point pour déterminer la direction de la rotation
    Utils::Math::Point m_v2;

    /// premier point pour l'axe de rotation de la méthode "rotation"
    Utils::Math::Point m_axis1;

    /// deuxième point pour l'axe de rotation de la méthode "rotation"
    Utils::Math::Point m_axis2;

    /// nom d'une arête du bloc pour déterminer la direction
    std::string m_coedge_name;
};
/*----------------------------------------------------------------------------*/
} // end namespace Topo
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
/*----------------------------------------------------------------------------*/

#endif /* MGX3D_TOPO_BLOCKMESHINGPROPERTYROTATIONAL_H_ */
