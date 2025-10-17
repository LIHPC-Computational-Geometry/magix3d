#ifndef MGX3D_GROUP_GROUP3D_H_
#define MGX3D_GROUP_GROUP3D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Internal { class Context; }
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 3D.
 * Permet l'accès aux entités 3D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
struct Group3D : public GroupEntity
{
    static const char* typeNameGroup3D;
    static const uint DIM = 3;

    void accept(ConstGroupEntityVisitor& v) const override {v.visit(this);}
    void accept(GroupEntityVisitor& v) override {v.visit(this);}
    Group3D(Internal::Context& ctx, const std::string & nom, bool isDefaultGroup, uint level=1);
    virtual ~Group3D() = default;
    int getDim() const override {return DIM;}
    std::string getTypeName() const override {return typeNameGroup3D;}
    Utils::Entity::objectType getType() const override {return Utils::Entity::Group3D;}
};
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP3D_H_ */
/*----------------------------------------------------------------------------*/
