%feature("docstring") Mgx3D::Topo::BlockMeshingProperty " 
Propriété de la discrétisation d'un bloc. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::getDir "
virtual uint Mgx3D::Topo::BlockMeshingProperty::getDir() const 

Accesseur sur la direction associée à la CoFace. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::getMeshLaw "
virtual meshLaw Mgx3D::Topo::BlockMeshingProperty::getMeshLaw() const =0

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::getMeshLawName "
virtual std::string Mgx3D::Topo::BlockMeshingProperty::getMeshLawName() const =0

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::isStructured "
virtual bool Mgx3D::Topo::BlockMeshingProperty::isStructured() const =0

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::~BlockMeshingProperty "
virtual Mgx3D::Topo::BlockMeshingProperty::~BlockMeshingProperty()

Destructeur. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::BlockMeshingProperty "
Mgx3D::Topo::BlockMeshingProperty::BlockMeshingProperty()

Constructeur par défaut. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingProperty::BlockMeshingProperty "
Mgx3D::Topo::BlockMeshingProperty::BlockMeshingProperty(const BlockMeshingProperty &pm)

Constructeur par copie. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayNetgen " 
Propriété de la discrétisation d'un bloc suivant une direction. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen " 
Propriété de la discrétisation d'un bloc suivant une direction. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::BlockMeshingPropertyDelaunayTetgen "
Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::BlockMeshingPropertyDelaunayTetgen(const DelaunayLaw delLaw=QC, const double val=2.0, const double val2=2.0)

Constructeur pour la méthode de maillage d'un bloc avec le Delaunay de Tetgen. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::BlockMeshingPropertyDelaunayTetgen "
Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::BlockMeshingPropertyDelaunayTetgen(double radius_edge_ratio, double max_volume)

Constructeur avec paramètres spécifiés. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getMeshLaw "
BlockMeshingProperty::meshLaw Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getMeshLawName "
std::string Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getRadiusEdgeRatio "
double Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getRadiusEdgeRatio() const 

Accesseur sur le facteur de qualité 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getVolumeMax "
double Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::getVolumeMax() const 

Accesseur sur le volume maximum d'une maille. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::isStructured "
bool Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::setQuiet "
void Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::setQuiet()

passe en mode silencieux 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::setVerbose "
void Mgx3D::Topo::BlockMeshingPropertyDelaunayTetgen::setVerbose()

passe en mode verbose 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional " 
Propriété de la discrétisation d'un bloc suivant une direction. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::BlockMeshingPropertyDirectional "
Mgx3D::Topo::BlockMeshingPropertyDirectional::BlockMeshingPropertyDirectional(Utils::Math::Point v1, Utils::Math::Point v2)

Constructeur avec direction définie par 2 points. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::BlockMeshingPropertyDirectional "
Mgx3D::Topo::BlockMeshingPropertyDirectional::BlockMeshingPropertyDirectional(meshDirLaw md)

Constructeur dont on connait la direction. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::BlockMeshingPropertyDirectional "
Mgx3D::Topo::BlockMeshingPropertyDirectional::BlockMeshingPropertyDirectional(std::string coedgeName)

Constructeur dont on connait la direction. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::getDir "
uint Mgx3D::Topo::BlockMeshingPropertyDirectional::getDir() const 

Accesseur sur la direction associée à la CoFace. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::getMeshLaw "
BlockMeshingProperty::meshLaw Mgx3D::Topo::BlockMeshingPropertyDirectional::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::getMeshLawName "
std::string Mgx3D::Topo::BlockMeshingPropertyDirectional::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyDirectional::isStructured "
bool Mgx3D::Topo::BlockMeshingPropertyDirectional::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion " 
Propriété pour un bloc associé à la méthode insertion. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion::BlockMeshingPropertyInsertion "
Mgx3D::Topo::BlockMeshingPropertyInsertion::BlockMeshingPropertyInsertion()



";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion::BlockMeshingPropertyInsertion "
Mgx3D::Topo::BlockMeshingPropertyInsertion::BlockMeshingPropertyInsertion(const bool AWithRefinement)



";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion::getMeshLaw "
BlockMeshingProperty::meshLaw Mgx3D::Topo::BlockMeshingPropertyInsertion::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion::getMeshLawName "
std::string Mgx3D::Topo::BlockMeshingPropertyInsertion::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion::isStructured "
bool Mgx3D::Topo::BlockMeshingPropertyInsertion::isStructured() const 

Indique si la méthode est structurée. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyInsertion::withRefinement "
bool Mgx3D::Topo::BlockMeshingPropertyInsertion::withRefinement() const 

Indique si le maillage doit être rafiné 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational " 
Propriété de la discrétisation d'un bloc suivant une direction par rotation. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::BlockMeshingPropertyRotational "
Mgx3D::Topo::BlockMeshingPropertyRotational::BlockMeshingPropertyRotational(Utils::Math::Point v1, Utils::Math::Point v2, Utils::Math::Point axis1, Utils::Math::Point axis2)

Constructeur avec 2 points qui définissent la direction et 2 autres points pour l'axe de rotation. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::BlockMeshingPropertyRotational "
Mgx3D::Topo::BlockMeshingPropertyRotational::BlockMeshingPropertyRotational(meshDirLaw md, Utils::Math::Point axis1, Utils::Math::Point axis2)

Constructeur une direction définie et 2 points pour l'axe de rotation. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::BlockMeshingPropertyRotational "
Mgx3D::Topo::BlockMeshingPropertyRotational::BlockMeshingPropertyRotational(std::string coedgeName, Utils::Math::Point axis1, Utils::Math::Point axis2)

Constructeur une arête et 2 points pour l'axe de rotation. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::getAxis "
void Mgx3D::Topo::BlockMeshingPropertyRotational::getAxis(Utils::Math::Point &axis1, Utils::Math::Point &axis2)


Retourne l'axe de rotation 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::getDir "
uint Mgx3D::Topo::BlockMeshingPropertyRotational::getDir() const 

Accesseur sur la direction associée à la CoFace. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::getMeshLaw "
BlockMeshingProperty::meshLaw Mgx3D::Topo::BlockMeshingPropertyRotational::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::getMeshLawName "
std::string Mgx3D::Topo::BlockMeshingPropertyRotational::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::isStructured "
bool Mgx3D::Topo::BlockMeshingPropertyRotational::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyRotational::setAxis "
void Mgx3D::Topo::BlockMeshingPropertyRotational::setAxis(Utils::Math::Point &axis1, Utils::Math::Point &axis2)


Change l'axe de rotation 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyTransfinite " 
Propriété de la discrétisation d'un bloc. 
";

%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyTransfinite::BlockMeshingPropertyTransfinite "
Mgx3D::Topo::BlockMeshingPropertyTransfinite::BlockMeshingPropertyTransfinite()



";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyTransfinite::getMeshLaw "
BlockMeshingProperty::meshLaw Mgx3D::Topo::BlockMeshingPropertyTransfinite::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyTransfinite::getMeshLawName "
std::string Mgx3D::Topo::BlockMeshingPropertyTransfinite::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::BlockMeshingPropertyTransfinite::isStructured "
bool Mgx3D::Topo::BlockMeshingPropertyTransfinite::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty " 
Propriété de la discrétisation d'une arête commune aux différentes discrétisations (uniforme, progression géométrique ...) 
";

%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::getDirect "
virtual bool Mgx3D::Topo::CoEdgeMeshingProperty::getDirect() const 

Accesseur sur le sens. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::getMeshLaw "
meshLaw Mgx3D::Topo::CoEdgeMeshingProperty::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::getMeshLawName "
virtual std::string Mgx3D::Topo::CoEdgeMeshingProperty::getMeshLawName() const =0

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::getNbEdges "
int Mgx3D::Topo::CoEdgeMeshingProperty::getNbEdges() const 

Accesseur sur le nombre de bras. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::getNbNodes "
int Mgx3D::Topo::CoEdgeMeshingProperty::getNbNodes() const 

Accesseur sur le nombre de noeuds. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::setDirect "
virtual void Mgx3D::Topo::CoEdgeMeshingProperty::setDirect(bool sens)

Modification du sens. 

";
%feature("docstring") Mgx3D::Topo::CoEdgeMeshingProperty::setNbEdges "
virtual void Mgx3D::Topo::CoEdgeMeshingProperty::setNbEdges(const int nb)

Modificateur sur le nombre de bras. 

";
%feature("docstring") Mgx3D::Topo::CoFaceMeshingProperty " 
Propriété de la discrétisation d'une face commune. 
";

%feature("docstring") Mgx3D::Topo::CoFaceMeshingProperty::getMeshLaw "
virtual meshLaw Mgx3D::Topo::CoFaceMeshingProperty::getMeshLaw() const =0

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::CoFaceMeshingProperty::getMeshLawName "
virtual std::string Mgx3D::Topo::CoFaceMeshingProperty::getMeshLawName() const =0

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::CoFaceMeshingProperty::isStructured "
virtual bool Mgx3D::Topo::CoFaceMeshingProperty::isStructured() const =0

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::CoFaceMeshingProperty::CoFaceMeshingProperty "
Mgx3D::Topo::CoFaceMeshingProperty::CoFaceMeshingProperty()

Constructeur par défaut. 

";
%feature("docstring") Mgx3D::Internal::ContextIfc " 
Un contexte correspond à l'exécution d'une séance de travail où toutes les opérations de Magix 3D peuvent être effectuées et aussi défaites. 
A un contexte est associé un certain de nombre de managers permettant la création d'entités propres à la session de travail. En particulier, si deux contextes de travail sont ouvertes simultanément, les opérations de undo/redo sont spécifiques à chaque contexte.
Un contexte peut avoir des entités sélectionnées, sur lesquelles seront effectués des traitements. Chaque contexte est doté à cet effet d'un gestionnaire de sélection qui lui est spécifique. 
";

%feature("docstring") Mgx3D::Internal::ContextIfc::clearSession "
virtual void Mgx3D::Internal::ContextIfc::clearSession()


Réinitialisation de la session 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getGeomManager "
virtual Mgx3D::Geom::GeomManagerIfc& Mgx3D::Internal::ContextIfc::getGeomManager()

Accesseur sur le manager géométrique Uneexception est levée en l'absence de manager associé. 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getGeomManager "
virtual const Mgx3D::Geom::GeomManagerIfc& Mgx3D::Internal::ContextIfc::getGeomManager() const 



";
%feature("docstring") Mgx3D::Internal::ContextIfc::getGroupManager "
virtual Mgx3D::Group::GroupManagerIfc& Mgx3D::Internal::ContextIfc::getGroupManager()

Accesseur sur le manager de groupes Uneexception est levée en l'absence de manager associé. 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getGroupManager "
virtual const Mgx3D::Group::GroupManagerIfc& Mgx3D::Internal::ContextIfc::getGroupManager() const 



";
%feature("docstring") Mgx3D::Internal::ContextIfc::getLengthUnit "
virtual Utils::Unit::lengthUnit Mgx3D::Internal::ContextIfc::getLengthUnit() const 

retourne l'unité de longueur 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getMeshManager "
virtual Mgx3D::Mesh::MeshManagerIfc& Mgx3D::Internal::ContextIfc::getMeshManager()

Accesseur sur le manager de maillage Uneexception est levée en l'absence de manager associé. 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getMeshManager "
virtual const Mgx3D::Mesh::MeshManagerIfc& Mgx3D::Internal::ContextIfc::getMeshManager() const 



";
%feature("docstring") Mgx3D::Internal::ContextIfc::getName "
const std::string& Mgx3D::Internal::ContextIfc::getName() const 


Accesseur sur le nom de l'instance du context Le nom unique de l'instance. 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getSelectedEntities "
virtual std::vector<std::string> Mgx3D::Internal::ContextIfc::getSelectedEntities() const 


Retourne un vecteur avec les identifiants des entités actuellement sélectionnées 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getTopoManager "
virtual Mgx3D::Topo::TopoManagerIfc& Mgx3D::Internal::ContextIfc::getTopoManager()

Accesseur sur le manager topologique Uneexception est levée en l'absence de manager associé. 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::getTopoManager "
virtual const Mgx3D::Topo::TopoManagerIfc& Mgx3D::Internal::ContextIfc::getTopoManager() const 



";
%feature("docstring") Mgx3D::Internal::ContextIfc::redo "
virtual void Mgx3D::Internal::ContextIfc::redo()


Rejoue la dernière commande défaite 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::savePythonScript "
virtual void Mgx3D::Internal::ContextIfc::savePythonScript(std::string fileName, bool withouEntityName, TkUtil::Charset::CHARSET charset)



fileName : nom du fichier pour la sauvegarde 
withouEntityName : suivant que l'on accepte que les noms des entités apparaissent ou pas
charset : jeu de caractères utilisé pour l'encodage du fichier (Charset::UTF_8, Charset::ISO_8859)

";
%feature("docstring") Mgx3D::Internal::ContextIfc::setLengthUnit "
virtual Internal::M3DCommandResultIfc* Mgx3D::Internal::ContextIfc::setLengthUnit(const Utils::Unit::lengthUnit &lu)

change l'unité de longueur 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::undo "
virtual void Mgx3D::Internal::ContextIfc::undo()


Déseffectue la dernière commande effectuée 

";
%feature("docstring") Mgx3D::Internal::ContextIfc::ContextIfc "
Mgx3D::Internal::ContextIfc::ContextIfc(const ContextIfc &)


Constructeurs de copie et opérateurs = : interdits. 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric " 
Propriété de la discrétisation d'une arête avec une discrétisation ayant une progression géométrique. 
";

%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::EdgeMeshingPropertyGeometric "
Mgx3D::Topo::EdgeMeshingPropertyGeometric::EdgeMeshingPropertyGeometric(int nb, double raison, bool isDirect=true, double meshingEdgeLength=0.0)


Constructeur avec un nombre de bras spécifié, la raison de la progression et la longueur du premier bras
Si la longueur du premier bras est non nulle, alors le ratio est calculé à l'aide de la longueur de l'arête (projetée s'il y a lieu) 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::getMeshLawName "
virtual std::string Mgx3D::Topo::EdgeMeshingPropertyGeometric::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::getRatio "
double Mgx3D::Topo::EdgeMeshingPropertyGeometric::getRatio()

le ratio entre deux bras successifs 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::setNbEdges "
virtual void Mgx3D::Topo::EdgeMeshingPropertyGeometric::setNbEdges(const int nb)

change le nombre de bras demandé pour un côté, cela nécessite de refaire l'initialisation 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::computePolynome "
double Mgx3D::Topo::EdgeMeshingPropertyGeometric::computePolynome(const double raison)

calcul du polynome suivant raison 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::computeRaison "
double Mgx3D::Topo::EdgeMeshingPropertyGeometric::computeRaison(const double lg)

calcul la raison suivant le ratio longueur total / longueur premier bras 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyGeometric::initSomme "
void Mgx3D::Topo::EdgeMeshingPropertyGeometric::initSomme()

initialisation interne (calcul de la somme des coeff) 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertySpecificSize " 
Propriété de la discrétisation d'une arête avec une discrétisation où est spécifiée la taille cible des segments qui discrétiseront l'arête topologique. 
";

%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertySpecificSize::EdgeMeshingPropertySpecificSize "
Mgx3D::Topo::EdgeMeshingPropertySpecificSize::EdgeMeshingPropertySpecificSize(double size)

Constructeur avec une taille cible. 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertySpecificSize::getMeshLawName "
virtual std::string Mgx3D::Topo::EdgeMeshingPropertySpecificSize::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyUniform " 
Propriété de la discrétisation d'une arête avec une discrétisation uniforme. 
";

%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyUniform::EdgeMeshingPropertyUniform "
Mgx3D::Topo::EdgeMeshingPropertyUniform::EdgeMeshingPropertyUniform(int nb)

Constructeur avec un nombre de bras spécifié 

";
%feature("docstring") Mgx3D::Topo::EdgeMeshingPropertyUniform::getMeshLawName "
virtual std::string Mgx3D::Topo::EdgeMeshingPropertyUniform::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH " 
Propriété de la discrétisation d'une face commune. 
";

%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::FaceMeshingPropertyDelaunayGMSH "
Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::FaceMeshingPropertyDelaunayGMSH()

Constructeur avec paramètres par défaut. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::FaceMeshingPropertyDelaunayGMSH "
Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::FaceMeshingPropertyDelaunayGMSH(double min, double max)

Constructeur avec paramètres spécifiés. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMax "
double Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMax() const 

Accesseur sur paramètre de taille maximum. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMeshLaw "
CoFaceMeshingProperty::meshLaw Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMeshLawName "
std::string Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMin "
double Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::getMin() const 

Accesseur sur paramètre de taille minimum. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::isStructured "
bool Mgx3D::Topo::FaceMeshingPropertyDelaunayGMSH::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen " 
Propriété de la discrétisation d'une face commune. 
";

%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::FaceMeshingPropertyDelaunayNetgen "
Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::FaceMeshingPropertyDelaunayNetgen()

Constructeur. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::getMeshLaw "
CoFaceMeshingProperty::meshLaw Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::getMeshLawName "
std::string Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::isStructured "
bool Mgx3D::Topo::FaceMeshingPropertyDelaunayNetgen::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional " 
Propriété de la discrétisation d'une face commune suivant une direction. 
";

%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::FaceMeshingPropertyDirectional "
Mgx3D::Topo::FaceMeshingPropertyDirectional::FaceMeshingPropertyDirectional(Utils::Math::Point v1, Utils::Math::Point v2)

Constructeur avec direction suivant 2 sommets. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::FaceMeshingPropertyDirectional "
Mgx3D::Topo::FaceMeshingPropertyDirectional::FaceMeshingPropertyDirectional(meshDirLaw md)

Constructeur dont on connait la direction. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::getDir "
uint Mgx3D::Topo::FaceMeshingPropertyDirectional::getDir() const 

Accesseur sur la direction associée à la CoFace. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::getMeshLaw "
CoFaceMeshingProperty::meshLaw Mgx3D::Topo::FaceMeshingPropertyDirectional::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::getMeshLawName "
std::string Mgx3D::Topo::FaceMeshingPropertyDirectional::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::isStructured "
bool Mgx3D::Topo::FaceMeshingPropertyDirectional::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyDirectional::permDir "
void Mgx3D::Topo::FaceMeshingPropertyDirectional::permDir()

inversion de la direction 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational " 
Propriété de la discrétisation d'une face commune. 
";

%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::FaceMeshingPropertyRotational "
Mgx3D::Topo::FaceMeshingPropertyRotational::FaceMeshingPropertyRotational(Utils::Math::Point v1, Utils::Math::Point v2, Utils::Math::Point axis1, Utils::Math::Point axis2)


Constructeur avec rotation suivant 2 sommets, autour d'un axe défini par deux points 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::FaceMeshingPropertyRotational "
Mgx3D::Topo::FaceMeshingPropertyRotational::FaceMeshingPropertyRotational(meshDirLaw md, Utils::Math::Point axis1, Utils::Math::Point axis2)


Constructeur avec rotation déterminée, autour d'un axe défini par deux points 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::getAxis "
void Mgx3D::Topo::FaceMeshingPropertyRotational::getAxis(Utils::Math::Point &axis1, Utils::Math::Point &axis2)


Retourne l'axe de rotation 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::getDir "
uint Mgx3D::Topo::FaceMeshingPropertyRotational::getDir() const 

Accesseur sur la direction associée à la CoFace. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::getMeshLaw "
CoFaceMeshingProperty::meshLaw Mgx3D::Topo::FaceMeshingPropertyRotational::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::getMeshLawName "
std::string Mgx3D::Topo::FaceMeshingPropertyRotational::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::isStructured "
bool Mgx3D::Topo::FaceMeshingPropertyRotational::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyRotational::setAxis "
void Mgx3D::Topo::FaceMeshingPropertyRotational::setAxis(Utils::Math::Point &axis1, Utils::Math::Point &axis2)


Change l'axe de rotation 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyTransfinite " 
Propriété de la discrétisation d'une face commune. 
";

%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyTransfinite::FaceMeshingPropertyTransfinite "
Mgx3D::Topo::FaceMeshingPropertyTransfinite::FaceMeshingPropertyTransfinite()

Constructeur. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyTransfinite::getMeshLaw "
CoFaceMeshingProperty::meshLaw Mgx3D::Topo::FaceMeshingPropertyTransfinite::getMeshLaw() const 

Accesseur sur la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyTransfinite::getMeshLawName "
std::string Mgx3D::Topo::FaceMeshingPropertyTransfinite::getMeshLawName() const 

Accesseur sur le nom de la méthode de maillage. 

";
%feature("docstring") Mgx3D::Topo::FaceMeshingPropertyTransfinite::isStructured "
bool Mgx3D::Topo::FaceMeshingPropertyTransfinite::isStructured() const 

Indique si la face est structurée. 

";
%feature("docstring") Mgx3D::Geom::GeomInfo " 

";

%feature("docstring") Mgx3D::Geom::GeomInfo::curves "
std::vector<std::string> Mgx3D::Geom::GeomInfo::curves() const 



";
%feature("docstring") Mgx3D::Geom::GeomInfo::groupsName "
std::vector<std::string> Mgx3D::Geom::GeomInfo::groupsName() const 



";
%feature("docstring") Mgx3D::Geom::GeomInfo::surfaces "
std::vector<std::string> Mgx3D::Geom::GeomInfo::surfaces() const 



";
%feature("docstring") Mgx3D::Geom::GeomInfo::topoEntities "
std::vector<std::string> Mgx3D::Geom::GeomInfo::topoEntities() const 



";
%feature("docstring") Mgx3D::Geom::GeomInfo::vertices "
std::vector<std::string> Mgx3D::Geom::GeomInfo::vertices() const 



";
%feature("docstring") Mgx3D::Geom::GeomInfo::volumes "
std::vector<std::string> Mgx3D::Geom::GeomInfo::volumes() const 



";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc " 
Interface du gestionnaire des opérations effectuées au niveau du module géométrique. 
Ce gestionnaire joue le rôle de fabrique de commande, mais aussi d' entités géométriques. La raison est technique et non pas conceptuelle. Lorsque l'on crée une entité géométrique via une commande, les entités incidentes de dimension inférieure doivent aussi être crées puis stockées au niveau du GeomManager. Il est donc le plus à même de remplir ce rôle. 
";

%feature("docstring") Mgx3D::Geom::GeomManagerIfc::addToGroup "
virtual Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::addToGroup(std::vector< std::string > &ve, int dim, const std::string &groupName)


Ajoute un volume au gestionnaire Ajoute une surface au gestionnaire Ajoute une courbe au gestionnaire Ajoute un sommet au gestionnaire Ajoute une entité au gestionnaire Enlève un volume du gestionnaire Enlève une surface au gestionnaire Enlève une courbe au gestionnaire Enlève un sommet au gestionnaire Enlève une entité au gestionnaire Retourne le nom du dernier Volume [OBSOLETE] Retourne le nom de la dernière Surface [OBSOLETE] Retourne le nom de la dernière Curve [OBSOLETE] Retourne le nom du dernier Vertex [OBSOLETE] Ajoute un groupe à un ensemble d'entités géométriques, suivant une dimension 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::common "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::common(std::vector< std::string > &entities)

Intersection Booléenne de n entités géométriques. 

entities : les entités sur lesquelles on travaille 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::copy "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::copy(std::vector< std::string > &e, bool withTopo, std::string groupName)

création d'entités géométrique par copie 

e : les entités géométriques que l'on veut copier 
withTopo : a vrai si l'on doit copier la topologie avec la géométrie NB: [EB] pas de paramètre par défaut (groupName=\"\") lorsqu'il y a un std::vector en argument (pb dans swig) 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::cut "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::cut(std::string tokeep, std::vector< std::string > &tocut)

Différence Booléenne de n entités géométriques avec la première entité qui est conservée. 

tokeep : l'entité que l'on conserve 
tocut : les entités que l'on retire de tokeep 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::destroy "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::destroy(std::vector< std::string > &es, bool propagateDown)

suppression d'entités géométriques. Si on supprime une entité géométrique incidente à des entités géométriques de dimension supérieure, ces dernières sont aussi supprimées. Pour les entités géométriques incidentes de dimension inférieure, elles sont supprimés si propagateDown vaut true, sinon elles sont conservées. 

es : les entités géométriques à supprimer 
propagateDown : indique si l'on supprime les entités incidentes de dimension inférieure 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::exportIGES "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::exportIGES(const std::string &n)

Export au format IGES. 

n : le nom du ficher dans lequel on exporte 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::exportMDL "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::exportMDL(std::vector< std::string > &ge, const std::string &n)

Export d'une sélection dans un fichier au format MDL. 

ge : la liste des noms des entités à exporter 
n : le nom du ficher dans lequel on exporte 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::exportSTEP "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::exportSTEP(const std::string &n)

Export au format STEP. 

n : le nom du ficher dans lequel on exporte 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::exportVTK "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::exportVTK(const std::string &n)

Export d'une sélection dans un fichier au format VTK. 

n : le nom du ficher dans lequel on exporte 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::fuse "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::fuse(std::vector< std::string > &entities)

Union Booléenne de n entités géométriques. 

entities : les entités sur lesquelles on travaille 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getCurves "
virtual std::vector<std::string> Mgx3D::Geom::GeomManagerIfc::getCurves(bool onlyVisible=true) const 

retourne la liste des courbes gérées par le manager 

onlyVisible : suivant si l'on veut les courbes \"détruites\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getInfos "
virtual GeomInfo Mgx3D::Geom::GeomManagerIfc::getInfos(std::string e, int dim)

récupère des informations sur les entités géométriques. 

e : l'entité géométrique dont on veut des informations. 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getNbCurves "
virtual int Mgx3D::Geom::GeomManagerIfc::getNbCurves(bool onlyVisible=true) const 

retourne le nombre de courbes gérées par le manager 

onlyVisible : suivant si l'on veut les courbes \"détruites\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getNbSurfaces "
virtual int Mgx3D::Geom::GeomManagerIfc::getNbSurfaces(bool onlyVisible=true) const 

retourne le nombre de surfaces gérées par le manager 

onlyVisible : suivant si l'on veut les surfaces \"détruites\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getNbVertices "
virtual int Mgx3D::Geom::GeomManagerIfc::getNbVertices(bool onlyVisible=true) const 

retourne le nombre de sommets gérées par le manager 

onlyVisible : suivant si l'on veut les sommets \"détruits\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getNbVolumes "
virtual int Mgx3D::Geom::GeomManagerIfc::getNbVolumes(bool onlyVisible=true) const 

retourne le nombre de volumes gérés 

onlyVisible : suivant si l'on veut les volumes \"détruits\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getSurfaces "
virtual std::vector<std::string> Mgx3D::Geom::GeomManagerIfc::getSurfaces(bool onlyVisible=true) const 

retourne la liste des surfaces gérées par le manager 

onlyVisible : suivant si l'on veut les surfaces \"détruites\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getVertices "
virtual std::vector<std::string> Mgx3D::Geom::GeomManagerIfc::getVertices(bool onlyVisible=true) const 

retourne la liste des sommets gérées par le manager 

onlyVisible : suivant si l'on veut les sommets \"détruits\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::getVolumes "
virtual std::vector<std::string> Mgx3D::Geom::GeomManagerIfc::getVolumes(bool onlyVisible=true) const 

retourne la liste des volumes gérées par le manager 

onlyVisible : suivant si l'on veut les volumes \"détruits\" 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::glue "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::glue(std::vector< std::string > &entities)

Collage d'entités géométriques. 

entities : les entités sur lesquelles on travaille 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::importCATIA "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::importCATIA(std::string n)

Import d'un fichier au format CATIA. 

n : le nom du ficher dont le contenu doit etre importe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::importIGES "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::importIGES(std::string n)

Import d'un fichier au format IGES. 

n : le nom du ficher dont le contenu doit etre importe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::importMDL "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::importMDL(std::string n, const bool all)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::importMDL(std::string n, std::string groupe)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::importMDL(std::string n, std::vector< std::string > &zones)

Import d'un fichier au format MDL. 

n : le nom du ficher dont le contenu doit etre importe 
all : à vrai si l'on veut importer toutes les entités, faux si l'on ne prend que ce qui est utile au 2D 
groupe : le nom du groupe dont on importe les zones et les entités associées 
zones : la liste des zones que l'on importe avec les entités associées 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::importSTEP "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::importSTEP(std::string n)

Import d'un fichier au format STEP. 

n : le nom du ficher dont le contenu doit etre importe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::joinCurves "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::joinCurves(std::vector< std::string > &entities)

joinCurves opération permettant de réunir plusieurs courbes en une seule 

entities : les courbes à réunir 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::joinSurfaces "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::joinSurfaces(std::vector< std::string > &entities)

joinSurfaces opération permettant de réunir plusieurs surfaces en une seule 

entities : les surfaces à réunir 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::makeRevol "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::makeRevol(std::vector< std::string > &entities, const Utils::Math::Rotation &rot, const bool keep)

création d'une entité surfacique ou volumique par révolution d'une entité géométrique de dimension 1 ou 2 respectivement. 

entities : les entities dont on fait la révolution 
rot : une rotation 
keep : indique si l'on conserve (true) ou pas (false) les entités de départ 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newArcCircle "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newArcCircle(std::string pc, std::string pd, std::string pe, const bool direct, std::string groupName=\"\")

création d'un arc de cercle à partir de 3 sommets et de la donnée d'une direction de création (directe ou indirecte). 
   Une exception est retournée


pc : centre de l'arc de cercle 
pd : point de départ de l'arc de cercle 
pe : point de fin de l'arc de cercle 
direct : indique si l'on tourne dans le sens direct ou indirect 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newArcEllipse "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newArcEllipse(std::string pc, std::string pd, std::string pe, const bool direct, std::string groupName=\"\")

création d'un arc d'ellipse à partir de 3 sommets et de la donnée d'une direction de création (directe ou indirecte) 

pc : centre de l'arc de cercle 
pd : point de départ de l'arc de cercle 
pe : point de fin de l'arc de cercle 
direct : indique si l'on tourne dans le sens direct ou indirect 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newBox "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newBox(const Point &pmin, const Point &pmax, std::string groupName=\"\")

création d'une boite parallèle aux axes Ox,Oy et Oz à partir des points pmin et pmax où pmin est le point de plus petites coordonnées (x,y,z) et pmax le point de plus grandes coordonnées (x,y,z) 

pmin : le point min de la boite 
pmax : le point max de la boite 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newBSpline "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newBSpline(std::vector< std::string > &vp, std::string groupName)

création d'une bspline à partir de la liste ordonnée de points vp 

points : une liste ordonnée de points 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newCircle "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newCircle(std::string p1, std::string p2, std::string p3, std::string groupName=\"\")

création d'un cercle à partir de 3 points par lesquels le cercle passera 

p1 : premier point 
p2 : second point 
p3 : dernier point 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newCone "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newCone(const double &dr1, const double &dr2, const Vector &dv, const double &da, std::string groupName=\"\")

création d'un cone suivant un axe, avec deux rayons et une longueur 

dr1 : le premier rayon du cone (à l'origine) 
dr2 : le deuxième rayon du cone 
lg : la longueur 
dv : le vecteur pour l'axe et la longueur 
da : l'angle de la portion de cone 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newCone "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newCone(const double &dr1, const double &dr2, const Vector &dv, const Utils::Portion::Type &dt, std::string groupName=\"\")



";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newCylinder "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newCylinder(const Point &pcentre, const double &dr, const Vector &dv, const double &da, std::string groupName=\"\")

création d'un cylindre suivant un cercle, sa base (un cercle défini par un centre et un rayon) et son axe 

pcentre : le centre du cercle de base 
dr : le rayon du cercle 
dv : le vecteur pour l'axe du cylindre 
da : l'angle de la portion de cylindre 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newCylinder "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newCylinder(const Point &pcentre, const double &dr, const Vector &dv, const Utils::Portion::Type &dt, std::string groupName=\"\")

création d'un cylindre suivant un cercle, sa base (un cercle défini par un centre et un rayon) et son axe 

pcentre : le centre du cercle de base 
dr : le rayon du cercle 
dv : le vecteur pour l'axe du cylindre 
dt : le type de portion de cylindre que l'on crée 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newPlanarSurface "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newPlanarSurface(std::vector< std::string > &curves, std::string groupName)

création d'une surface planaire à partir d'un ensemble de courbes 

curves : le nom des courbes définissant le contour 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newPrism "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newPrism(std::string base, const Vector &dp, std::string groupName=\"\")

création d'un prisme à partir d'une surface et d'un vecteur de direction 

base : la surface à extruder 
dp : le vecteur d'extrusion 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newSegment "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newSegment(std::string n1, std::string n2, std::string groupName=\"\")

création d'un segment à partir des sommets v1, v2 

v1 : le premier sommet 
v2 : le second somment 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newSphere "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newSphere(const Point &pcentre, const double &radius, const double &angle, std::string groupName=\"\")
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newSphere(const Point &pcentre, const double &radius, const Utils::Portion::Type &dt, std::string groupName=\"\")

Création d'une sphère. 

pcentre : Le centre de la sphère 
radius : Le rayon de la sphère 
angle : L'angle pour avoir un quartier de sphère (ex : PI/2 fournit un 1/4 de sphère) 
dt : portion de sphere à créer 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newVertex "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newVertex(std::string vertexName, std::string curveName, std::string groupName=\"\")
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newVertex(std::string curveName, const double &param, std::string groupName=\"\")
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newVertex(const Point &p, std::string groupName=\"\")
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newVertex(const double &x, const double &y, const double &z, std::string groupName=\"\")

création d'un sommet géométrique à partir d'un point p(x,y,z) et d'une courbe. Le sommet crée est le projet de p sur la courbe. 
création d'un sommet géométrique à partir d'une courbet et d'un parametrage a dans [0,1] indiquant la position du point sur la courbe. 
création d'un sommet géométrique à partir d'un point (x,y,z) 
création d'un sommet géométrique à partir d'un point (x,y,z) 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::newVerticesCurvesAndPlanarSurface "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::newVerticesCurvesAndPlanarSurface(std::vector< Utils::Math::Point > &points, std::string groupName)

Création d'une surface planaire à partir d'un ensemble de points Création des sommets géométriques et des segments en même temps. 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::removeFromGroup "
virtual Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::removeFromGroup(std::vector< std::string > &ve, int dim, const std::string &groupName)


Enlève un groupe à un ensemble d'entités géométriques, suivant une dimension 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::rotate "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::rotate(std::vector< std::string > &entities, const Utils::Math::Rotation &rot)

rotation d'une ou plusieurs entités géométrique 

entities : les entities dont on fait la rotation 
rot : la rotation 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::scale "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::scale(std::vector< std::string > &geo, const double factor)

création d'un objet géométrique par homothétie 

geo : les objets d'origine 
factor : le facteur d'homothétie 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::section "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::section(std::vector< std::string > &entities, std::string tool)

Section d'un groupe d'entités géométrique par un outil. 

entities : les entités que l'on veut couper 
tool : l'entité pour découper 
planeGroupName : le nom du groupe dans lequel on place toutes les entites sur le plan 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::sectionByPlane "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::sectionByPlane(std::vector< std::string > &entities, Utils::Math::Plane *tool, std::string planeGroupName)



";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::setGroup "
virtual Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::setGroup(std::vector< std::string > &ve, int dim, const std::string &groupName)


Défini le groupe pour un ensemble d'entités géométriques, suivant une dimension 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::translate "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::translate(std::vector< std::string > &ve, const Vector &dp)

translation des entités (identifiée par un nom unique pour python) suivant le vecteur de translation défini par dp 

ve : nom des entités géométrique à translater 
dp : le vecteur de translation 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::unionFaces "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Geom::GeomManagerIfc::unionFaces(std::vector< std::string > &entities)

Nettoyage de volumes. 

entities : les entités sur lesquelles on travaille 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::~GeomManagerIfc "
virtual Mgx3D::Geom::GeomManagerIfc::~GeomManagerIfc()

Destructeur. RAS. 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::GeomManagerIfc "
Mgx3D::Geom::GeomManagerIfc::GeomManagerIfc(const std::string &name, Internal::ContextIfc *c)

Constructeur. 

c : le contexte ce qui permet d'accéder entre autre au CommandManager 

";
%feature("docstring") Mgx3D::Geom::GeomManagerIfc::GeomManagerIfc "
Mgx3D::Geom::GeomManagerIfc::GeomManagerIfc(const GeomManagerIfc &)

Constructeur de copie. Interdit. 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc " 
Interface de gestionnaire des opérations effectuées au niveau des groupes. 
";

%feature("docstring") Mgx3D::Group::GroupManagerIfc::addCartesianPerturbation "
virtual void Mgx3D::Group::GroupManagerIfc::addCartesianPerturbation(const std::string &nom, PyObject *py_obj)


Ajoute une modification du maillage pour un groupe (2D ou 3D)
Il peut s'agir d'une perturbation sous forme de fonction Python Les coordonnées sont cartésiennes

nom : nom du groupe avec lequel on effectue la perturbation 
py_obj : l'objet python auquel il est fait appel pour modifier le maillage 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::addPolarPerturbation "
virtual void Mgx3D::Group::GroupManagerIfc::addPolarPerturbation(const std::string &nom, PyObject *py_obj)


Ajoute une modification du maillage pour un groupe (2D ou 3D)
Il peut s'agir d'une perturbation sous forme de fonction Python Les coordonnées sont polaires

nom : nom du groupe avec lequel on effectue la perturbation 
py_obj : l'objet python auquel il est fait appel pour modifier le maillage 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::addSmoothing "
virtual void Mgx3D::Group::GroupManagerIfc::addSmoothing(const std::string &nom, Mesh::SurfacicSmoothing &sm)
virtual void Mgx3D::Group::GroupManagerIfc::addSmoothing(const std::string &nom, Mesh::VolumicSmoothing &sm)


Ajoute un lissage surfacique ou volumique du maillage pour un groupe 2D ou 3D

nom : nom du groupe avec lequel on effectue le lissage 
sm : la classe qui défini le lissage surfacique ou volumique

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::addToGroup "
virtual void Mgx3D::Group::GroupManagerIfc::addToGroup(std::vector< std::string > &ve, int dim, const std::string &groupName)


Ajoute le contenu d'un ensemble géométrique à un groupe donné 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::getBlocksEntities "
virtual std::vector<std::string> Mgx3D::Group::GroupManagerIfc::getBlocksEntities(const std::vector< std::string > &vg)


Retourne les noms des blocks géométriques à partir des groupes sélectionnées 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::getGeomEntities "
virtual std::vector<std::string> Mgx3D::Group::GroupManagerIfc::getGeomEntities(std::vector< std::string > &vg)


Retourne les entités géométriques à partir des groupes sélectionnées 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::getInfos "
virtual std::string Mgx3D::Group::GroupManagerIfc::getInfos(const std::string &name, int dim) const 


Retourne une string avec les informations relatives à l'entité 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::getVisibles "
virtual std::vector<std::string> Mgx3D::Group::GroupManagerIfc::getVisibles() const 

retourne la liste des noms des groupes visibles 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::GroupManagerIfc "
Mgx3D::Group::GroupManagerIfc::GroupManagerIfc(const std::string &name, Internal::ContextIfc *c)

Constructeur. 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::mesh "
virtual void Mgx3D::Group::GroupManagerIfc::mesh(const std::vector< std::string > &vg)


Lance la commande de maillage pour les entitées topologiques associées aux groupes sélectionnés 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::removeFromGroup "
virtual void Mgx3D::Group::GroupManagerIfc::removeFromGroup(std::vector< std::string > &ve, int dim, const std::string &groupName)


Enlève le contenu d'un ensemble géométrique à un groupe donné 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::rotate "
virtual void Mgx3D::Group::GroupManagerIfc::rotate(std::vector< std::string > &vg, const Utils::Math::Rotation &rot)


Opération de rotation d'un ou plusieurs groupes donc des géométries et des topologies (si elles existent)

vg : noms des groupes à modifier 
rot : la rotation 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::scale "
virtual void Mgx3D::Group::GroupManagerIfc::scale(std::vector< std::string > &vg, const double factor)


Opération d'homothétie d'un ou plusieurs groupes donc des géométries et des topologies (si elles existent)

vg : noms des groupes à modifier 
factor : le facteur d'homothétie 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::setGroup "
virtual void Mgx3D::Group::GroupManagerIfc::setGroup(std::vector< std::string > &ve, int dim, const std::string &groupName)


Défini le contenu d'un ensemble géométrique à un groupe donné 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::translate "
virtual void Mgx3D::Group::GroupManagerIfc::translate(std::vector< std::string > &vg, const Vector &dp)


Opération de translation d'un ou plusieurs groupes donc des géométries et des topologies (si elles existent)

vg : noms des groupes à translater 
dp : le vecteur de translation 

";
%feature("docstring") Mgx3D::Group::GroupManagerIfc::~GroupManagerIfc "
virtual Mgx3D::Group::GroupManagerIfc::~GroupManagerIfc()

Destructeur. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc " 
Gestionnaire des opérations effectuées au niveau du module de maillage. Le gestionnaire de maillage est l'objet qui stocke un lien vers une interface avec le maillage (GMDS) pour une session donnée. 
";

%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::compareWithMesh "
virtual bool Mgx3D::Mesh::MeshManagerIfc::compareWithMesh(std::string nom)

Compare le maillage actuel avec un maillage sur disque, return true si ok. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getInfos "
virtual std::string Mgx3D::Mesh::MeshManagerIfc::getInfos(const std::string &name, int dim) const 


Retourne une string avec les informations relatives à l'entité 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getNbClouds "
virtual int Mgx3D::Mesh::MeshManagerIfc::getNbClouds(bool onlyVisible=true) const 


Retourne les Nuages Retourne les Surfaces Retourne les Volumes 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getNbFaces "
virtual int Mgx3D::Mesh::MeshManagerIfc::getNbFaces()



";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getNbNodes "
virtual int Mgx3D::Mesh::MeshManagerIfc::getNbNodes()



";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getNbRegions "
virtual int Mgx3D::Mesh::MeshManagerIfc::getNbRegions()



";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getNbSurfaces "
virtual int Mgx3D::Mesh::MeshManagerIfc::getNbSurfaces(bool onlyVisible=true) const 



";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getNbVolumes "
virtual int Mgx3D::Mesh::MeshManagerIfc::getNbVolumes(bool onlyVisible=true) const 



";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::getStrategy "
virtual strategy Mgx3D::Mesh::MeshManagerIfc::getStrategy()

Accesseur sur la strategie. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::MeshManagerIfc "
Mgx3D::Mesh::MeshManagerIfc::MeshManagerIfc(const std::string &name, Internal::ContextIfc *c)

Constructeur. 

c : le contexte ce qui permet d'accéder entre autre au CommandManager 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::newAllBlocksMesh "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Mesh::MeshManagerIfc::newAllBlocksMesh()

Génère le maillage pour l'ensemble des blocs. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::newAllFacesMesh "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Mesh::MeshManagerIfc::newAllFacesMesh()

Génère le maillage pour l'ensemble des faces (communes) 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::newBlocksMesh "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Mesh::MeshManagerIfc::newBlocksMesh(std::vector< std::string > &entities)

Génère le maillage pour une liste de blocs. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::newFacesMesh "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Mesh::MeshManagerIfc::newFacesMesh(std::vector< std::string > &entities)

Génère le maillage pour une liste de faces (communes) 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::setStrategy "
virtual void Mgx3D::Mesh::MeshManagerIfc::setStrategy(const strategy &st)

Modificateur de strategie. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::smooth "
virtual void Mgx3D::Mesh::MeshManagerIfc::smooth()

Lissage du maillage. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::writeMli "
virtual void Mgx3D::Mesh::MeshManagerIfc::writeMli(std::string nom)

Sauvegarde d'un maillage au format lima (mli) 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::~MeshManagerIfc "
virtual Mgx3D::Mesh::MeshManagerIfc::~MeshManagerIfc()

Destructeur. 

";
%feature("docstring") Mgx3D::Mesh::MeshManagerIfc::MeshManagerIfc "
Mgx3D::Mesh::MeshManagerIfc::MeshManagerIfc(const MeshManagerIfc &)

Constructeur de copie. Interdit. 

";
%feature("docstring") MeshModificationItf " 

";

%feature("docstring") Mgx3D::Utils::Math::Plane " 

";

%feature("docstring") Mgx3D::Utils::Math::Plane::getNormal "
virtual Vector Mgx3D::Utils::Math::Plane::getNormal() const 



";
%feature("docstring") Mgx3D::Utils::Math::Plane::getPoint "
virtual Point Mgx3D::Utils::Math::Plane::getPoint() const 



";
%feature("docstring") Mgx3D::Utils::Math::Plane::Plane "
Mgx3D::Utils::Math::Plane::Plane()


Constructeur par défaut définissant le plan XY 

";
%feature("docstring") Mgx3D::Utils::Math::Plane::Plane "
Mgx3D::Utils::Math::Plane::Plane(Point &p, Vector &v)


Constructeur Définition d'un plan à partir d'un point et d'un vecteur normal

p : un point 
v : un vecteur, qui définit la normale au plan 

";
%feature("docstring") Mgx3D::Utils::Math::Plane::Plane "
Mgx3D::Utils::Math::Plane::Plane(Point &p1, Point &p2, Point &p3)


Constructeur Définition d'un plan à partir de trois points

p1 : un premier point 
p2 : un second point 
p3 : un plan point 

";
%feature("docstring") Mgx3D::Utils::Math::Plane::Plane "
Mgx3D::Utils::Math::Plane::Plane(const XYZ &type, const double offset=0)


Constructeur Définition d'un plan comme étant parallèle à XY, YZ ou ZY. Le déplacement est indiqué par un décalage

type : le type de plan (XY, XZ ou YZ) 
offset : le décalage par rapport au plan initial de type  

";
%feature("docstring") Mgx3D::Utils::Math::Plane::Plane "
Mgx3D::Utils::Math::Plane::Plane(const Plane &p)



p3 : le plan à copier 

";
%feature("docstring") Mgx3D::Utils::Math::Point " 

";

%feature("docstring") Mgx3D::Utils::Math::Point::dot "
virtual double Mgx3D::Utils::Math::Point::dot(const Point &v2) const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::getCoord "
virtual double Mgx3D::Utils::Math::Point::getCoord(int i)



";
%feature("docstring") Mgx3D::Utils::Math::Point::getX "
virtual double Mgx3D::Utils::Math::Point::getX() const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::getY "
virtual double Mgx3D::Utils::Math::Point::getY() const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::getZ "
virtual double Mgx3D::Utils::Math::Point::getZ() const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::isEpsilonEqual "
virtual bool Mgx3D::Utils::Math::Point::isEpsilonEqual(const Point &v) const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::length "
virtual double Mgx3D::Utils::Math::Point::length(const Point &v) const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::length2 "
virtual double Mgx3D::Utils::Math::Point::length2(const Point &v) const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::norme "
double Mgx3D::Utils::Math::Point::norme() const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::norme2 "
double Mgx3D::Utils::Math::Point::norme2() const 



";
%feature("docstring") Mgx3D::Utils::Math::Point::Point "
Mgx3D::Utils::Math::Point::Point(double x0, double y0, double z0)



";
%feature("docstring") Mgx3D::Utils::Math::Point::Point "
Mgx3D::Utils::Math::Point::Point(const Point &p)



";
%feature("docstring") Mgx3D::Utils::Math::Point::Point "
Mgx3D::Utils::Math::Point::Point()



";
%feature("docstring") Mgx3D::Utils::Math::Point::Point "
Mgx3D::Utils::Math::Point::Point(const Spherical &s)



";
%feature("docstring") Mgx3D::Utils::Math::Point::setCoord "
virtual void Mgx3D::Utils::Math::Point::setCoord(int i, double d)



";
%feature("docstring") Mgx3D::Utils::Math::Point::setX "
virtual void Mgx3D::Utils::Math::Point::setX(double v)



";
%feature("docstring") Mgx3D::Utils::Math::Point::setXYZ "
virtual void Mgx3D::Utils::Math::Point::setXYZ(double x, double y, double z)



";
%feature("docstring") Mgx3D::Utils::Math::Point::setY "
virtual void Mgx3D::Utils::Math::Point::setY(double v)



";
%feature("docstring") Mgx3D::Utils::Math::Point::setZ "
virtual void Mgx3D::Utils::Math::Point::setZ(double v)



";
%feature("docstring") Mgx3D::Utils::Portion " 

";

%feature("docstring") Mgx3D::Utils::Portion::getAngleInDegree "
static double Mgx3D::Utils::Portion::getAngleInDegree(Type t)



";
%feature("docstring") Mgx3D::Utils::Portion::getAngleInRadian "
static double Mgx3D::Utils::Portion::getAngleInRadian(Type t)



";
%feature("docstring") Mgx3D::Utils::Portion::getName "
static std::string Mgx3D::Utils::Portion::getName(Type t)



";
%feature("docstring") Mgx3D::Utils::Math::Rotation " 

Classe pour définir une rotation
On se limite dans un premier temps à une rotation défini suivant un axe et un angle (en degrés) 
";

%feature("docstring") Mgx3D::Utils::Math::Rotation::getAngle "
virtual double Mgx3D::Utils::Math::Rotation::getAngle() const 



";
%feature("docstring") Mgx3D::Utils::Math::Rotation::getAxis "
virtual AXIS Mgx3D::Utils::Math::Rotation::getAxis() const 



";
%feature("docstring") Mgx3D::Utils::Math::Rotation::getAxis1 "
virtual Point Mgx3D::Utils::Math::Rotation::getAxis1() const 



";
%feature("docstring") Mgx3D::Utils::Math::Rotation::getAxis2 "
virtual Point Mgx3D::Utils::Math::Rotation::getAxis2() const 



";
%feature("docstring") Mgx3D::Utils::Math::Rotation::Rotation "
Mgx3D::Utils::Math::Rotation::Rotation(const Point &p1, const Point &p2, const double &angle)

Rotation suivant un axe défini par 2 points et un angle en degrés. 

";
%feature("docstring") Mgx3D::Utils::Math::Rotation::Rotation "
Mgx3D::Utils::Math::Rotation::Rotation(AXIS axis, const double &angle)


Rotation suivant un axe du repère cartésien et un angle en degrés
Une exception est levée si axis est différent de X, Y ou Z. 

";
%feature("docstring") Mgx3D::Utils::Math::Rotation::Rotation "
Mgx3D::Utils::Math::Rotation::Rotation(const Rotation &r)



";
%feature("docstring") Mgx3D::Utils::Math::RotX " 

Classe pour définir une rotation suivant l'axe des X 
";

%feature("docstring") Mgx3D::Utils::Math::RotX::RotX "
Mgx3D::Utils::Math::RotX::RotX(const double &angle)



";
%feature("docstring") Mgx3D::Utils::Math::RotX::RotX "
Mgx3D::Utils::Math::RotX::RotX(const RotX &r)



";
%feature("docstring") Mgx3D::Utils::Math::RotY " 

Classe pour définir une rotation suivant l'axe des Y 
";

%feature("docstring") Mgx3D::Utils::Math::RotY::RotY "
Mgx3D::Utils::Math::RotY::RotY(const double &angle)



";
%feature("docstring") Mgx3D::Utils::Math::RotY::RotY "
Mgx3D::Utils::Math::RotY::RotY(const RotX &r)



";
%feature("docstring") Mgx3D::Utils::Math::RotZ " 

Classe pour définir une rotation suivant l'axe des Z 
";

%feature("docstring") Mgx3D::Utils::Math::RotZ::RotZ "
Mgx3D::Utils::Math::RotZ::RotZ(const double &angle)



";
%feature("docstring") Mgx3D::Utils::Math::RotZ::RotZ "
Mgx3D::Utils::Math::RotZ::RotZ(const RotX &r)



";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing " 

Objet qui va modifier un maillage suivant un lissage surfacique 
";

%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::getMethod "
eSurfacicMethod Mgx3D::Mesh::SurfacicSmoothing::getMethod() const 

accesseur sur la méthode de lissage 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::getNbIterations "
int Mgx3D::Mesh::SurfacicSmoothing::getNbIterations() const 

accesseur sur le nombre d'itérations 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::getSolver "
eSolver Mgx3D::Mesh::SurfacicSmoothing::getSolver() const 

accesseur sur le solver 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::setMethod "
void Mgx3D::Mesh::SurfacicSmoothing::setMethod(eSurfacicMethod methodeLissage)

change la méthode de lissage 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::setNbIterations "
void Mgx3D::Mesh::SurfacicSmoothing::setNbIterations(int nbIterations)

change le nombre d'itérations 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::setSolver "
void Mgx3D::Mesh::SurfacicSmoothing::setSolver(eSolver solver)

change le type de solver utilisé par Mesquite 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::SurfacicSmoothing "
Mgx3D::Mesh::SurfacicSmoothing::SurfacicSmoothing()

Constructeur pour fonction de lissage surfacique avec les arguments par défaut. 

";
%feature("docstring") Mgx3D::Mesh::SurfacicSmoothing::SurfacicSmoothing "
Mgx3D::Mesh::SurfacicSmoothing::SurfacicSmoothing(int nbIterations, eSurfacicMethod methodeLissage, eSolver solver)

Constructeur avec paramètres différents du défaut. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc " 
Interface de gestionnaire des opérations effectuées au niveau du module topologique. 
Pour une documentation plus complète, voir TopoManager TopoManager 
";

%feature("docstring") Mgx3D::Topo::TopoManagerIfc::alignVertices "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::alignVertices(std::vector< std::string > &vertices_names)

Aligne les sommets sélectionnés en fonction de leur projection et des extrémités de la sélection. 
Il faut sélectionner un ensemble de sommets reliés entre eux par des arêtes. Les sommets extrémités ne sont pas déplacés. Les sommets entre ces extrémités sont placés sur l'intersection entre le segment délimités par les 2 sommets et par la courbe ou surface sur laquelle est faite la projection et simplement projeté sur le segment s'il n'y a pas de projection (ou si elle est faite sur un point).

vertices_names : la liste des noms des sommets concernés 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::copy "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::copy(std::vector< std::string > &vb, std::string vo)

création d'entités topologique par copie 

vb : les blocs topologiques que l'on veut copier 
vo : le volume auquel on associe les blocs 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::destroy "
virtual void Mgx3D::Topo::TopoManagerIfc::destroy(std::vector< std::string > &ve, bool propagate)

Destruction d'entités topologiques. 

ve : noms des entités topologiques à détruire 
propagate : booléen à faux si on ne détruit que les entités, à vrai si on détruit les entités de dimensions inférieures qui ne sont pas reliées à une autre entité de dimension dim 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::extendSplitBlock "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::extendSplitBlock(std::string nbloc, std::string narete)

Découpe un bloc en deux en utilisant une arête qui coupe déjà une de ses faces. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::extendSplitFace "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::extendSplitFace(std::string coface_name, std::string nsommet)

Découpage d'une face structurée 2D ou 3D en deux suivant un sommet. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::fuse "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::fuse(std::vector< std::string > &coedges_names)

Fait la fusion de contours (Topo::CoEdges) 
getFusableEdges

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getBorderFaces "
virtual std::vector<std::string> Mgx3D::Topo::TopoManagerIfc::getBorderFaces() const 


Retourne la liste des faces communes au bord Ce sont les cofaces reliés à un seul bloc 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getDefaultNbMeshingEdges "
virtual int Mgx3D::Topo::TopoManagerIfc::getDefaultNbMeshingEdges()


Retourne la discrétisation par défaut pour les arêtes 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getDirOnBlock "
virtual std::string Mgx3D::Topo::TopoManagerIfc::getDirOnBlock(const std::string &aname, const std::string &bname) const 


Retourne une string avec les informations sur la direction de l'arête dans le bloc 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getFusableEdges "
virtual std::vector<std::vector<std::string> > Mgx3D::Topo::TopoManagerIfc::getFusableEdges()

Retourne la liste de groupes de noms d'arêtes succeptible d'être fusionnées. 
fuse

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getInfos "
virtual std::string Mgx3D::Topo::TopoManagerIfc::getInfos(const std::string &name, int dim) const 


Retourne une string avec les informations relatives à l'entité 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getInvalidBlocks "
virtual std::vector<std::string> Mgx3D::Topo::TopoManagerIfc::getInvalidBlocks() const 


Retourne la liste des blocs invalides 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getInvalidFaces "
virtual std::vector<std::string> Mgx3D::Topo::TopoManagerIfc::getInvalidFaces() const 


Retourne la liste des faces communes invalides Par exemple les faces structurées avec des discrétisations inégales pour 2 côtés opposés 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getNbBlocks "
virtual int Mgx3D::Topo::TopoManagerIfc::getNbBlocks() const 


le nombre de blocs référencés par le TopoManager 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getSemiConformalFaces "
virtual std::vector<std::string> Mgx3D::Topo::TopoManagerIfc::getSemiConformalFaces() const 


Retourne la liste des faces communes semi-conformes 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::getVisibleBlocks "
virtual std::vector<std::string> Mgx3D::Topo::TopoManagerIfc::getVisibleBlocks() const 

retourne la liste des noms des blocs visibles 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::glue2Blocks "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::glue2Blocks(std::string na, std::string nb)

Fusion des faces communes entre 2 blocs. 
Le premier bloc dont les faces prennent la place des autres. Le deuxième bloc dont les sommets d'une de ses faces doivent être le plus proche possible de celle de l'autre 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::glue2Faces "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::glue2Faces(std::string na, std::string nb)

Fusion de 2 faces communes. 
La 1ère Face commune prend la place de l'autre. Les sommets doivent être le plus proche possible 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::glue2Vertices "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::glue2Vertices(std::string na, std::string nb)

Fusion de 2 sommets. 
Le 1er sommet prend la place de l'autre. Attention, les arêtes et les faces ne sont pas colléees pour autant 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::importMDL "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::importMDL(std::string n, const bool all)

Import d'un fichier au format MDL (Géométrie et topologie) 

n le nom du ficher dont le contenu doit etre importe
all à vrai si l'on veut importer toutes les entités, faux si l'on ne prend que ce qui est utile au 2D

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::makeBlocksByRevol "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::makeBlocksByRevol(std::vector< std::string > &coedges_names, const Utils::Portion::Type &dt)

Construction de la topologie 3D par extrusion/rotation de la topologie 2D tout en faisant apparaitre un o-grid au centre. 

coedges_names : Les noms des arêtes pour repérer le tracé de l'ogrid On fait la révolution pour les faces reliées à ces arêtes. 
dt : la portion de révolution. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newBoxWithTopo "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newBoxWithTopo(const Point &pmin, const Point &pmax, bool meshStructured=true, std::string groupName=\"\")

Création d'une boite parallèle aux axes Ox,Oy et Oz à partir des points pmin et pmax où pmin est le point de plus petites coordonnées (x,y,z) et pmax le point de plus grandes coordonnées (x,y,z), avec sa topologie associée. 

pmin : le point min de la boite 
pmax : le point max de la boite 
meshStructured : un booléen pour choisir si le maillage doit être structuré ou non 
groupName : optionnellement un nom de groupe

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newBoxWithTopo "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newBoxWithTopo(const Point &pmin, const Point &pmax, const int ni, const int nj, const int nk, std::string groupName=\"\")

Création d'une boite parallèle aux axes Ox,Oy et Oz à partir des points pmin et pmax où pmin est le point de plus petites coordonnées (x,y,z) et pmax le point de plus grandes coordonnées (x,y,z), avec sa topologie associée en ni X nj X nk mailles. 

pmin : le point min de la boite 
pmax : le point max de la boite 
ni : le nombre de bras sur la direction des x 
nj : le nombre de bras sur la direction des y 
nk : le nombre de bras sur la direction des z 
groupName : optionnellement un nom de groupe

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newCylinderWithTopo "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newCylinderWithTopo(const Point &pcentre, const double &dr, const Vector &dv, const double &da, bool meshStructured, const double &rat, const int naxe, const int ni, const int nr, std::string groupName=\"\")

Création d'un cylindre avec une topologie, suivant un cercle, sa base (un cercle défini par un centre et un rayon) et son axe. 

pcentre : le centre du cercle de base 
dr : le rayon du cercle 
dv : le vecteur pour l'axe du cylindre 
da : l'angle de la portion de cylindre 
meshStructured : un booléen pour choisir si le maillage doit être structuré ou non 
rat : le ratio pour placer les sommets de l'o-grid, entre le centre et le rayon (vaut 1 dans le cas d'un seul bloc) 
naxe : le nombre de bras suivant l'axe du cylindre 
ni : le nombre de bras entre le point triple et le plan de symétrie 
nr : le nombre de couches entre le bloc central et la surface externe 
groupName : optionnellement un nom de groupe 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newFreeTopoOnGeometry "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newFreeTopoOnGeometry(std::string ne)

Création d'un block (ou d'une face) topologique structuré sans association. 
Ne fonctionne que pour un volume ou une surface Seule l'entité de plus haut niveau (Block ou CoFace) sera associée à l'entité géométrique

ne : le nom de l'entité géométrique sur laquelle s'appuiera la topologie 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newIJBoxesWithTopo "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newIJBoxesWithTopo(int ni, int nj, bool alternateStruture)

Création d'une grille de ni par nj boites répartis sur Ox et Oy les boites sont toutes de taille 1x1x1 et ont une topologie associée structurée si alternateStruture == false et sinon alternativement une structurée une non-structurée. 

ni : le nombre de boite dans la direction Ox 
nj : le nombre de boite dans la direction Oy 
alternateStruture : vrai si l'on veut des blocs alternativement st/non-st

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newIJKBoxesWithTopo "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newIJKBoxesWithTopo(int ni, int nj, int nk, bool alternateStruture)

Création d'une grille de ni par nj par nk boites répartis sur Ox, Oy et Oz les boites sont toutes de taille 1x1x1 et ont une topologie associée structurée si alternateStruture == false et sinon alternativement une structurée une non-structurée. 

ni : le nombre de boite dans la direction Ox 
nj : le nombre de boite dans la direction Oy 
nk : le nombre de boite dans la direction Oz 
alternateStruture : vrai si l'on veut des blocs alternativement st/non-st

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newInsertionTopoOnGeometry "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newInsertionTopoOnGeometry(std::string ne)

Création d'une topologie pour l'insertion s'appuyant sur une géométrie. 

ne : le nom de l'entité géométrique sur laquelle s'appuiera la topologie 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newSphereWithTopo "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newSphereWithTopo(const Point &pcentre, const double &radius, const Utils::Portion::Type &dt, bool meshStructured, const double &rat, const int ni, const int nr, std::string groupName=\"\")

Création d'une sphère avec une topologie, suivant un centre, son rayon et pour une portion donnée. 

pcentre : le centre de la sphère 
dr : le rayon de la sphère 
dt : portion à créer 
meshStructured : un booléen pour choisir si le maillage doit être structuré ou non 
rat : le ratio pour placer les sommets de l'o-grid, entre le centre et le rayon (vaut 1 dans le cas d'un seul bloc) 
ni : le nombre de bras entre le point triple et le plan de symétrie 
nr : le nombre de couches entre le bloc central et la surface externe 
groupName : optionnellement un nom de groupe

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newStructuredTopoOnGeometry "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newStructuredTopoOnGeometry(std::string ne)

Création d'une topologie structurée s'appuyant sur une géométrie. 
Retourne une exception si la topologie créée ne peut pas être rendue structurée
Ne fonctionne que pour un volume ou une surface

ne : le nom de l'entité géométrique sur laquelle s'appuiera la topologie 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newTopoOGridOnGeometry "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newTopoOGridOnGeometry(std::string ne, const double &rat)

Création d'une topologie structurée en o-grid. 
La topologie créée s'adapte à un cylindre (5 blocs) ou à une sphère

ne : le nom de l'entité géométrique sur laquelle s'appuiera la topologie 
rat : le ratio de la distance entre l'axe du cylindre et son rayon pour placer le sommet du o-grid 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::newUnstructuredTopoOnGeometry "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::newUnstructuredTopoOnGeometry(std::string ne)

Création d'une topologie non-structurée s'appuyant sur une géométrie. 
La topologie créée est non-structurée

ne : le nom de l'entité géométrique sur laquelle s'appuiera la topologie 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::refine "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::refine(int ratio)

Raffine toute les arêtes. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::reverseDirection "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::reverseDirection(std::vector< std::string > &edge_names)

Change le sens de la discrétisation pour une liste d'arête communes. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::rotate "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::rotate(std::vector< std::string > &ve, const Utils::Math::Rotation &rot, const bool withGeom)


Opération de rotation de la topologie (et de la géométrie suivant option)
La rotation des objets topologiques de haut niveau (bloc par exemple) entraine celle des objets de niveau inférieurs (face, arêtes et sommets) et réciproquement.

rot : la rotation 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::scale "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::scale(std::vector< std::string > &ve, const double &facteur, const bool withGeom)


Opération d'homothétie de la topologie (et de la géométrie suivant option) L'homothétie des objets topologiques de haut niveau (bloc par exemple) entraine celle des objets de niveau inférieurs (face, arêtes et sommets) et réciproquement.

facteur : supérieur à 0 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setAllMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setAllMeshingProperty(Topo::CoEdgeMeshingProperty &emp)

Change la propriété de discrétisation pour toutes les arêtes. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setAllMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setAllMeshingProperty(Topo::CoFaceMeshingProperty &emp)

Change la propriété de discrétisation pour toutes les faces. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setAllMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setAllMeshingProperty(Topo::BlockMeshingProperty &emp)

Change la propriété de discrétisation pour tous les blocs. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setBlockMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setBlockMeshingProperty(Topo::BlockMeshingProperty &emp, std::string bl)

Change la propriété de discrétisation pour un bloc. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setDefaultNbMeshingEdges "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setDefaultNbMeshingEdges(int nb)


Commande qui change le nombre de bras par défaut pour une arête 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setEdgeMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setEdgeMeshingProperty(Topo::CoEdgeMeshingProperty &emp, std::string ed)

Change la propriété de discrétisation pour une arête. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setFaceMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setFaceMeshingProperty(Topo::CoFaceMeshingProperty &emp, std::string cf)

Change la propriété de discrétisation pour une face commune. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setGeomAssociation "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setGeomAssociation(std::vector< std::string > &topo_entities_names, std::string geom_entity_name, bool move_vertices)


Affecte la projection des entités topologiques vers une entité géométrique
Si le nom de l'entité géométrique est vide, on supprime les associations existantes. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setMeshingProperty(Topo::CoEdgeMeshingProperty &emp, std::vector< std::string > &edge_names)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setMeshingProperty(Topo::CoFaceMeshingProperty &emp, std::vector< std::string > &face_names)

Change la propriété de discrétisation pour une liste d'arête ou faces communes. 
La propriété emp est clonée. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setMeshingProperty(Topo::BlockMeshingProperty &emp, std::vector< std::string > &bloc_names)

Change la propriété de discrétisation pour une liste de blocs. 
La propriété emp est clonée. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setNbMeshingEdges "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setNbMeshingEdges(std::string edge, int nb, std::vector< std::string > &frozed_edges_names)

Change la discrétisation pour une arête commune (en nombre de bras seulement) et propage le changement aux arêtes parallèles pour maintenir la structuration suivant les faces communes (si structuré) 
Un ensemble d'arêtes peuvent être figées pour empécher le changement 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setParallelMeshingProperty "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setParallelMeshingProperty(CoEdgeMeshingProperty &emp, std::string coedge_name)

Change la propriété de discrétisation pour une arête et ses arêtes parallèles. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::setVertexLocation "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::setVertexLocation(std::vector< std::string > &vertices_names, const bool changeX, const double &xPos, const bool changeY, const double &yPos, const bool changeZ, const double &zPos)


Opération de déplacement de sommets
Le déplacement se fait pour un ensemble de points, Il est spécifié les coordonnées modifiées et leur nouvelle valeur

vertices_names : les noms des sommets modifiés 
changeX : vrai si la coordonnée en X est à modifier 
xPos : la nouvelle valeur pour la coordonnées en X 
changeY : vrai si la coordonnée en Y est à modifier 
yPos : la nouvelle valeur pour la coordonnées en Y 
changeZ : vrai si la coordonnée en Z est à modifier 
zPos : la nouvelle valeur pour la coordonnées en Z 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::snapAllProjectedVertices "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::snapAllProjectedVertices()


Déplace tous les sommets en fonction de leur projection 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::snapProjectedVertices "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::snapProjectedVertices(std::vector< std::string > &vertices_names)


Déplace les sommets sélectionnés en fonction de leur projection

vertices_names : la liste des noms des sommets concernés 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::snapVertices "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::snapVertices(std::string nom1, std::string nom2, bool project_on_first)

Fusionne un sommet avec un autre, pour un couple de sommets ou pour les sommets de 2 faces, ou pour 2 arêtes. 
On sélectionne une première face maitresse et une seconde qui va aller se coller sur la première. On peut aussi ne sélectionner que 2 sommets. Si désiré, on place chacun des sommets au milieu des couples.
Des entités sont ammenées à disparaitre.
Les entités sélectionnées doivent être sur un même bloc topologique.

nom1 : une première entité du type sommet, arête ou face commune 
nom2 : une deuxième entité du même type que la précédente 
project_on_first : si l'on projette sur la première entité, sinon on place au milieu des sommets 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitAllFaces "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitAllFaces(std::string narete, const double &ratio_dec=0.5, const double &ratio_ogrid=0.5)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitAllFaces(std::string narete, const Point &pt, const double &ratio_ogrid=0.5)

Découpe toutes les faces structurées 2D en deux. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitBlock "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitBlock(std::string nbloc, std::string narete, const double &ratio)

Découpe un bloc en deux. 
On utilise une arête et un ratio pour positionner un premier sommet qui va être inséré sur l'arête. Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio. Une face basée sur ces 4 arêtes est créée, elle coupe le bloc en 2. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitBlocks "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitBlocks(std::vector< std::string > &blocs_names, std::string narete, const double &ratio)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitBlocks(std::vector< std::string > &blocs_names, std::string narete, const Point &pt)

Découpe un ensembles de blocs en deux. 
On utilise une arête et un ratio pour positionner un premier sommet qui va être inséré sur l'arête. Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio. Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2. Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock. 
On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet qui va être inséré sur l'arête. Les 3 autres arêtes parallèles sont aussi découpées avec le même ratio. Une face basée sur ces 4 arêtes est créée, elle coupe un premier bloc en 2. Ensuite les autres blocs sont découpés comme cela serait le cas avec extendSplitBlock. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitBlocksWithOgrid "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitBlocksWithOgrid(std::vector< std::string > &blocs_names, std::vector< std::string > &cofaces_names, const double &ratio, int nb_bras)

Découpe un ensemble de blocs suivant un o-grid. 

blocs_names : la liste des noms de blocs à découper 
cofaces_names : la liste des faces communes à considérer comme étant entre 2 blocs (ce qui permet de ne pas construire le bloc contre ces faces communes) 
ratio_ogrid : ratio pour positionner les nouveaux sommets, entre les sommets du bord de la sélection et le centre des blocs ou faces concernées 
nb_bras : le nombre de couches de mailles autour du centre du o-grid à créer (c.a.d. le nombre de bras pour les arêtes entre les sommets du bord de la sélection et ceux créés) 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitEdge "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitEdge(std::string coedge_name, const double &ratio_dec=0.5)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitEdge(std::string coedge_name, const Point &pt)

Découpe d'une arête commune en 2. 
On utilise un ratio pour positionner le sommet 
On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet qui va être inséré sur l'arête. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitFace "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitFace(std::string coface_name, std::string narete, const double &ratio_dec, bool project_on_meshing_edges=true)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitFace(std::string coface_name, std::string narete, const Point &pt, bool project_on_meshing_edges=true)

Découpage d'une face structurée 2D ou 3D en deux suivant un ratio. 
On utilise une arête et un ratio pour positionner un premier sommet qui va être inséré sur l'arête. L'autre arête parallèle est aussi découpée avec le même ratio. 
On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet qui va être inséré sur l'arête. L'autre arête parallèle est aussi découpée avec le même ratio induit de la première coupe. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::splitFaces "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitFaces(std::vector< std::string > &cofaces_names, std::string narete, const double &ratio_dec, const double &ratio_ogrid)
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::splitFaces(std::vector< std::string > &cofaces_names, std::string narete, const Point &pt, const double &ratio_ogrid)

Découpe des faces structurées 2D en deux. 
On utilise une arête et un ratio pour positionner un premier sommet qui va être inséré sur l'arête. Les autres arêtes parallèles sont aussi découpées avec le même ratio. Si ratio_ogrid vaut 0, alors le tracé de l'ogrid peut passer par l'axe si une face n'a que 3 côté, sinon on découpe la face en 3 (ogrid en 2D) avec un sommet placé avec ce ratio entre le sommet dans le prolongement sur l'axe et le sommet à l'entrée dans la face. 
On utilise une arête et un point que l'on projette sur cette arête pour positionner un premier sommet qui va être inséré sur l'arête. Les autres arêtes parallèles sont aussi découpées avec le même ratio induit par le premier sommet. Si ratio_ogrid vaut 0, alors le tracé de l'ogrid peut passer par l'axe si une face n'a que 3 côté, sinon on découpe la face en 3 (ogrid en 2D) avec un sommet placé avec ce ratio entre le sommet dans le prolongement sur l'axe et le sommet à l'entrée dans la face. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::TopoManagerIfc "
Mgx3D::Topo::TopoManagerIfc::TopoManagerIfc(const std::string &name, Internal::ContextIfc *c)

Constructeur. 

c : le contexte ce qui permet d'accéder entre autre au CommandManager 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::translate "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::translate(std::vector< std::string > &ve, const Vector &dp, const bool withGeom)


Opération de translation de la topologie (et de la géométrie suivant option)
La translation des objets topologiques de haut niveau (bloc par exemple) entraine celle des objets de niveau inférieurs (face, arêtes et sommets) et réciproquement.

dp : le vecteur de translation 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::unrefine "
virtual Mgx3D::Internal::M3DCommandResultIfc* Mgx3D::Topo::TopoManagerIfc::unrefine(std::string nbloc, std::string narete, int ratio)

Déraffine un bloc structuré suivant une direction. 
On utilise une arête pour déterminer la direction 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::~TopoManagerIfc "
virtual Mgx3D::Topo::TopoManagerIfc::~TopoManagerIfc()

Destructeur. RAS. 

";
%feature("docstring") Mgx3D::Topo::TopoManagerIfc::TopoManagerIfc "
Mgx3D::Topo::TopoManagerIfc::TopoManagerIfc(const TopoManagerIfc &)


Constructeur de copie : interdit. 

";
%feature("docstring") Mgx3D::Utils::Unit " 

";

%feature("docstring") Mgx3D::Utils::Unit::computeFactor "
static double Mgx3D::Utils::Unit::computeFactor(const lengthUnit &luFrom, const lengthUnit &luTo)

calcul le ratio pour exprimer des luFrom en luTo (10 pour passer de cm à mm) 

";
%feature("docstring") Mgx3D::Utils::Unit::computeMeterFactor "
static double Mgx3D::Utils::Unit::computeMeterFactor(const lengthUnit &lu)

calcul le ratio pour exprimer des \"lu\" en mètres (1e-2 pour des cm) 

";
%feature("docstring") Mgx3D::Utils::Unit::getScriptCommand "
static TkUtil::UTF8String Mgx3D::Utils::Unit::getScriptCommand(const lengthUnit &lu)

retourne l'UTF8String pour génération d'une ligne de commande python 

";
%feature("docstring") Mgx3D::Utils::Unit::toLengthUnit "
static lengthUnit Mgx3D::Utils::Unit::toLengthUnit(const std::string &name)

convertion d'une string en une unité de longueur 

";
%feature("docstring") Mgx3D::Utils::Unit::toMeterExposantFactor "
static int Mgx3D::Utils::Unit::toMeterExposantFactor(const lengthUnit &lu)

retourne l'exposant en base 10 de \"lu\" par rapport au mètre (-2 pour les cm) 

";
%feature("docstring") Mgx3D::Utils::Unit::toShortString "
static std::string Mgx3D::Utils::Unit::toShortString(const lengthUnit &lu)

conversion en une string (\"cm\" ...) 

";
%feature("docstring") Mgx3D::Utils::Unit::toString "
static std::string Mgx3D::Utils::Unit::toString(const lengthUnit &lu)

conversion en une string (\"mètre\" ...) 

";
%feature("docstring") Mgx3D::Utils::Math::Vector " 

";

%feature("docstring") Mgx3D::Utils::Math::Vector::abs "
virtual double Mgx3D::Utils::Math::Vector::abs() const 



";
%feature("docstring") Mgx3D::Utils::Math::Vector::abs2 "
virtual double Mgx3D::Utils::Math::Vector::abs2() const 



";
%feature("docstring") Mgx3D::Utils::Math::Vector::dot "
virtual double Mgx3D::Utils::Math::Vector::dot(const Vector &v) const 



";
%feature("docstring") Mgx3D::Utils::Math::Vector::Vector "
Mgx3D::Utils::Math::Vector::Vector(double x0, double y0, double z0)



";
%feature("docstring") Mgx3D::Utils::Math::Vector::Vector "
Mgx3D::Utils::Math::Vector::Vector(const Vector &p)



";
%feature("docstring") Mgx3D::Utils::Math::Vector::Vector "
Mgx3D::Utils::Math::Vector::Vector(const Point &p)



";
%feature("docstring") Mgx3D::Utils::Math::Vector::Vector "
Mgx3D::Utils::Math::Vector::Vector()



";
%feature("docstring") Mgx3D::Utils::Math::Vector::Vector "
Mgx3D::Utils::Math::Vector::Vector(const Point &p1, const Point &p2)



";
%feature("docstring") Mgx3D::Mesh::VolumicSmoothing " 

Objet qui va modifier un maillage suivant un lissage volumique 
";

%feature("docstring") Mgx3D::Mesh::VolumicSmoothing::getMethod "
eVolumicMethod Mgx3D::Mesh::VolumicSmoothing::getMethod() const 

accesseur sur la méthode de lissage 

";
%feature("docstring") Mgx3D::Mesh::VolumicSmoothing::getNbIterations "
int Mgx3D::Mesh::VolumicSmoothing::getNbIterations() const 

accesseur sur le nombre d'itérations 

";
%feature("docstring") Mgx3D::Mesh::VolumicSmoothing::setMethod "
void Mgx3D::Mesh::VolumicSmoothing::setMethod(eVolumicMethod methodeLissage)

change la méthode de lissage 

";
%feature("docstring") Mgx3D::Mesh::VolumicSmoothing::setNbIterations "
void Mgx3D::Mesh::VolumicSmoothing::setNbIterations(int nbIterations)

change le nombre d'itérations 

";
%feature("docstring") Mgx3D::Mesh::VolumicSmoothing::VolumicSmoothing "
Mgx3D::Mesh::VolumicSmoothing::VolumicSmoothing()

Constructeur pour fonction de lissage volumique avec les arguments par défaut. 

";
%feature("docstring") Mgx3D::Mesh::VolumicSmoothing::VolumicSmoothing "
Mgx3D::Mesh::VolumicSmoothing::VolumicSmoothing(int nbIterations, eVolumicMethod methodeLissage)

Constructeur avec paramètres différents du défaut. 

";
%feature("docstring") gmds " 

";

%feature("docstring") Mgx3D " 
Espace de nom du projet Magix 3D. 
Toutes les classes et types utilisés dans Magix3D sont dans ce namespace. 
";

%feature("docstring") Mgx3D::getContextIfc "
Mgx3D::Internal::ContextIfc* Mgx3D::getContextIfc(const char *name)


Pour les pythoneries. Le contexte dont le nom est transmis en argument. 

";
%feature("docstring") Mgx3D::getFirstContextIfc "
Mgx3D::Internal::ContextIfc* Mgx3D::getFirstContextIfc()


Pour les pythoneries. Le premier contexte trouvé 

";
%feature("docstring") Mgx3D::Geom " 

";

%feature("docstring") Mgx3D::Group " 

";

%feature("docstring") Mgx3D::Internal " 
Espace de nom des classes communes au projet de haut niveau. 
";

%feature("docstring") Mgx3D::Mesh " 

";

%feature("docstring") Mgx3D::Topo " 

";

%feature("docstring") Mgx3D::Utils " 
Espace de nom des utilitaires communs au projet. 
";

%feature("docstring") Mgx3D::Utils::Math " 
Espace de nom des utilitaires spécifiquement pour ce qui est mathématique. 
";

