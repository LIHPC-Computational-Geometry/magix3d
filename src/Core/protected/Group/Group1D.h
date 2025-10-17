#ifndef MGX3D_GROUP_GROUP1D_H_
#define MGX3D_GROUP_GROUP1D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Internal { class Context; }
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 1D.
 * Permet l'accès aux entités 1D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
struct Group1D : public GroupEntity
{
    static const char* typeNameGroup1D;
    static const uint DIM = 1;

    void accept(ConstGroupEntityVisitor& v) const override {v.visit(this);}
    void accept(GroupEntityVisitor& v) override {v.visit(this);}
    Group1D(Internal::Context& ctx, const std::string & nom, bool isDefaultGroup, uint level=1);
    virtual ~Group1D() = default;
    int getDim() const override {return DIM;}
    std::string getTypeName() const override {return typeNameGroup1D;}
    Utils::Entity::objectType getType() const override {return Utils::Entity::Group1D;}
};
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP1D_H_ */
/*----------------------------------------------------------------------------*/
