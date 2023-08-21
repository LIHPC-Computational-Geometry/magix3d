//%module pyMagix3D
%{
// désormais (gcc 4.8) ptrdiff_t est dans std mais swig 1.3 n'est pas à jour
using std::ptrdiff_t;
%}


%include "std_string.i"
%include "std_list.i"
%include "std_vector.i"
%include "typemaps.i"
%include "exception.i"
 
// -------------------------
// non interprété par swig : 
// -------------------------
%{
#include "Internal/ContextIfc.h"
#include "Internal/Context.h"
#include "Internal/M3DCommandManager.h"
#include "Internal/M3DCommandResultIfc.h"
#include "Utils/Point.h"
#include "Utils/Spherical.h"
#include "Utils/Cylindrical.h"
#include "Utils/Vector.h"
#include "Utils/Plane.h"
#include "Utils/Rotation.h"
#include "Utils/RotX.h"
#include "Utils/RotY.h"
#include "Utils/RotZ.h"
#include "Utils/Constants.h"
#include "Utils/Landmark.h"
#include "Utils/Unit.h"
#include "Utils/SwigCompletion.h"

#include "Geom/GeomManagerIfc.h"
#include "Geom/GeomInfo.h"

#include "Topo/TopoInfo.h"
#include "Topo/TopoManagerIfc.h"
#include "Topo/EdgeMeshingPropertyUniform.h"
#include "Topo/EdgeMeshingPropertyUniformSmoothFix.h"
#include "Topo/EdgeMeshingPropertyGeometric.h"
#include "Topo/EdgeMeshingPropertyBigeometric.h"
#include "Topo/EdgeMeshingPropertyHyperbolic.h"
#include "Topo/EdgeMeshingPropertySpecificSize.h"
#include "Topo/EdgeMeshingPropertyInterpolate.h"
#include "Topo/EdgeMeshingPropertyGlobalInterpolate.h"
#include "Topo/EdgeMeshingPropertyTabulated.h"
#include "Topo/EdgeMeshingPropertyBeta.h"
#include "Topo/FaceMeshingPropertyDirectional.h"
#include "Topo/FaceMeshingPropertyOrthogonal.h"
#include "Topo/FaceMeshingPropertyRotational.h"
#include "Topo/FaceMeshingPropertyTransfinite.h"
#include "Topo/FaceMeshingPropertyDelaunayGMSH.h"
#include "Topo/FaceMeshingPropertyQuadPairing.h"
#ifdef USE_MESHGEMS
#include "Topo/FaceMeshingPropertyMeshGems.h"
#endif	// USE_MESHGEMS
#include "Topo/BlockMeshingPropertyDirectional.h"
#include "Topo/BlockMeshingPropertyOrthogonal.h"
#include "Topo/BlockMeshingPropertyRotational.h"
#include "Topo/BlockMeshingPropertyTransfinite.h"
#ifdef USE_MESHGEMS
#include "Topo/BlockMeshingPropertyDelaunayMeshGems.h"
#endif	// USE_MESHGEMS
#include "Topo/BlockMeshingPropertyDelaunayTetgen.h"
#include "Topo/BlockMeshingPropertyInsertion.h"

#include "Mesh/MeshManagerIfc.h"
#include "Smoothing/SurfacicSmoothing.h"
#include "Smoothing/VolumicSmoothing.h"

#include "Mesh/MeshModificationBySepa.h"

#include "Group/GroupManagerIfc.h"

#include "SysCoord/SysCoordManagerIfc.h"

#include "Structured/StructuredMeshManagerIfc.h"

#include <patchlevel.h>	// PY_VERSION
#if PY_MAJOR_VERSION >= 3
#define PyString_Check PyUnicode_Check
#define PyString_AsString PyUnicode_AsUTF8
#define PyString_FromString PyUnicode_FromString
#endif	// # PY_MAJOR_VERSION < 3
%}
 
// -------------------------------
// interprété par swig : typemaps
// -------------------------------


%exception{ 
  try{
    $action
  }
  catch(TkUtil::Exception& e){
    char buf [4096];
    strncpy (buf, e.getMessage().iso().c_str(), 4095);
    buf [4095]	= '\0';
    SWIG_exception (SWIG_RuntimeError, buf);
  } 
  catch (...) {
    SWIG_exception (SWIG_RuntimeError,"exception inconnue (via Magix3D) ...");
  }
}


// -------------------------------

%typemap(typecheck) std::vector<std::string>& { 
   if (PyList_Check($input)){
     $1 = 1;
     for (int i = 0; i<PyList_Size($input); i++) {
        PyObject* obj = PyList_GetItem( $input, i );
        if (!PyString_Check(obj))
          $1 = 0;
     }
   }
   else
     $1 = 0;
}

%typemap(in) std::vector<std::string>& { 
  if (!PyList_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "On attendait une liste");
    return NULL;
  }
  
  $1 = new std::vector<std::string>(PyList_Size($input));
  for (int i = 0; i<$1->size(); i++) {
    PyObject* obj = PyList_GetItem( $input, i );
    if (!PyString_Check(obj)){
      printf("erreur objet %d n'est pas une string\n", i);
      return NULL;
    }
    else
      (*$1)[i] = PyString_AsString(obj);
  }
}

%typemap(freearg) std::vector<std::string>& {
  delete ($1);
}

%typemap(out) std::vector<std::string> {
    $result=PyList_New($1.size() );
    std::vector<std::string>::const_iterator iter;
    int k;
    for(iter = $1.begin(), k=0; iter != $1.end(); ++iter, ++k) {
        PyList_SetItem($result,k,PyString_FromString( (*iter).c_str() ));
    }
}

%typemap(newfree) std::vector<std::string> {
    delete $1;
}

// ------------------------------- 

%typemap(in) std::vector<Mgx3D::Utils::Math::Point>& { 
  //std::cout<<"on entre dans typemap(in) pour vector<Point>"<<std::endl;
  if (!PyList_Check($input)) {
    PyErr_SetString(PyExc_TypeError, "On attendait une liste");
    return NULL;
  }
  
  $1 = new std::vector<Mgx3D::Utils::Math::Point>(PyList_Size($input));
  for (int i = 0; i<$1->size(); i++) {
    PyObject* obj = PyList_GetItem( $input, i );
    void *argp1 = 0 ;
    if (PyList_Check(obj)){
      double coord[3] = {0,0,0};
      for (int j = 0; j<PyList_Size(obj); j++) {
      	PyObject* objj = PyList_GetItem( obj, j );
       	if (PyNumber_Check(objj)) {
           coord[j] = (float) PyFloat_AsDouble(objj);
        } else {
           PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
           return NULL;
        }
      }
      
      (*$1)[i] = Mgx3D::Utils::Math::Point(coord[0], coord[1], coord[2]);
    }
    else if (SWIG_IsOK(SWIG_ConvertPtr(obj, &argp1, SWIGTYPE_p_Mgx3D__Utils__Math__Point, 0))) {
      (*$1)[i] = *(reinterpret_cast< Mgx3D::Utils::Math::Point * >(argp1));
    }
    else {
      printf("erreur objet %d n'est pas une liste\n", i);
      PyErr_SetString(PyExc_ValueError,"Erreur, il faut donner une liste de listes ou une liste d'objets Point");  
      return NULL;
    }
      
  }
}

%typemap(freearg) std::vector<Mgx3D::Utils::Math::Point>& {
  delete ($1);
}

// -------------------------------

%typemap(out) std::vector<std::vector<std::string> > {
    $result=PyList_New($1.size() );
    std::vector< std::vector<std::string> >::const_iterator iter1;
    int k1;
    for(iter1 = $1.begin(), k1=0; iter1 != $1.end(); ++iter1, ++k1) {
    	PyList_SetItem($result,k1,PyList_New((*iter1).size() ));
    	int k2;
    	std::vector<std::string>::const_iterator iter2;
    	for(iter2 = (*iter1).begin(), k2=0; iter2 != (*iter1).end(); ++iter2, ++k2) {
        	PyList_SetItem(PyList_GetItem( $result, k1),k2,PyString_FromString( (*iter2).c_str() ));
        }
    }
}

// -------------------------------
// Macro pour typemap(typecheck) pour un vector<type>

%{
#define TYPEMAP_VECTOR_ENTITIES_TYPECHECK(arg, swtype, type) \
{ \
   if (PyList_Check(arg)){ \
     _v = 1; \
     for (int i = 0; i<PyList_Size(arg); i++) { \
        PyObject* obj = PyList_GetItem( arg, i ); \
        void *argpi = 0 ; \
        if (SWIG_ConvertPtr(obj, &argpi,swtype, 0 )!=-1){ \
          type* ge = reinterpret_cast< type * >(argpi); \
          if (ge == 0) \
            _v = 0; \
        } \
     } \
   } \
   else \
     _v = 0; \
}
%}

// Macro pour typemap(in) pour un vector<type>

%{
#define TYPEMAP_VECTOR_ENTITIES_IN(obj1, arg2, swtype, type, typeName) \
{ \
  if (!PyList_Check(obj1)) { \
    PyErr_SetString(PyExc_TypeError, "On attendait une liste"); \
    return NULL; \
  } \
  arg2 = new std::vector<type*>(PyList_Size(obj1)); \
  for (int i = 0; i<arg2->size(); i++) { \
    void *argpi = 0 ; \
    PyObject* obj = PyList_GetItem( obj1, i ); \
    if (SWIG_ConvertPtr(obj, &argpi,swtype, 0 )!=-1) \
       (*arg2)[i] = reinterpret_cast< type * >(argpi); \
    else { \
      PyErr_SetString(PyExc_TypeError, "On attendait une liste de " typeName); \
      return NULL; \
    } \
  } \
}
%}

// Macro pour typemap(out) pour un vector<type>

%{
#define TYPEMAP_VECTOR_ENTITIES_OUT(result, resultobj, swtype, type)\
{\
    resultobj=PyList_New((&result)->size() );\
    std::vector<type*>::const_iterator iter;\
    int k;\
    for(iter = (&result)->begin(), k=0; iter != (&result)->end(); ++iter, ++k) {\
        PyObject *pyobj = SWIG_NewPointerObj(SWIG_as_voidptr(*iter), swtype, 0 );\
        PyList_SetItem(resultobj,k,pyobj);\
    }\
}
%}



// ----------------------------------------------------------
// interprété par swig : documentation des fonctions membres
// fichier actuellement généré avec xsltproc
// et modifié manuellement, cf swig.xsl
// ----------------------------------------------------------
%include "swig_doc.i"

// ---------------------------------------------------------
// interprété par swig : déclaration des fonction pour swig
// ---------------------------------------------------------
%include Utils/SwigCompletion.h	// should be first !
%include Utils/CommandIfc.h
%include Internal/ContextIfc.h
%include Internal/Context.h
%include Internal/M3DCommandManager.h
%include Utils/CommandResultIfc.h
%include Utils/CommandResult.h
%include Internal/M3DCommandResultIfc.h
%include Utils/Point.h
%include Utils/Spherical.h
%include Utils/Cylindrical.h
%include Utils/Vector.h
%include Utils/Plane.h
%include Utils/Entity.h
%include Utils/Rotation.h
%include Utils/RotX.h
%include Utils/RotY.h
%include Utils/RotZ.h
%include Utils/Constants.h
%include Utils/Landmark.h
%include Utils/Unit.h

%include Geom/GeomInfo.h
%include Geom/GeomManagerIfc.h

%include Topo/TopoInfo.h
%include Topo/TopoManagerIfc.h
%include Topo/CoEdgeMeshingProperty.h
%include Topo/EdgeMeshingPropertyUniform.h
%include Topo/EdgeMeshingPropertyUniformSmoothFix.h
%include Topo/EdgeMeshingPropertyGeometric.h
%include Topo/EdgeMeshingPropertyBigeometric.h
%include Topo/EdgeMeshingPropertyHyperbolic.h
%include Topo/EdgeMeshingPropertySpecificSize.h
%include Topo/EdgeMeshingPropertyInterpolate.h
%include Topo/EdgeMeshingPropertyGlobalInterpolate.h
%include Topo/EdgeMeshingPropertyTabulated.h
%include Topo/EdgeMeshingPropertyBeta.h
%include Topo/CoFaceMeshingProperty.h
%include Topo/FaceMeshingPropertyDirectional.h
%include Topo/FaceMeshingPropertyOrthogonal.h
%include Topo/FaceMeshingPropertyRotational.h
%include Topo/FaceMeshingPropertyTransfinite.h
%include Topo/FaceMeshingPropertyDelaunayGMSH.h
%include Topo/FaceMeshingPropertyQuadPairing.h
%include Topo/FaceMeshingPropertyMeshGems.h
%include Topo/BlockMeshingProperty.h
%include Topo/BlockMeshingPropertyDirectional.h
%include Topo/BlockMeshingPropertyOrthogonal.h
%include Topo/BlockMeshingPropertyRotational.h
%include Topo/BlockMeshingPropertyTransfinite.h
%include Topo/BlockMeshingPropertyDelaunayMeshGems.h
%include Topo/BlockMeshingPropertyDelaunayTetgen.h
%include Topo/BlockMeshingPropertyInsertion.h

%include Mesh/MeshManagerIfc.h
%include Smoothing/SurfacicSmoothing.h
%include Smoothing/VolumicSmoothing.h

%include Mesh/MeshModificationBySepa.h

%include Group/GroupManagerIfc.h

%include SysCoord/SysCoordManagerIfc.h

%include Structured/StructuredMeshManagerIfc.h


// extensions: fonctions supplémentaires depuis python
%extend Mgx3D::Utils::Math::Point {
  char *__str__() {
    static char temp[256];
	sprintf(temp,"[ %g, %g, %g ]", $self->getX(),$self->getY(),$self->getZ());
	return &temp[0];
  }
}
%extend Mgx3D::Utils::Math::Spherical {
  char *__str__() {
    static char temp[256];
	sprintf(temp,"[ %g, %g, %g ]", $self->getRho(),$self->getTheta(),$self->getPhi());
	return &temp[0];
  }
}
%extend Mgx3D::Utils::Math::Cylindrical {
  char *__str__() {
    static char temp[256];
	sprintf(temp,"[ %g, %g, %g ]", $self->getRho(),$self->getPhi(),$self->getZ());
	return &temp[0];
  }
}

// surcharges de fonctions pour utilisation de script Erebos
// il est nécessaire de faire 'from pyMagix3D import *' au début et non 'import pyMagix3D as Mgx3D'
%pythoncode %{
# ---------------------------------------------------------
# source récupéré dans Erebos
import sys
def file2list(fich, format = (float, float)):
    # Le format est-il libre (nombre de colonnes indetermine') ?
    if type(format) == type(None):
        is_format_libre = True
    else:
        # le format est il compatible ?
        for f in format:
            if type(f) != type(int):
                print (str(f), ": format non autorise.")
                print ("Le format doit representer un type (int, float, str, ...)")
                sys.exit(1)
        is_format_libre = False
    fd = open(fich, "r")
    lignes = fd.readlines()
    fd.close()
    listglob = []
    if is_format_libre == False:
        nbf = len(format)
    numligne = 0
    for line in lignes:
        numligne += 1
        # les commentaires sont ignores
        if line.startswith("#") or line.startswith("//") or line.isspace():
            continue
        ind = line.rfind("\n")
        newline = line[:ind]
        elts = newline.split(" ")
        ll = []
        if is_format_libre == True:
            # Nb de colonnes inconnu
            for e in elts:
                if e != "":
                    ll.append(e)
        else:
            # Nb de colonnes connu et format connu
            i = 0
            try:
                for e in elts:
                    if e != "":
                        if i < nbf:
                            conversion = format[i](e)
                            ll.append(conversion)
                            i += 1
                        else:
                            print ("fichier", fich, ", ligne", numligne, ":\"", newline, "\" incoherente avec le format", format)
                            sys.exit(1)
                        
                if i < nbf:
                    print ("fichier", fich, ", ligne", numligne, ":\"", newline, "\" incoherente avec le format", format)
                    sys.exit(1)
            except ValueError:
                print ("fichier", fich, ", ligne", numligne, ":\"", newline, "\" incoherente avec le format", format)
                sys.exit(1)
        # Format libre ou non, on insere la nouvelle liste
        listglob.append( ll )
    return listglob
# ---------------------------------------------------------

def Sphere(rayon):
	return getStdContext().getGeomManager().newSphere(Point(0, 0, 0), rayon, Portion.ENTIER)

def Parallelepipede(longueur, largeur, hauteur):
    dx = longueur/2
    dy = largeur/2
    dz = hauteur/2
    return getStdContext().getGeomManager().newBox(Point(-dx, -dy, -dz), Point(dx, dy, dz))

def Cylindre(longueur, rayon):
    dx = longueur/2
    return getStdContext().getGeomManager().newCylinder(Point(-dx, 0, 0), rayon, Vector(longueur, 0, 0), 360)
    
def Cone(longueur, rayon):
    return getStdContext().getGeomManager().newCone(0, rayon, Vector(longueur, 0, 0), 360)
    
def DemiEspace(cote):
    return getStdContext().getGeomManager().newBox(Point(0, -cote, -cote), Point(2*cote, cote, cote), "")

def Cercle(rayon):
    pt1=getStdContext().getGeomManager().newVertex(Point(rayon, 0, 0), "pt_pour_cercle")
    pt2=getStdContext().getGeomManager().newVertex(Point(-rayon, 0, 0), "pt_pour_cercle")
    pt3=getStdContext().getGeomManager().newVertex(Point(0, rayon, 0), "pt_pour_cercle")
    return getStdContext().getGeomManager().newCircle(pt1, pt2, pt3, "")
    
def Polygon(sepaIn):
    sepa=[]
    if (type(sepaIn)==type("")):
        sepa=file2list(sepaIn)
    else:
        sepa=sepaIn
    # si le 1er et le dernier point sont confondus alors on le supprime
    nbPts=len(sepa)
    if sepa[0][0] == sepa[nbPts-1][0] and sepa[0][1] == sepa[nbPts-1][1]:
        res = sepa.pop()
        nbPts=len(sepa)
    return getStdContext().getGeomManager().newVerticesCurvesAndPlanarSurface(sepa, "")

def Surface(modele, groupe=None, zones=None, zone=None):
    if modele[0]=='/':
        loc_modele=modele
    else:
        loc_modele="../"+modele
    if groupe != None :
        return getStdContext().getGeomManager().importMDL(loc_modele, groupe)
    elif zone != None :
        return getStdContext().getGeomManager().importMDL(loc_modele, [zone])
    elif zones != None :
        return getStdContext().getGeomManager().importMDL(loc_modele, zones)
    else :
        print ("Erreur avec création d'une surface par lecture dans ",loc_modele)

def Shape():
    print ("Shape A FAIRE ???")
    
def Extrude(s, vec, coeff = 1):
    return getStdContext().getGeomManager().newPrism(s.getSurface(), vec, "")

def Rev(s):
    return getStdContext().getGeomManager().makeRevol(s.getSurface(), RotX(360), False)

def EulerDir(vect):
	if vect.getY() == 1.0:
		return RotZ(90)
	elif vect.getY() == -1.0:
		return RotZ(-90)
	elif vect.getZ() == 1.0:
		return RotY(-90)
	elif vect.getZ() == -1.0:
		return RotY(90)
	elif vect.getX() == -1.0:
		return RotY(180)
	else:
		print ("cas EulerDir non implémenté")
		return RotZ(0)

def GeoAngles(phi, teta, psy):
	if teta == 0.0 and  psy == 0.0:
		return RotZ(phi)
	elif phi == 0.0 and  psy == 0.0:
		return RotY(-teta)
	elif phi == 0.0 and  teta == 0.0:
		return RotX(psy)
	else:
		print ("cas GeoAngles non implémenté")
		return RotZ(0)

def EulerAngles(phi, teta, psy):
	if teta == 90:
		if psy == 0.0:
			return RotZ(phi)
		elif phi == 0.0:
			return RotY(psy)
		else:
			print ("cas EulerAngles non implémenté")
			return RotZ(0)
	elif teta == 0:
		if psy == 0.0:
			return RotZ(phi)
		elif phi == 0.0:
			return RotZ(psy)
		else:
			print ("cas EulerAngles non implémenté")
			return RotZ(0)
	else:
		print ("cas EulerAngles (teta != 90) non implémenté")
		return RotZ(0)

# ---------------------------------------------------------
# pour faire un filtre entre deux listes, retourne la partie commune
def common(l1,l2):
	return [i for i in l1 if i in l2]

%}
// ---------------------------------------------------------

%extend Mgx3D::Internal::M3DCommandResultIfc {
public:
    // - __sub__ cut
    Mgx3D::Internal::M3DCommandResultIfc* __sub__(Mgx3D::Internal::M3DCommandResultIfc* autre) {
        std::vector<std::string> to_cut = $self->getVolumes();
        std::vector<std::string> cutter = autre->getVolumes();
        return Mgx3D::getStdContext()->getGeomManager().cut(to_cut, cutter);
    }
    
    // & __and__ common
    Mgx3D::Internal::M3DCommandResultIfc* __and__(Mgx3D::Internal::M3DCommandResultIfc* autre) {
        std::vector<std::string> vols = $self->getVolumes();
        std::vector<std::string> vol2 = autre->getVolumes();
        vols.insert(vols.end(), vol2.begin(), vol2.end());
        return Mgx3D::getStdContext()->getGeomManager().common(vols);
    }
    
    // | __or__ fuse
    Mgx3D::Internal::M3DCommandResultIfc* __or__(Mgx3D::Internal::M3DCommandResultIfc* autre) {
        std::vector<std::string> vols = $self->getVolumes();
        std::vector<std::string> vol2 = autre->getVolumes();
        vols.insert(vols.end(), vol2.begin(), vol2.end());
        return Mgx3D::getStdContext()->getGeomManager().fuse(vols);
    }
    
    // Not
    Mgx3D::Internal::M3DCommandResultIfc* Not(){
        std::cout<< "Not A FAIRE ???"<<std::endl;
        return 0;
    }
    
    // ^ __xor__ rotate
    Mgx3D::Internal::M3DCommandResultIfc* __xor__(Mgx3D::Utils::Math::Rotation* rotation) {
        std::vector<std::string> vols = $self->getVolumes();
        //return Mgx3D::getStdContext()->getGeomManager().rotate(vols, *rotation);
        Mgx3D::getStdContext()->getGeomManager().rotate(vols, *rotation);
        return $self;
    }

    // + Vect __add__ translate
    Mgx3D::Internal::M3DCommandResultIfc* __add__(Mgx3D::Utils::Math::Vector* vect){
        std::vector<std::string> vols = $self->getVolumes();
        //return 
        Mgx3D::getStdContext()->getGeomManager().translate(vols, *vect);
        return $self;
    }
    
    // - Vect __add__ translate
    Mgx3D::Internal::M3DCommandResultIfc* __sub__(Mgx3D::Utils::Math::Vector* vect){
        std::vector<std::string> vols = $self->getVolumes();
        Mgx3D::Utils::Math::Vector vect2(-vect->getX(), -vect->getY(), -vect->getZ());
        //return 
        Mgx3D::getStdContext()->getGeomManager().translate(vols, vect2);
        return $self;
    }
    
    // * __mul__ scale
    Mgx3D::Internal::M3DCommandResultIfc* __mul__(double factor) {
        std::vector<std::string> vols = $self->getVolumes();
        return Mgx3D::getStdContext()->getGeomManager().scale(vols, factor);
    }
    
    // Extrude
    Mgx3D::Internal::M3DCommandResultIfc* Extrude (Mgx3D::Utils::Math::Vector* vec, int coeff=1) {
        return Mgx3D::getStdContext()->getGeomManager().newPrism($self->getSurface(), *vec, "");
    }
    
    // Rev
    Mgx3D::Internal::M3DCommandResultIfc* Rev () {
        Mgx3D::Utils::Math::Rotation rotation = Mgx3D::Utils::Math::RotX(360);
        std::vector<std::string> surfs = $self->getSurfaces();
        return Mgx3D::getStdContext()->getGeomManager().makeRevol(surfs, rotation, false);
    }
    
};

