#ifndef _DESCRIPTION_SERVICE_H_
#define _DESCRIPTION_SERVICE_H_

#include "Utils/Entity.h"
#include "Utils/SerializedRepresentation.h"
#include "Geom/GeomEntityVisitor.h"

namespace Mgx3D::Geom
{
    class GeomEntity;
}

/**
 * - voir si alsoComputed est vraiment utile, pas réussi à comprendre comment Qt le passe à vrai
 *
 */
namespace Mgx3D::Services
{
    class DescriptionService : public Geom::ConstGeomEntityVisitor
    {
    public:
        static Utils::SerializedRepresentation* describe(const Geom::GeomEntity* e, const bool alsoComputed);

        DescriptionService(const bool alsoComputed);
        ~DescriptionService() override = default;
        void visit(const Geom::Vertex*) override;
        void visit(const Geom::Curve*) override;
        void visit(const Geom::Surface*) override;
        void visit(const Geom::Volume*) override;

    private:
        void visitEntity(const Utils::Entity*);
        void visitGeomEntity(const Geom::GeomEntity*);

        template <typename T>
        void buildSerializedRepresentation(Utils::SerializedRepresentation &description, const std::string &title, const Utils::EntitySet<T *> elements) const;
        template <typename T>
        void buildSerializedRepresentation(Utils::SerializedRepresentation &description, const std::string &title, const std::vector<T *> elements) const;

        bool m_also_computed;
        Utils::SerializedRepresentation *m_representation;
    };
}

#endif /* _DESCRIPTION_SERVICE_H_ */
