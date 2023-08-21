// Gmsh - Copyright (C) 1997-2013 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@geuz.org>.

#include <string.h>
#include <sstream>
#include "GmshConfig.h"
#include "GmshMessage.h"
#include "GModel.h"
#include "Numeric.h"
#include "StringUtils.h"
#include "Geo.h"
#include "GeoStringInterface.h"
#include "OpenFile.h"
#include "Context.h"
#include "OS.h"

#if defined(HAVE_PARSER)
#include "Parser.h"
#endif

#if defined(HAVE_ONELAB)
#include "onelab.h"
#endif

void add_infile(std::string text, std::string fileName, bool forceDestroy)
{
  // make sure we don't add stuff in a non-geo file
  if(!CTX::instance()->expertMode) {
    std::vector<std::string> split = SplitFileName(fileName);
    if(split[2].size() && split[2] != ".geo" && split[2] != ".GEO"){
      std::ostringstream sstream;
      sstream <<
        "A scripting command is going to be appended to a non-`.geo' file. Are\n"
        "you sure you want to proceed?\n\n"
        "You probably want to create a new `.geo' file containing the command\n"
        "`Merge \"" << split[1] + split[2] << "\";' and use that file instead.\n\n"
        "(To disable this warning in the future, select `Enable expert mode'\n"
        "in the option dialog.)";
      int ret = Msg::GetAnswer(sstream.str().c_str(), 2, "Cancel", "Proceed as is",
                               "Create new `.geo' file");
      if(ret == 2){
        std::string newFileName = split[0] + split[1] + ".geo";
        if(CTX::instance()->confirmOverwrite) {
          if(!StatFile(newFileName)){
            std::ostringstream sstream;
            sstream << "File '" << fileName << "' already exists.\n\n"
              "Do you want to replace it?";
            if(!Msg::GetAnswer(sstream.str().c_str(), 0, "Cancel", "Replace"))
              return;
          }
        }
        FILE *fp = fopen(newFileName.c_str(), "w");
        if(!fp) {
          Msg::Error("Unable to open file '%s'", newFileName.c_str());
          return;
        }
        fprintf(fp, "Merge \"%s\";\n%s\n", (split[1] + split[2]).c_str(), text.c_str());
        fclose(fp);
        OpenProject(newFileName);
        return;
      }
      else if(ret == 0)
        return;
    }
  }

#if defined(HAVE_PARSER)
  std::string tmpFileName = CTX::instance()->homeDir + CTX::instance()->tmpFileName;
  FILE *gmsh_yyin_old = gmsh_yyin;
  if(!(gmsh_yyin = fopen(tmpFileName.c_str(), "w"))) {
    Msg::Error("Unable to open temporary file '%s'", tmpFileName.c_str());
    gmsh_yyin = gmsh_yyin_old;
    return;
  }
  fprintf(gmsh_yyin, "%s\n", text.c_str());
  fclose(gmsh_yyin);
  gmsh_yyin = fopen(tmpFileName.c_str(), "r");
  while(!feof(gmsh_yyin)) {
    gmsh_yyparse();
  }
  fclose(gmsh_yyin);
  gmsh_yyin = gmsh_yyin_old;

  if(forceDestroy){
    // we need to start from scratch (e.g. if the command just parsed
    // could have deleted some entities)
    GModel::current()->destroy();
  }
  GModel::current()->importGEOInternals();
  CTX::instance()->mesh.changed = ENT_ALL;

  FILE *fp = fopen(fileName.c_str(), "a");
  if(!fp) {
    Msg::Error("Unable to open file '%s'", fileName.c_str());
    return;
  }
  fprintf(fp, "%s\n", text.c_str());
  fclose(fp);
#else
  Msg::Error("GEO file creation not available without Gmsh parser");
#endif

  // mark all Gmsh data as changed in onelab (will force e.g. a reload and a
  // remesh)
#if defined(HAVE_ONELAB)
  onelab::server::instance()->setChanged(true, "Gmsh");
#endif
}

void coherence(std::string fileName)
{
  add_infile("Coherence;", fileName, true);
}

static std::string list2string(List_T *list)
{
  std::ostringstream sstream;
  for(int i = 0; i < List_Nbr(list); i++){
    int num;
    List_Read(list, i, &num);
    if(i) sstream << ", ";
    sstream << num;
  }
  return sstream.str();
}

void delet(List_T *list, std::string fileName, std::string what)
{
  std::ostringstream sstream;
  sstream << "Delete {\n  " << what << "{" << list2string(list) << "};\n}";
  add_infile(sstream.str(), fileName, true);
}

void add_charlength(List_T *list, std::string fileName, std::string lc)
{
  std::ostringstream sstream;
  sstream << "Characteristic Length {" << list2string(list) << "} = " << lc << ";";
  add_infile(sstream.str(), fileName);
}

void add_recosurf(List_T *list, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Recombine Surface {" << list2string(list) << "};";
  add_infile(sstream.str(), fileName);
}

void add_trsfline(std::vector<int> &l, std::string fileName, std::string type,
                  std::string typearg, std::string pts)
{
  std::ostringstream sstream;
  sstream << "Transfinite Line {";
  for(unsigned int i = 0; i < l.size(); i++){
    if(i) sstream << ", ";
    sstream << l[i];
  }
  sstream << "} = " << pts;
  if(typearg.size()) sstream << " Using " << type << " " << typearg;
  sstream << ";";
  add_infile(sstream.str(), fileName);
}

void add_trsfsurf(std::vector<int> &l, std::string fileName, std::string dir)
{
  std::ostringstream sstream;
  sstream << "Transfinite Surface {" << l[0] << "}";
  if(l.size() > 1){
    sstream << " = {";
    for(unsigned int i = 1; i < l.size(); i++) {
      if(i > 1) sstream << ", ";
      sstream << l[i];
    }
    sstream << "}";
  }
  if(dir != "Left")
    sstream << " " << dir;
  sstream << ";";
  add_infile(sstream.str(), fileName);
}

void add_trsfvol(std::vector<int> &l, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Transfinite Volume{" << l[0] << "} = {";
  for(unsigned int i = 1; i < l.size(); i++) {
    if(i > 1) sstream << ", ";
    sstream << l[i];
  }
  sstream << "};";
  add_infile(sstream.str(), fileName);
}

void add_embedded(std::string what, std::vector<int> &l, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Point{";
  for(unsigned int i = 1; i < l.size(); i++) {
    if(i > 1) sstream << ", ";
    sstream << l[i];
  }
  sstream << "} In Surface{" << l[0] << "};";
  add_infile(sstream.str(), fileName, true);
}

void add_param(std::string par, std::string value, std::string label,
               std::string path, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "DefineConstant[ " << par << " = { " << value;
  if(label.size()) sstream << ", Label \"" << label << "\"";
  if(path.size()) sstream << ", Path \"" << path << "\"";
  sstream << "}];";
  add_infile(sstream.str(), fileName);
}

void add_point(std::string fileName, std::string x, std::string y,
               std::string z, std::string lc)
{
  std::ostringstream sstream;
  sstream << "Point(" << NEWPOINT() << ") = {" << x << ", " << y << ", "
          << z ;
  if(lc.size()) sstream << ", " << lc;
  sstream << "};";
  add_infile(sstream.str(), fileName);
}

void add_field_option(int field_id, std::string option_name,
                      std::string option_value, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Field[" << field_id << "]." << option_name << " = "
          << option_value << ";";
  add_infile(sstream.str(), fileName);
}

void add_field(int field_id, std::string type_name, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Field[" << field_id << "] = " << type_name << ";";
  add_infile(sstream.str(), fileName);
}

void delete_field(int field_id, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Delete Field [" << field_id << "];";
  add_infile(sstream.str(), fileName);
}

void set_background_field(int field_id, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Background Field = " << field_id << ";";
  add_infile(sstream.str(), fileName);
}

void add_multline(std::string type, std::vector<int> &p, std::string fileName)
{
  std::ostringstream sstream;
  sstream << type << "(" << NEWLINE() << ") = {";
  for(unsigned int i = 0; i < p.size(); i++) {
    if(i) sstream << ", ";
    sstream << p[i];
  }
  sstream << "};";
  add_infile(sstream.str(), fileName);
}

void add_circ(int p1, int p2, int p3, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Circle(" << NEWLINE() << ") = {" << p1 << ", " << p2 << ", "
          << p3 << "};";
  add_infile(sstream.str(), fileName);
}

void add_ell(int p1, int p2, int p3, int p4, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Ellipse(" << NEWLINE() << ") = {" << p1 << ", " << p2 << ", "
          << p3 << ", " << p4 << "};";
  add_infile(sstream.str(), fileName);
}

void add_lineloop(List_T *list, std::string fileName, int *numloop)
{
  if(recognize_loop(list, numloop)) return;
  *numloop = NEWLINELOOP();
  std::ostringstream sstream;
  sstream << "Line Loop(" << *numloop << ") = {" << list2string(list) << "};";
  add_infile(sstream.str(), fileName);
}

void add_surf(std::string type, List_T *list, std::string fileName)
{
  std::ostringstream sstream;
  sstream << type << "(" << NEWSURFACE() << ") = {" << list2string(list) << "};";
  add_infile(sstream.str(), fileName);
}

void add_surfloop(List_T *list, std::string fileName, int *numloop)
{
  if(recognize_surfloop(list, numloop)) return;
  *numloop = NEWSURFACELOOP();
  std::ostringstream sstream;
  sstream << "Surface Loop(" << *numloop << ") = {" << list2string(list) << "};";
  add_infile(sstream.str(), fileName);
}

void add_vol(List_T *list, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Volume(" << NEWVOLUME() << ") = {" << list2string(list) << "};";
  add_infile(sstream.str(), fileName);
}

void add_physical(std::string type, List_T *list, std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Physical " << type << "(" << NEWPHYSICAL() << ") = {"
          << list2string(list) << "};";
  add_infile(sstream.str(), fileName);
}

void add_compound(std::string type, List_T *list, std::string fileName)
{
  std::ostringstream sstream;
  if(SplitFileName(fileName)[2] != ".geo") sstream << "CreateTopology;\n";
  if (type == "Surface"){
    sstream << "Compound " << type << "(" << NEWSURFACE()+1000 << ") = {"
	    << list2string(list) << "};";
  }
  else if (type == "Line"){
    sstream << "Compound " << type << "(" << NEWLINE()+1000 << ") = {"
	    << list2string(list) << "};";
  }
  else{
    sstream << "Compound " << type << "(" << NEWREG() << ") = {"
	    << list2string(list) << "};";
  }
  add_infile(sstream.str(), fileName);
}

void translate(int add, List_T *list, std::string fileName, std::string what,
               std::string tx, std::string ty, std::string tz)
{
  std::ostringstream sstream;
  sstream << "Translate {" << tx << ", " << ty << ", " << tz << "} {\n  ";
  if(add) sstream << "Duplicata { ";
  sstream << what << "{" << list2string(list) << "};";
  if(add) sstream << " }";
  sstream << "\n}";
  add_infile(sstream.str(), fileName);
}

void rotate(int add, List_T *list, std::string fileName, std::string what,
            std::string ax, std::string ay, std::string az,
            std::string px, std::string py, std::string pz, std::string angle)
{
  std::ostringstream sstream;
  sstream << "Rotate {{" << ax << ", " << ay << ", " << az << "}, {"
          << px << ", " << py << ", " << pz << "}, " << angle << "} {\n  ";
  if(add) sstream << "Duplicata { ";
  sstream << what << "{" << list2string(list) << "};";
  if(add) sstream << " }";
  sstream << "\n}";
  add_infile(sstream.str(), fileName);
}

void dilate(int add, List_T *list, std::string fileName, std::string what,
            std::string dx, std::string dy, std::string dz, std::string df)
{
  std::ostringstream sstream;
  sstream << "Dilate {{" << dx << ", " << dy << ", " << dz << "}, " << df << "} {\n  ";
  if(add) sstream << "Duplicata { ";
  sstream << what << "{" << list2string(list) << "};";
  if(add) sstream << " }";
  sstream << "\n}";
  add_infile(sstream.str(), fileName);
}

void symmetry(int add, List_T *list, std::string fileName, std::string what,
              std::string sa, std::string sb, std::string sc, std::string sd)
{
  std::ostringstream sstream;
  sstream << "Symmetry {" << sa << ", " << sb << ", " << sc << ", " << sd << "} {\n  ";
  if(add) sstream << "Duplicata { ";
  sstream << what << "{" << list2string(list) << "};";
  if(add) sstream << " }";
  sstream << "\n}";
  add_infile(sstream.str(), fileName);
}

void extrude(List_T *list, std::string fileName, std::string what,
             std::string tx, std::string ty, std::string tz)
{
  std::ostringstream sstream;
  sstream << "Extrude {" << tx << ", " << ty << ", " << tz << "} {\n  " << what
          << "{" << list2string(list) << "};\n}";
  add_infile(sstream.str(), fileName);
}

void protude(List_T *list, std::string fileName, std::string what,
             std::string ax, std::string ay, std::string az,
             std::string px, std::string py, std::string pz, std::string angle)
{
  std::ostringstream sstream;
  sstream << "Extrude {{" << ax << ", " << ay << ", " << az << "}, {"
          << px << ", " << py << ", " << pz << "}, " << angle << "} {\n  "
          << what << "{" << list2string(list) << "};\n}";
  add_infile(sstream.str(), fileName);
}

void split_edge(int edge_id, List_T *vertices,std::string fileName)
{
  std::ostringstream sstream;
  sstream << "Split Line(" << edge_id << ") {" << list2string(vertices) << "};";
  add_infile(sstream.str(), fileName, true);
}
