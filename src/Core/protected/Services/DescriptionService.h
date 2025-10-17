#ifndef _DESCRIPTION_SERVICE_H_
#define _DESCRIPTION_SERVICE_H_

#include "Utils/Entity.h"
#include "Utils/SerializedRepresentation.h"
#include "Geom/GeomEntityVisitor.h"
#include "Group/GroupEntityVisitor.h"

namespace Mgx3D::Geom  { class GeomEntity;  }
namespace Mgx3D::Group { class GroupEntity; }

/**
 * - voir si alsoComputed est vraiment utile, pas réussi à comprendre comment Qt le passe à vrai
 *
 */
namespace Mgx3D::Services
{
    class DescriptionService : public Geom::ConstGeomEntityVisitor, public Group::ConstGroupEntityVisitor
    {
    public:
        template <typename T, typename = std::enable_if_t<std::is_base_of<Utils::Entity, T>::value>>
        static Utils::SerializedRepresentation* getDescription(const T* e, const bool alsoComputed)
        {
            DescriptionService ds(alsoComputed);
            e->accept(ds);
            return ds.m_representation;
        }

        DescriptionService(const bool alsoComputed);
        ~DescriptionService() override = default;

        void visit(const Geom::Vertex*) override;
        void visit(const Geom::Curve*) override;
        void visit(const Geom::Surface*) override;
        void visit(const Geom::Volume*) override;

        void visit(const Group::Group0D*) override;
        void visit(const Group::Group1D*) override;
        void visit(const Group::Group2D*) override;
        void visit(const Group::Group3D*) override;

    private:
        void visitEntity(const Utils::Entity*);
        void visitGeomEntity(const Geom::GeomEntity*);
        void visitGroupEntity(const Group::GroupEntity*);

        template <typename T>
        void buildSerializedRepresentation(Utils::SerializedRepresentation &description, const std::string &title, const Utils::EntitySet<T *> elements) const;
        template <typename T>
        void buildSerializedRepresentation(Utils::SerializedRepresentation &description, const std::string &title, const std::vector<T *> elements) const;

        bool m_also_computed;
        Utils::SerializedRepresentation *m_representation;
    };
}

#endif /* _DESCRIPTION_SERVICE_H_ */
