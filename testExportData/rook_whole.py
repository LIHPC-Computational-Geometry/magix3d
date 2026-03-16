# -*- coding: utf-8 -*-

import sys

import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# Création du sommet Pt0000
ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
# Création du sommet Pt0001
ctx.getGeomManager().newVertex (Mgx3D.Point(.5, 0, 0))
# Création du sommet Pt0002
ctx.getGeomManager().newVertex (Mgx3D.Point(.5, 2, 0))
# Création du sommet Pt0003
ctx.getGeomManager().newVertex (Mgx3D.Point(0, 2, 0))
# Création du segment Crb0000
ctx.getGeomManager().newSegment("Pt0000", "Pt0001")
# Création du segment Crb0001
ctx.getGeomManager().newSegment("Pt0001", "Pt0002")
# Création du segment Crb0002
ctx.getGeomManager().newSegment("Pt0002", "Pt0003")
# Création du segment Crb0003
ctx.getGeomManager().newSegment("Pt0003", "Pt0000")
# Création du sommet Pt0004
ctx.getGeomManager().newVertex (Mgx3D.Point(4, 0, 0))
# Création du sommet Pt0005
ctx.getGeomManager().newVertex (Mgx3D.Point(4, 1, 0))
# Création du segment Crb0004
ctx.getGeomManager().newSegment("Pt0001", "Pt0004")
# Création du segment Crb0005
ctx.getGeomManager().newSegment("Pt0004", "Pt0005")
# Création du segment Crb0006
ctx.getGeomManager().newSegment("Pt0005", "Pt0002")
# Création de la surface Surf0000
ctx.getGeomManager().newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003"] , "")
# Création de la surface Surf0001
ctx.getGeomManager().newPlanarSurface( ["Crb0004", "Crb0005", "Crb0006", "Crb0001"] , "")
# Création du sommet Pt0006
ctx.getGeomManager().newVertex (Mgx3D.Point(4.5, 0, 0))
# Création du sommet Pt0007
ctx.getGeomManager().newVertex (Mgx3D.Point(4.5, 1, 0))
# Création du segment Crb0007
ctx.getGeomManager().newSegment("Pt0004", "Pt0006")
# Création du segment Crb0008
ctx.getGeomManager().newSegment("Pt0006", "Pt0007")
# Création du segment Crb0009
ctx.getGeomManager().newSegment("Pt0007", "Pt0005")
# Création du sommet Pt0008
ctx.getGeomManager().newVertex (Mgx3D.Point(4.5, 1.5, 0))
# Création du sommet Pt0009
ctx.getGeomManager().newVertex (Mgx3D.Point(4, 1.5, 0))
# Création du segment Crb0010
ctx.getGeomManager().newSegment("Pt0007", "Pt0008")
# Création du segment Crb0011
ctx.getGeomManager().newSegment("Pt0008", "Pt0009")
# Création du segment Crb0012
ctx.getGeomManager().newSegment("Pt0009", "Pt0005")
# Création de la surface Surf0002
ctx.getGeomManager().newPlanarSurface( ["Crb0007", "Crb0008", "Crb0009", "Crb0005"] , "")
# Création de la surface Surf0003
ctx.getGeomManager().newPlanarSurface( ["Crb0009", "Crb0010", "Crb0011", "Crb0012"] , "")
# Création du sommet Pt0010
ctx.getGeomManager().newVertex (Mgx3D.Point(5.5, 1.5, 0))
# Création du sommet Pt0011
ctx.getGeomManager().newVertex (Mgx3D.Point(5.5, 2, 0))
# Création du sommet Pt0012
ctx.getGeomManager().newVertex (Mgx3D.Point(4, 2, 0))
# Création du segment Crb0013
ctx.getGeomManager().newSegment("Pt0008", "Pt0010")
# Création du segment Crb0014
ctx.getGeomManager().newSegment("Pt0010", "Pt0011")
# Création du segment Crb0015
ctx.getGeomManager().newSegment("Pt0011", "Pt0012")
# Création du segment Crb0016
ctx.getGeomManager().newSegment("Pt0012", "Pt0009")
# Création de la surface Surf0004
ctx.getGeomManager().newPlanarSurface( ["Crb0011", "Crb0013", "Crb0014", "Crb0015", "Crb0016"] , "")
# Création d'une face topologique structurée sur une géométrie (Surf0000)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0000")
# Création d'une face topologique structurée sur une géométrie (Surf0001)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0001")
# Création d'une face topologique structurée sur une géométrie (Surf0002)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0002")
# Création d'une face topologique structurée sur une géométrie (Surf0003)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0003")
# Création d'un bloc unitaire mis dans le groupe border
ctx.getTopoManager().newFreeTopoInGroup ("border", 2)
# Affectation d'une projection vers Pt0011 pour les entités topologiques Som0013
ctx.getTopoManager().setGeomAssociation (["Som0013"], "Pt0011", True)
# Affectation d'une projection vers Pt0010 pour les entités topologiques Som0010
ctx.getTopoManager().setGeomAssociation (["Som0010"], "Pt0010", True)
# Affectation d'une projection vers Pt0012 pour les entités topologiques Som0012
ctx.getTopoManager().setGeomAssociation (["Som0012"], "Pt0012", True)
# Affectation d'une projection vers Pt0009 pour les entités topologiques Som0011
ctx.getTopoManager().setGeomAssociation (["Som0011"], "Pt0009", True)
# Découpage de la face Fa0004
ctx.getTopoManager().splitFace ("Fa0004", "Ar0013", .7, True)
# Affectation d'une projection vers Pt0008 pour les entités topologiques Som0014
ctx.getTopoManager().setGeomAssociation (["Som0014"], "Pt0008", True)
# Affectation d'une projection vers Crb0014 pour les entités topologiques Ar0016
ctx.getTopoManager().setGeomAssociation (["Ar0016"], "Crb0014", True)
# Affectation d'une projection vers Crb0015 pour les entités topologiques Som0015
ctx.getTopoManager().setGeomAssociation (["Som0015"], "Crb0015", True)
# Affectation d'une projection vers Crb0015 pour les entités topologiques Ar0020 Ar0019
ctx.getTopoManager().setGeomAssociation (["Ar0020", "Ar0019"], "Crb0015", True)
# Affectation d'une projection vers Crb0016 pour les entités topologiques Ar0014
ctx.getTopoManager().setGeomAssociation (["Ar0014"], "Crb0016", True)
# Affectation d'une projection vers Crb0011 pour les entités topologiques Ar0018
ctx.getTopoManager().setGeomAssociation (["Ar0018"], "Crb0011", True)
# Modification de la position des sommets topologiques Som0015 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0015"], True, 4.5, True, 2, True, 0)
# Affectation d'une projection vers Surf0004 pour les entités topologiques Ar0021 Fa0006 Fa0005
ctx.getTopoManager().setGeomAssociation (["Ar0021", "Fa0006", "Fa0005"], "Surf0004", True)
# Changement de discrétisation pour Ar0019
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0019")
# Changement de discrétisation pour Ar0020
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0020")
# Fusion entre arêtes Ar0011 et Ar0018
ctx.getTopoManager().fuse2Edges ("Ar0011","Ar0018")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0008", .5, .5)
# Construction Topo et Geom 3D avec o-grid par révolution
ctx.getTopoManager().makeBlocksByRevol (["Ar0032"], Mgx3D.Portion.QUART)
# Modifie le groupe top
ctx.getGeomManager().addToGroup (["Vol0004"], 3, "top")
# Création du cylindre Vol0005
ctx.getGeomManager().newCylinder (Mgx3D.Point(5, 0, 0), 3, Mgx3D.Vector(1, 0, 0), 4.500000e+01)
# Rotation de Vol0005 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 315] 
ctx.getGeomManager().rotate(["Vol0005"], Mgx3D.RotX(315))
# Différences entre géométries avec plusieurs entités à couper
ctx.getGeomManager().cut(["Vol0004"], ["Vol0005"])
# Découpage de tous les blocs suivant l'arête Ar0017
ctx.getTopoManager().splitAllBlocks ("Ar0017",.5)
# Destruction des entités topologiques Bl0018
ctx.getTopoManager().destroy (["Bl0018"], True)
# Affectation d'une projection vers Pt0041 pour les entités topologiques Som0027
ctx.getTopoManager().setGeomAssociation (["Som0027"], "Pt0041", True)
# Affectation d'une projection vers Pt0044 pour les entités topologiques Som0051
ctx.getTopoManager().setGeomAssociation (["Som0051"], "Pt0044", True)
# Affectation d'une projection vers Pt0043 pour les entités topologiques Som0050
ctx.getTopoManager().setGeomAssociation (["Som0050"], "Pt0043", True)
# Affectation d'une projection vers Crb0070 pour les entités topologiques Ar0114
ctx.getTopoManager().setGeomAssociation (["Ar0114"], "Crb0070", True)
# Affectation d'une projection vers Crb0069 pour les entités topologiques Ar0113
ctx.getTopoManager().setGeomAssociation (["Ar0113"], "Crb0069", True)
# Affectation d'une projection vers Pt0045 pour les entités topologiques Som0053
ctx.getTopoManager().setGeomAssociation (["Som0053"], "Pt0045", True)
# Affectation d'une projection vers Pt0046 pour les entités topologiques Som0052
ctx.getTopoManager().setGeomAssociation (["Som0052"], "Pt0046", True)
# Affectation d'une projection vers Crb0075 pour les entités topologiques Ar0062 Ar0121
ctx.getTopoManager().setGeomAssociation (["Ar0062", "Ar0121"], "Crb0075", True)
# Affectation d'une projection vers Crb0076 pour les entités topologiques Ar0120
ctx.getTopoManager().setGeomAssociation (["Ar0120"], "Crb0076", True)
# Affectation d'une projection vers Crb0073 pour les entités topologiques Ar0124
ctx.getTopoManager().setGeomAssociation (["Ar0124"], "Crb0073", True)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Ar0125
ctx.getTopoManager().setGeomAssociation (["Ar0125"], "Crb0072", True)
# Affectation d'une projection vers Crb0074 pour les entités topologiques Ar0123
ctx.getTopoManager().setGeomAssociation (["Ar0123"], "Crb0074", True)
# Affectation d'une projection vers Surf0035 pour les entités topologiques Fa0076
ctx.getTopoManager().setGeomAssociation (["Fa0076"], "Surf0035", True)
# Affectation d'une projection vers Surf0036 pour les entités topologiques Fa0088
ctx.getTopoManager().setGeomAssociation (["Fa0088"], "Surf0036", True)
# Rotation d'une copie d'une géométrie avec sa topologie
ctx.getGeomManager().copyAndRotate(["Vol0000", "Vol0001", "Vol0002", "Vol0003", "Vol0006"], Mgx3D.RotX(90), True,"")
# Rotation d'une copie d'une géométrie avec sa topologie
ctx.getGeomManager().copyAndRotate(["Vol0000", "Vol0001", "Vol0002", "Vol0003", "Vol0006"], Mgx3D.RotX(180), True,"")
# Rotation d'une copie d'une géométrie avec sa topologie
ctx.getGeomManager().copyAndRotate(["Vol0000", "Vol0001", "Vol0002", "Vol0003", "Vol0006"], Mgx3D.RotX(270), True,"")
# Collage entre géométries avec topologies
ctx.getGeomManager().glue(["Vol0000", "Vol0001", "Vol0002", "Vol0003", "Vol0006", "Vol0007", "Vol0008", "Vol0009", "Vol0010", "Vol0011", "Vol0012", "Vol0013", "Vol0014", "Vol0015", "Vol0016", "Vol0017", "Vol0018", "Vol0019", "Vol0020", "Vol0021"])
