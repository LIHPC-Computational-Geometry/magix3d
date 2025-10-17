#ifndef MGX3D_GROUP_GROUP0D_H_
#define MGX3D_GROUP_GROUP0D_H_
/*----------------------------------------------------------------------------*/
#include "Group/GroupEntity.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Internal { class Context; }
/*----------------------------------------------------------------------------*/
namespace Mgx3D::Group {
/*----------------------------------------------------------------------------*/
/**
 * \brief Groupe 0D.
 * Permet l'accès aux entités 0D appartenant à un même groupe, ainsi qu'aux entités
 * de niveaux inférieurs
 */
struct Group0D : public GroupEntity
{
    static const char* typeNameGroup0D;
    static const uint DIM = 0;

    void accept(ConstGroupEntityVisitor& v) const override {v.visit(this);}
    void accept(GroupEntityVisitor& v) override {v.visit(this);}
    Group0D(Internal::Context& ctx, const std::string & nom, bool isDefaultGroup, uint level=1);
    virtual ~Group0D() = default;
    int getDim() const override {return DIM;}
    std::string getTypeName() const override {return typeNameGroup0D;}
    Utils::Entity::objectType getType() const override {return Utils::Entity::Group0D;}
};
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D::Group
/*----------------------------------------------------------------------------*/
#endif /* MGX3D_GROUP_GROUP0D_H_ */
/*----------------------------------------------------------------------------*/
