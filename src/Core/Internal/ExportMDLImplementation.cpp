/*----------------------------------------------------------------------------*/
#ifdef USE_MDLPARSER
/*----------------------------------------------------------------------------*/
#include "Internal/Context.h"
#include "Internal/ExportMDLImplementation.h"
#include "Internal/InfoCommand.h"
#include "Geom/GeomManager.h"
#include "Geom/GeomDisplayRepresentation.h"
#include "Geom/Vertex.h"
#include "Geom/Curve.h"
#include "Geom/Surface.h"
#include "Topo/Vertex.h"
#include "Topo/CoEdge.h"
#include "Topo/Edge.h"
#include "Topo/CoFace.h"
#include "Utils/Point.h"
#include "Utils/Unit.h"
#include "Group/GroupManager.h"
/*----------------------------------------------------------------------------*/
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
/*----------------------------------------------------------------------------*/
#include <TkUtil/Exception.h>
#include <TkUtil/TraceLog.h>
#include <TkUtil/UTF8String.h>
#include <sstream>
#include <iostream>
#include <vector>
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
/*----------------------------------------------------------------------------*/
namespace Internal {
//#define _DEBUG_EXPORT
//#define _DEBUG_EXPORT2
/*----------------------------------------------------------------------------*/
ExportMDLImplementation::
ExportMDLImplementation(Internal::Context& c, std::vector<Geom::GeomEntity*>& ge, const std::string& n)
: m_context(c), m_filename(n), m_geomEntities(ge)
{

}
/*----------------------------------------------------------------------------*/
ExportMDLImplementation::
~ExportMDLImplementation()
{
#ifdef _DEBUG_EXPORT
    std::cout<<"Destruction de ExportMDLImplementation"<<std::endl;
#endif
}
/*----------------------------------------------------------------------------*/
void ExportMDLImplementation::perform(Internal::InfoCommand* icmd)
{
    if (icmd == 0)
    	throw TkUtil::Exception(TkUtil::UTF8String ("Erreur interne: InfoCommand non initialisé", TkUtil::Charset::UTF_8));

#ifdef _DEBUG_EXPORT
    std::cout<<"ExportMDLImplementation::perform en cours "<<std::endl;
#endif

    mdl_info.nb_error         = 0;
    mdl_info.error_length     = 0;
    mdl_info.error_str        = 0;
    mdl_info.version          = 1;
    mdl_info.create_date      = 0;
    mdl_info.last_modif_date  = 0;
    mdl_info.length_unit      = 0;
    if (m_context.getLengthUnit ( ) != Utils::Unit::undefined)
    	mdl_info.length_unit = m_context.getLengthUnit ( );
    mdl_info.title            = "Exportation depuis Magix3D";
    mdl_info.comment          = "";
    mdl_info.nb_commands      = 0;
    mdl_info.commands         = 0;

    for (uint i=0; i<IdObLast; i++)
        m_obs_id[i] = 0;

    for (std::vector<Geom::GeomEntity*>::iterator iter = m_geomEntities.begin();
            iter != m_geomEntities.end(); ++iter){
        Geom::GeomEntity* ge = *iter;
#ifdef _DEBUG_EXPORT
    std::cout<<"ge = "<<ge->getName()<<std::endl;
#endif
        if (ge->getDim() == 0){
            Geom::Vertex* ve = dynamic_cast<Geom::Vertex*>(ge);
            save(ve);
        }
        else if (ge->getDim() == 1){
            Geom::Curve* cv = dynamic_cast<Geom::Curve*>(ge);
            save(cv);
        }
        else if (ge->getDim() == 2){
            Geom::Surface* sf = dynamic_cast<Geom::Surface*>(ge);
            save(sf);
        }

    } // end for iter = m_geomEntities.begin()

#ifdef _DEBUG_EXPORT2
    std::cout<<"Les commandes: "<<std::endl;
    for(uint i=0; i<mdl_info.nb_commands; i++ ){
        T_MdlCommand &cur = mdl_info.commands[i];
        std::cout<<" cmd["<<i<<"] name "<<cur.name<<", type "<<cur.type<<std::endl;
    }
#endif

#ifdef _DEBUG_EXPORT
    std::cout<<"MdlWrite dans "<<m_filename<<std::endl;
#endif
    int ret = ::MdlWrite(m_filename.c_str(), &mdl_info);
    if (ret){
		TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
        messErr << "Erreur lors de l'écriture du fichier au format Mdl: "<<m_filename;
        throw TkUtil::Exception(messErr);
    }

#ifdef _DEBUG_EXPORT
    std::cout<<"Nettoyage mémoire"<<std::endl;
#endif
    // Liberation des structures allouees.
    for(uint i=0; i<mdl_info.nb_commands; i++ ){
      T_MdlCommand &cur = mdl_info.commands[i];
      if (cur.type==MdlLineSup){
        if (cur.u.linesup.x) delete[] cur.u.linesup.x; cur.u.linesup.x = 0;
        if (cur.u.linesup.y) delete[] cur.u.linesup.y; cur.u.linesup.y = 0;
      }
      if (cur.type==MdlArea){
        if (cur.u.area.data) delete[] cur.u.area.data; cur.u.area.data = 0;
      }
    }

    mdl_cmd_list.clear();
    mdl_info.nb_commands = 0;
    mdl_info.commands    = 0;


}
/*----------------------------------------------------------------------------*/
uint ExportMDLImplementation::
save(Geom::Vertex* ve)
{
    if (ve == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur lors de l'écriture du fichier au format Mdl, une entité n'a pas été convertie en Vertex", TkUtil::Charset::UTF_8));

    if (m_cor_uniqueId_mdlId[ve->getUniqueId()] == 0){

        T_MdlCommand& cmd = nextCommand(ve->getName(), MdlPoint, m_obs_id[IdObPoint]++);

        // on stocke l'id+1 pour distinguer les cas où ce n'est pas défini (donc à 0)
        m_cor_uniqueId_mdlId[ve->getUniqueId()] = m_obs_id[IdObPoint];

        cmd.u.point.cd.x1 = ve->getX();
        cmd.u.point.cd.x2 = ve->getY();
        cmd.u.point.cd.is_polaire = 0;
#ifdef _DEBUG_EXPORT
        std::cout<<"save ve = "<<ve->getName()<<" en "<<ve->getX()<<", "<<ve->getY()<<" ( => "<<m_cor_uniqueId_mdlId[ve->getUniqueId()]-1<<")"<<std::endl;
#endif
    }
    else {
#ifdef _DEBUG_EXPORT
        std::cout<<"save (déjà vu) ve = "<<ve->getName()<<" en "<<ve->getX()<<", "<<ve->getY()<<" ( => "<<m_cor_uniqueId_mdlId[ve->getUniqueId()]-1<<")"<<std::endl;
#endif
    }

    return m_cor_uniqueId_mdlId[ve->getUniqueId()]-1;
}
/*----------------------------------------------------------------------------*/
uint ExportMDLImplementation::
save(Utils::Math::Point pt, std::string& nom)
{
    T_MdlCommand& cmd = nextCommand(nom, MdlPoint, m_obs_id[IdObPoint]++);

    cmd.u.point.cd.x1 = pt.getX();
    cmd.u.point.cd.x2 = pt.getY();
    cmd.u.point.cd.is_polaire = 0;
#ifdef _DEBUG_EXPORT
    std::cout<<"save pt = "<<nom<<" en "<<pt.getX()<<", "<<pt.getY()<<std::endl;
#endif

    return m_obs_id[IdObPoint]-1;
}
/*----------------------------------------------------------------------------*/
uint ExportMDLImplementation::
save(Geom::Curve* cv)
{
    if (cv == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur lors de l'écriture du fichier au format Mdl, une entité n'a pas été convertie en Curve", TkUtil::Charset::UTF_8));

    if (m_cor_uniqueId_mdlId[cv->getUniqueId()] == 0){
        if (cv->isLinear()){
            auto vertices = cv->getVertices();
            if (vertices.size() != 2){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur lors de l'écriture du fichier au format Mdl, entité "<<cv->getName()<<" a autre chose que 2 sommets ("<<vertices.size()<<" dans notre cas)";
                throw TkUtil::Exception(messErr);
            }

            uint idP1 = save(vertices[0]);
            uint idP2 = save(vertices[1]);

            T_MdlCommand& cmd = nextCommand(cv->getName(), MdlSegment, m_obs_id[IdObSegment]++);
            m_cor_uniqueId_commandId[cv->getUniqueId()] = mdl_info.nb_commands-1;

            // on stocke l'id+1 pour distinguer les cas où ce n'est pas défini (donc à 0)
            m_cor_uniqueId_mdlId[cv->getUniqueId()] = m_obs_id[IdObSegment];

            cmd.model1d.pt1 = idP1;
            cmd.model1d.pt2 = idP2;
#ifdef _DEBUG_EXPORT
            std::cout<<"save cv = "<<cv->getName()
                    <<" entre "<<vertices[0]->getName()<<" ("<<idP1
                    <<") et "<<vertices[1]->getName()<<" ("<<idP2<<")"
                    <<" ( => "<<m_cor_uniqueId_mdlId[cv->getUniqueId()]-1<<")"
                    <<std::endl;
#endif
        }
        else if (cv->isCircle() || cv->isEllipse()){
            auto vertices = cv->getVertices();
            if (vertices.size() != 2){
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
                messErr << "Erreur lors de l'écriture du fichier au format Mdl, entité "<<cv->getName()<<" a autre chose que 2 sommets ("<<vertices.size()<<" dans notre cas)";
                messErr << "\nAttention, le format MDL ne supporte pas les cercles fermés";
                throw TkUtil::Exception(messErr);
            }

            uint idP1 = save(vertices[0]);
            uint idP2 = save(vertices[1]);
            std::string centerName = cv->getName()+"Center";

            // 1 seule représentation pour une courbe ou une ellipse
            TopoDS_Edge edge = cv->getOCCEdges()[0];
            Utils::Math::Point center = getCenter(edge);

            uint idP3 = save(center, centerName);

            T_MdlCommand& cmd = nextCommand(cv->getName(), MdlEllipse, m_obs_id[IdObCurve]++);
            m_cor_uniqueId_commandId[cv->getUniqueId()] = mdl_info.nb_commands-1;

            // on stocke l'id+1 pour distinguer les cas où ce n'est pas défini (donc à 0)
            m_cor_uniqueId_mdlId[cv->getUniqueId()] = m_obs_id[IdObCurve];

            cmd.model1d.pt1 = idP1;
            cmd.model1d.pt2 = idP2;
            cmd.u.arc.center = idP3;
            cmd.u.arc.is_circle = cv->isCircle();

            // on part de l'hypothèse que les arc de cercles font moins d'un 1/2 tour
            // on utilise le produit vectoriel pour donner une direction
            Utils::Math::Vector v1(vertices[0]->getCoord()-center);
            Utils::Math::Vector v2(vertices[1]->getCoord()-center);
            Utils::Math::Vector v3 = v1*v2;
            cmd.u.arc.direction = (v3.getZ()>=0.0);

#ifdef _DEBUG_EXPORT
            std::cout<<"save cv = "<<cv->getName()
                    <<" entre "<<vertices[0]->getName()<<" ("<<idP1
                    <<") et "<<vertices[1]->getName()<<" ("<<idP2<<")"
                    <<" centré sur "<<centerName
                    <<std::endl;
#endif
        }
        else if (cv->isBSpline()){

            // on utilise la DisplayRepresentation pour avoir un minimum de sommets
            // qui décrivent le contour (NB: deflexion de 0.01 non utilisée)
            Geom::GeomDisplayRepresentation rep(Utils::DisplayRepresentation::WIRE, 0.01);

            cv->getRepresentation(rep, true);
            std::vector<Utils::Math::Point>&   points      = rep.getPoints ( );

            auto vertices = cv->getVertices();

            std::string nomSupport = std::string("Su")+cv->getName();
            T_MdlCommand& cmd = nextCommand(nomSupport, MdlLineSup, m_obs_id[IdObLineSup]++);

//            // on stocke l'id+1 pour distinguer les cas où ce n'est pas défini (donc à 0)
//            m_cor_uniqueId_mdlId[cv->getUniqueId()] = m_obs_id[IdObLineSup];

            cmd.u.linesup.is_region = false;
            cmd.u.linesup.nb = points.size();
            cmd.u.linesup.x  = new double[points.size()];
            cmd.u.linesup.y  = new double[points.size()];
            for(size_t j=0; j<points.size(); j++ ){
                cmd.u.linesup.x[j] = points[j].getX();
                cmd.u.linesup.y[j] = points[j].getY();
            }
#ifdef _DEBUG_EXPORT
            std::cout<<"save support pour cv = "<<nomSupport
                    <<" avec "<<points.size()<<" points"
                    <<" et "<<vertices.size()<<" sommets"
                    <<std::endl;

            std::cout<<" vertices[0]->getPoint(): "<<vertices[0]->getPoint()<<std::endl;
            std::cout<<" vertices.back()->getPoint(): "<<vertices.back()->getPoint()<<std::endl;

            std::cout<<" points[0]: "<<points[0]<<std::endl;
            std::cout<<" points.back(): "<<points.back()<<std::endl;

#endif

            // vérification que les points sont dans l'ordre de parcours de la courbe
            double same_dir = true;
            if (vertices[0]->getPoint().isEpsilonEqual(points[0], Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
            		&& vertices.back()->getPoint().isEpsilonEqual(points.back(), Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
            	same_dir = true;
            else if (vertices[0]->getPoint().isEpsilonEqual(points.back(), Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon)
            		&& vertices.back()->getPoint().isEpsilonEqual(points[0], Utils::Math::MgxNumeric::mgxGeomDoubleEpsilon))
            	same_dir = false;
            else {
				TkUtil::UTF8String	messErr (TkUtil::Charset::UTF_8);
            	messErr << "Erreur lors de l'écriture du fichier au format Mdl, entité "<<cv->getName()
            			<<", on ne retrouve pas les sommets aux extrémités parmis les points de la représentation";
            	throw TkUtil::Exception(messErr);
            }


            // création d'une ligne qui s'appuie sur ce support
            uint idP1 = save(same_dir?vertices[0]:vertices.back());
            uint idP2 = save(same_dir?vertices.back():vertices[0]);
            T_MdlCommand& cmd2   = nextCommand(cv->getName(),MdlLine,m_obs_id[IdObLine]++);
            m_cor_uniqueId_commandId[cv->getUniqueId()] = mdl_info.nb_commands-1;

            cmd2.model1d.pt1 = idP1;
            cmd2.model1d.pt2 = idP2;
            cmd2.u.line.line  = m_obs_id[IdObLineSup]-1;
            cmd2.u.line.nbegin = 2;
            cmd2.u.line.nend   = points.size()-1;

            // on stocke l'id+1 pour distinguer les cas où ce n'est pas défini (donc à 0)
            m_cor_uniqueId_mdlId[cv->getUniqueId()] = m_obs_id[IdObLine];

#ifdef _DEBUG_EXPORT
            std::cout<<"save cv = "<<cv->getName()
                    <<" entre "<<vertices[0]->getName()<<" ("<<idP1
                    <<") et "<<vertices.back()->getName()<<" ("<<idP2<<")"
                    <<" qui pointe sur "<<nomSupport<<" id "<<cmd2.u.line.line
                    <<" de "<<cmd2.u.line.nbegin<<" à "<<cmd2.u.line.nend
                    <<" ( => "<<m_cor_uniqueId_mdlId[cv->getUniqueId()]-1<<")"
                    <<std::endl;
#endif

        }
        else {
            std::cerr<<"ATTENTION, la courbe "<<cv->getName()<<" n'est pas prise en charge"<<std::endl;
        }
    } // end if (m_cor_uniqueId_mdlId[cv->getUniqueId()] == 0)

    return m_cor_uniqueId_mdlId[cv->getUniqueId()]-1;
}
/*----------------------------------------------------------------------------*/
uint ExportMDLImplementation::
save(Geom::Surface* sf)
{
#ifdef _DEBUG_EXPORT
    std::cout<<"ExportMDLImplementation::save("<<sf->getName()<<")"<<std::endl;
#endif

    if (sf == 0)
        throw TkUtil::Exception(TkUtil::UTF8String ("Erreur lors de l'écriture du fichier au format Mdl, une entité n'a pas été convertie en Surface", TkUtil::Charset::UTF_8));

    if (m_cor_uniqueId_mdlId[sf->getUniqueId()] == 0){
        auto curves = sf->getCurves();
        uint nb_cd = curves.size();

        // pour être certain de la la sauvegarde des courbes
        for (uint i=0; i<nb_cd; i++)
            save(curves[i]);

        T_MdlCommand& cmd = nextCommand(sf->getName(), MdlArea, m_obs_id[IdObArea]++);
        // on stocke l'id+1 pour distinguer les cas où ce n'est pas défini (donc à 0)
        m_cor_uniqueId_mdlId[sf->getUniqueId()] = m_obs_id[IdObArea];

        cmd.u.area.nb_cut_data = nb_cd;
//        std::cout<<"nb_cd = "<<nb_cd<<std::endl;
        cmd.u.area.algo_mesh = "Delaunay";
        cmd.u.area.algo_smooth = 0;
        Group::GroupManager& gm = m_context.getGroupManager();
        std::vector<Group::GroupEntity*> grps = gm.getGroupsFor(sf);
        cmd.u.area.groups_name = 0;
        if (!grps.empty()){
            TkUtil::UTF8String grps_str (TkUtil::Charset::UTF_8);
            for (uint i=0; i<grps.size(); i++){
                if (i)
                    grps_str << " ";
                grps_str << grps[i]->getName();
            }
            char* grps_ch = new char[grps_str.length()+1];
            cmd.u.area.groups_name = grps_ch;
            strcpy(grps_ch, grps_str.iso().c_str());
            grps_ch[grps_str.length()] = '\0';
        }

        cmd.u.area.algo_mesh_options = 0;
        cmd.u.area.algo_smooth_options = 0;

        cmd.u.area.opts[0] = false;//OptInArm
        cmd.u.area.opts[1] = true;//OptOutArm
        cmd.u.area.opts[2] = true;//OptMesh
        cmd.u.area.opts[3] = true;//OptOGroup,
        cmd.u.area.opts[4] = false;//OptCutLeft
        cmd.u.area.opts[5] = false;//OptCutRight
        cmd.u.area.opts[6] = false;//OptSemiConfAfter
        cmd.u.area.opts[7] = false;//OptTriMeshOneNode


        cmd.u.area.data = new T_MdlCutData[nb_cd];

        for( size_t i=0; i<nb_cd; i++ ){

            T_MdlCutData  &data = cmd.u.area.data[i];


            data.name       = curves[i]->getName().c_str();
//std::cout<<"================== "<<i<<"==========="<<std::endl;
//std::cout<<"name = "<<data.name<<std::endl;
            data.id         = save(curves[i]);
//std::cout<<"id = "<<data.id<<std::endl;
            data.cmd_type   = (_MdlTypeCommand) 0;
            if (curves[i]->isLinear())
                data.cmd_type   = (_MdlTypeCommand) 0;
            else if (curves[i]->isCircle() || curves[i]->isEllipse())
                data.cmd_type   = (_MdlTypeCommand) 1;
            else if (curves[i]->isBSpline())
                data.cmd_type   = (_MdlTypeCommand) 2;

            data.command_id = m_cor_uniqueId_commandId[curves[i]->getUniqueId()];
//std::cout<<"command_id = "<<data.command_id<<std::endl;
            data.logical_id = i+1;
//std::cout<<"logical_id = "<<data.logical_id<<std::endl;
            data.nb_mesh    = 10;
            data.type       = MdlCutUniform;
            data.prog1      = 1.0;
            data.prog2      = 1.0;
            data.arm1       = 1.0;
            data.arm2       = 1.0;
            data.pole       = -1;
            data.is_inverse = false;
            data.is_dup_nodes = false;


        } // end for i

    } // end if (m_cor_uniqueId_mdlId[sf->getUniqueId()] == 0)

    return m_cor_uniqueId_mdlId[sf->getUniqueId()]-1;
}
/*----------------------------------------------------------------------------*/
T_MdlCommand& ExportMDLImplementation::
nextCommand(std::string nm, int t, size_t id)
{
#ifdef _DEBUG_EXPORT2
    std::cout<<"nextCommand("<<nm<<", "<<t<<", "<<id<<")"<<std::endl;
#endif

    T_MdlCommand cmd; memset(&cmd,0,sizeof(cmd));

    cmd.type          = (eMdlTypeCommand)t;

    cmd.name          = nm.c_str();

    cmd.id            = id;
    cmd.bloc_id       = 0;
    cmd.is_redefined  = 0;

    mdl_cmd_list.push_back(cmd);
    mdl_info.commands = &(*mdl_cmd_list.begin());
    mdl_info.nb_commands++;

    return mdl_info.commands[mdl_info.nb_commands-1];
}
/*----------------------------------------------------------------------------*/
Utils::Math::Point ExportMDLImplementation::
getCenter(const TopoDS_Edge& edge)
{
    Standard_Real first_param, last_param;
    Handle_Geom_Curve curve = BRep_Tool::Curve(edge, first_param, last_param);
    TkUtil::UTF8String error_msg("Erreur interne, type non prévu : seules courbe et ellipse cont autorisées", TkUtil::Charset::UTF_8);
    if (curve.IsNull())
    	throw TkUtil::Exception(error_msg);

    if (curve->DynamicType()==STANDARD_TYPE(Geom_Ellipse)){
        Handle(Geom_Ellipse) ellipse = Handle(Geom_Ellipse)::DownCast(curve);
        gp_Elips elips = ellipse->Elips();
        const gp_Pnt& loc = elips.Location();

        return Utils::Math::Point(loc.X(), loc.Y(), loc.Z());

    } else if (curve->DynamicType()==STANDARD_TYPE(Geom_Circle)){
        Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(curve);
        gp_Circ circ = circle->Circ();
        const gp_Ax2& ax = circ.Position();
        const gp_Pnt& loc = ax.Location();

        return Utils::Math::Point(loc.X(), loc.Y(), loc.Z());
    }
    else
        throw TkUtil::Exception(error_msg);
}
/*----------------------------------------------------------------------------*/
} // end namespace Internal
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
#endif  // USE_MDLPARSER
/*----------------------------------------------------------------------------*/





