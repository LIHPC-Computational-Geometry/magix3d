import pyMagix3D as Mgx3D

def test_new_topo_entity():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 1, 0),"")
    # Création de l'arête Ar0000
    tm.newTopoEntity(["Som0000", "Som0001"], 1, "AAA")
    assert tm.getNbEdges()==1
    assert "AAA" in tm.getInfos("Ar0000", 1).groups()
    try:
        tm.newTopoEntity(["Som0000", "Som0001"], 1, "BBB", False)
    except Exception as e:
        assert str(e) == "CommandNewTopo une arete (Ar0000) existe déjà entre Som0000 et Som0001"
    tm.newTopoEntity(["Som0000", "Som0001"], 1, "BBB", True)
    assert tm.getNbEdges()==2
    assert "BBB" in tm.getInfos("Ar0001", 1).groups()

def test_new_edge():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 1, 0),"")
    # Création de l'arête Ar0000
    tm.newTopoEntity(["Som0000", "Som0001"], 1, "AAA")
    # Création de la edge Edge0000
    tm.newEdge("Som0000", "Som0001", ["Ar0000"])
    assert "Edge0000" in tm.getInfos("Ar0000", 1).edges()

def test_new_coface():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 1, 0),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 1),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 1, 1),"")
    # Création de l'arête Ar0000
    tm.newTopoEntity(["Som0000", "Som0001"], 1, "")
    tm.newEdge("Som0000", "Som0001", ["Ar0000"])
    # Création de l'arête Ar0001
    tm.newTopoEntity(["Som0001", "Som0003"], 1, "")
    tm.newEdge("Som0001", "Som0003", ["Ar0001"])
    # Création de l'arête Ar0002
    tm.newTopoEntity(["Som0003", "Som0002"], 1, "")
    tm.newEdge("Som0003", "Som0002", ["Ar0002"])
    # Création de l'arête Ar0003
    tm.newTopoEntity(["Som0002", "Som0000"], 1, "")
    tm.newEdge("Som0002", "Som0000", ["Ar0003"])
    # Création de la face Fa0000
    tm.newCoFace(["Edge0000", "Edge0001", "Edge0002", "Edge0003"], ["Som0000", "Som0001", "Som0003", "Som0002"], True, False)
    assert tm.getNbFaces()==1

def test_new_coface_with_hole():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()
    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création du cylindre Vol0001
    gm.newCylinder (Mgx3D.Point(.5, .5, 0), .2, Mgx3D.Vector(0, 0, 1), 3.600000e+02, "C")
    # Différences entre géométries avec plusieurs entités à couper
    gm.cut (["Vol0000"], ["Vol0001"])
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0.7, 0.5, 0),"") #Som0009
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0.7, 0.5, 1),"") #Som0007
    # Création de l'arête Ar0000
    tm.newTopoEntity(["Som0001", "Som0001"], 1, "", True) #Ar0009
    tm.setGeomAssociation(["Ar0000"], "Crb0012", True)
    tm.newEdge("Som0001", "Som0001", ["Ar0000"]) #Edge0026
    # Création de l'arête Ar0001
    tm.newTopoEntity(["Som0001", "Som0000"], 1, "", True) #Ar0014
    tm.setGeomAssociation(["Ar0001"], "Crb0013", True)
    tm.newEdge("Som0000", "Som0001", ["Ar0001"]) #Edge0027
    # Création de l'arête Ar0002
    tm.newTopoEntity(["Som0000", "Som0000"], 1, "", True) #Ar0013
    tm.setGeomAssociation(["Ar0002"], "Crb0014", True)
    tm.newEdge("Som0000", "Som0000", ["Ar0002"]) #Edge0028
    # Création de l'arête Ar0003
    tm.newEdge("Som0001", "Som0000", ["Ar0001"]) #Ar0014
    # Création de la face Fa0000
    tm.newCoFace(["Edge0000", "Edge0001", "Edge0002", "Edge0003"], ["Som0001", "Som0001", "Som0000", "Som0000"], False, True)
    assert tm.getNbFaces()==1
    tm.setGeomAssociation(["Fa0000"], "Surf0006", False)

def test_new_face():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 1, 0),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 1),"")
    # Création d'un sommet topologique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 1, 1),"")
    # Création de l'arête Ar0000
    tm.newTopoEntity(["Som0000", "Som0001"], 1, "")
    tm.newEdge("Som0000", "Som0001", ["Ar0000"])
    # Création de l'arête Ar0001
    tm.newTopoEntity(["Som0001", "Som0003"], 1, "")
    tm.newEdge("Som0001", "Som0003", ["Ar0001"])
    # Création de l'arête Ar0002
    tm.newTopoEntity(["Som0003", "Som0002"], 1, "")
    tm.newEdge("Som0003", "Som0002", ["Ar0002"])
    # Création de l'arête Ar0003
    tm.newTopoEntity(["Som0002", "Som0000"], 1, "")
    tm.newEdge("Som0002", "Som0000", ["Ar0003"])
    # Création de la face Fa0000
    tm.newCoFace(["Edge0000", "Edge0001", "Edge0002", "Edge0003"], ["Som0000", "Som0001", "Som0003", "Som0002"], True, False)
    tm.newFace(["Fa0000"], ["Som0000", "Som0001", "Som0002", "Som0003"], True)
    assert tm.getNbFaces()==1
    assert "Face0000" in tm.getInfos("Fa0000", 2).faces()

def test_new_block():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création du sommet Som0000
    tm.newTopoVertex(Mgx3D.Point(0.0, 0.0, 0.0), "")
    # Création du sommet Som0001
    tm.newTopoVertex(Mgx3D.Point(1.0, 0.0, 0.0), "")
    # Création du sommet Som0002
    tm.newTopoVertex(Mgx3D.Point(0.0, 1.0, 0.0), "")
    # Création du sommet Som0003
    tm.newTopoVertex(Mgx3D.Point(1.0, 1.0, 0.0), "")
    # Création du sommet Som0004
    tm.newTopoVertex(Mgx3D.Point(0.0, 0.0, 1.0), "")
    # Création du sommet Som0005
    tm.newTopoVertex(Mgx3D.Point(1.0, 0.0, 1.0), "")
    # Création du sommet Som0006
    tm.newTopoVertex(Mgx3D.Point(0.0, 1.0, 1.0), "")
    # Création du sommet Som0007
    tm.newTopoVertex(Mgx3D.Point(1.0, 1.0, 1.0), "")

    # Création de l'arête Ar0000
    tm.newTopoEntity(["Som0000", "Som0001"], 1, "", True)
    # Création de l'arête Ar0001
    tm.newTopoEntity(["Som0000", "Som0002"], 1, "", True)
    # Création de l'arête Ar0002
    tm.newTopoEntity(["Som0002", "Som0003"], 1, "", True)
    # Création de l'arête Ar0003
    tm.newTopoEntity(["Som0001", "Som0003"], 1, "", True)
    # Création de l'arête Ar0004
    tm.newTopoEntity(["Som0004", "Som0005"], 1, "", True)
    # Création de l'arête Ar0005
    tm.newTopoEntity(["Som0004", "Som0006"], 1, "", True)
    # Création de l'arête Ar0006
    tm.newTopoEntity(["Som0006", "Som0007"], 1, "", True)
    # Création de l'arête Ar0007
    tm.newTopoEntity(["Som0005", "Som0007"], 1, "", True)
    # Création de l'arête Ar0008
    tm.newTopoEntity(["Som0001", "Som0005"], 1, "", True)
    # Création de l'arête Ar0009
    tm.newTopoEntity(["Som0000", "Som0004"], 1, "", True)
    # Création de l'arête Ar0010
    tm.newTopoEntity(["Som0003", "Som0007"], 1, "", True)
    # Création de l'arête Ar0011
    tm.newTopoEntity(["Som0002", "Som0006"], 1, "", True)

    # Création de la coface Fa0000
    coface_edges = []
    tm.newEdge('Som0000', 'Som0001', ["Ar0000"])
    coface_edges.append('Edge0000')
    tm.newEdge('Som0000', 'Som0002', ["Ar0001"])
    coface_edges.append('Edge0001')
    tm.newEdge('Som0002', 'Som0003', ["Ar0002"])
    coface_edges.append('Edge0002')
    tm.newEdge('Som0001', 'Som0003', ["Ar0003"])
    coface_edges.append('Edge0003')
    tm.newCoFace(coface_edges, ["Som0001", "Som0000", "Som0002", "Som0003"],True, False)
    # Création de la coface Fa0001
    coface_edges = []
    tm.newEdge('Som0004', 'Som0005', ["Ar0004"])
    coface_edges.append('Edge0004')
    tm.newEdge('Som0004', 'Som0006', ["Ar0005"])
    coface_edges.append('Edge0005')
    tm.newEdge('Som0006', 'Som0007', ["Ar0006"])
    coface_edges.append('Edge0006')
    tm.newEdge('Som0005', 'Som0007', ["Ar0007"])
    coface_edges.append('Edge0007')
    tm.newCoFace(coface_edges, ["Som0005", "Som0004", "Som0006", "Som0007"],True, False)
    # Création de la coface Fa0002
    coface_edges = []
    tm.newEdge('Som0000', 'Som0001', ["Ar0000"])
    coface_edges.append('Edge0008')
    tm.newEdge('Som0001', 'Som0005', ["Ar0008"])
    coface_edges.append('Edge0009')
    tm.newEdge('Som0004', 'Som0005', ["Ar0004"])
    coface_edges.append('Edge0010')
    tm.newEdge('Som0000', 'Som0004', ["Ar0009"])
    coface_edges.append('Edge0011')
    tm.newCoFace(coface_edges, ["Som0000", "Som0001", "Som0005", "Som0004"],True, False)
    # Création de la coface Fa0003
    coface_edges = []
    tm.newEdge('Som0002', 'Som0003', ["Ar0002"])
    coface_edges.append('Edge0012')
    tm.newEdge('Som0003', 'Som0007', ["Ar0010"])
    coface_edges.append('Edge0013')
    tm.newEdge('Som0006', 'Som0007', ["Ar0006"])
    coface_edges.append('Edge0014')
    tm.newEdge('Som0002', 'Som0006', ["Ar0011"])
    coface_edges.append('Edge0015')
    tm.newCoFace(coface_edges, ["Som0002", "Som0003", "Som0007", "Som0006"],True, False)
    # Création de la coface Fa0004
    coface_edges = []
    tm.newEdge('Som0001', 'Som0003', ["Ar0003"])
    coface_edges.append('Edge0016')
    tm.newEdge('Som0001', 'Som0005', ["Ar0008"])
    coface_edges.append('Edge0017')
    tm.newEdge('Som0005', 'Som0007', ["Ar0007"])
    coface_edges.append('Edge0018')
    tm.newEdge('Som0003', 'Som0007', ["Ar0010"])
    coface_edges.append('Edge0019')
    tm.newCoFace(coface_edges, ["Som0003", "Som0001", "Som0005", "Som0007"],True, False)
    # Création de la coface Fa0005
    coface_edges = []
    tm.newEdge('Som0000', 'Som0002', ["Ar0001"])
    coface_edges.append('Edge0020')
    tm.newEdge('Som0000', 'Som0004', ["Ar0009"])
    coface_edges.append('Edge0021')
    tm.newEdge('Som0004', 'Som0006', ["Ar0005"])
    coface_edges.append('Edge0022')
    tm.newEdge('Som0002', 'Som0006', ["Ar0011"])
    coface_edges.append('Edge0023')
    tm.newCoFace(coface_edges, ["Som0002", "Som0000", "Som0004", "Som0006"],True, False)

    # Création de la face Face0000
    tm.newFace(["Fa0000"], ["Som0001", "Som0000", "Som0002", "Som0003"], True)
    # Création de la face Face0001
    tm.newFace(["Fa0001"], ["Som0005", "Som0004", "Som0006", "Som0007"], True)
    # Création de la face Face0002
    tm.newFace(["Fa0002"], ["Som0000", "Som0001", "Som0005", "Som0004"], True)
    # Création de la face Face0003
    tm.newFace(["Fa0003"], ["Som0002", "Som0003", "Som0007", "Som0006"], True)
    # Création de la face Face0004
    tm.newFace(["Fa0004"], ["Som0003", "Som0001", "Som0005", "Som0007"], True)
    # Création de la face Face0005
    tm.newFace(["Fa0005"], ["Som0002", "Som0000", "Som0004", "Som0006"], True)

    # Création du bloc Bl0000 ayant pour faces ['Face0000', 'Face0001', 'Face0002', 'Face0003', 'Face0004', 'Face0005'] et pour sommets ['Som0001', 'Som0005', 'Som0003', 'Som0007', 'Som0000', 'Som0004', 'Som0002', 'Som0006']
    tm.newBlock(["Face0000", "Face0001", "Face0002", "Face0003", "Face0004", "Face0005"], ["Som0001", "Som0005", "Som0003", "Som0007", "Som0000", "Som0004", "Som0002", "Som0006"], True, "")
    assert tm.getNbBlocks()==1