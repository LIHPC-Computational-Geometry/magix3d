#!/ccc/products/nec/bin/maillage_python
# -*- coding: iso-8859-15 -*-
# validé avec la version 1.6.3 le 11/5/2017
# ne fonctionne pas avec les version (1.6.4 et 1.6.5)
# fonctionne avec la version 1.14.2 le 11/06/2019

import sys
import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# construction de la géométrie
# Création d'un cylindre avec une topologie
ctx.getTopoManager().newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 5.4, Mgx3D.Vector(10, 0, 0), 360, True, .5, 10, 10, 10, "OR")
# Création d'un cylindre
ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 5, Mgx3D.Vector(10, 0, 0), 360, "VIDE")
# Création d'un cylindre
ctx.getGeomManager().newCylinder (Mgx3D.Point(5, 4.5, 0), 1.2, Mgx3D.Vector(0, 3, 0), 360, "OR")
# Section
ctx.getGeomManager().section(["Vol0002"], "Surf0003")
# Destruction
ctx.getGeomManager().destroy(["Vol0004"], True)
# Création d'un cylindre
ctx.getGeomManager().newCylinder (Mgx3D.Point(5, 4.5, 0), 1, Mgx3D.Vector(0, 3, 0), 360, "VIDE")
# Section
ctx.getGeomManager().section(["Vol0005"], "Surf0009")
# Destruction
ctx.getGeomManager().destroy(["Vol0007"], True)
# Fusion booléenne entre géométries avec topologies
ctx.getGeomManager().fuse(["Vol0000", "Vol0003"])
# Collage entre géométries avec topologies
ctx.getGeomManager().glue(["Vol0001", "Vol0006", "Vol0008"])
# Modifie le groupe OR
ctx.getGeomManager().setGroup (["Vol0010"], 3, "OR")
# Modifie le groupe VIDE
ctx.getGeomManager().setGroup (["Vol0009", "Vol0006"], 3, "VIDE")

# construction de la topologie
# Création d'un bloc topologique structuré sans projection
ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0006")
# Découpage des blocs Bl0001 Bl0002 Bl0003 Bl0000, suivant l'arête Ar0017
ctx.getTopoManager().splitBlocks (["Bl0001", "Bl0002", "Bl0003", "Bl0000"], "Ar0017",.8)
# Projection automatique pour les sommets topologiques Som0048 Som0051 Som0052 Som0055 Som0054 ... 
ctx.getTopoManager().projectVerticesOnNearestGeomEntities (["Som0048", "Som0051", "Som0052", "Som0055", "Som0054", "Som0053", "Som0050", "Som0049"], ["Crb0003", "Crb0005"], True)
# Projection automatique pour les arêtes topologiques Ar0083 Ar0099 Ar0097 Ar0090 Ar0082 ... 
ctx.getTopoManager().projectEdgesOnCurves (["Ar0083", "Ar0099", "Ar0097", "Ar0090", "Ar0082", "Ar0098", "Ar0096", "Ar0089"])
# Affectation d'une projection vers Surf0005 pour les entités topologiques Som0018 Som0008 Som0026 Som0027
ctx.getTopoManager().setGeomAssociation (["Som0018", "Som0008", "Som0026", "Som0027", "Ar0077", "Ar0075", "Ar0012", "Ar0005", "Ar0037", "Ar0028", "Ar0086", "Ar0092"], "Surf0005", True)
# Affectation d'une projection vers Surf0004 pour les entités topologiques Som0022 Som0012 Som0030 Som0031 Ar0073 ... 
ctx.getTopoManager().setGeomAssociation (["Som0022", "Som0012", "Som0030", "Som0031", "Ar0073", "Ar0014", "Ar0039", "Ar0007", "Ar0030", "Ar0084", "Ar0079", "Ar0094"], "Surf0004", True)
# Affectation d'une projection vers Vol0009 pour les entités topologiques Bl0008 Bl0006 Bl0004 Bl0013 Bl0011
ctx.getTopoManager().setGeomAssociation (["Bl0008", "Bl0006", "Bl0004", "Bl0013", "Bl0011"], "Vol0009", False)
# Projection automatique pour les arêtes topologiques Ar0076 Ar0095 Ar0088 Ar0081
ctx.getTopoManager().projectEdgesOnCurves (["Ar0076", "Ar0095", "Ar0088", "Ar0081"])
# Projection automatique pour les faces topologiques Fa0001 Fa0008 Fa0012 Fa0021 Fa0028 ... (utiliser Python ...)
ctx.getTopoManager().projectFacesOnSurfaces (["Fa0001", "Fa0008", "Fa0012", "Fa0021", "Fa0028", "Fa0029", "Fa0040", "Fa0042", "Fa0037", "Fa0044", "Fa0045", "Fa0047", "Fa0049", "Fa0051", "Fa0053", "Fa0055", "Fa0057", "Fa0058", "Fa0038", "Fa0060", "Fa0062", "Fa0063"])

# mailler pour valider ...
# Découpage des blocs Bl0007 Bl0006, suivant l'arête Ar0023
ctx.getTopoManager().splitBlocks (["Bl0007", "Bl0006"], "Ar0023",.4)
# Découpage des blocs Bl0014 Bl0016, suivant l'arête Ar0101
ctx.getTopoManager().splitBlocks (["Bl0014", "Bl0016"], "Ar0101",.4)
# Découpage des blocs Bl0019 Bl0021, suivant l'arête Ar0111
ctx.getTopoManager().splitBlocks (["Bl0019", "Bl0021"], "Ar0111",.4)
# Découpage des blocs Bl0022 Bl0024, suivant l'arête Ar0140
ctx.getTopoManager().splitBlocks (["Bl0022", "Bl0024"], "Ar0140",.7)

# Destruction des entités topologiques Bl0027
ctx.getTopoManager().destroy (["Bl0027"], True)
# Découpage en O-grid des blocs structurés Bl0005
ctx.getTopoManager().splitBlocksWithOgrid (["Bl0005"], ["Fa0032", "Fa0033"], .8, 2)

# Affectation d'une projection vers Crb0013 pour les entités topologiques Som0087 Som0083 Som0082 Som0086 Ar0191 ...   
ctx.getTopoManager().setGeomAssociation (["Som0087", "Som0083", "Som0082", "Som0086", "Ar0191", "Ar0195", "Ar0187", "Ar0194"], "Crb0012", True)
# Affectation d'une projection vers Crb0006 pour les entités topologiques Som0047 Som0043 Som0042 Som0046 Ar0071 ... 
ctx.getTopoManager().setGeomAssociation (["Som0047", "Som0043", "Som0042", "Som0046", "Ar0071", "Ar0063", "Ar0070", "Ar0061"], "Crb0006", True)
# Affectation d'une projection vers Crb0019 pour les entités topologiques Som0070 Som0076 Som0045 Som0041 
ctx.getTopoManager().setGeomAssociation (["Som0070", "Som0076", "Som0045", "Som0041", "Som0074", "Som0069", "Som0044", "Som0040"], "Crb0019", True)
# Affectation d'une projection vers Crb0017 pour les entités topologiques Som0077 Som0081 Som0071 Som0085 
ctx.getTopoManager().setGeomAssociation (["Som0077", "Som0081", "Som0071", "Som0085", "Som0080", "Som0075", "Som0068", "Som0084"], "Crb0017", True)

# Changement de discrétisation pour Ar0071 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0071", 4, [])
# Changement de discrétisation pour Ar0063 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0063", 2, [])
# Fusion entre faces Fa0120 et Fa0136
ctx.getTopoManager().fuse2Faces ("Fa0120","Fa0136")
# Fusion entre faces Fa0129 et Fa0112
ctx.getTopoManager().fuse2Faces ("Fa0112","Fa0129")
# Fusion entre faces Fa0137 et Fa0104
ctx.getTopoManager().fuse2Faces ("Fa0104","Fa0137")
# Fusion entre faces Fa0118 et Fa0133
ctx.getTopoManager().fuse2Faces ("Fa0118","Fa0133")
# Fusion entre faces Fa0116 et Fa0142
ctx.getTopoManager().fuse2Faces ("Fa0116","Fa0142")
# Affectation d'une projection vers Vol0006 pour les entités topologiques Bl0034
ctx.getTopoManager().setGeomAssociation (["Bl0034"], "Vol0006", False)
# Affectation d'une projection vers Vol0010 pour les entités topologiques Bl0031 Bl0032 Bl0030 Bl0033
ctx.getTopoManager().setGeomAssociation (["Bl0031", "Bl0032", "Bl0030", "Bl0033"], "Vol0010", False)
# Affectation d'une projection vers Surf0019 pour les entités topologiques Som0056 Som0062 Som0057 Som0063 Ar0120 ... 
ctx.getTopoManager().setGeomAssociation (["Som0056", "Som0062", "Som0057", "Som0063", "Ar0120", "Ar0119", "Ar0100", "Ar0033", "Ar0044", "Ar0122", "Ar0121", "Ar0102"], "Surf0019", True)
# Changement de discrétisation pour Ar0138 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0138", 16, [])
# Changement de discrétisation pour Ar0160 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0160", 16, [])
# Changement de discrétisation pour Ar0158 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0158", 10, [])
# Changement de discrétisation pour Ar0063 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0063", 10, [])
# Changement de discrétisation pour Ar0120 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0120", 15, [])
# Changement de discrétisation pour Ar0100 avec propagation
ctx.getTopoManager().setNbMeshingEdges ("Ar0100", 15, [])
# Découpage de la face Fa0035
ctx.getTopoManager().splitFace ("Fa0035", "Ar0063", .5, True)
# Découpage de la face Fa0145
ctx.getTopoManager().splitFace ("Fa0145", "Ar0195", .5, True)
# Affectation d'une projection vers Crb0019 pour les entités topologiques Ar0164 Ar0163 Ar0199 Ar0198 Ar0158 
ctx.getTopoManager().setGeomAssociation (["Ar0164", "Ar0163", "Ar0199", "Ar0198", "Ar0158"], "Crb0019", True)
# Affectation d'une projection vers Crb0017 pour les entités topologiques Ar0167 Ar0165 Ar0203 Ar0204 Ar0159 
ctx.getTopoManager().setGeomAssociation (["Ar0167", "Ar0165", "Ar0203", "Ar0204", "Ar0159"], "Crb0017", True)
# Affectation d'une projection vers Crb0018 pour les entités topologiques Ar0200 
ctx.getTopoManager().setGeomAssociation (["Ar0200"], "Crb0018", True)
# Affectation d'une projection vers Crb0015 pour les entités topologiques Ar0205 
ctx.getTopoManager().setGeomAssociation (["Ar0205"], "Crb0015", True)

# Affectation d'une projection vers Surf0015 pour les entités topologiques Fa0141 Fa0149 Fa0148 Fa0140 Fa0144
ctx.getTopoManager().setGeomAssociation (["Fa0141", "Fa0149", "Fa0148", "Fa0140", "Fa0144"], "Surf0016", True)
# Affectation d'une projection vers Surf0018 pour les entités topologiques Fa0031 Fa0147 Fa0146 Fa0030 Fa0034
ctx.getTopoManager().setGeomAssociation (["Fa0031", "Fa0147", "Fa0146", "Fa0030", "Fa0034"], "Surf0018", True)
# Affectation d'une projection vers Surf0021 pour les entités topologiques Fa0135 Fa0138 Fa0131 Fa0139
ctx.getTopoManager().setGeomAssociation (["Fa0135", "Fa0138", "Fa0131", "Fa0139"], "Surf0021", True)
# Affectation d'une projection vers Surf0013 pour les entités topologiques Fa0143
ctx.getTopoManager().setGeomAssociation (["Fa0143"], "Surf0013", True)
# Affectation d'une projection vers Surf0017 pour les entités topologiques Fa0116
ctx.getTopoManager().setGeomAssociation (["Fa0116"], "Surf0015", True)
# Découpage en O-grid des blocs structurés Bl0034 Bl0029
ctx.getTopoManager().splitBlocksWithOgrid (["Bl0034", "Bl0029"], ["Fa0143"], .5, 4)
# Changement de discrétisation pour les blocs et faces avec méthode transfinie
ctx.getTopoManager().replaceTransfiniteByDirectionalMeshMethodAsPossible()
# Création du maillage pour tous les blocs
ctx.getMeshManager().newAllBlocksMesh()
# Sauvegarde du maillage (mli)
ctx.getMeshManager().writeMli("cylindreOreille.mli2")
