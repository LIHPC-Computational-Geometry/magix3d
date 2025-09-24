#include "Services/DescriptionService.h"
#include "Utils/SerializedRepresentation.h"
#include "Geom/IncidentGeomEntitiesVisitor.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/CoFace.h"
#include "Topo/Block.h"
#include "Group/Group0D.h"
#include "Group/Group1D.h"
#include "Group/Group2D.h"
#include "Group/Group3D.h"
#include <TkUtil/NumericServices.h>
#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>

using Property = Mgx3D::Utils::SerializedRepresentation::Property;

namespace Mgx3D::Services
{
	Utils::SerializedRepresentation *DescriptionService::describe(const Geom::GeomEntity *e, const bool alsoComputed)
	{
		DescriptionService ds(alsoComputed);
		e->accept(ds);
		return ds.m_representation;
	}

	DescriptionService::DescriptionService(const bool alsoComputed)
		: m_also_computed(alsoComputed), m_representation(nullptr)
	{
	}

	void DescriptionService::visit(const Geom::Vertex *e)
	{
		visitGeomEntity(e);
		std::vector<double> coords;
		coords.push_back(e->getX());
		coords.push_back(e->getY());
		coords.push_back(e->getZ());
		Property coordsProp("Coordonnées", coords);
		Utils::SerializedRepresentation propertyGeomDescription("Propriété géométrique", coordsProp.getValue());
		propertyGeomDescription.addProperty(coordsProp);

#ifdef _DEBUG // Issue#111
		// précision OpenCascade ou autre
		TkUtil::UTF8String precStr(TkUtil::Charset::UTF_8);
		precStr << BRep_Tool::Tolerance(e->getOCCVertex());

		propertyGeomDescription.addProperty(Property("Précision", precStr.ascii()));
#endif // _DEBUG

		m_representation->addPropertiesSet(propertyGeomDescription);
		m_representation->setSummary(coordsProp.getValue());
	}

	void DescriptionService::visit(const Geom::Curve *e)
	{
		visitGeomEntity(e);
		Utils::SerializedRepresentation propertyGeomDescription("Propriétés géométriques", "");

		if (true == m_also_computed)
		{
			// recuperation de la longueur
			TkUtil::UTF8String volStr(TkUtil::Charset::UTF_8);
			volStr << e->getArea();
			propertyGeomDescription.addProperty(Property("Longueur", volStr.ascii()));
		}

		auto occ_edges = e->getOCCEdges();
#ifdef _DEBUG // Issue#111
		// précision OpenCascade
		for (uint i = 0; i < occ_edges.size(); i++)
		{
			TkUtil::UTF8String precStr(TkUtil::Charset::UTF_8);
			precStr << BRep_Tool::Tolerance(occ_edges[i]);
			propertyGeomDescription.addProperty(Property("Précision", precStr.ascii()));
		}
#endif // _DEBUG

		// on ajoute des infos du style: c'est une droite, un arc de cercle, une ellipse, une b-spline
		TkUtil::UTF8String typeStr(TkUtil::Charset::UTF_8);
		bool isABSpline = false;
		if (e->isLinear())
			typeStr << "segment";
		else if (e->isCircle())
			typeStr << "cercle";
		else if (e->isEllipse())
			typeStr << "ellipse";
		else if (e->isBSpline())
		{
			isABSpline = true;
			typeStr << "b-spline";
		}
		else if (occ_edges.size() > 1)
			typeStr << "composée";
		else
			typeStr << "quelconque";

		propertyGeomDescription.addProperty(Property("Type", typeStr));

#ifdef _DEBUG
		if (isABSpline)
		{
			TkUtil::UTF8String nbStr1(TkUtil::Charset::UTF_8);
			TkUtil::UTF8String nbStr2(TkUtil::Charset::UTF_8);

			// c'est une spline => une seule représentation de type Edge
			TopoDS_Edge edge = occ_edges[0];
			Standard_Real first_param, last_param;
			Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);

			Handle(Geom_BSplineCurve) bspline = Handle(Geom_BSplineCurve)::DownCast(curve);

			nbStr1 << (long int)bspline->NbKnots();
			nbStr2 << (long int)bspline->NbPoles();

			propertyGeomDescription.addProperty(
				Property("Nb noeuds", nbStr1.ascii()));
			propertyGeomDescription.addProperty(
				Property("Nb poles", nbStr2.ascii()));
		}

		// affichage des paramètres extrémas
		double first, last;
		e->getParameters(first, last);
		TkUtil::UTF8String param1(TkUtil::Charset::UTF_8);
		param1 << first;
		TkUtil::UTF8String param2(TkUtil::Charset::UTF_8);
		param2 << last;
		propertyGeomDescription.addProperty(Property("Param first", param1.ascii()));
		propertyGeomDescription.addProperty(Property("Param last", param2.ascii()));
#endif

		m_representation->addPropertiesSet(propertyGeomDescription);
	}

	void DescriptionService::visit(const Geom::Surface *e)
	{
		visitGeomEntity(e);
		Utils::SerializedRepresentation propertyGeomDescription("Propriétés géométriques", "");

		if (true == m_also_computed)
		{
			// recuperation de l'aire
			TkUtil::UTF8String volStr(TkUtil::Charset::UTF_8);
			volStr << e->getArea();
			propertyGeomDescription.addProperty(Property("Aire", volStr.ascii()));
		}

		auto occ_faces = e->getOCCFaces();
#ifdef _DEBUG // Issue#111
			  // précision OpenCascade ou autre
		for (uint i = 0; i < occ_faces.size(); i++)
		{
			TkUtil::UTF8String precStr(TkUtil::Charset::UTF_8);
			precStr << BRep_Tool::Tolerance(occ_faces[i]);
			propertyGeomDescription.addProperty(Property("Précision", precStr.ascii()));
		}
#endif // _DEBUG

		// on ajoute des infos du style: c'est un plan
		TkUtil::UTF8String typeStr(TkUtil::Charset::UTF_8);
		if (e->isPlanar())
			typeStr << "plan";
		else if (occ_faces.size() > 1)
			typeStr << "composée";
		else
			typeStr << "quelconque";

		propertyGeomDescription.addProperty(Property("Type", typeStr.ascii()));
		m_representation->addPropertiesSet(propertyGeomDescription);
	}

	void DescriptionService::visit(const Geom::Volume *e)
	{
		visitGeomEntity(e);
		Utils::SerializedRepresentation propertyGeomDescription("Propriétés géométriques", "");

		if (true == m_also_computed)
		{
			// récupération du volume
			TkUtil::UTF8String volStr(TkUtil::Charset::UTF_8);
			volStr << e->getArea();
			propertyGeomDescription.addProperty(Property("Volume", volStr.ascii()));
		}

		// pour afficher les spécifictés de certaines propriétés
		e->getGeomProperty()->addDescription(propertyGeomDescription);
		m_representation->addPropertiesSet(propertyGeomDescription);
	}

	void DescriptionService::visitEntity(const Utils::Entity *e)
	{
		m_representation = e->Utils::Entity::getDescription(m_also_computed);
	}

	void DescriptionService::visitGeomEntity(const Geom::GeomEntity *e)
	{
		visitEntity(e);
		Utils::SerializedRepresentation relationsGeomDescription("Relations géométriques", "");
		Geom::GetUpIncidentGeomEntitiesVisitor vup;
		Geom::GetDownIncidentGeomEntitiesVisitor vdown;
		Geom::GetAdjacentGeomEntitiesVisitor vadj;
		e->accept(vup);
		e->accept(vdown);
		e->accept(vadj);

		// On y ajoute les éléments géométriques en relation avec celui-ci :
		auto v = vdown.getVertices();
		v.insert(vadj.getVertices().begin(), vadj.getVertices().end());
		buildSerializedRepresentation(relationsGeomDescription, "Sommets", v);
		TkUtil::UTF8String summary(TkUtil::Charset::UTF_8);
		for (auto vert : v)
			summary << vert->getName() << " ";
		m_representation->setSummary(summary.ascii());

		auto c = vup.getCurves();
		c.insert(vdown.getCurves().begin(), vdown.getCurves().end());
		c.insert(vadj.getCurves().begin(), vadj.getCurves().end());
		buildSerializedRepresentation(relationsGeomDescription, "Courbes", c);

		auto s = vup.getSurfaces();
		s.insert(vdown.getSurfaces().begin(), vdown.getSurfaces().end());
		s.insert(vadj.getSurfaces().begin(), vadj.getSurfaces().end());
		buildSerializedRepresentation(relationsGeomDescription, "Surfaces", s);

		auto vol = vup.getVolumes();
		vol.insert(vadj.getVolumes().begin(), vadj.getVolumes().end());
		buildSerializedRepresentation(relationsGeomDescription, "Volumes", vol);

		m_representation->addPropertiesSet(relationsGeomDescription);

		// la Topologie s'il y en a une
		std::vector<Topo::TopoEntity *> topo_entities = e->getRefTopo();
		if (!topo_entities.empty())
		{
			Utils::SerializedRepresentation topoDescription("Relations topologiques", "");

			std::vector<Topo::Vertex *> vertices;
			std::vector<Topo::CoEdge *> coedges;
			std::vector<Topo::CoFace *> cofaces;
			std::vector<Topo::Block *> blocks;

			for (Topo::TopoEntity *topo : topo_entities)
			{
				if (topo->getDim() == 0)
					vertices.push_back(dynamic_cast<Topo::Vertex *>(topo));
				else if (topo->getDim() == 1)
					coedges.push_back(dynamic_cast<Topo::CoEdge *>(topo));
				else if (topo->getDim() == 2)
					cofaces.push_back(dynamic_cast<Topo::CoFace *>(topo));
				else if (topo->getDim() == 3)
					blocks.push_back(dynamic_cast<Topo::Block *>(topo));
			}

			buildSerializedRepresentation(topoDescription, "Sommets topologiques", vertices);
			buildSerializedRepresentation(topoDescription, "Arêtes topologiques", coedges);
			buildSerializedRepresentation(topoDescription, "Faces topologiques", cofaces);
			buildSerializedRepresentation(topoDescription, "Blocs topologiques", blocks);

			m_representation->addPropertiesSet(topoDescription);
		}
		else
		{
			m_representation->addProperty(Property("Relations topologiques", std::string("Aucune")));
		}

		// Les groupes s'il y en a
		std::vector<Group::GroupEntity *> groups;
		e->getGroups(groups);
		if (!groups.empty())
		{
			Utils::SerializedRepresentation groupeDescription("Relations vers des groupes", "");

			std::vector<Group::Group0D *> g0;
			std::vector<Group::Group1D *> g1;
			std::vector<Group::Group2D *> g2;
			std::vector<Group::Group3D *> g3;

			for (Group::GroupEntity *grp : groups)
			{
				if (grp->getDim() == 0)
					g0.push_back(dynamic_cast<Group::Group0D *>(grp));
				else if (grp->getDim() == 1)
					g1.push_back(dynamic_cast<Group::Group1D *>(grp));
				else if (grp->getDim() == 2)
					g2.push_back(dynamic_cast<Group::Group2D *>(grp));
				else if (grp->getDim() == 3)
					g3.push_back(dynamic_cast<Group::Group3D *>(grp));
			}

			buildSerializedRepresentation(groupeDescription, "Groupes 0D", g0);
			buildSerializedRepresentation(groupeDescription, "Groupes 1D", g1);
			buildSerializedRepresentation(groupeDescription, "Groupes 2D", g2);
			buildSerializedRepresentation(groupeDescription, "Groupes 3D", g3);

			m_representation->addPropertiesSet(groupeDescription);
			groupeDescription.setSummary(TkUtil::NumericConversions::toStr(groups.size()));
		}
		else
		{
			m_representation->addProperty(Property("Relations vers des groupes", std::string("Aucune")));
		}
#ifdef _DEBUG
		Utils::SerializedRepresentation occGeomDescription("Propriétés OCC", "");
		// observation du HashCode retourné par OCC

		auto add_description = [&](const TopoDS_Shape &sh)
		{
			int hc = sh.HashCode(INT_MAX);
			occGeomDescription.addProperty(Property("HashCode", (long int)hc));
		};
		e->apply(add_description);
		m_representation->addPropertiesSet(occGeomDescription);
#endif
	}

	template <typename T>
	void DescriptionService::
		buildSerializedRepresentation(Utils::SerializedRepresentation &description, const std::string &title, const Utils::EntitySet<T *> elements) const
	{
		if (!elements.empty())
		{
			Utils::SerializedRepresentation sr(title, TkUtil::NumericConversions::toStr(elements.size()));
			for (auto elt : elements)
				sr.addProperty(Property(elt->getName(), *elt));
			description.addPropertiesSet(sr);
		}
	}

	template <typename T>
	void DescriptionService::
		buildSerializedRepresentation(Utils::SerializedRepresentation &description, const std::string &title, const std::vector<T *> elements) const
	{
		if (!elements.empty())
		{
			Utils::SerializedRepresentation sr(title, TkUtil::NumericConversions::toStr(elements.size()));
			for (auto elt : elements)
				sr.addProperty(Property(elt->getName(), *elt));
			description.addPropertiesSet(sr);
		}
	}

}
