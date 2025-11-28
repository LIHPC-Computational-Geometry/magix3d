import pyMagix3D as Mgx3D
import math

def test_split_ogrid(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession()
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(5, 0, 0), 9.000000e+01)
    #  Création du volume Vol0001
    ctx.getGeomManager().newSphere (Mgx3D.Point(2.5, 0, 0), 1.5, Mgx3D.Portion.DEMI)
    #  Création du volume Vol0002
    ctx.getGeomManager().newSphere (Mgx3D.Point(2.5, 0, 0), 1.25, Mgx3D.Portion.DEMI)
    #  Création du volume Vol0003
    ctx.getGeomManager().newSphere (Mgx3D.Point(2.5, 0, 0), 1, Mgx3D.Portion.DEMI)
    #  Rotation de Vol0000 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 90]
    ctx.getGeomManager ( ).rotate (["Vol0000"], Mgx3D.RotX(90))
    #  Rotation de Vol0000 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 90]
    ctx.getGeomManager ( ).rotate (["Vol0000"], Mgx3D.RotX(90))
    #  Rotation de Vol0003 Vol0002 Vol0001 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 270]
    ctx.getGeomManager ( ).rotate (["Vol0003","Vol0002","Vol0001"], Mgx3D.RotX(270))
    #  Création du volume Vol0004
    ctx.getGeomManager().newSphere (Mgx3D.Point(2.5, 0, 0), .75, Mgx3D.Portion.DEMI)
    #  Rotation de Vol0004 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 270]
    ctx.getGeomManager ( ).rotate (["Vol0004"], Mgx3D.RotX(270))
    #  Destruction de  Vol0001
    ctx.getGeomManager().destroy(["Vol0001"], True)
    # Section par un plan de Vol0002 suivant  [  [ 0, 0, 0] ,  [ 0, 1, 0] ]
    ctx.getGeomManager ( ).sectionByPlane (["Vol0002"], Mgx3D.Plane(Mgx3D.Point(0, 0, 0), Mgx3D.Vector(0, 1, 0)), "")
    # Destruction de  Vol0006
    ctx.getGeomManager().destroy(["Vol0006"], True)
    # Section par un plan de Vol0003 suivant  [  [ 0, 0, 0] ,  [ 0, 1, 0] ]
    ctx.getGeomManager ( ).sectionByPlane (["Vol0003"], Mgx3D.Plane(Mgx3D.Point(0, 0, 0), Mgx3D.Vector(0, 1, 0)), "")
    # Destruction de  Vol0008
    ctx.getGeomManager().destroy(["Vol0008"], True)
    # Section par un plan de Vol0004 suivant  [  [ 0, 0, 0] ,  [ 0, 1, 0] ]
    ctx.getGeomManager ( ).sectionByPlane (["Vol0004"], Mgx3D.Plane(Mgx3D.Point(0, 0, 0), Mgx3D.Vector(0, 1, 0)), "")
    # Destruction de  Vol0010
    ctx.getGeomManager().destroy(["Vol0010"], True)
    # Rotation de Vol0009 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 90]
    ctx.getGeomManager ( ).rotate (["Vol0009"], Mgx3D.RotX(90))
    # Symétrie plane de  Vol0009
    ctx.getGeomManager ( ).mirror (["Vol0009"], Mgx3D.Plane(Mgx3D.Point(0, 0, 0), Mgx3D.Vector(0, 0, 1)))
    # Création d'une face topologique structurée sur une géométrie (Surf0034)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0034")
    # Collage entre géométries avec topologies
    ctx.getGeomManager ( ).glue (["Vol0005","Vol0000","Vol0007","Vol0009"])
    # Fusion Booléenne de  Vol0014 Vol0016 Vol0012
    ctx.getGeomManager ( ).fuse (["Vol0014","Vol0016","Vol0012"])
    # Fusion de surfaces Surf0049 Surf0058 Surf0042 Surf0045
    ctx.getGeomManager ( ).joinSurfaces (["Surf0049","Surf0058","Surf0042","Surf0045"])
    # Fusion de surfaces avec maj de la topologie
    ctx.getGeomManager ( ).joinSurfaces (["Surf0043","Surf0055"])
    # Fusion de surfaces avec maj de la topologie
    ctx.getGeomManager ( ).joinSurfaces (["Surf0044","Surf0050"])
    # Suppression d'entités géométriques et suppression des liens topologiques
    ctx.getGeomManager().destroy(["Vol0017", "Vol0018", "Vol0011", "Vol0015", "Vol0013"], False)
    ctx.getGeomManager().destroy(["Surf0002", "Surf0063", "Surf0054", "Surf0040", "Surf0062", "Surf0059", "Surf0051", "Surf0056", "Surf0033", "Surf0031", "Surf0046", "Surf0039", "Surf0060", "Surf0048", "Surf0053", "Surf0057", "Surf0001","Surf0041"], True)
    # Fusion de courbes avec maj de la topologie
    ctx.getGeomManager ( ).joinCurves (["Crb0059","Crb0068"])
    # Fusion de courbes Crb0030 Crb0035
    ctx.getGeomManager ( ).joinCurves (["Crb0030","Crb0035"])
    # Fusion de courbes Crb0057 Crb0063
    ctx.getGeomManager ( ).joinCurves (["Crb0057","Crb0063"])
    # Affectation d'une projection vers Crb0079 pour les entités topologiques Ar0001
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0001"], "Crb0079", True)
    # Affectation d'une projection vers Crb0041 pour les entités topologiques Ar0000
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0000"], "Crb0041", True)
    # Affectation d'une projection vers Crb0046 pour les entités topologiques Ar0002
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0002"], "Crb0046", True)
    # Création d'une face topologique structurée sur une géométrie (Surf0061)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0061")
    # Création d'une face topologique structurée sur une géométrie (Surf0038)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0038")
    # Création d'une face topologique structurée sur une géométrie (Surf0065)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0065")
    # Création d'un bloc unitaire mis dans le groupe test
    ctx.getTopoManager().newFreeTopoInGroup ("test", 2)
    # Translation d'une topologie avec sa géométrie
    ctx.getTopoManager ( ).translate (["Fa0004"], Mgx3D.Vector(1, 1.5, 0), True)
    # Changement de discrétisation pour les arêtes Ar0001 Ar0013
    emp = Mgx3D.EdgeMeshingPropertyUniform(60)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0001","Ar0013"])
    # Changement de discrétisation pour Ar0009
    emp = Mgx3D.EdgeMeshingPropertyUniform(20)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0009")
    # Changement de discrétisation pour Ar0006
    emp = Mgx3D.EdgeMeshingPropertyUniform(20)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0006")
    # Changement de discrétisation pour Ar0017
    emp = Mgx3D.EdgeMeshingPropertyUniform(60)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0017")
    # Découpage de l'arête Ar0015
    ctx.getTopoManager().splitEdge ("Ar0015", .336)
    # Découpage de l'arête Ar0019
    ctx.getTopoManager().splitEdge ("Ar0019", .5)
    # Modification de la position des sommets topologiques Som0013 en coordonnées cartésiennes
    ctx.getTopoManager ( ).setVertexLocation (["Som0013"], True, 1.5, True, 7.5e-8, True, -1.22465e-16)
    # Modification de la position des sommets topologiques Som0014 en coordonnées cartésiennes
    ctx.getTopoManager ( ).setVertexLocation (["Som0014"], True, 1.25, True, -3.06162e-16, True, -1.53081e-16)
    # Modification de la position des sommets topologiques Som0016 en coordonnées cartésiennes
    ctx.getTopoManager ( ).setVertexLocation (["Som0016"], True, 3.5, True, 7.5e-8, True, -1.66533e-23)
    # Modification de la position des sommets topologiques Som0015 en coordonnées cartésiennes
    ctx.getTopoManager ( ).setVertexLocation (["Som0015"], True, 3.75, True, -3.06162e-16, True, 6.79816e-32)
    # Modification de la position des sommets topologiques Som0018 en coordonnées cartésiennes
    ctx.getTopoManager ( ).setVertexLocation (["Som0018"], True, 3.25, True, 1, True, -2.77556e-16)
    # Modification de la position des sommets topologiques Som0017 en coordonnées cartésiennes
    ctx.getTopoManager ( ).setVertexLocation (["Som0017"], True, 1.75, True, 1, True, -3.2348e-16)
    # Affectation d'une projection vers Crb0081 pour les entités topologiques Ar0020
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0020"], "Crb0081", True)
    # Fusion entre arêtes Ar0013 et Ar0017
    ctx.getTopoManager().fuse2Edges ("Ar0013","Ar0017")
    # Fusion entre arêtes Ar0006 et Ar0018
    ctx.getTopoManager().fuse2Edges ("Ar0006","Ar0018")
    # Fusion entre arêtes Ar0009 et Ar0021
    ctx.getTopoManager().fuse2Edges ("Ar0009","Ar0021")
    # Découpage de toutes les faces structurées
    ctx.getTopoManager().splitAllFaces ("Ar0013", .5, 0, True)
