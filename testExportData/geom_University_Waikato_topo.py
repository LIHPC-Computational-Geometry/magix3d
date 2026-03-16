# -*- coding: utf-8 -*-


import sys

import pyMagix3D as Mgx3D
ctx = Mgx3D.getStdContext()

# Création du sommet Pt0000
ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
# Création du sommet Pt0001
ctx.getGeomManager().newVertex (Mgx3D.Point(3, 3, 0))
# Création du sommet Pt0002
ctx.getGeomManager().newVertex (Mgx3D.Point(1, 2, 0))
# Création de l'arc de cercle Crb0000
ctx.getGeomManager().newArcCircle("Pt0002", "Pt0000", "Pt0001")
# Création du sommet Pt0003
ctx.getGeomManager().newVertex (Mgx3D.Point(3, 2.8, 0))
# Création du sommet Pt0004
ctx.getGeomManager().newVertex (Mgx3D.Point(3.2, 2.8, 0))
# Création du sommet Pt0005
ctx.getGeomManager().newVertex (Mgx3D.Point(.2, 0, 0))
# Création du sommet Pt0006
ctx.getGeomManager().newVertex (Mgx3D.Point(1.15, 1.85, 0))
# Création de l'arc de cercle Crb0001
ctx.getGeomManager().newArcCircle("Pt0006", "Pt0005", "Pt0003")
# Création du segment Crb0002
ctx.getGeomManager().newSegment("Pt0003", "Pt0004")
# Création du sommet Pt0007
ctx.getGeomManager().newVertex (Mgx3D.Point(3.2, 0, 0))
# Création du segment Crb0003
ctx.getGeomManager().newSegment("Pt0005", "Pt0007")
# Création du segment Crb0004
ctx.getGeomManager().newSegment("Pt0004", "Pt0007")
# Création du segment Crb0005
ctx.getGeomManager().newSegment("Pt0000", "Pt0005")
# Création du sommet Pt0008
ctx.getGeomManager().newVertex (Mgx3D.Point(3.5, 0, 0))
# Création du sommet Pt0009
ctx.getGeomManager().newVertex (Mgx3D.Point(6.1, 2.6, 0))
# Création du sommet Pt0010
ctx.getGeomManager().newVertex (Mgx3D.Point(6.1, 0, 0))
# Création de l'arc de cercle Crb0006
ctx.getGeomManager().newArcCircle("Pt0010", "Pt0008", "Pt0009", True)
# Création du sommet Pt0011
ctx.getGeomManager().newVertex (Mgx3D.Point(3.2, 3, 0))
# Création de l'arc de cercle Crb0007
ctx.getGeomManager().newArcCircle("Pt0007", "Pt0002", "Pt0001", True)
# Destruction de  Crb0007
ctx.getGeomManager().destroy(["Crb0007"], True)
# Création du sommet Pt0012
ctx.getGeomManager().newVertex (Mgx3D.Point(7.5, 2.6, 0))
# Création du sommet Pt0013
ctx.getGeomManager().newVertex (Mgx3D.Point(10.1, 0, 0))
# Création du sommet Pt0014
ctx.getGeomManager().newVertex (Mgx3D.Point(7.5, 0, 0))
# Création de l'arc de cercle Crb0008
ctx.getGeomManager().newArcCircle("Pt0014", "Pt0012", "Pt0013", True)
# Création du segment Crb0009
ctx.getGeomManager().newSegment("Pt0009", "Pt0012")
# Destruction de  Crb0008
ctx.getGeomManager().destroy(["Crb0008"], True)
# Création du sommet Pt0015
ctx.getGeomManager().newVertex (Mgx3D.Point(10.1, .5, 0))
# Création du sommet Pt0016
ctx.getGeomManager().newVertex (Mgx3D.Point(10.05, .7, 0))
# Création du sommet Pt0017
ctx.getGeomManager().newVertex (Mgx3D.Point(10.8, .7, 0))
# Création du sommet Pt0018
ctx.getGeomManager().newVertex (Mgx3D.Point(10.8, .5, 0))
# Création du sommet Pt0019
ctx.getGeomManager().newVertex (Mgx3D.Point(11.2, .9, 0))
# Création du sommet Pt0020
ctx.getGeomManager().newVertex (Mgx3D.Point(10.8, .9, 0))
# Création du sommet Pt0021
ctx.getGeomManager().newVertex (Mgx3D.Point(7.5, .7, 0))
# Création de l'arc de cercle Crb0010
ctx.getGeomManager().newArcCircle("Pt0021", "Pt0012", "Pt0016", True)
# Création de l'arc de cercle Crb0011
ctx.getGeomManager().newArcCircle("Pt0020", "Pt0018", "Pt0019", True)
# Création du segment Crb0012
ctx.getGeomManager().newSegment("Pt0016", "Pt0017")
# Destruction de  Pt0015
ctx.getGeomManager().destroy(["Pt0015"], True)
# Création du sommet Pt0022
ctx.getGeomManager().newVertex (Mgx3D.Point(9.85, .7, 0))
# Création du sommet Pt0023
ctx.getGeomManager().newVertex (Mgx3D.Point(9.85, .5, 0))
# Création du sommet Pt0024
ctx.getGeomManager().newVertex (Mgx3D.Point(7.5, 2.5, 0))
# Création du sommet Pt0025
ctx.getGeomManager().newVertex (Mgx3D.Point(6.1, 2.5, 0))
# Création du sommet Pt0026
ctx.getGeomManager().newVertex (Mgx3D.Point(3.6, 0, 0))
# Création du sommet Pt0027
ctx.getGeomManager().newVertex (Mgx3D.Point(9.9, .55, 0))
# Destruction de  Pt0022 Pt0023
ctx.getGeomManager().destroy(["Pt0022", "Pt0023"], True)
# Translation de Crb0011 suivant  [ 0, 0.05, 0] 
ctx.getGeomManager().translate(["Crb0011"], Mgx3D.Vector(0, 0.05, 0))
# Création de l'arc de cercle Crb0013
ctx.getGeomManager().newArcCircle("Pt0010", "Pt0026", "Pt0025", True)
# Création du segment Crb0014
ctx.getGeomManager().newSegment("Pt0025", "Pt0024")
# Création du sommet Pt0028
ctx.getGeomManager().newVertex (Mgx3D.Point(7.5, .55, 0))
# Translation de Pt0027 suivant  [ 0.025, 0, 0] 
ctx.getGeomManager().translate(["Pt0027"], Mgx3D.Vector(0.025, 0, 0))
# Translation de Pt0027 suivant  [ 0.025, 0, 0] 
ctx.getGeomManager().translate(["Pt0027"], Mgx3D.Vector(0.025, 0, 0))
# Translation de Crb0011 suivant  [ 0, 0.05, 0] 
ctx.getGeomManager().translate(["Crb0011"], Mgx3D.Vector(0, 0.05, 0))
# Translation de Pt0027 suivant  [ 0, 0.05, 0] 
ctx.getGeomManager().translate(["Pt0027"], Mgx3D.Vector(0, 0.05, 0))
# Création du segment Crb0015
ctx.getGeomManager().newSegment("Pt0027", "Pt0018")
# Translation de Pt0028 suivant  [ 0, 0.05, 0] 
ctx.getGeomManager().translate(["Pt0028"], Mgx3D.Vector(0, 0.05, 0))
# Création de l'arc de cercle Crb0016
ctx.getGeomManager().newArcCircle("Pt0028", "Pt0024", "Pt0027", True)
# Création du sommet Pt0029
ctx.getGeomManager().newVertex (Mgx3D.Point(11.7, 2.5, 0))
# Création du sommet Pt0030
ctx.getGeomManager().newVertex (Mgx3D.Point(11.5, 2.3, 0))
# Création du sommet Pt0031
ctx.getGeomManager().newVertex (Mgx3D.Point(11.2, 2, 0))
# Création du sommet Pt0032
ctx.getGeomManager().newVertex (Mgx3D.Point(11.5, 2, 0))
# Création du sommet Pt0033
ctx.getGeomManager().newVertex (Mgx3D.Point(11.7, 2.3, 0))
# Création de l'arc de cercle Crb0017
ctx.getGeomManager().newArcCircle("Pt0032", "Pt0031", "Pt0030", True)
# Création de l'arc de cercle Crb0018
ctx.getGeomManager().newArcCircle("Pt0033", "Pt0030", "Pt0029", True)
# Création du sommet Pt0034
ctx.getGeomManager().newVertex (Mgx3D.Point(10.8, 2.2, 0))
# Création du sommet Pt0035
ctx.getGeomManager().newVertex (Mgx3D.Point(11.2, 2.2, 0))
# Création du sommet Pt0036
ctx.getGeomManager().newVertex (Mgx3D.Point(11.2, 2.6, 0))
# Création de l'arc de cercle Crb0019
ctx.getGeomManager().newArcCircle("Pt0035", "Pt0034", "Pt0036", True)
# Création du segment Crb0020
ctx.getGeomManager().newSegment("Pt0034", "Pt0017")
# Création du segment Crb0021
ctx.getGeomManager().newSegment("Pt0019", "Pt0031")
# Création du sommet Pt0037
ctx.getGeomManager().newVertex (Mgx3D.Point(15, 2.5, 0))
# Création du sommet Pt0038
ctx.getGeomManager().newVertex (Mgx3D.Point(15.75, 2.6, 0))
# Création du sommet Pt0039
ctx.getGeomManager().newVertex (Mgx3D.Point(17.25, 1.5, 0))
# Création du segment Crb0022
ctx.getGeomManager().newSegment("Pt0036", "Pt0038")
# Création du segment Crb0023
ctx.getGeomManager().newSegment("Pt0029", "Pt0037")
# Création du segment Crb0024
ctx.getGeomManager().newSegment("Pt0038", "Pt0039")
# Création du sommet Pt0040
ctx.getGeomManager().newVertex (Mgx3D.Point(15.75, 3, 0))
# Création du segment Crb0025
ctx.getGeomManager().newSegment("Pt0001", "Pt0040")
# Création du segment Crb0026
ctx.getGeomManager().newSegment("Pt0040", "Pt0038")
# Création du sommet Pt0041
ctx.getGeomManager().newVertex (Mgx3D.Point(17.25, 1.425, 0))
# Création du sommet Pt0042
ctx.getGeomManager().newVertex (Mgx3D.Point(15.5, 2.3, 0))
# Création du sommet Pt0043
ctx.getGeomManager().newVertex (Mgx3D.Point(16.1, 1.85, 0))
# Création du sommet Pt0044
ctx.getGeomManager().newVertex (Mgx3D.Point(16.6, 1.6, 0))
# Création du sommet Pt0045
ctx.getGeomManager().newVertex (Mgx3D.Point(17.25, 2.6, 0))
# Création du sommet Pt0046
ctx.getGeomManager().newVertex (Mgx3D.Point(18.4, 1.85, 0))
# Translation de Pt0045 suivant  [ 0, .2, 0] 
ctx.getGeomManager().translate(["Pt0045"], Mgx3D.Vector(0, .2, 0))
# Translation de Pt0045 suivant  [ 0, .2, 0] 
ctx.getGeomManager().translate(["Pt0045"], Mgx3D.Vector(0, .2, 0))
# Translation de Pt0045 suivant  [ 0, .2, 0] 
ctx.getGeomManager().translate(["Pt0045"], Mgx3D.Vector(0, .2, 0))
# Création de l'arc de cercle Crb0027
ctx.getGeomManager().newArcCircle("Pt0045", "Pt0043", "Pt0046", True)
# Translation de Pt0042 suivant  [ 0, .1, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(0, .1, 0))
# Translation de Pt0042 suivant  [ -.2, 0.01, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(-.2, 0.01, 0))
# Translation de Pt0042 suivant  [ .1, 0.01, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(.1, 0.01, 0))
# Translation de Pt0042 suivant  [ 0, 0.01, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(0, 0.01, 0))
# Translation de Pt0042 suivant  [ 0, 0.01, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(0, 0.01, 0))
# Translation de Pt0042 suivant  [ 0, 0.01, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(0, 0.01, 0))
# Translation de Pt0042 suivant  [ 1, -.5, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(1, -.5, 0))
# Translation de Pt0042 suivant  [ -.3, 0, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(-.3, 0, 0))
# Translation de Pt0042 suivant  [ -0.05, 0, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(-0.05, 0, 0))
# Translation de Pt0042 suivant  [ -0.05, 0, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(-0.05, 0, 0))
# Translation de Pt0042 suivant  [ -.1, .1, 0] 
ctx.getGeomManager().translate(["Pt0042"], Mgx3D.Vector(-.1, .1, 0))
# Création du sommet Pt0047
ctx.getGeomManager().newVertex (Mgx3D.Point(15.7, 2.25, 0))
# Création de l'arc de cercle Crb0028
ctx.getGeomManager().newArcCircle("Pt0047", "Pt0042", "Pt0037")
# Création du segment Crb0029
ctx.getGeomManager().newSegment("Pt0042", "Pt0043")
# Création du sommet Pt0048
ctx.getGeomManager().newVertex (Mgx3D.Point(18.35, 1.9, 0))
# Création de l'arc de cercle Crb0030
ctx.getGeomManager().newArcCircle("Pt0045", "Pt0039", "Pt0048", True)
# Création du sommet Pt0049
ctx.getGeomManager().newVertex (Mgx3D.Point(2.0039949494e1, 2.6, 0))
# Création du sommet Pt0050
ctx.getGeomManager().newVertex (Mgx3D.Point(20.0399, 0, 0))
# Création de l'arc de cercle Crb0031
ctx.getGeomManager().newArcCircle("Pt0050", "Pt0048", "Pt0049", True)
# Création du sommet Pt0051
ctx.getGeomManager().newVertex (Mgx3D.Point(20.0399, 2.55, 0))
# Création de l'arc de cercle Crb0032
ctx.getGeomManager().newArcCircle("Pt0050", "Pt0046", "Pt0051", True)
# Création du segment Crb0033
ctx.getGeomManager().newSegment("Pt0049", "Pt0051")
# Création du sommet Pt0052
ctx.getGeomManager().newVertex (Mgx3D.Point(15, 1.35, 0))
# Création du sommet Pt0053
ctx.getGeomManager().newVertex (Mgx3D.Point(12, 1.35, 0))
# Création du sommet Pt0054
ctx.getGeomManager().newVertex (Mgx3D.Point(12, 0, 0))
# Création du sommet Pt0055
ctx.getGeomManager().newVertex (Mgx3D.Point(15, 0, 0))
# Création du segment Crb0034
ctx.getGeomManager().newSegment("Pt0008", "Pt0026")
# Création du segment Crb0035
ctx.getGeomManager().newSegment("Pt0007", "Pt0008")
# Destruction de  Pt0011
ctx.getGeomManager().destroy(["Pt0011"], True)
# Création du segment Crb0036
ctx.getGeomManager().newSegment("Pt0050", "Pt0055")
# Création du segment Crb0037
ctx.getGeomManager().newSegment("Pt0051", "Pt0050")
# Création du segment Crb0038
ctx.getGeomManager().newSegment("Pt0055", "Pt0052")
# Création du segment Crb0039
ctx.getGeomManager().newSegment("Pt0052", "Pt0037")
# Création du segment Crb0040
ctx.getGeomManager().newSegment("Pt0054", "Pt0055")
# Création du segment Crb0041
ctx.getGeomManager().newSegment("Pt0053", "Pt0054")
# Création du segment Crb0042
ctx.getGeomManager().newSegment("Pt0053", "Pt0052")
# Création du segment Crb0043
ctx.getGeomManager().newSegment("Pt0053", "Pt0029")
# Création du segment Crb0044
ctx.getGeomManager().newSegment("Pt0026", "Pt0054")
# Création du sommet Pt0056
ctx.getGeomManager().newVertex (Mgx3D.Point(6.2, 2.6, 0))
# Création du sommet Pt0057
ctx.getGeomManager().newVertex (Mgx3D.Point(6.4, 2.6, 0))
# Création du sommet Pt0058
ctx.getGeomManager().newVertex (Mgx3D.Point(7.4, 2.6, 0))
# Création du sommet Pt0059
ctx.getGeomManager().newVertex (Mgx3D.Point(7.2, 2.6, 0))
# Création du sommet Pt0060
ctx.getGeomManager().newVertex (Mgx3D.Point(7.1, 2.8, 0))
# Création du sommet Pt0061
ctx.getGeomManager().newVertex (Mgx3D.Point(7.5, 2.8, 0))
# Création du sommet Pt0062
ctx.getGeomManager().newVertex (Mgx3D.Point(6.1, 2.8, 0))
# Création du sommet Pt0063
ctx.getGeomManager().newVertex (Mgx3D.Point(6.5, 2.8, 0))
# Destruction de  Crb0009
ctx.getGeomManager().destroy(["Crb0009"], True)
# Création du segment Crb0045
ctx.getGeomManager().newSegment("Pt0004", "Pt0062")
# Création du segment Crb0046
ctx.getGeomManager().newSegment("Pt0056", "Pt0062")
# Création du segment Crb0047
ctx.getGeomManager().newSegment("Pt0056", "Pt0057")
# Création du segment Crb0048
ctx.getGeomManager().newSegment("Pt0056", "Pt0009")
# Création du segment Crb0049
ctx.getGeomManager().newSegment("Pt0063", "Pt0057")
# Création du segment Crb0050
ctx.getGeomManager().newSegment("Pt0063", "Pt0060")
# Création du segment Crb0051
ctx.getGeomManager().newSegment("Pt0059", "Pt0060")
# Création du segment Crb0052
ctx.getGeomManager().newSegment("Pt0059", "Pt0058")
# Création du segment Crb0053
ctx.getGeomManager().newSegment("Pt0058", "Pt0012")
# Création du segment Crb0054
ctx.getGeomManager().newSegment("Pt0058", "Pt0061")
# Création du sommet Pt0064
ctx.getGeomManager().newVertex (Mgx3D.Point(11.2, 2.8, 0))
# Création du sommet Pt0065
ctx.getGeomManager().newVertex (Mgx3D.Point(11.6, 2.8, 0))
# Création du sommet Pt0066
ctx.getGeomManager().newVertex (Mgx3D.Point(11.5, 2.6, 0))
# Création du sommet Pt0067
ctx.getGeomManager().newVertex (Mgx3D.Point(11.3, 2.6, 0))
# Création du sommet Pt0068
ctx.getGeomManager().newVertex (Mgx3D.Point(15.5, 2.6, 0))
# Création du sommet Pt0069
ctx.getGeomManager().newVertex (Mgx3D.Point(15.4, 2.8, 0))
# Destruction de  Crb0022
ctx.getGeomManager().destroy(["Crb0022"], True)
# Création du segment Crb0055
ctx.getGeomManager().newSegment("Pt0038", "Pt0068")
# Création du segment Crb0056
ctx.getGeomManager().newSegment("Pt0069", "Pt0068")
# Création du segment Crb0057
ctx.getGeomManager().newSegment("Pt0069", "Pt0065")
# Création du segment Crb0058
ctx.getGeomManager().newSegment("Pt0066", "Pt0068")
# Création du segment Crb0059
ctx.getGeomManager().newSegment("Pt0066", "Pt0065")
# Création du segment Crb0060
ctx.getGeomManager().newSegment("Pt0066", "Pt0067")
# Création du segment Crb0061
ctx.getGeomManager().newSegment("Pt0067", "Pt0036")
# Création du segment Crb0062
ctx.getGeomManager().newSegment("Pt0067", "Pt0064")
# Création du segment Crb0063
ctx.getGeomManager().newSegment("Pt0061", "Pt0064")
# Création de la surface Surf0000
ctx.getGeomManager().newPlanarSurface( ["Crb0005", "Crb0001", "Crb0002", "Crb0045", "Crb0046", "Crb0047", "Crb0049", "Crb0050", "Crb0051", "Crb0052", "Crb0054", "Crb0063", "Crb0062", "Crb0060", "Crb0059", "Crb0057", "Crb0056", "Crb0055", "Crb0026", "Crb0025", "Crb0000"] , "COMPOSITE")
# Création de la surface Surf0001
ctx.getGeomManager().newPlanarSurface( ["Crb0003", "Crb0002", "Crb0004", "Crb0001"] , "ALUMINIUM")
# Création du segment Crb0064
ctx.getGeomManager().newSegment("Pt0057", "Pt0059")
# Création du segment Crb0065
ctx.getGeomManager().newSegment("Pt0017", "Pt0018")
# Création de la surface Surf0002
ctx.getGeomManager().newPlanarSurface( ["Crb0013", "Crb0014", "Crb0016", "Crb0015", "Crb0065", "Crb0012", "Crb0010", "Crb0053", "Crb0052", "Crb0064", "Crb0047", "Crb0048", "Crb0006", "Crb0034"] , "")
# Création de la surface Surf0003
ctx.getGeomManager().newPlanarSurface( ["Crb0011", "Crb0021", "Crb0017", "Crb0018", "Crb0023", "Crb0028", "Crb0029", "Crb0027", "Crb0032", "Crb0033", "Crb0031", "Crb0030", "Crb0024", "Crb0055", "Crb0058", "Crb0060", "Crb0061", "Crb0019", "Crb0020", "Crb0065"] , "TITANE")
# Création de la surface Surf0004
ctx.getGeomManager().newPlanarSurface( ["Crb0040", "Crb0038", "Crb0042", "Crb0041"] , "FUEL")
# Création de la surface Surf0005
ctx.getGeomManager().newPlanarSurface( ["Crb0042", "Crb0039", "Crb0023", "Crb0043"] , "FUEL")
# Création de la surface Surf0006
ctx.getGeomManager().newPlanarSurface( ["Crb0058", "Crb0056", "Crb0057", "Crb0059"] , "AIR")
# Création de la surface Surf0007
ctx.getGeomManager().newPlanarSurface( ["Crb0012", "Crb0020", "Crb0019", "Crb0061", "Crb0062", "Crb0063", "Crb0054", "Crb0053", "Crb0010"] , "AIR")
# Création de la surface Surf0008
ctx.getGeomManager().newPlanarSurface( ["Crb0064", "Crb0051", "Crb0050", "Crb0049"] , "AIR")
# Création de la surface Surf0009
ctx.getGeomManager().newPlanarSurface( ["Crb0006", "Crb0048", "Crb0046", "Crb0045", "Crb0004", "Crb0035"] , "")
# Destruction de  Surf0003
ctx.getGeomManager().destroy(["Surf0003"], False)
# Destruction de  Crb0024 Crb0029
ctx.getGeomManager().destroy(["Crb0024", "Crb0029"], False)
# Destruction de  Crb0037
ctx.getGeomManager().destroy(["Crb0037"], False)
# Homothétie de Crb0027 Crb0032 Crb0031 Crb0030 Crb0033 de facteur 1, 1.5, 1
ctx.getGeomManager().scale(["Crb0027", "Crb0032", "Crb0031", "Crb0030", "Crb0033"], 1.000000e+00, 1.500000e+00, 1.000000e+00)
# Translation de Crb0033 Crb0032 Crb0031 Crb0030 Crb0027 suivant  [ 0, -1.3, 0] 
ctx.getGeomManager().translate(["Crb0033", "Crb0032", "Crb0031", "Crb0030", "Crb0027"], Mgx3D.Vector(0, -1.3, 0))
# Translation de Crb0027 Crb0030 Crb0031 Crb0033 Crb0032 suivant  [ .1, 0, 0] 
ctx.getGeomManager().translate(["Crb0027", "Crb0030", "Crb0031", "Crb0033", "Crb0032"], Mgx3D.Vector(.1, 0, 0))
# Translation de Crb0027 Crb0030 Crb0031 Crb0033 Crb0032 suivant  [ .1, 0, 0] 
ctx.getGeomManager().translate(["Crb0027", "Crb0030", "Crb0031", "Crb0033", "Crb0032"], Mgx3D.Vector(.1, 0, 0))
# Création du segment Crb0066
ctx.getGeomManager().newSegment("Pt0042", "Pt0043")
# Création du segment Crb0067
ctx.getGeomManager().newSegment("Pt0038", "Pt0039")
# Destruction de  Pt0050 Crb0036
ctx.getGeomManager().destroy(["Pt0050", "Crb0036"], True)
# Création du sommet Pt0070
ctx.getGeomManager().newVertex (Mgx3D.Point(20.2399, 0, 0))
# Création du segment Crb0068
ctx.getGeomManager().newSegment("Pt0070", "Pt0051")
# Création du segment Crb0069
ctx.getGeomManager().newSegment("Pt0070", "Pt0055")
# Destruction de  Surf0001
ctx.getGeomManager().destroy(["Surf0001"], False)
# Création de la surface Surf0010
ctx.getGeomManager().newPlanarSurface( ["Crb0003", "Crb0004", "Crb0002", "Crb0001"] , "ALUMINIUM")
# Création de la surface Surf0011
ctx.getGeomManager().newPlanarSurface( ["Crb0044", "Crb0041", "Crb0043", "Crb0018", "Crb0017", "Crb0021", "Crb0011", "Crb0015", "Crb0016", "Crb0014", "Crb0013"] , "AIR")
# Création de la surface Surf0012
ctx.getGeomManager().newPlanarSurface( ["Crb0011", "Crb0021", "Crb0017", "Crb0018", "Crb0023", "Crb0028", "Crb0066", "Crb0027", "Crb0032", "Crb0033", "Crb0031", "Crb0030", "Crb0067", "Crb0055", "Crb0058", "Crb0060", "Crb0061", "Crb0019", "Crb0020", "Crb0065"] , "TITANE")
# Création de la surface Surf0013
ctx.getGeomManager().newPlanarSurface( ["Crb0069", "Crb0068", "Crb0032", "Crb0027", "Crb0066", "Crb0028", "Crb0039", "Crb0038"] , "AIR")
# Modifie le groupe TITANE
ctx.getGeomManager().addToGroup (["Surf0002"], 2, "TITANE")
# Création d'une face topologique structurée sur une géométrie (Surf0004)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0004")
# Création d'une face topologique structurée sur une géométrie (Surf0005)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0005")
# Création d'une face topologique structurée sur une géométrie (Surf0006)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0006")
# Création d'une face topologique structurée sur une géométrie (Surf0008)
ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0008")
# Création d'une face topologique structurée sans projection (Surf0010)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0010")
# Création d'une face topologique structurée sans projection (Surf0013)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0013")
# Création d'une face topologique structurée sans projection (Surf0000)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0000")
# Affectation d'une projection vers Pt0006 pour les entités topologiques Som0014
ctx.getTopoManager().setGeomAssociation (["Som0014"], "Pt0006", True)
# Affectation d'une projection vers Pt0005 pour les entités topologiques Som0023
ctx.getTopoManager().setGeomAssociation (["Som0023"], "Pt0005", True)
# Affectation d'une projection vers Pt0000 pour les entités topologiques Som0022
ctx.getTopoManager().setGeomAssociation (["Som0022"], "Pt0000", True)
# Affectation d'une projection vers Pt0038 pour les entités topologiques Som0024
ctx.getTopoManager().setGeomAssociation (["Som0024"], "Pt0038", True)
# Affectation d'une projection vers Pt0040 pour les entités topologiques Som0025
ctx.getTopoManager().setGeomAssociation (["Som0025"], "Pt0040", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0026", Mgx3D.Point(3, 3, 0), .5)
# Suppression des projections pour les entités topologiques Som0024
ctx.getTopoManager().setGeomAssociation (["Som0024"], "", True)
# Modification de la position des sommets topologiques Som0024 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0024"], False, 0, True, 2.8, False, 0)
# Affectation d'une projection vers Pt0001 pour les entités topologiques Som0026
ctx.getTopoManager().setGeomAssociation (["Som0026"], "Pt0001", True)
# Affectation d'une projection vers Pt0003 pour les entités topologiques Som0027
ctx.getTopoManager().setGeomAssociation (["Som0027"], "Pt0003", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0029", Mgx3D.Point(1.15, 1.85, 0), .5)
# Translation de la topologie seule Som0029 Som0028
ctx.getTopoManager().translate (["Som0029", "Som0028"], Mgx3D.Vector(-.45, .45, 0), False)
# Affectation d'une projection vers Crb0000 pour les entités topologiques Som0029
ctx.getTopoManager().setGeomAssociation (["Som0029"], "Crb0000", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0016", Mgx3D.Point(3, 2.8, 0), .5)
# Affectation d'une projection vers Pt0003 pour les entités topologiques Som0031
ctx.getTopoManager().setGeomAssociation (["Som0031"], "Pt0003", True)
# Modification de la position des sommets topologiques Som0030 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0030"], True, 3, False, 0, False, 0)
# Affectation d'une projection vers Pt0037 pour les entités topologiques Som0018
ctx.getTopoManager().setGeomAssociation (["Som0018"], "Pt0037", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0022", Mgx3D.Point(17.45, .95, 0), .5)
# Modification de la position des sommets topologiques Som0032 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0032"], True, 17.6147, True, 8.51373e-1, True, 0)
# Création d'une face topologique structurée sans projection (Surf0011)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0011")
# Modification de la position des sommets topologiques Som0037 Som0036 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0037", "Som0036"], True, 9.95, False, 0, False, 0)
# Fusion de courbes Crb0006 Crb0048
ctx.getGeomManager().joinCurves(["Crb0006", "Crb0048"])
# Fusion de courbes Crb0053 Crb0010
ctx.getGeomManager().joinCurves(["Crb0053", "Crb0010"])
# Fusion de courbes Crb0013 Crb0014 Crb0016
ctx.getGeomManager().joinCurves(["Crb0013", "Crb0014", "Crb0016"])
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0050", Mgx3D.Point(6.2, 2.6, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0052", Mgx3D.Point(7.4, 2.6, 0), .5)
# Changement de discrétisation pour les arêtes Ar0000 Ar0001 Ar0002 Ar0003 Ar0004 ... 
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0000", "Ar0001", "Ar0002", "Ar0003", "Ar0004", "Ar0005", "Ar0006", "Ar0007", "Ar0008", "Ar0009", "Ar0010", "Ar0011", "Ar0012", "Ar0013", "Ar0014", "Ar0015", "Ar0017", "Ar0019", "Ar0021", "Ar0023", "Ar0025", "Ar0028", "Ar0030", "Ar0031", "Ar0032", "Ar0033", "Ar0034", "Ar0035", "Ar0036", "Ar0037", "Ar0038", "Ar0039", "Ar0040", "Ar0041", "Ar0042", "Ar0043", "Ar0044", "Ar0045", "Ar0046", "Ar0047", "Ar0049", "Ar0051", "Ar0053", "Ar0055", "Ar0056", "Ar0057", "Ar0058", "Ar0059", "Ar0060"])
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0056", Mgx3D.Point(7.2, 2.6, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0061", Mgx3D.Point(6.4, 2.6, 0), .5)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Som0034 Som0037
ctx.getTopoManager().setGeomAssociation (["Som0034", "Som0037"], "Crb0072", True)
# Modification de la position des sommets topologiques Som0038 Som0039 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0038", "Som0039"], True, 6.2, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0044 Som0045 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0044", "Som0045"], True, 6.4, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0040 Som0041 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0040", "Som0041"], True, 7.4, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0042 Som0043 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0042", "Som0043"], True, 7.2, False, 0, False, 0)
# Découpage en O-grid des faces structurées Fa0016 Fa0023 Fa0022 Fa0021 Fa0019
ctx.getTopoManager().splitFacesWithOgrid (["Fa0016", "Fa0023", "Fa0022", "Fa0021", "Fa0019"], ["Ar0053", "Ar0068", "Ar0069", "Ar0064", "Ar0059"], .2, 10)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0049", Mgx3D.Point(9.95, .6, 0), .5)
# Affectation d'une projection vers Pt0027 pour les entités topologiques Som0056
ctx.getTopoManager().setGeomAssociation (["Som0056"], "Pt0027", True)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Som0055
ctx.getTopoManager().setGeomAssociation (["Som0055"], "Crb0072", True)
# Modification de la position des sommets topologiques Som0055 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0055"], False, 0, True, .6, False, 0)
# Déplace les sommets topologiques  Som0055 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0055"])
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0028", Mgx3D.Point(15.4, 2.8, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0117", Mgx3D.Point(11.6, 2.8, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0120", Mgx3D.Point(11.2, 2.8, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0127", Mgx3D.Point(7.45, 2.7, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0130", Mgx3D.Point(7.15, 2.7, 0), .5)
# Modification de la position des sommets topologiques Som0062 Som0063 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0062", "Som0063"], True, 15.4, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0065 Som0064 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0065", "Som0064"], True, 11.6, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0067 Som0066 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0067", "Som0066"], True, 11.2, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0069 Som0068 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0069", "Som0068"], True, 7.5, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0071 Som0070 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0071", "Som0070"], True, 7.1, False, 0, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0135", Mgx3D.Point(6.5, 2.8, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0142", Mgx3D.Point(6.1, 2.8, 0), .5)
# Modification de la position des sommets topologiques Som0074 Som0075 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0074", "Som0075"], True, 6.1, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0073 Som0072 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0073", "Som0072"], True, 6.5, False, 0, False, 0)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().addToGroup (["Fa0064"], 2, "COMPOSITE")
# Modification de la position des sommets topologiques Som0079 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0079"], True, 15.75, True, 2.8, True, 0)
# Modification de la position des sommets topologiques Som0076 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0076"], True, 15.75, True, 2.6, True, 0)
# Modification de la position des sommets topologiques Som0078 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0078"], True, 15.4, True, 2.8, True, 0)
# Modification de la position des sommets topologiques Som0077 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0077"], True, 15.5, True, 2.6, True, 0)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().addToGroup (["Fa0065"], 2, "COMPOSITE")
# Translation de la topologie seule Fa0065
ctx.getTopoManager().translate (["Fa0065"], Mgx3D.Vector(10.5, 2.6, 0), False)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().addToGroup (["Fa0066"], 2, "COMPOSITE")
# Translation de la topologie seule Fa0066
ctx.getTopoManager().translate (["Fa0066"], Mgx3D.Vector(6.4, 2.6, 0), False)
# Affectation d'une projection vers Pt0067 pour les entités topologiques Som0081
ctx.getTopoManager().setGeomAssociation (["Som0081"], "Pt0067", True)
# Affectation d'une projection vers Pt0065 pour les entités topologiques Som0083
ctx.getTopoManager().setGeomAssociation (["Som0083"], "Pt0065", True)
# Affectation d'une projection vers Pt0064 pour les entités topologiques Som0082
ctx.getTopoManager().setGeomAssociation (["Som0082"], "Pt0064", True)
# Affectation d'une projection vers Pt0061 pour les entités topologiques Som0087
ctx.getTopoManager().setGeomAssociation (["Som0087"], "Pt0061", True)
# Affectation d'une projection vers Pt0059 pour les entités topologiques Som0085
ctx.getTopoManager().setGeomAssociation (["Som0085"], "Pt0059", True)
# Affectation d'une projection vers Pt0060 pour les entités topologiques Som0086
ctx.getTopoManager().setGeomAssociation (["Som0086"], "Pt0060", True)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().addToGroup (["Fa0067"], 2, "COMPOSITE")
# Translation de la topologie seule Fa0067
ctx.getTopoManager().translate (["Fa0067"], Mgx3D.Vector(5.4, 2.6, 0), False)
# Affectation d'une projection vers Pt0056 pour les entités topologiques Som0089
ctx.getTopoManager().setGeomAssociation (["Som0089"], "Pt0056", True)
# Affectation d'une projection vers Pt0063 pour les entités topologiques Som0091
ctx.getTopoManager().setGeomAssociation (["Som0091"], "Pt0063", True)
# Affectation d'une projection vers Pt0062 pour les entités topologiques Som0090
ctx.getTopoManager().setGeomAssociation (["Som0090"], "Pt0062", True)
# Affectation d'une projection vers Crb0047 pour les entités topologiques Ar0066
ctx.getTopoManager().setGeomAssociation (["Ar0066"], "Crb0047", True)
# Affectation d'une projection vers Crb0052 pour les entités topologiques Ar0062
ctx.getTopoManager().setGeomAssociation (["Ar0062"], "Crb0052", True)
# Affectation d'une projection vers Crb0070 pour les entités topologiques Som0035 Som0055 Som0034
ctx.getTopoManager().setGeomAssociation (["Som0035", "Som0055", "Som0034"], "Crb0070", True)
# Affectation d'une projection vers Crb0071 pour les entités topologiques Som0037
ctx.getTopoManager().setGeomAssociation (["Som0037"], "Crb0071", True)
# Modification de la position des sommets topologiques Som0056 Som0036 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0056", "Som0036"], True, 10.05, False, 0, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0096", Mgx3D.Point(3.6, 0, 0), .5)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Som0093 Som0094 Som0095 Som0096 Som0092 ... 
ctx.getTopoManager().setGeomAssociation (["Som0093", "Som0094", "Som0095", "Som0096", "Som0092", "Som0098", "Som0099", "Som0097", "Som0100"], "Crb0072", True)
# Modification de la position des sommets topologiques Som0101 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0101"], True, 9.95, False, 0, False, 0)
# Affectation d'une projection vers Pt0027 pour les entités topologiques Som0100
ctx.getTopoManager().setGeomAssociation (["Som0100"], "Pt0027", True)
# Modification de la position des sommets topologiques Som0037 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0037"], False, 0, True, 1.83848, False, 0)
# Affectation d'une projection vers Crb0071 pour les entités topologiques Som0037
ctx.getTopoManager().setGeomAssociation (["Som0037"], "Crb0071", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0098", 0.05, .5)
# Affectation d'une projection vers Pt0016 pour les entités topologiques Som0110
ctx.getTopoManager().setGeomAssociation (["Som0110"], "Pt0016", True)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Som0111
ctx.getTopoManager().setGeomAssociation (["Som0111"], "Crb0072", True)
# Affectation d'une projection vers Crb0070 pour les entités topologiques Som0109
ctx.getTopoManager().setGeomAssociation (["Som0109"], "Crb0070", True)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Som0108
ctx.getTopoManager().setGeomAssociation (["Som0108"], "Crb0072", True)
# Fusion de courbes Crb0032 Crb0027
ctx.getGeomManager().joinCurves(["Crb0032", "Crb0027"])
# Fusion de courbes Crb0073 Crb0066
ctx.getGeomManager().joinCurves(["Crb0073", "Crb0066"])
# Fusion de courbes avec maj de la topologie
ctx.getGeomManager().joinCurves(["Crb0074", "Crb0028"])
# Fusion de courbes Crb0031 Crb0030
ctx.getGeomManager().joinCurves(["Crb0031", "Crb0030"])
# Modification de la position des sommets topologiques Som0032 Som0033 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0032", "Som0033"], True, 17.45, False, 0, False, 0)
# Affectation d'une projection vers Crb0075 pour les entités topologiques Som0032
ctx.getTopoManager().setGeomAssociation (["Som0032"], "Crb0075", True)
# Création d'une face topologique structurée sans projection (Surf0011)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0011")
# Translation de la topologie seule Ar0224
ctx.getTopoManager().translate (["Ar0224"], Mgx3D.Vector(7.9, 0, 0), False)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0224", .4, .5)
# Modification de la position des sommets topologiques Som0117 Som0116 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0117", "Som0116"], False, 0, True, 1.35, False, 0)
# Affectation d'une projection vers Pt0029 pour les entités topologiques Som0115
ctx.getTopoManager().setGeomAssociation (["Som0115"], "Pt0029", True)
# Changement de discrétisation pour les arêtes Ar0103 Ar0104 Ar0106 Ar0108 Ar0109 ... 
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0103", "Ar0104", "Ar0106", "Ar0108", "Ar0109", "Ar0110", "Ar0112", "Ar0113", "Ar0114", "Ar0116", "Ar0118", "Ar0119", "Ar0121", "Ar0123", "Ar0124", "Ar0126", "Ar0128", "Ar0129", "Ar0131", "Ar0133", "Ar0134", "Ar0136", "Ar0138", "Ar0139", "Ar0141", "Ar0143", "Ar0144", "Ar0145", "Ar0146", "Ar0147", "Ar0148", "Ar0149", "Ar0150", "Ar0151", "Ar0152", "Ar0153", "Ar0154", "Ar0155", "Ar0156", "Ar0157", "Ar0158", "Ar0159", "Ar0160", "Ar0161", "Ar0162", "Ar0163", "Ar0164", "Ar0165", "Ar0166", "Ar0167", "Ar0168", "Ar0169", "Ar0170", "Ar0171", "Ar0172", "Ar0173", "Ar0174", "Ar0175", "Ar0176", "Ar0177", "Ar0178", "Ar0179", "Ar0180", "Ar0181", "Ar0182", "Ar0183", "Ar0184", "Ar0186", "Ar0187", "Ar0188", "Ar0189", "Ar0190", "Ar0191", "Ar0192", "Ar0193", "Ar0195", "Ar0196", "Ar0197", "Ar0198", "Ar0199", "Ar0200", "Ar0201", "Ar0202", "Ar0203", "Ar0204", "Ar0205", "Ar0206", "Ar0207", "Ar0208", "Ar0209", "Ar0210", "Ar0211", "Ar0212", "Ar0213", "Ar0214", "Ar0215", "Ar0216", "Ar0217", "Ar0218", "Ar0219", "Ar0220", "Ar0221", "Ar0222", "Ar0223", "Ar0225", "Ar0227", "Ar0228", "Ar0229", "Ar0230", "Ar0231", "Ar0232"])
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0228", Mgx3D.Point(11.5, 2.3, 0), .5)
# Affectation d'une projection vers Pt0030 pour les entités topologiques Som0118
ctx.getTopoManager().setGeomAssociation (["Som0118"], "Pt0030", True)
# Affectation d'une projection vers Crb0018 pour les entités topologiques Som0112
ctx.getTopoManager().setGeomAssociation (["Som0112"], "Crb0018", True)
# Découpage en O-grid des faces structurées Fa0108 Fa0107 Fa0105
ctx.getTopoManager().splitFacesWithOgrid (["Fa0108", "Fa0107", "Fa0105"], ["Ar0225", "Ar0236", "Ar0235", "Ar0230"], .5, 10)
# Destruction des entités topologiques Fa0101 Fa0103 Fa0095 Fa0093 Fa0089 ... 
ctx.getTopoManager().destroy (["Fa0101", "Fa0103", "Fa0095", "Fa0093", "Fa0089", "Fa0091", "Fa0087", "Fa0096", "Fa0098"], True)
# Fusion entre arêtes Ar0184 et Ar0217
ctx.getTopoManager().fuse2Edges ("Ar0184","Ar0217")
# Fusion entre arêtes Ar0214 et Ar0183
ctx.getTopoManager().fuse2Edges ("Ar0214","Ar0183")
# Fusion entre arêtes Ar0104 et Ar0199
ctx.getTopoManager().fuse2Edges ("Ar0104","Ar0199")
# Collage entre 2 listes d'arêtes
ctx.getTopoManager().fuse2EdgeList (["Ar0110", "Ar0109", "Ar0113", "Ar0114", "Ar0190"], ["Ar0205", "Ar0204", "Ar0208", "Ar0211", "Ar0223"])
# Destruction des entités topologiques Fa0082
ctx.getTopoManager().destroy (["Fa0082"], True)
# Affectation d'une projection vers Surf0002 pour les entités topologiques Fa0099
ctx.getTopoManager().setGeomAssociation (["Fa0099"], "Surf0002", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0097 Fa0080 Fa0041 Fa0090 Fa0088 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0097", "Fa0080", "Fa0041", "Fa0090", "Fa0088", "Fa0086", "Fa0092", "Fa0094", "Fa0102"], "Surf0011", True)
# Affectation d'une projection vers Surf0002 pour les entités topologiques Fa0100
ctx.getTopoManager().setGeomAssociation (["Fa0100"], "Surf0002", True)
# Fusion de surfaces avec maj de la topologie
ctx.getGeomManager().joinSurfaces(["Surf0002", "Surf0012"])
# Création d'une face topologique structurée sans projection (Surf0007)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0007")
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0116
ctx.getTopoManager().setGeomAssociation (["Fa0116"], "Surf0011", True)
# Translation de la topologie seule Ar0251
ctx.getTopoManager().translate (["Ar0251"], Mgx3D.Vector(3.8, 0, 0), False)
# Modification de la position des sommets topologiques Som0125 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0125"], False, 0, True, 2.3, False, 0)
# Modification de la position des sommets topologiques Som0128 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0128"], True, 11.5, True, 2.3, False, 0)
# Modification de la position des sommets topologiques Som0127 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0127"], True, 11.5, True, -1e-7, False, 0)
# Modification de la position des sommets topologiques Som0126 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0126"], False, 0, True, 0, False, 0)
# Modification de la position des sommets topologiques Som0125 Som0126 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0125", "Som0126"], True, 10.8, False, 0, False, 0)
# Suppression des projections pour les entités topologiques Ar0254 Ar0251 Ar0252 Ar0253
ctx.getTopoManager().setGeomAssociation (["Ar0254", "Ar0251", "Ar0252", "Ar0253"], "", True)
# Suppression des projections pour les entités topologiques Som0125 Som0126
ctx.getTopoManager().setGeomAssociation (["Som0125", "Som0126"], "", True)
# Suppression des projections pour les entités topologiques Som0127
ctx.getTopoManager().setGeomAssociation (["Som0127"], "", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0254", .6, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0251", Mgx3D.Point(10.8, .6, 0), .5)
# Modification de la position des sommets topologiques Som0131 Som0132 Som0133 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0131", "Som0132", "Som0133"], False, 0, True, .6, False, 0)
# Modification de la position des sommets topologiques Som0129 Som0132 Som0130 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0129", "Som0132", "Som0130"], True, 11.2, False, 0, False, 0)
# Translation de la topologie seule Som0131
ctx.getTopoManager().translate (["Som0131"], Mgx3D.Vector(.1, 0, 0), False)
# Translation de la topologie seule Som0126
ctx.getTopoManager().translate (["Som0126"], Mgx3D.Vector(.1, 0, 0), False)
# Découpage en O-grid des faces structurées Fa0119
ctx.getTopoManager().splitFacesWithOgrid (["Fa0119"], ["Ar0260", "Ar0255"], .5, 10)
# Suppression des projections pour les entités topologiques Som0135 Som0136 Som0134
ctx.getTopoManager().setGeomAssociation (["Som0135", "Som0136", "Som0134"], "", True)
# Suppression des projections pour les entités topologiques Ar0264 Ar0262 Ar0263 Ar0271 Ar0268 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0264", "Ar0262", "Ar0263", "Ar0271", "Ar0268", "Ar0274", "Ar0270", "Ar0273", "Ar0272", "Ar0269"], "", True)
# Suppression des projections pour les entités topologiques Ar0267
ctx.getTopoManager().setGeomAssociation (["Ar0267"], "", True)
# Suppression des projections pour les entités topologiques Som0132
ctx.getTopoManager().setGeomAssociation (["Som0132"], "", True)
# Modification de la position des sommets topologiques Som0135 Som0136 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0135", "Som0136"], False, 0, True, .7, False, 0)
# Modification de la position des sommets topologiques Som0135 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0135"], True, 10.9, False, 0, False, 0)
# Translation de la topologie seule Som0135 Som0131
ctx.getTopoManager().translate (["Som0135", "Som0131"], Mgx3D.Vector(0, -.1, 0), False)
# Translation de la topologie seule Som0134 Som0129
ctx.getTopoManager().translate (["Som0134", "Som0129"], Mgx3D.Vector(.1, 0, 0), False)
# Destruction des entités topologiques Fa0125
ctx.getTopoManager().destroy (["Fa0125"], True)
# Changement de discrétisation pour Ar0274
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0274")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0274", Mgx3D.Point(11.2, 2, 0), .5)
# Affectation d'une projection vers Pt0031 pour les entités topologiques Som0139
ctx.getTopoManager().setGeomAssociation (["Som0139"], "Pt0031", True)
# Affectation d'une projection vers Pt0032 pour les entités topologiques Som0138
ctx.getTopoManager().setGeomAssociation (["Som0138"], "Pt0032", True)
# Modification de la position des sommets topologiques Som0137 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0137"], False, 0, True, 2, False, 0)
# Destruction des entités topologiques Fa0128 Fa0126
ctx.getTopoManager().destroy (["Fa0128", "Fa0126"], True)
# Affectation d'une projection vers Crb0011 pour les entités topologiques Som0136
ctx.getTopoManager().setGeomAssociation (["Som0136"], "Crb0011", True)
# Affectation d'une projection vers Crb0011 pour les entités topologiques Som0135
ctx.getTopoManager().setGeomAssociation (["Som0135"], "Crb0011", True)
# Translation de la topologie seule Som0132
ctx.getTopoManager().translate (["Som0132"], Mgx3D.Vector(-0.025, 0.025, 0), False)
# Modification de la position des sommets topologiques Som0133 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0133"], False, 0, True, .625, False, 0)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Translation de la topologie seule Fa0130
ctx.getTopoManager().translate (["Fa0130"], Mgx3D.Vector(10.5, 2, 0), False)
# Translation de la topologie seule Ar0285
ctx.getTopoManager().translate (["Ar0285"], Mgx3D.Vector(0, -.7, 0), False)
# Translation de la topologie seule Ar0284
ctx.getTopoManager().translate (["Ar0284"], Mgx3D.Vector(.7, 0, 0), False)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().addToGroup (["Fa0130"], 2, "COMPOSITE")
# Affectation d'une projection vers Crb0017 pour les entités topologiques Som0142
ctx.getTopoManager().setGeomAssociation (["Som0142"], "Crb0017", True)
# Découpage en O-grid des faces structurées Fa0130
ctx.getTopoManager().splitFacesWithOgrid (["Fa0130"], ["Ar0283", "Ar0286"], .46, 10)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0006", Mgx3D.Point(1.171275000425e1, 2.451125, -4.25000007270893e-9), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0296", Mgx3D.Point(1.1751000017e1, 2.3045, -1.70000002908357e-8), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0003", Mgx3D.Point(11.5, .625, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0250", Mgx3D.Point(11.5, .625, 0), .5)
# Changement de discrétisation pour Ar0301
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0301")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0248", Mgx3D.Point(11.5, 2, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0318", Mgx3D.Point(11.5, 2.219, 0), .5)
# Modification de la position des sommets topologiques Som0160 Som0159 Som0161 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0160", "Som0159", "Som0161"], False, 0, True, 2.219, False, 0)
# Modification de la position des sommets topologiques Som0157 Som0156 Som0158 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0157", "Som0156", "Som0158"], False, 0, True, 2, False, 0)
# Affectation d'une projection vers Crb0043 pour les entités topologiques Som0158
ctx.getTopoManager().setGeomAssociation (["Som0158"], "Crb0043", True)
# Affectation d'une projection vers Crb0043 pour les entités topologiques Som0161
ctx.getTopoManager().setGeomAssociation (["Som0161"], "Crb0043", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0299", Mgx3D.Point(1.17732212390012e1, 2.21931858382854, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0335", Mgx3D.Point(1.18303433628793e1, 2.00035044229619, 0), .5)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Translation de la topologie seule Fa0156
ctx.getTopoManager().translate (["Fa0156"], Mgx3D.Vector(9.9, 0, 0), False)
# Translation de la topologie seule Ar0344
ctx.getTopoManager().translate (["Ar0344"], Mgx3D.Vector(0.05, 0, 0), False)
# Translation de la topologie seule Ar0345
ctx.getTopoManager().translate (["Ar0345"], Mgx3D.Vector(-2.98575e-3, -3.88057e-1, 0), False)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0344", Mgx3D.Point(10.9, .5, 0), .5)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0157 Fa0158
ctx.getTopoManager().setGeomAssociation (["Fa0157", "Fa0158"], "Surf0011", True)
# Affectation d'une projection vers Pt0027 pour les entités topologiques Som0168
ctx.getTopoManager().setGeomAssociation (["Som0168"], "Pt0027", True)
# Modification de la position des sommets topologiques Som0171 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0171"], True, 10.9, True, .5, False, 0)
# Changement de discrétisation pour Ar0099
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0099")
# Changement de discrétisation pour Ar0112
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0112")
# Changement de discrétisation pour Ar0188
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0188")
# Changement de discrétisation pour les arêtes Ar0095 Ar0188 Ar0093 Ar0103 Ar0108 ... 
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0095", "Ar0188", "Ar0093", "Ar0103", "Ar0108", "Ar0106", "Ar0112", "Ar0099", "Ar0193"])
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0093", .2, .5)
# Modification de la position des sommets topologiques Som0170 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0170"], True, 9.95, True, .48, False, 0)
# Création d'une face topologique structurée sans projection (Surf0009)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0009")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0379", .3, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0386", .7, .5)
# Modification de la position des sommets topologiques Som0187 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0187"], False, 0, True, 1.96, False, 0)
# Translation de la topologie seule Som0187
ctx.getTopoManager().translate (["Som0187"], Mgx3D.Vector(-0.05, 0.05, 0), False)
# Translation de la topologie seule Som0187
ctx.getTopoManager().translate (["Som0187"], Mgx3D.Vector(-0.05, 0.05, 0), False)
# Affectation d'une projection vers Pt0062 pour les entités topologiques Som0184
ctx.getTopoManager().setGeomAssociation (["Som0184"], "Pt0062", True)
# Modification de la position des sommets topologiques Som0189 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0189"], True, 6.15019, True, 2.69961, True, 0)
# Modification de la position des sommets topologiques Som0185 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0185"], True, 3.35607, True, 0, True, 0)
# Modification de la position des sommets topologiques Som0185 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0185"], True, 3.35019, True, 0, True, 0)
# Découpage en O-grid des faces structurées Fa0180
ctx.getTopoManager().splitFacesWithOgrid (["Fa0180"], ["Ar0392", "Ar0383"], .9, 10)
# Destruction des entités topologiques Fa0184
ctx.getTopoManager().destroy (["Fa0184"], True)
# Modification de la position des sommets topologiques Som0191 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0191"], True, 4.26152, True, 1.83848, False, 0)
# Affectation d'une projection vers Pt0056 pour les entités topologiques Som0192
ctx.getTopoManager().setGeomAssociation (["Som0192"], "Pt0056", True)
# Affectation d'une projection vers Pt0008 pour les entités topologiques Som0190
ctx.getTopoManager().setGeomAssociation (["Som0190"], "Pt0008", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0041", Mgx3D.Point(3.1999999, 1.96000004, -1.00000000495516e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0034", Mgx3D.Point(8.6499997e-1, 1.29499997, -3.00000000787846e-8), .5)
# Changement de discrétisation pour les arêtes Ar0399
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0399"])
# Changement de discrétisation pour Ar0387
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0387")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0387", Mgx3D.Point(3.57524333753756, 6.20970043843907e-1, 0), .5)
# Affectation d'une projection vers Crb0070 pour les entités topologiques Som0179
ctx.getTopoManager().setGeomAssociation (["Som0179"], "Crb0070", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0420", Mgx3D.Point(3.54835214246264, 4.99092387364327e-1, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0402", Mgx3D.Point(3.1999999, 6.075999434e-1, -1.00000000495516e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0431", Mgx3D.Point(3.1999999, 4.89999935e-1, -1.00000000495516e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0410", Mgx3D.Point(4.08999922e-1, 4.06999922e-1, -7.80000002048399e-8), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0447", Mgx3D.Point(3.70999918e-1, 3.32999918e-1, -8.20000002153445e-8), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0147", Mgx3D.Point(4.09999996, 2.8000001, -1.00000000495516e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0457", Mgx3D.Point(3.2, 1.4, 0), .5)
# Modification de la position des sommets topologiques Som0215 Som0214 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0215", "Som0214"], True, 4.1, False, 0, False, 0)
# Modification de la position des sommets topologiques Som0217 Som0216 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0217", "Som0216"], True, 3.2, False, 0, False, 0)
# Affectation d'une projection vers Crb0000 pour les entités topologiques Som0212 Som0210 Som0196
ctx.getTopoManager().setGeomAssociation (["Som0212", "Som0210", "Som0196"], "Crb0000", True)
# Affectation d'une projection vers Crb0001 pour les entités topologiques Som0197 Som0213 Som0211
ctx.getTopoManager().setGeomAssociation (["Som0197", "Som0213", "Som0211"], "Crb0001", True)
# Modification de la position des sommets topologiques Som0194 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0194"], True, 7.40488e-1, True, 1.3545, False, 0)
# Modification de la position des sommets topologiques Som0205 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0205"], True, 3.15455e-1, True, 4.79901e-1, False, 0)
# Modification de la position des sommets topologiques Som0208 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0208"], True, 2.96711e-1, True, 4.19183e-1, False, 0)
# Modification de la position des sommets topologiques Som0200 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0200"], True, 3.57524, True, .62097, False, 0)
# Modification de la position des sommets topologiques Som0203 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0203"], True, 3.54835, True, 4.99092e-1, False, 0)
# Modification de la position des sommets topologiques Som0199 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0199"], True, 3.2, True, .616, False, 0)
# Modification de la position des sommets topologiques Som0202 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0202"], True, 3.2, True, .504, False, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(3.2000001, 6.15999944e-1, -1.00000000262615e-7), Mgx3D.Point(3.57524, .62097, 0), ["Som0198"])
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(3.2000001, 5.03999936e-1, -1.00000000262615e-7), Mgx3D.Point(3.54835, 4.99092e-1, 0), ["Som0201"])
# Modification de la position des sommets topologiques Som0198 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0198"], True, 3.42339, True, 6.18959e-1, True, -3.66667e-8)
# Modification de la position des sommets topologiques Som0201 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0201"], True, 3.367, True, 5.01647e-1, True, -4.66667e-8)
# Modification de la position des sommets topologiques Som0198 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0198"], True, 3.37871, True, 6.18367e-1, True, -4.93334e-8)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0007", Mgx3D.Point(11.7, 2.5, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0123", Mgx3D.Point(11.79, 2.8, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0468", Mgx3D.Point(15, 2.5, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0473", Mgx3D.Point(14.906, 2.8, 0), .5)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Translation de la topologie seule Fa0223
ctx.getTopoManager().translate (["Fa0223"], Mgx3D.Vector(9.89701, 6.11943e-1, 0), False)
# Affectation d'une projection vers Pt0027 pour les entités topologiques Som0227
ctx.getTopoManager().setGeomAssociation (["Som0227"], "Pt0027", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0223
ctx.getTopoManager().setGeomAssociation (["Fa0223"], "Surf0014", True)
# Affectation d'une projection vers Pt0017 pour les entités topologiques Som0229
ctx.getTopoManager().setGeomAssociation (["Som0229"], "Pt0017", True)
# Affectation d'une projection vers Pt0016 pour les entités topologiques Som0228
ctx.getTopoManager().setGeomAssociation (["Som0228"], "Pt0016", True)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0224
ctx.getTopoManager().translate (["Fa0224"], Mgx3D.Vector(10.2, 1, 0), False)
# Modification de la position des sommets topologiques Som0230 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0230"], True, 11.0886, True, 7.22987e-1, False, 0)
# Affectation d'une projection vers Crb0020 pour les entités topologiques Som0232
ctx.getTopoManager().setGeomAssociation (["Som0232"], "Crb0020", True)
# Affectation d'une projection vers Crb0020 pour les entités topologiques Som0231
ctx.getTopoManager().setGeomAssociation (["Som0231"], "Crb0020", True)
# Modification de la position des sommets topologiques Som0231 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0231"], True, 10.8, True, 8.94675e-1, True, 0)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0225
ctx.getTopoManager().translate (["Fa0225"], Mgx3D.Vector(19.2399, 2.525, 0), False)
# Modification de la position des sommets topologiques Som0235 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0235"], True, 17.45, True, 8.39878e-1, False, 0)
# Modification de la position des sommets topologiques Som0237 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0237"], True, 20.2399, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0236 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0236"], True, 17.45, True, .95, False, 0)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0226
ctx.getTopoManager().translate (["Fa0226"], Mgx3D.Vector(14, 2.5, 0), False)
# Modification de la position des sommets topologiques Som0239 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0239"], True, 11.7, True, 2.5, False, 0)
# Modification de la position des sommets topologiques Som0240 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0240"], True, 11.7, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0241 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0241"], True, 14.98, True, 2.6, False, 0)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0227
ctx.getTopoManager().translate (["Fa0227"], Mgx3D.Vector(16.45, 8.39878e-1, 0), False)
# Modification de la position des sommets topologiques Som0245 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0245"], True, 17.45, True, .95, False, 0)
# Modification de la position des sommets topologiques Som0244 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0244"], True, 15.75, True, 2.6, False, 0)
# Affectation d'une projection vers Pt0047 pour les entités topologiques Som0243
ctx.getTopoManager().setGeomAssociation (["Som0243"], "Pt0047", True)
# Modification de la position des sommets topologiques Som0243 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0243"], True, 15.6257, True, 2.3037, True, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0042", Mgx3D.Point(15.6257, 2.3037, 0), .5)
# Modification de la position des sommets topologiques Som0246 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0246"], True, 15.6257, True, 2.3037, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0507", Mgx3D.Point(15.5, 2.6, 0), .5)
# Affectation d'une projection vers Crb0075 pour les entités topologiques Som0248
ctx.getTopoManager().setGeomAssociation (["Som0248"], "Crb0075", True)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0232
ctx.getTopoManager().translate (["Fa0232"], Mgx3D.Vector(10.0886, 7.22987e-1, 0), False)
# Modification de la position des sommets topologiques Som0251 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0251"], True, 10.897, True, 6.11943e-1, False, 0)
# Modification de la position des sommets topologiques Som0252 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0252"], True, 10.8, True, .7, False, 0)
# Modification de la position des sommets topologiques Som0253 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0253"], True, 10.8, True, 8.94675e-1, False, 0)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0233
ctx.getTopoManager().translate (["Fa0233"], Mgx3D.Vector(14.6257, 2.3037, 0), False)
# Modification de la position des sommets topologiques Som0257 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0257"], True, 15.75, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0255 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0255"], True, 15, True, 2.5, False, 0)
# Modification de la position des sommets topologiques Som0256 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0256"], True, 14.98, True, 2.6, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0523", Mgx3D.Point(15.5, 2.6, 0), .5)
# Modification de la position des sommets topologiques Som0258 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0258"], True, 15.5, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0259 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0259"], True, 15.4333, True, 2.40724, False, 0)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0225 Fa0227 Fa0235 Fa0234 Fa0226 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0225", "Fa0227", "Fa0235", "Fa0234", "Fa0226", "Fa0232", "Fa0224"], "Surf0014", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0133 Fa0132 Fa0131
ctx.getTopoManager().setGeomAssociation (["Fa0133", "Fa0132", "Fa0131"], "Surf0011", True)
# Fusion de courbes avec maj de la topologie
ctx.getGeomManager().joinCurves(["Crb0020", "Crb0019", "Crb0061"])
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0236
ctx.getTopoManager().translate (["Fa0236"], Mgx3D.Vector(10.2, 2, 0), False)
# Translation de la topologie seule Fa0236
ctx.getTopoManager().translate (["Fa0236"], Mgx3D.Vector(.5, .5, 0), False)
# Modification de la position des sommets topologiques Som0263 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0263"], True, 11.7, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0262 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0262"], True, 11.5, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0261 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0261"], True, 11.5586, True, 2.44142, False, 0)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0237
ctx.getTopoManager().translate (["Fa0237"], Mgx3D.Vector(10.2, 2, 0), False)
# Modification de la position des sommets topologiques Som0267 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0267"], True, 11.5586, True, 2.44142, False, 0)
# Modification de la position des sommets topologiques Som0266 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0266"], True, 11.5, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0265 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0265"], True, 10.8, True, 2, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0535", Mgx3D.Point(1.1287867965644e1, 2.21213203435596, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0541", Mgx3D.Point(11.5, 2.3, 0), .5)
# Modification de la position des sommets topologiques Som0269 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0269"], True, 11.2879, True, 2.21213, False, 0)
# Modification de la position des sommets topologiques Som0270 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0270"], True, 11.5, True, 2.3, False, 0)
# Modification de la position des sommets topologiques Som0268 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0268"], True, 10.9278, True, 2.4931, True, 0)
# Modification de la position des sommets topologiques Som0271 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0271"], True, 11.3, True, 2.6, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0133", Mgx3D.Point(10.9278, 2.4931, 0), .5)
# Translation de la topologie seule Som0272 Som0273
ctx.getTopoManager().translate (["Som0272", "Som0273"], Mgx3D.Vector(-.1, 0, 0), False)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Translation de la topologie seule Fa0244
ctx.getTopoManager().translate (["Fa0244"], Mgx3D.Vector(9.9278, 2.4931, 0), False)
# Translation de la topologie seule Fa0244
ctx.getTopoManager().translate (["Fa0244"], Mgx3D.Vector(.3722, .1069, 0), False)
# Modification de la position des sommets topologiques Som0277 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0277"], True, 11.2, True, 2.8, False, 0)
# Modification de la position des sommets topologiques Som0275 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0275"], True, 10.9278, True, 2.4931, False, 0)
# Modification de la position des sommets topologiques Som0276 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0276"], True, 10.841, True, 2.8, False, 0)
# Création d'une face topologique structurée sans projection (Surf0007)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0007")
# Modification de la position des sommets topologiques Som0280 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0280"], True, 10.8, True, .7, False, 0)
# Modification de la position des sommets topologiques Som0281 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0281"], True, 10.841, True, 2.8, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0558", .8, .5)
# Translation de la topologie seule Ar0565
ctx.getTopoManager().translate (["Ar0565"], Mgx3D.Vector(.1, 0, 0), False)
# Translation de la topologie seule Ar0565
ctx.getTopoManager().translate (["Ar0565"], Mgx3D.Vector(.1, 0, 0), False)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0565", .9, .5)
# Translation de la topologie seule Ar0570
ctx.getTopoManager().translate (["Ar0570"], Mgx3D.Vector(0, .1, 0), False)
# Translation de la topologie seule Ar0573
ctx.getTopoManager().translate (["Ar0573"], Mgx3D.Vector(-0.0369, -.59, 1e-7), False)
# Translation de la topologie seule Ar0567
ctx.getTopoManager().translate (["Ar0567"], Mgx3D.Vector(-.5, 0, 0), False)
# Affectation d'une projection vers Pt0061 pour les entités topologiques Som0278
ctx.getTopoManager().setGeomAssociation (["Som0278"], "Pt0061", True)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(7.4, 2.6, 0), Mgx3D.Point(7.5, 2.8, 0), ["Som0285"])
# Modification de la position des sommets topologiques Som0285 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0285"], False, 0, True, 2.69961, False, 0)
# Affectation d'une projection vers Crb0054 pour les entités topologiques Som0285
ctx.getTopoManager().setGeomAssociation (["Som0285"], "Crb0054", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0550", Mgx3D.Point(9.85279998, 2.80000002, -1.00000000892655e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0343", Mgx3D.Point(1.031999998e1, 6.9999998e-1, -1.00000000892655e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0487", Mgx3D.Point(1.031999998e1, 6.9999998e-1, -1.00000000892655e-7), .5)
# Modification de la position des sommets topologiques Som0292 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0292"], True, 10.3292, True, .6, False, 0)
# Modification de la position des sommets topologiques Som0293 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0293"], True, 10.32, True, .7, False, 0)
# Modification de la position des sommets topologiques Som0283 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0283"], True, 9.83511, True, 2.8, False, 0)
# Découpage en O-grid des faces structurées Fa0248
ctx.getTopoManager().splitFacesWithOgrid (["Fa0248"], ["Ar0561", "Ar0569"], .9, 10)
# Destruction des entités topologiques Fa0262
ctx.getTopoManager().destroy (["Fa0262"], True)
# Modification de la position des sommets topologiques Som0295 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0295"], True, 9.34883, True, 1.8529, False, 0)
# Modification de la position des sommets topologiques Som0294 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0294"], True, 10.05, True, .7, False, 0)
# Modification de la position des sommets topologiques Som0296 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0296"], True, 7.4, True, 2.6, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0159", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0165", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0012", .5, .5)
# Modification de la position des sommets topologiques Som0285 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0285"], True, 7.45, True, 2.7, False, 0)
# Changement de discrétisation pour Ar0567
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0567")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0572", Mgx3D.Point(10.9278, 2.4931, 0), .5)
# Modification de la position des sommets topologiques Som0305 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0305"], True, 10.9278, True, 2.4931, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0602", Mgx3D.Point(7.48, 2.76, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0610", Mgx3D.Point(7.12, 2.76, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0605", Mgx3D.Point(6.48, 2.76, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0393", Mgx3D.Point(6.12, 2.76, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0403", Mgx3D.Point(3.1999999, 2.52000008, -1.00000000495516e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0411", Mgx3D.Point(1.013496, 1.68483333333333, -1.00000000262615e-8), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0508", .45, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0043", .45, .5)
# Modification de la position des sommets topologiques Som0320 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0320"], True, 16.4457, True, 1.30752, True, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0503", Mgx3D.Point(16.4457, 1.30752, 0), .5)
# Modification de la position des sommets topologiques Som0324 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0324"], True, 16.4457, True, 1.30752, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0664", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0663", .5, .5)
# Modification de la position des sommets topologiques Som0328 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0328"], True, 18.0917, True, 1.02615, True, -3.5628e-8)
# Modification de la position des sommets topologiques Som0326 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0326"], True, 19.4868, True, 2.27369, True, 0)
# Changement de discrétisation pour Ar0497
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0497")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0497", Mgx3D.Point(18.0917, 1.02615, -3.5628e-8), .5)
# Modification de la position des sommets topologiques Som0331 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0331"], True, 18.0917, True, 1.02615, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0685", Mgx3D.Point(1.87062806642846e1, 1.60944294944806, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0691", Mgx3D.Point(19.4868, 2.27369, 0), .5)
# Modification de la position des sommets topologiques Som0333 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0333"], True, 18.7276, True, 1.6354, True, 0)
# Modification de la position des sommets topologiques Som0333 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0333"], True, 18.6752, True, 1.70744, True, 0)
# Modification de la position des sommets topologiques Som0334 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0334"], True, 19.4603, True, 2.3609, True, 0)
# Modification de la position des sommets topologiques Som0335 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0335"], True, 19.4868, True, 2.27369, False, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0019", Mgx3D.Point(15, 2.454, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0699", Mgx3D.Point(15, 2.3045, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0725", Mgx3D.Point(15, 2.218595, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0751", Mgx3D.Point(15, 1.99906, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0777", Mgx3D.Point(15, 1.35, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0803", Mgx3D.Point(15, .621, 0), .5)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0238 Fa0240 Fa0241 Fa0236
ctx.getTopoManager().setGeomAssociation (["Fa0238", "Fa0240", "Fa0241", "Fa0236"], "Surf0014", True)
# Affectation d'une projection vers Surf0007 pour les entités topologiques Fa0244
ctx.getTopoManager().setGeomAssociation (["Fa0244"], "Surf0007", True)
# Destruction des entités topologiques Fa0272 Fa0271 Fa0244
ctx.getTopoManager().destroy (["Fa0272", "Fa0271", "Fa0244"], True)
# Création d'un bloc unitaire mis dans le groupe AIR
ctx.getTopoManager().newFreeTopoInGroup ("AIR", 2)
# Translation de la topologie seule Fa0401
ctx.getTopoManager().translate (["Fa0401"], Mgx3D.Vector(9.8, 2, 0), False)
# Découpage de la face Fa0401
ctx.getTopoManager().splitFace ("Fa0401", "Ar0857", .5, True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0862", .5, .5)
# Découpage en O-grid des faces structurées Fa0404
ctx.getTopoManager().splitFacesWithOgrid (["Fa0404"], ["Ar0858", "Ar0865"], .5, 10)
# Destruction des entités topologiques Fa0410
ctx.getTopoManager().destroy (["Fa0410"], True)
# Modification de la position des sommets topologiques Som0393 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0393"], True, 11.2, True, 2.8, False, 0)
# Modification de la position des sommets topologiques Som0399 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0399"], True, 11.3, True, 2.6, False, 0)
# Modification de la position des sommets topologiques Som0398 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0398"], True, 10.841, True, 2.8, False, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(11.2, 2.8, 0), Mgx3D.Point(11.3, 2.6, 0), ["Som0394"])
# Modification de la position des sommets topologiques Som0400 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0400"], True, 10.9278, True, 2.4931, False, 0)
# Modification de la position des sommets topologiques Som0401 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0401"], True, 10.8, True, 2, False, 0)
# Modification de la position des sommets topologiques Som0391 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0391"], True, 9.81262, True, 2.1, False, 0)
# Modification de la position des sommets topologiques Som0395 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0395"], True, 9.82611, True, 2.52, False, 0)
# Modification de la position des sommets topologiques Som0392 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0392"], True, 9.83511, True, 2.8, False, 0)
# Modification de la position des sommets topologiques Som0397 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0397"], True, 10.6025, True, 2.02, True, 8.97756e-9)
# Modification de la position des sommets topologiques Som0396 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0396"], True, 10.7705, True, 2.55258, True, 0)
# Modification de la position des sommets topologiques Som0394 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0394"], True, 11.2524, True, 2.69515, True, 0)
# Modification de la position des sommets topologiques Som0396 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0396"], True, 10.7926, True, 2.59963, True, 0)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0481", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0153", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0155", .5, .5)
# Modification de la position des sommets topologiques Som0394 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0394"], True, 11.25, True, 2.7, False, 0)
# Changement de discrétisation pour les arêtes Ar0000 Ar0002 Ar0005 Ar0011 Ar0023 ... 
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0000", "Ar0002", "Ar0005", "Ar0011", "Ar0023", "Ar0025", "Ar0031", "Ar0033", "Ar0035", "Ar0036", "Ar0037", "Ar0038", "Ar0051", "Ar0057", "Ar0062", "Ar0064", "Ar0066", "Ar0067", "Ar0068", "Ar0069", "Ar0080", "Ar0082", "Ar0084", "Ar0086", "Ar0087", "Ar0089", "Ar0091", "Ar0104", "Ar0109", "Ar0110", "Ar0113", "Ar0114", "Ar0116", "Ar0118", "Ar0119", "Ar0124", "Ar0126", "Ar0128", "Ar0129", "Ar0134", "Ar0136", "Ar0138", "Ar0139", "Ar0141", "Ar0143", "Ar0144", "Ar0146", "Ar0148", "Ar0149", "Ar0150", "Ar0152", "Ar0154", "Ar0156", "Ar0158", "Ar0162", "Ar0166", "Ar0167", "Ar0168", "Ar0169", "Ar0170", "Ar0171", "Ar0172", "Ar0173", "Ar0174", "Ar0175", "Ar0176", "Ar0177", "Ar0178", "Ar0179", "Ar0180", "Ar0181", "Ar0182", "Ar0184", "Ar0186", "Ar0187", "Ar0190", "Ar0192", "Ar0195", "Ar0197", "Ar0200", "Ar0202", "Ar0206", "Ar0210", "Ar0213", "Ar0214", "Ar0215", "Ar0219", "Ar0220", "Ar0222", "Ar0227", "Ar0233", "Ar0238", "Ar0239", "Ar0240", "Ar0241", "Ar0242", "Ar0243", "Ar0244", "Ar0245", "Ar0246", "Ar0247", "Ar0249", "Ar0257", "Ar0258", "Ar0261", "Ar0263", "Ar0264", "Ar0265", "Ar0267", "Ar0269", "Ar0270", "Ar0273", "Ar0276", "Ar0277", "Ar0279", "Ar0281", "Ar0282", "Ar0284", "Ar0285", "Ar0287", "Ar0288", "Ar0289", "Ar0290", "Ar0291", "Ar0292", "Ar0293", "Ar0295", "Ar0297", "Ar0298", "Ar0300", "Ar0302", "Ar0303", "Ar0304", "Ar0305", "Ar0306", "Ar0307", "Ar0308", "Ar0309", "Ar0310", "Ar0311", "Ar0312", "Ar0313", "Ar0314", "Ar0315", "Ar0316", "Ar0317", "Ar0320", "Ar0321", "Ar0322", "Ar0324", "Ar0325", "Ar0326", "Ar0327", "Ar0328", "Ar0329", "Ar0330", "Ar0331", "Ar0332", "Ar0334", "Ar0336", "Ar0337", "Ar0338", "Ar0339", "Ar0340", "Ar0341", "Ar0342", "Ar0347", "Ar0348", "Ar0349", "Ar0350", "Ar0352", "Ar0353", "Ar0354", "Ar0355", "Ar0356", "Ar0357", "Ar0358", "Ar0359", "Ar0360", "Ar0361", "Ar0362", "Ar0363", "Ar0364", "Ar0365", "Ar0366", "Ar0367", "Ar0368", "Ar0369", "Ar0370", "Ar0371", "Ar0372", "Ar0373", "Ar0374", "Ar0375", "Ar0376", "Ar0377", "Ar0382", "Ar0391", "Ar0394", "Ar0395", "Ar0396", "Ar0397", "Ar0400", "Ar0406", "Ar0409", "Ar0414", "Ar0416", "Ar0417", "Ar0419", "Ar0421", "Ar0422", "Ar0423", "Ar0424", "Ar0425", "Ar0426", "Ar0427", "Ar0428", "Ar0429", "Ar0430", "Ar0432", "Ar0433", "Ar0435", "Ar0437", "Ar0438", "Ar0439", "Ar0440", "Ar0441", "Ar0442", "Ar0443", "Ar0444", "Ar0445", "Ar0446", "Ar0448", "Ar0450", "Ar0451", "Ar0452", "Ar0453", "Ar0454", "Ar0455", "Ar0456", "Ar0458", "Ar0460", "Ar0461", "Ar0462", "Ar0463", "Ar0464", "Ar0465", "Ar0466", "Ar0467", "Ar0470", "Ar0472", "Ar0474", "Ar0476", "Ar0477", "Ar0478", "Ar0479", "Ar0480", "Ar0482", "Ar0483", "Ar0484", "Ar0485", "Ar0486", "Ar0488", "Ar0490", "Ar0491", "Ar0492", "Ar0493", "Ar0494", "Ar0496", "Ar0498", "Ar0499", "Ar0500", "Ar0501", "Ar0502", "Ar0504", "Ar0506", "Ar0512", "Ar0513", "Ar0514", "Ar0515", "Ar0517", "Ar0518", "Ar0519", "Ar0520", "Ar0522", "Ar0524", "Ar0525", "Ar0526", "Ar0527", "Ar0528", "Ar0529", "Ar0530", "Ar0531", "Ar0532", "Ar0533", "Ar0534", "Ar0536", "Ar0538", "Ar0540", "Ar0542", "Ar0543", "Ar0544", "Ar0545", "Ar0546", "Ar0547", "Ar0549", "Ar0551", "Ar0552", "Ar0562", "Ar0566", "Ar0570", "Ar0571", "Ar0573", "Ar0574", "Ar0575", "Ar0576", "Ar0577", "Ar0578", "Ar0579", "Ar0580", "Ar0581", "Ar0582", "Ar0583", "Ar0584", "Ar0585", "Ar0586", "Ar0587", "Ar0588", "Ar0589", "Ar0590", "Ar0591", "Ar0592", "Ar0593", "Ar0594", "Ar0596", "Ar0597", "Ar0599", "Ar0601", "Ar0603", "Ar0604", "Ar0606", "Ar0608", "Ar0609", "Ar0612", "Ar0613", "Ar0614", "Ar0617", "Ar0618", "Ar0622", "Ar0624", "Ar0626", "Ar0627", "Ar0630", "Ar0631", "Ar0632", "Ar0634", "Ar0636", "Ar0637", "Ar0640", "Ar0641", "Ar0642", "Ar0644", "Ar0645", "Ar0648", "Ar0649", "Ar0650", "Ar0652", "Ar0653", "Ar0655", "Ar0657", "Ar0658", "Ar0659", "Ar0660", "Ar0661", "Ar0668", "Ar0669", "Ar0670", "Ar0671", "Ar0672", "Ar0673", "Ar0674", "Ar0675", "Ar0676", "Ar0678", "Ar0679", "Ar0680", "Ar0681", "Ar0683", "Ar0686", "Ar0687", "Ar0689", "Ar0690", "Ar0692", "Ar0693", "Ar0694", "Ar0695", "Ar0696", "Ar0697", "Ar0698", "Ar0700", "Ar0702", "Ar0703", "Ar0705", "Ar0706", "Ar0708", "Ar0709", "Ar0711", "Ar0712", "Ar0714", "Ar0716", "Ar0718", "Ar0719", "Ar0720", "Ar0722", "Ar0723", "Ar0724", "Ar0726", "Ar0728", "Ar0729", "Ar0731", "Ar0732", "Ar0734", "Ar0735", "Ar0737", "Ar0738", "Ar0740", "Ar0742", "Ar0744", "Ar0745", "Ar0746", "Ar0748", "Ar0749", "Ar0750", "Ar0752", "Ar0754", "Ar0755", "Ar0757", "Ar0758", "Ar0760", "Ar0761", "Ar0763", "Ar0764", "Ar0766", "Ar0768", "Ar0770", "Ar0771", "Ar0772", "Ar0774", "Ar0775", "Ar0776", "Ar0778", "Ar0780", "Ar0781", "Ar0783", "Ar0784", "Ar0786", "Ar0787", "Ar0789", "Ar0790", "Ar0792", "Ar0794", "Ar0796", "Ar0797", "Ar0798", "Ar0800", "Ar0801", "Ar0802", "Ar0804", "Ar0806", "Ar0807", "Ar0809", "Ar0810", "Ar0812", "Ar0813", "Ar0815", "Ar0816", "Ar0818", "Ar0820", "Ar0822", "Ar0823", "Ar0824", "Ar0826", "Ar0827", "Ar0828", "Ar0829", "Ar0830", "Ar0831", "Ar0832", "Ar0833", "Ar0834", "Ar0835", "Ar0836", "Ar0837", "Ar0838", "Ar0839", "Ar0840", "Ar0841", "Ar0842", "Ar0843", "Ar0844", "Ar0845", "Ar0846", "Ar0847", "Ar0848", "Ar0849", "Ar0850", "Ar0851", "Ar0852", "Ar0853", "Ar0013", "Ar0039", "Ar0040", "Ar0160", "Ar0164", "Ar0384", "Ar0385", "Ar0563", "Ar0615", "Ar0616", "Ar0623", "Ar0625", "Ar0628", "Ar0629", "Ar0633", "Ar0635", "Ar0638", "Ar0639", "Ar0643", "Ar0646", "Ar0647", "Ar0651", "Ar0654", "Ar0656", "Ar0859", "Ar0860", "Ar0861", "Ar0863", "Ar0864", "Ar0866", "Ar0867", "Ar0868", "Ar0869", "Ar0870", "Ar0871", "Ar0872", "Ar0873", "Ar0875", "Ar0876", "Ar0878", "Ar0879", "Ar0880", "Ar0881", "Ar0882", "Ar0883", "Ar0884", "Ar0885", "Ar0886", "Ar0887", "Ar0888", "Ar0889", "Ar0890", "Ar0891", "Ar0892", "Ar0893", "Ar0894", "Ar0895", "Ar0896", "Ar0897", "Ar0898"])
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0573", Mgx3D.Point(10.6025, 2.02, 8.97756e-9), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0590", Mgx3D.Point(1.0703999996e1, 6.99999996e-1, -1e-7), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0582", Mgx3D.Point(1.0783448e1, 6.095544e-1, 0), .5)
# Modification de la position des sommets topologiques Som0413 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0413"], True, 10.7843, True, 5.99188e-1, False, 0)
# Modification de la position des sommets topologiques Som0322 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0322"], True, 18.6967, True, 1.5979, False, 0)
# Modification de la position des sommets topologiques Som0336 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0336"], True, 15, True, 2.454, False, 0)
# Modification de la position des sommets topologiques Som0345 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0345"], True, 15, True, 2.3045, False, 0)
# Modification de la position des sommets topologiques Som0354 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0354"], True, 15, True, 2.2186, False, 0)
# Modification de la position des sommets topologiques Som0363 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0363"], True, 15, True, 1.99906, False, 0)
# Modification de la position des sommets topologiques Som0381 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0381"], True, 15, True, .621, False, 0)
# Modification de la position des sommets topologiques Som0319 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0319"], True, 1.0135, True, 1.68483, False, 0)
# Affectation d'une projection vers Crb0000 pour les entités topologiques Som0318
ctx.getTopoManager().setGeomAssociation (["Som0318"], "Crb0000", True)
# Modification de la position des sommets topologiques Som0409 Som0402 Som0404 Som0407 Som0406 ...  en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0409", "Som0402", "Som0404", "Som0407", "Som0406", "Som0394", "Som0408"], False, 0, True, 2.76, False, 0)
# Modification de la position des sommets topologiques Som0403 Som0405 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0403", "Som0405"], False, 0, True, 2.76, False, 0)
# Affectation d'une projection vers Crb0062 pour les entités topologiques Som0408
ctx.getTopoManager().setGeomAssociation (["Som0408"], "Crb0062", True)
# Modification de la position des sommets topologiques Som0408 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0408"], False, 0, True, 2.76, False, 0)
# Déplace les sommets topologiques  Som0408 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0408"])
# Modification de la position des sommets topologiques Som0408 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0408"], False, 0, True, 2.76, False, 0)
# Déplace les sommets topologiques  Som0408 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0408"])
# Déplace les sommets topologiques  Som0403 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0403"])
# Modification de la position des sommets topologiques Som0403 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0403"], False, 0, True, 2.76, False, 0)
# Déplace les sommets topologiques  Som0403 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0403"])
# Modification de la position des sommets topologiques Som0403 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0403"], False, 0, True, 2.76, False, 0)
# Déplace les sommets topologiques  Som0403 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0403"])
# Translation de la topologie seule Som0409 Som0402
ctx.getTopoManager().translate (["Som0409", "Som0402"], Mgx3D.Vector(0.02976, -0.00048, 0), False)
# Déplace les sommets topologiques  Som0405 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0405"])
# Modification de la position des sommets topologiques Som0405 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0405"], False, 0, True, 2.76, False, 0)
# Déplace les sommets topologiques  Som0405 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0405"])
# Modification de la position des sommets topologiques Som0405 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0405"], False, 0, True, 2.76, False, 0)
# Déplace les sommets topologiques  Som0405 suivant la projection sur la géométrie
ctx.getTopoManager().snapProjectedVertices (["Som0405"])
# Translation de la topologie seule Som0407 Som0404
ctx.getTopoManager().translate (["Som0407", "Som0404"], Mgx3D.Vector(-0.02976, -0.00048, 0), False)
# Modification de la position des sommets topologiques Som0394 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0394"], False, 0, True, 2.75952, False, 0)
# Modification de la position des sommets topologiques Som0394 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0394"], True, 11.2202, True, 2.75952, False, 0)
# Changement de discrétisation pour les arêtes Ar0000 Ar0002 Ar0005 Ar0011 Ar0023 ... 
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setMeshingProperty (emp,["Ar0000", "Ar0002", "Ar0005", "Ar0011", "Ar0023", "Ar0025", "Ar0031", "Ar0033", "Ar0035", "Ar0036", "Ar0037", "Ar0038", "Ar0051", "Ar0057", "Ar0062", "Ar0064", "Ar0066", "Ar0067", "Ar0068", "Ar0069", "Ar0080", "Ar0082", "Ar0084", "Ar0086", "Ar0087", "Ar0089", "Ar0091", "Ar0104", "Ar0109", "Ar0110", "Ar0113", "Ar0114", "Ar0116", "Ar0118", "Ar0119", "Ar0124", "Ar0126", "Ar0128", "Ar0129", "Ar0134", "Ar0136", "Ar0138", "Ar0139", "Ar0141", "Ar0143", "Ar0144", "Ar0146", "Ar0148", "Ar0149", "Ar0150", "Ar0152", "Ar0154", "Ar0156", "Ar0158", "Ar0162", "Ar0166", "Ar0167", "Ar0168", "Ar0169", "Ar0170", "Ar0171", "Ar0172", "Ar0173", "Ar0174", "Ar0175", "Ar0176", "Ar0177", "Ar0178", "Ar0179", "Ar0180", "Ar0181", "Ar0182", "Ar0184", "Ar0186", "Ar0187", "Ar0190", "Ar0192", "Ar0195", "Ar0197", "Ar0200", "Ar0202", "Ar0206", "Ar0210", "Ar0213", "Ar0214", "Ar0215", "Ar0219", "Ar0220", "Ar0222", "Ar0227", "Ar0233", "Ar0238", "Ar0239", "Ar0240", "Ar0241", "Ar0242", "Ar0243", "Ar0244", "Ar0245", "Ar0246", "Ar0247", "Ar0249", "Ar0257", "Ar0258", "Ar0261", "Ar0263", "Ar0264", "Ar0265", "Ar0267", "Ar0269", "Ar0270", "Ar0273", "Ar0276", "Ar0277", "Ar0279", "Ar0281", "Ar0282", "Ar0284", "Ar0285", "Ar0287", "Ar0288", "Ar0289", "Ar0290", "Ar0291", "Ar0292", "Ar0293", "Ar0295", "Ar0297", "Ar0298", "Ar0300", "Ar0302", "Ar0303", "Ar0304", "Ar0305", "Ar0306", "Ar0307", "Ar0308", "Ar0309", "Ar0310", "Ar0311", "Ar0312", "Ar0313", "Ar0314", "Ar0315", "Ar0316", "Ar0317", "Ar0320", "Ar0321", "Ar0322", "Ar0324", "Ar0325", "Ar0326", "Ar0327", "Ar0328", "Ar0329", "Ar0330", "Ar0331", "Ar0332", "Ar0334", "Ar0336", "Ar0337", "Ar0338", "Ar0339", "Ar0340", "Ar0341", "Ar0342", "Ar0347", "Ar0348", "Ar0349", "Ar0350", "Ar0352", "Ar0353", "Ar0354", "Ar0355", "Ar0356", "Ar0357", "Ar0358", "Ar0359", "Ar0360", "Ar0361", "Ar0362", "Ar0363", "Ar0364", "Ar0365", "Ar0366", "Ar0367", "Ar0368", "Ar0369", "Ar0370", "Ar0371", "Ar0372", "Ar0373", "Ar0374", "Ar0375", "Ar0376", "Ar0377", "Ar0382", "Ar0391", "Ar0394", "Ar0395", "Ar0396", "Ar0397", "Ar0400", "Ar0406", "Ar0409", "Ar0414", "Ar0416", "Ar0417", "Ar0419", "Ar0421", "Ar0422", "Ar0423", "Ar0424", "Ar0425", "Ar0426", "Ar0427", "Ar0428", "Ar0429", "Ar0430", "Ar0432", "Ar0433", "Ar0435", "Ar0437", "Ar0438", "Ar0439", "Ar0440", "Ar0441", "Ar0442", "Ar0443", "Ar0444", "Ar0445", "Ar0446", "Ar0448", "Ar0450", "Ar0451", "Ar0452", "Ar0453", "Ar0454", "Ar0455", "Ar0456", "Ar0458", "Ar0460", "Ar0461", "Ar0462", "Ar0463", "Ar0464", "Ar0465", "Ar0466", "Ar0467", "Ar0470", "Ar0472", "Ar0474", "Ar0476", "Ar0477", "Ar0478", "Ar0479", "Ar0480", "Ar0482", "Ar0483", "Ar0484", "Ar0485", "Ar0486", "Ar0488", "Ar0490", "Ar0491", "Ar0492", "Ar0493", "Ar0494", "Ar0496", "Ar0498", "Ar0499", "Ar0500", "Ar0501", "Ar0502", "Ar0504", "Ar0506", "Ar0512", "Ar0513", "Ar0514", "Ar0515", "Ar0517", "Ar0518", "Ar0519", "Ar0520", "Ar0522", "Ar0524", "Ar0525", "Ar0526", "Ar0527", "Ar0528", "Ar0529", "Ar0530", "Ar0531", "Ar0532", "Ar0533", "Ar0534", "Ar0536", "Ar0538", "Ar0540", "Ar0542", "Ar0543", "Ar0544", "Ar0545", "Ar0546", "Ar0547", "Ar0549", "Ar0551", "Ar0552", "Ar0566", "Ar0570", "Ar0571", "Ar0574", "Ar0575", "Ar0576", "Ar0577", "Ar0578", "Ar0580", "Ar0581", "Ar0583", "Ar0584", "Ar0586", "Ar0588", "Ar0589", "Ar0591", "Ar0592", "Ar0593", "Ar0594", "Ar0596", "Ar0597", "Ar0599", "Ar0601", "Ar0603", "Ar0604", "Ar0606", "Ar0608", "Ar0609", "Ar0612", "Ar0613", "Ar0614", "Ar0617", "Ar0618", "Ar0622", "Ar0624", "Ar0626", "Ar0627", "Ar0630", "Ar0631", "Ar0632", "Ar0634", "Ar0636", "Ar0637", "Ar0640", "Ar0641", "Ar0642", "Ar0644", "Ar0645", "Ar0648", "Ar0649", "Ar0650", "Ar0652", "Ar0653", "Ar0655", "Ar0657", "Ar0658", "Ar0659", "Ar0660", "Ar0661", "Ar0668", "Ar0669", "Ar0670", "Ar0671", "Ar0672", "Ar0673", "Ar0674", "Ar0675", "Ar0676", "Ar0678", "Ar0679", "Ar0680", "Ar0681", "Ar0683", "Ar0686", "Ar0687", "Ar0689", "Ar0690", "Ar0692", "Ar0693", "Ar0694", "Ar0695", "Ar0696", "Ar0697", "Ar0698", "Ar0700", "Ar0702", "Ar0703", "Ar0705", "Ar0706", "Ar0708", "Ar0709", "Ar0711", "Ar0712", "Ar0714", "Ar0716", "Ar0718", "Ar0719", "Ar0720", "Ar0722", "Ar0723", "Ar0724", "Ar0726", "Ar0728", "Ar0729", "Ar0731", "Ar0732", "Ar0734", "Ar0735", "Ar0737", "Ar0738", "Ar0740", "Ar0742", "Ar0744", "Ar0745", "Ar0746", "Ar0748", "Ar0749", "Ar0750", "Ar0752", "Ar0754", "Ar0755", "Ar0757", "Ar0758", "Ar0760", "Ar0761", "Ar0763", "Ar0764", "Ar0766", "Ar0768", "Ar0770", "Ar0771", "Ar0772", "Ar0774", "Ar0775", "Ar0776", "Ar0778", "Ar0780", "Ar0781", "Ar0783", "Ar0784", "Ar0786", "Ar0787", "Ar0789", "Ar0790", "Ar0792", "Ar0794", "Ar0796", "Ar0797", "Ar0798", "Ar0800", "Ar0801", "Ar0802", "Ar0804", "Ar0806", "Ar0807", "Ar0809", "Ar0810", "Ar0812", "Ar0813", "Ar0815", "Ar0816", "Ar0818", "Ar0820", "Ar0822", "Ar0823", "Ar0824", "Ar0826", "Ar0827", "Ar0828", "Ar0829", "Ar0830", "Ar0831", "Ar0832", "Ar0833", "Ar0834", "Ar0835", "Ar0836", "Ar0837", "Ar0838", "Ar0839", "Ar0840", "Ar0841", "Ar0842", "Ar0843", "Ar0844", "Ar0845", "Ar0846", "Ar0847", "Ar0848", "Ar0849", "Ar0850", "Ar0851", "Ar0852", "Ar0853", "Ar0013", "Ar0039", "Ar0040", "Ar0160", "Ar0164", "Ar0384", "Ar0385", "Ar0563", "Ar0615", "Ar0616", "Ar0623", "Ar0625", "Ar0628", "Ar0629", "Ar0633", "Ar0635", "Ar0638", "Ar0639", "Ar0643", "Ar0646", "Ar0647", "Ar0651", "Ar0654", "Ar0656", "Ar0859", "Ar0860", "Ar0861", "Ar0863", "Ar0864", "Ar0866", "Ar0867", "Ar0868", "Ar0869", "Ar0870", "Ar0871", "Ar0872", "Ar0873", "Ar0875", "Ar0876", "Ar0878", "Ar0879", "Ar0880", "Ar0881", "Ar0882", "Ar0883", "Ar0884", "Ar0885", "Ar0886", "Ar0887", "Ar0888", "Ar0889", "Ar0890", "Ar0891", "Ar0892", "Ar0893", "Ar0894", "Ar0895", "Ar0896", "Ar0897", "Ar0898", "Ar0899", "Ar0900", "Ar0901", "Ar0902", "Ar0903", "Ar0904", "Ar0905", "Ar0906", "Ar0907", "Ar0908", "Ar0909", "Ar0910", "Ar0911", "Ar0912", "Ar0913", "Ar0914", "Ar0915", "Ar0916"])
# Modification de la position des sommets topologiques Som0314 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0314"], True, 6.12, True, 2.76, False, 0)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Ar0168
ctx.getTopoManager().setGeomAssociation (["Ar0168"], "Surf0014", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Ar0171 Ar0173 Ar0177
ctx.getTopoManager().setGeomAssociation (["Ar0171", "Ar0173", "Ar0177"], "Surf0014", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Ar0166 Ar0184 Ar0374 Ar0180
ctx.getTopoManager().setGeomAssociation (["Ar0166", "Ar0184", "Ar0374", "Ar0180"], "Surf0014", True)
# Affectation d'une projection vers Surf0000 pour les entités topologiques Ar0636 Ar0608 Ar0164 Ar0160 Ar0626 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0636", "Ar0608", "Ar0164", "Ar0160", "Ar0626", "Ar0603", "Ar0156", "Ar0898", "Ar0152", "Ar0893"], "Surf0000", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Ar0186
ctx.getTopoManager().setGeomAssociation (["Ar0186"], "Surf0014", True)
# Fusion entre arêtes Ar0400 et Ar0051
ctx.getTopoManager().fuse2Edges ("Ar0400","Ar0051")
# Fusion entre arêtes Ar0215 et Ar0421
ctx.getTopoManager().fuse2Edges ("Ar0215","Ar0421")
# Fusion entre arêtes Ar0429 et Ar0372
ctx.getTopoManager().fuse2Edges ("Ar0429","Ar0372")
# Fusion entre arêtes Ar0373 et Ar0428
ctx.getTopoManager().fuse2Edges ("Ar0373","Ar0428")
# Fusion entre arêtes Ar0441 et Ar0455
ctx.getTopoManager().fuse2Edges ("Ar0441","Ar0455")
# Fusion entre arêtes Ar0454 et Ar0442
ctx.getTopoManager().fuse2Edges ("Ar0454","Ar0442")
# Fusion entre arêtes Ar0648 et Ar0655
ctx.getTopoManager().fuse2Edges ("Ar0648","Ar0655")
# Fusion entre arêtes Ar0647 et Ar0656
ctx.getTopoManager().fuse2Edges ("Ar0647","Ar0656")
# Fusion entre arêtes Ar0033 et Ar0039
ctx.getTopoManager().fuse2Edges ("Ar0033","Ar0039")
# Fusion entre arêtes Ar0040 et Ar0464
ctx.getTopoManager().fuse2Edges ("Ar0040","Ar0464")
# Fusion entre arêtes Ar0651 et Ar0639
ctx.getTopoManager().fuse2Edges ("Ar0651","Ar0639")
# Fusion entre arêtes Ar0650 et Ar0640
ctx.getTopoManager().fuse2Edges ("Ar0650","Ar0640")
# Fusion entre arêtes Ar0437 et Ar0417
ctx.getTopoManager().fuse2Edges ("Ar0437","Ar0417")
# Fusion entre arêtes Ar0445 et Ar0425
ctx.getTopoManager().fuse2Edges ("Ar0445","Ar0425")
# Fusion entre arêtes Ar0444 et Ar0426
ctx.getTopoManager().fuse2Edges ("Ar0444","Ar0426")
# Fusion entre arêtes Ar0385 et Ar0460
ctx.getTopoManager().fuse2Edges ("Ar0385","Ar0460")
# Fusion entre arêtes Ar0606 et Ar0397
ctx.getTopoManager().fuse2Edges ("Ar0606","Ar0397")
# Fusion entre arêtes Ar0634 et Ar0642
ctx.getTopoManager().fuse2Edges ("Ar0634","Ar0642")
# Fusion entre arêtes Ar0635 et Ar0643
ctx.getTopoManager().fuse2Edges ("Ar0635","Ar0643")
# Fusion entre arêtes Ar0146 et Ar0164
ctx.getTopoManager().fuse2Edges ("Ar0146","Ar0164")
# Fusion entre arêtes Ar0066 et Ar0162
ctx.getTopoManager().fuse2Edges ("Ar0066","Ar0162")
# Fusion entre arêtes Ar0604 et Ar0612
ctx.getTopoManager().fuse2Edges ("Ar0604","Ar0612")
# Fusion entre arêtes Ar0011 et Ar0067
ctx.getTopoManager().fuse2Edges ("Ar0011","Ar0067")
# Fusion entre arêtes Ar0630 et Ar0632
ctx.getTopoManager().fuse2Edges ("Ar0630","Ar0632")
# Fusion entre arêtes Ar0629 et Ar0633
ctx.getTopoManager().fuse2Edges ("Ar0629","Ar0633")
# Fusion entre arêtes Ar0013 et Ar0143
ctx.getTopoManager().fuse2Edges ("Ar0013","Ar0143")
# Fusion entre arêtes Ar0138 et Ar0160
ctx.getTopoManager().fuse2Edges ("Ar0138","Ar0160")
# Fusion entre arêtes Ar0625 et Ar0628
ctx.getTopoManager().fuse2Edges ("Ar0625","Ar0628")
# Fusion entre arêtes Ar0624 et Ar0627
ctx.getTopoManager().fuse2Edges ("Ar0624","Ar0627")
# Fusion entre arêtes Ar0599 et Ar0609
ctx.getTopoManager().fuse2Edges ("Ar0599","Ar0609")
# Fusion entre arêtes Ar0062 et Ar0158
ctx.getTopoManager().fuse2Edges ("Ar0062","Ar0158")
# Fusion entre arêtes Ar0594 et Ar0601
ctx.getTopoManager().fuse2Edges ("Ar0594","Ar0601")
# Fusion entre arêtes Ar0617 et Ar0622
ctx.getTopoManager().fuse2Edges ("Ar0617","Ar0622")
# Fusion entre arêtes Ar0616 et Ar0623
ctx.getTopoManager().fuse2Edges ("Ar0616","Ar0623")
# Fusion entre arêtes Ar0057 et Ar0597
ctx.getTopoManager().fuse2Edges ("Ar0057","Ar0597")
# Fusion entre arêtes Ar0563 et Ar0576
ctx.getTopoManager().fuse2Edges ("Ar0563","Ar0576")
# Fusion entre arêtes Ar0596 et Ar0219
ctx.getTopoManager().fuse2Edges ("Ar0596","Ar0219")
# Fusion entre arêtes Ar0222 et Ar0488
ctx.getTopoManager().fuse2Edges ("Ar0222","Ar0488")
# Fusion entre arêtes Ar0348 et Ar0375
ctx.getTopoManager().fuse2Edges ("Ar0348","Ar0375")
# Fusion entre arêtes Ar0347 et Ar0376
ctx.getTopoManager().fuse2Edges ("Ar0347","Ar0376")
# Fusion entre arêtes Ar0592 et Ar0589
ctx.getTopoManager().fuse2Edges ("Ar0592","Ar0589")
# Fusion entre arêtes Ar0584 et Ar0588
ctx.getTopoManager().fuse2Edges ("Ar0584","Ar0588")
# Fusion entre arêtes Ar0901 et Ar0904
ctx.getTopoManager().fuse2Edges ("Ar0901","Ar0904")
# Fusion entre arêtes Ar0914 et Ar0907
ctx.getTopoManager().fuse2Edges ("Ar0914","Ar0907")
# Fusion entre arêtes Ar0899 et Ar0866
ctx.getTopoManager().fuse2Edges ("Ar0899","Ar0866")
# Fusion entre arêtes Ar0615 et Ar0861
ctx.getTopoManager().fuse2Edges ("Ar0615","Ar0861")
# Fusion entre arêtes Ar0577 et Ar0868
ctx.getTopoManager().fuse2Edges ("Ar0577","Ar0868")
# Fusion entre arêtes Ar0549 et Ar0869
ctx.getTopoManager().fuse2Edges ("Ar0549","Ar0869")
# Fusion entre arêtes Ar0873 et Ar0900
ctx.getTopoManager().fuse2Edges ("Ar0873","Ar0900")
# Fusion entre arêtes Ar0876 et Ar0538
ctx.getTopoManager().fuse2Edges ("Ar0876","Ar0538")
# Fusion entre arêtes Ar0545 et Ar0875
ctx.getTopoManager().fuse2Edges ("Ar0545","Ar0875")
# Fusion entre arêtes Ar0895 et Ar0859
ctx.getTopoManager().fuse2Edges ("Ar0895","Ar0859")
# Fusion entre arêtes Ar0894 et Ar0871
ctx.getTopoManager().fuse2Edges ("Ar0894","Ar0871")
# Fusion entre arêtes Ar0154 et Ar0546
ctx.getTopoManager().fuse2Edges ("Ar0154","Ar0546")
# Fusion entre arêtes Ar0126 et Ar0156
ctx.getTopoManager().fuse2Edges ("Ar0126","Ar0156")
# Changement de discrétisation pour Ar0903
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0903")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0903", Mgx3D.Point(10.8, 8.94675e-1, 0), .5)
# Changement de discrétisation pour Ar0965
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0965")
# Fusion entre arêtes Ar0967 et Ar0519
ctx.getTopoManager().fuse2Edges ("Ar0967","Ar0519")
# Changement de discrétisation pour Ar0964
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0964")
# Fusion entre arêtes Ar0968 et Ar0492
ctx.getTopoManager().fuse2Edges ("Ar0968","Ar0492")
# Fusion entre arêtes Ar0494 et Ar0281
ctx.getTopoManager().fuse2Edges ("Ar0494","Ar0281")
# Fusion entre arêtes Ar0540 et Ar0284
ctx.getTopoManager().fuse2Edges ("Ar0540","Ar0284")
# Fusion entre arêtes Ar0285 et Ar0543
ctx.getTopoManager().fuse2Edges ("Ar0285","Ar0543")
# Fusion entre arêtes Ar0880 et Ar0896
ctx.getTopoManager().fuse2Edges ("Ar0880","Ar0896")
# Fusion entre arêtes Ar0881 et Ar0897
ctx.getTopoManager().fuse2Edges ("Ar0881","Ar0897")
# Fusion entre arêtes Ar0531 et Ar0536
ctx.getTopoManager().fuse2Edges ("Ar0531","Ar0536")
# Fusion entre arêtes Ar0544 et Ar0233
ctx.getTopoManager().fuse2Edges ("Ar0544","Ar0233")
# Fusion entre arêtes Ar0327 et Ar0289
ctx.getTopoManager().fuse2Edges ("Ar0327","Ar0289")
# Fusion entre arêtes Ar0328 et Ar0293
ctx.getTopoManager().fuse2Edges ("Ar0328","Ar0293")
# Fusion entre arêtes Ar0470 et Ar0474
ctx.getTopoManager().fuse2Edges ("Ar0470","Ar0474")
# Fusion entre arêtes Ar0467 et Ar0532
ctx.getTopoManager().fuse2Edges ("Ar0467","Ar0532")
# Changement de discrétisation pour Ar0276
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0276")
# Changement de discrétisation pour Ar0277
emp = Mgx3D.EdgeMeshingPropertyUniform(36)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0277")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0276", .475, .5)
# Changement de discrétisation pour Ar0979
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0979")
# Changement de discrétisation pour Ar0978
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0978")
# Fusion entre arêtes Ar0320 et Ar0973
ctx.getTopoManager().fuse2Edges ("Ar0320","Ar0973")
# Fusion entre arêtes Ar0311 et Ar0972
ctx.getTopoManager().fuse2Edges ("Ar0311","Ar0972")
# Fusion entre arêtes Ar0322 et Ar0338
ctx.getTopoManager().fuse2Edges ("Ar0322","Ar0338")
# Fusion entre arêtes Ar0315 et Ar0304
ctx.getTopoManager().fuse2Edges ("Ar0315","Ar0304")
# Fusion entre arêtes Ar0314 et Ar0305
ctx.getTopoManager().fuse2Edges ("Ar0314","Ar0305")
# Fusion entre arêtes Ar0331 et Ar0336
ctx.getTopoManager().fuse2Edges ("Ar0331","Ar0336")
# Fusion entre arêtes Ar0246 et Ar0302
ctx.getTopoManager().fuse2Edges ("Ar0246","Ar0302")
# Fusion entre arêtes Ar0239 et Ar0295
ctx.getTopoManager().fuse2Edges ("Ar0239","Ar0295")
# Fusion entre arêtes Ar0500 et Ar0533
ctx.getTopoManager().fuse2Edges ("Ar0500","Ar0533")
# Fusion entre arêtes Ar0477 et Ar0501
ctx.getTopoManager().fuse2Edges ("Ar0477","Ar0501")
# Fusion entre arêtes Ar0005 et Ar0499
ctx.getTopoManager().fuse2Edges ("Ar0005","Ar0499")
# Fusion entre arêtes Ar0480 et Ar0484
ctx.getTopoManager().fuse2Edges ("Ar0480","Ar0484")
# Fusion entre arêtes Ar0479 et Ar0485
ctx.getTopoManager().fuse2Edges ("Ar0479","Ar0485")
# Fusion entre arêtes Ar0478 et Ar0525
ctx.getTopoManager().fuse2Edges ("Ar0478","Ar0525")
# Fusion entre arêtes Ar0502 et Ar0522
ctx.getTopoManager().fuse2Edges ("Ar0502","Ar0522")
# Fusion entre arêtes Ar0698 et Ar0297
ctx.getTopoManager().fuse2Edges ("Ar0698","Ar0297")
# Fusion entre arêtes Ar0300 et Ar0724
ctx.getTopoManager().fuse2Edges ("Ar0300","Ar0724")
# Fusion entre arêtes Ar0750 et Ar0334
ctx.getTopoManager().fuse2Edges ("Ar0750","Ar0334")
# Fusion entre arêtes Ar0776 et Ar0341
ctx.getTopoManager().fuse2Edges ("Ar0776","Ar0341")
# Fusion entre arêtes Ar0802 et Ar0340
ctx.getTopoManager().fuse2Edges ("Ar0802","Ar0340")
# Fusion entre arêtes Ar0828 et Ar0307
ctx.getTopoManager().fuse2Edges ("Ar0828","Ar0307")
# Fusion entre arêtes Ar0829 et Ar0306
ctx.getTopoManager().fuse2Edges ("Ar0829","Ar0306")
# Fusion entre arêtes Ar0528 et Ar0512
ctx.getTopoManager().fuse2Edges ("Ar0528","Ar0512")
# Fusion entre arêtes Ar0886 et Ar0891
ctx.getTopoManager().fuse2Edges ("Ar0886","Ar0891")
# Fusion entre arêtes Ar0885 et Ar0892
ctx.getTopoManager().fuse2Edges ("Ar0885","Ar0892")
# Fusion entre arêtes Ar0118 et Ar0152
ctx.getTopoManager().fuse2Edges ("Ar0118","Ar0152")
# Fusion entre arêtes Ar0150 et Ar0526
ctx.getTopoManager().fuse2Edges ("Ar0150","Ar0526")
# Fusion entre arêtes Ar0504 et Ar0524
ctx.getTopoManager().fuse2Edges ("Ar0504","Ar0524")
# Fusion entre arêtes Ar0658 et Ar0669
ctx.getTopoManager().fuse2Edges ("Ar0658","Ar0669")
# Fusion entre arêtes Ar0668 et Ar0659
ctx.getTopoManager().fuse2Edges ("Ar0668","Ar0659")
# Fusion entre arêtes Ar0678 et Ar0686
ctx.getTopoManager().fuse2Edges ("Ar0678","Ar0686")
# Fusion entre arêtes Ar0689 et Ar0679
ctx.getTopoManager().fuse2Edges ("Ar0689","Ar0679")
# Fusion entre arêtes Ar0674 et Ar0695
ctx.getTopoManager().fuse2Edges ("Ar0674","Ar0695")
# Affectation d'une projection vers Crb0076 pour les entités topologiques Ar0694 Ar0693 Ar0690 Ar0683
ctx.getTopoManager().setGeomAssociation (["Ar0694", "Ar0693", "Ar0690", "Ar0683"], "Crb0076", True)
# Affectation d'une projection vers Crb0075 pour les entités topologiques Ar0678 Ar0689 Ar0673 Ar0674
ctx.getTopoManager().setGeomAssociation (["Ar0678", "Ar0689", "Ar0673", "Ar0674"], "Crb0075", True)
# Affectation d'une projection vers Crb0075 pour les entités topologiques Ar0668 Ar0658 Ar0513 Ar0528
ctx.getTopoManager().setGeomAssociation (["Ar0668", "Ar0658", "Ar0513", "Ar0528"], "Crb0075", True)
# Affectation d'une projection vers Crb0067 pour les entités topologiques Ar0671 Ar0670
ctx.getTopoManager().setGeomAssociation (["Ar0671", "Ar0670"], "Crb0067", True)
# Affectation d'une projection vers Crb0026 pour les entités topologiques Ar0889 Ar0890 Ar0025
ctx.getTopoManager().setGeomAssociation (["Ar0889", "Ar0890", "Ar0025"], "Crb0026", True)
# Affectation d'une projection vers Crb0025 pour les entités topologiques Ar0116 Ar0483 Ar0482 Ar0472 Ar0128 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0116", "Ar0483", "Ar0482", "Ar0472", "Ar0128", "Ar0551", "Ar0575", "Ar0574", "Ar0141", "Ar0136", "Ar0148", "Ar0458", "Ar0463", "Ar0462"], "Crb0025", True)
# Affectation d'une projection vers Crb0055 pour les entités topologiques Ar0150
ctx.getTopoManager().setGeomAssociation (["Ar0150"], "Crb0055", True)
# Affectation d'une projection vers Crb0062 pour les entités topologiques Ar0894 Ar0895
ctx.getTopoManager().setGeomAssociation (["Ar0894", "Ar0895"], "Crb0062", True)
# Affectation d'une projection vers Crb0060 pour les entités topologiques Ar0154
ctx.getTopoManager().setGeomAssociation (["Ar0154"], "Crb0060", True)
# Affectation d'une projection vers Crb0018 pour les entités topologiques Ar0227 Ar0544
ctx.getTopoManager().setGeomAssociation (["Ar0227", "Ar0544"], "Crb0018", True)
# Affectation d'une projection vers Crb0017 pour les entités topologiques Ar0285 Ar0540
ctx.getTopoManager().setGeomAssociation (["Ar0285", "Ar0540"], "Crb0017", True)
# Affectation d'une projection vers Crb0063 pour les entités topologiques Ar0549 Ar0577 Ar0563
ctx.getTopoManager().setGeomAssociation (["Ar0549", "Ar0577", "Ar0563"], "Crb0063", True)
# Affectation d'une projection vers Crb0079 pour les entités topologiques Ar0545 Ar0876 Ar0979 Ar0978
ctx.getTopoManager().setGeomAssociation (["Ar0545", "Ar0876", "Ar0979", "Ar0978"], "Crb0079", True)
# Fusion de courbes avec maj de la topologie
ctx.getGeomManager().joinCurves(["Crb0011", "Crb0021"])
# Fusion de courbes avec maj de la topologie
ctx.getGeomManager().joinCurves(["Crb0015", "Crb0082"])
# Affectation d'une projection vers Crb0083 pour les entités topologiques Ar0975 Ar0517 Ar0976 Ar0915 Ar0914 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0975", "Ar0517", "Ar0976", "Ar0915", "Ar0914", "Ar0584"], "Crb0083", True)
# Affectation d'une projection vers Crb0012 pour les entités topologiques Ar0905 Ar0901 Ar0592
ctx.getTopoManager().setGeomAssociation (["Ar0905", "Ar0901", "Ar0592"], "Crb0012", True)
# Affectation d'une projection vers Crb0071 pour les entités topologiques Ar0940 Ar1004 Ar1005 Ar0057
ctx.getTopoManager().setGeomAssociation (["Ar0940", "Ar1004", "Ar1005", "Ar0057"], "Crb0071", True)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Ar0182 Ar1006 Ar0943 Ar1007
ctx.getTopoManager().setGeomAssociation (["Ar0182", "Ar1006", "Ar0943", "Ar1007"], "Crb0072", True)
# Affectation d'une projection vers Crb0054 pour les entités topologiques Ar0594 Ar0616 Ar0617
ctx.getTopoManager().setGeomAssociation (["Ar0594", "Ar0616", "Ar0617"], "Crb0054", True)
# Affectation d'une projection vers Crb0052 pour les entités topologiques Ar0062
ctx.getTopoManager().setGeomAssociation (["Ar0062"], "Crb0052", True)
# Affectation d'une projection vers Crb0072 pour les entités topologiques Ar0179 Ar0175 Ar0176 Ar0170 Ar0999 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0179", "Ar0175", "Ar0176", "Ar0170", "Ar0999", "Ar0998", "Ar0934", "Ar0369", "Ar0370"], "Crb0072", True)
# Affectation d'une projection vers Crb0070 pour les entités topologiques Ar0400 Ar1001 Ar1002 Ar0938 Ar0429 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0400", "Ar1001", "Ar1002", "Ar0938", "Ar0429", "Ar0373"], "Crb0070", True)
# Affectation d'une projection vers Crb0051 pour les entités topologiques Ar0599 Ar0624 Ar0625
ctx.getTopoManager().setGeomAssociation (["Ar0599", "Ar0624", "Ar0625"], "Crb0051", True)
# Affectation d'une projection vers Crb0050 pour les entités topologiques Ar0013
ctx.getTopoManager().setGeomAssociation (["Ar0013"], "Crb0050", True)
# Affectation d'une projection vers Crb0049 pour les entités topologiques Ar0604 Ar0630 Ar0629
ctx.getTopoManager().setGeomAssociation (["Ar0604", "Ar0630", "Ar0629"], "Crb0049", True)
# Affectation d'une projection vers Crb0046 pour les entités topologiques Ar0606 Ar0634 Ar0635
ctx.getTopoManager().setGeomAssociation (["Ar0606", "Ar0634", "Ar0635"], "Crb0046", True)
# Affectation d'une projection vers Crb0045 pour les entités topologiques Ar0385 Ar0384
ctx.getTopoManager().setGeomAssociation (["Ar0385", "Ar0384"], "Crb0045", True)
# Affectation d'une projection vers Crb0002 pour les entités topologiques Ar0040
ctx.getTopoManager().setGeomAssociation (["Ar0040"], "Crb0002", True)
# Affectation d'une projection vers Crb0001 pour les entités topologiques Ar0033 Ar0647 Ar0648 Ar1018 Ar1019 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0033", "Ar0647", "Ar0648", "Ar1018", "Ar1019", "Ar0955", "Ar0441", "Ar0454"], "Crb0001", True)
# Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0452 Ar0453 Ar0958 Ar1022 Ar1023 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0452", "Ar0453", "Ar0958", "Ar1022", "Ar1023", "Ar0653", "Ar0654", "Ar0035"], "Crb0000", True)
# Affectation d'une projection vers Crb0004 pour les entités topologiques Ar0651 Ar0650 Ar1013 Ar0948 Ar1012 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0651", "Ar0650", "Ar1013", "Ar0948", "Ar1012", "Ar0444", "Ar0445"], "Crb0004", True)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(11.2, 1.39697677676464, 0), Mgx3D.Point(1.14999999e1, 1.35, -1e-7), ["Som0435"])
# Modification de la position des sommets topologiques Som0435 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0435"], True, 11.2872, True, 1.38332, True, -2.9065e-8)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(9.89252527089106, 7.91811571064405e-1, 0), Mgx3D.Point(1.024389298e1, 9.0999998539998e-1, 0), ["Som0425"])
# Modification de la position des sommets topologiques Som0425 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0425"], True, 9.99505, True, 8.64349e-1, True, 0)
# Modification de la position des sommets topologiques Som0448 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0448"], True, 9.62247, True, 1.34663, True, 0)
# Modification de la position des sommets topologiques Som0447 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0447"], True, 9.72351, True, 1.40098, True, 0)
# Modifie le groupe AIR
ctx.getTopoManager().removeFromGroup (["Fa0407", "Fa0405", "Fa0409", "Fa0406", "Fa0408"], 2, "AIR")
# Affectation d'une projection vers Surf0007 pour les entités topologiques Fa0405 Fa0407 Fa0406 Fa0408 Fa0409
ctx.getTopoManager().setGeomAssociation (["Fa0405", "Fa0407", "Fa0406", "Fa0408", "Fa0409"], "Surf0007", True)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().removeFromGroup (["Fa0468", "Fa0467", "Fa0232"], 2, "COMPOSITE")
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0232 Fa0467 Fa0468
ctx.getTopoManager().setGeomAssociation (["Fa0232", "Fa0467", "Fa0468"], "Surf0014", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0258 Fa0423 Fa0424
ctx.getTopoManager().setGeomAssociation (["Fa0258", "Fa0423", "Fa0424"], "Surf0014", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0123 Fa0466 Fa0464 Fa0122 Fa0463 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0123", "Fa0466", "Fa0464", "Fa0122", "Fa0463", "Fa0465"], "Surf0011", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0131 Fa0132 Fa0109 Fa0110
ctx.getTopoManager().setGeomAssociation (["Fa0131", "Fa0132", "Fa0109", "Fa0110"], "Surf0011", True)
# Affectation d'une projection vers Surf0005 pour les entités topologiques Fa0135 Fa0137 Fa0153 Fa0155 Fa0154
ctx.getTopoManager().setGeomAssociation (["Fa0135", "Fa0137", "Fa0153", "Fa0155", "Fa0154"], "Surf0005", True)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().removeFromGroup (["Fa0417", "Fa0418", "Fa0419", "Fa0420", "Fa0263", "Fa0273", "Fa0274", "Fa0265", "Fa0277", "Fa0278"], 2, "COMPOSITE")
# Affectation d'une projection vers Surf0000 pour les entités topologiques Fa0265 Fa0277 Fa0278 Fa0273 Fa0274 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0265", "Fa0277", "Fa0278", "Fa0273", "Fa0274", "Fa0263", "Fa0419", "Fa0420", "Fa0418", "Fa0417"], "Surf0000", True)
# Affectation d'une projection vers Surf0009 pour les entités topologiques Fa0281 Fa0183
ctx.getTopoManager().setGeomAssociation (["Fa0281", "Fa0183"], "Surf0009", True)
# Affectation d'une projection vers Surf0013 pour les entités topologiques Fa0313 Fa0319
ctx.getTopoManager().setGeomAssociation (["Fa0313", "Fa0319"], "Surf0013", True)
# Affectation d'une projection vers Surf0013 pour les entités topologiques Fa0307
ctx.getTopoManager().setGeomAssociation (["Fa0307"], "Surf0013", True)
# Affectation d'une projection vers Crb0068 pour les entités topologiques Ar0718 Ar0744 Ar0770 Ar0796 Ar0822 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0718", "Ar0744", "Ar0770", "Ar0796", "Ar0822", "Ar0848", "Ar0847"], "Crb0068", True)
# Affectation d'une projection vers Crb0069 pour les entités topologiques Ar0676 Ar0681 Ar0675 Ar0661 Ar0680 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0676", "Ar0681", "Ar0675", "Ar0661", "Ar0680", "Ar0660", "Ar0514", "Ar0515"], "Crb0069", True)
# Affectation d'une projection vers Crb0038 pour les entités topologiques Ar0828 Ar0829
ctx.getTopoManager().setGeomAssociation (["Ar0828", "Ar0829"], "Crb0038", True)
# Affectation d'une projection vers Crb0039 pour les entités topologiques Ar0802 Ar0776 Ar0750 Ar0698
ctx.getTopoManager().setGeomAssociation (["Ar0802", "Ar0776", "Ar0750", "Ar0698"], "Crb0039", True)
# Fusion entre arêtes Ar0312 et Ar0265
ctx.getTopoManager().fuse2Edges ("Ar0312","Ar0265")
# Affectation d'une projection vers Crb0012 pour les entités topologiques Ar0905
ctx.getTopoManager().setGeomAssociation (["Ar0905"], "Crb0012", True)
# Affectation d'une projection vers Crb0079 pour les entités topologiques Ar0967
ctx.getTopoManager().setGeomAssociation (["Ar0967"], "Crb0079", True)
# Fusion entre arêtes Ar0273 et Ar0517
ctx.getTopoManager().fuse2Edges ("Ar0273","Ar0517")
# Affectation d'une projection vers Crb0044 pour les entités topologiques Ar0249 Ar0242 Ar0258 Ar0257 Ar0911 ... 
ctx.getTopoManager().setGeomAssociation (["Ar0249", "Ar0242", "Ar0258", "Ar0257", "Ar0911", "Ar0912", "Ar0580", "Ar0192", "Ar0091", "Ar0064", "Ar0069", "Ar0068", "Ar0082", "Ar0187"], "Crb0044", True)
# Affectation d'une projection vers Crb0034 pour les entités topologiques Ar0186
ctx.getTopoManager().setGeomAssociation (["Ar0186"], "Crb0034", True)
# Affectation d'une projection vers Crb0035 pour les entités topologiques Ar0395 Ar0382
ctx.getTopoManager().setGeomAssociation (["Ar0395", "Ar0382"], "Crb0035", True)
# Affectation d'une projection vers Crb0003 pour les entités topologiques Ar0038 Ar0037
ctx.getTopoManager().setGeomAssociation (["Ar0038", "Ar0037"], "Crb0003", True)
# Affectation d'une projection vers Crb0005 pour les entités topologiques Ar0023
ctx.getTopoManager().setGeomAssociation (["Ar0023"], "Crb0005", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Ar0312
ctx.getTopoManager().setGeomAssociation (["Ar0312"], "Surf0011", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Ar0328
ctx.getTopoManager().setGeomAssociation (["Ar0328"], "Surf0011", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Ar0222
ctx.getTopoManager().setGeomAssociation (["Ar0222"], "Surf0014", True)
# Affectation d'une projection vers Crb0012 pour les entités topologiques Ar0905
ctx.getTopoManager().setGeomAssociation (["Ar0905"], "Crb0012", True)
# Affectation d'une projection vers Crb0083 pour les entités topologiques Ar0906
ctx.getTopoManager().setGeomAssociation (["Ar0906"], "Crb0083", True)
# Affectation d'une projection vers Pt0049 pour les entités topologiques Som0237
ctx.getTopoManager().setGeomAssociation (["Som0237"], "Pt0049", True)
# Modification de la position des sommets topologiques Som0339 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0339"], True, 16.4248, True, 1.28799, True, 0)
# Modification de la position des sommets topologiques Som0338 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0338"], True, 15.6034, True, 2.26882, True, 0)
# Modification de la position des sommets topologiques Som0339 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0339"], True, 16.4203, True, 1.27682, True, 0)
# Modification de la position des sommets topologiques Som0340 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0340"], True, 17.45, True, 8.02952e-1, True, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(16.4203, 1.27682, 0), Mgx3D.Point(1.6396499957e1, 0, 0), ["Som0375", "Som0384", "Som0366", "Som0357", "Som0348"])
# Modification de la position des sommets topologiques Som0321 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0321"], True, 16.3199, True, 0, True, 0)
# Modification de la position des sommets topologiques Som0249 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0249"], True, 15.2816, True, 0, True, 0)
# Modification de la position des sommets topologiques Som0337 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0337"], True, 15.407, True, 2.36317, True, -6.41201e-5)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(15.407, 2.36317, -6.41201e-5), Mgx3D.Point(15.2816, 0, 0), ["Som0382", "Som0373", "Som0364", "Som0355", "Som0346"])
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(15.6034, 2.26882, 0), Mgx3D.Point(1.5514499921e1, 0, 0), ["Som0383", "Som0374", "Som0365", "Som0356", "Som0347"])
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(16.4203, 1.27682, 0), Mgx3D.Point(16.3199, 0, 0), ["Som0384", "Som0375", "Som0366", "Som0357", "Som0348"])
# Modification de la position des sommets topologiques Som0329 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0329"], True, 18.147, True, 0, True, 0)
# Modification de la position des sommets topologiques Som0344 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0344"], True, 18.111, True, 1.00011, True, 0)
# Modification de la position des sommets topologiques Som0344 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0344"], True, 18.1149, True, 9.90479e-1, True, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(18.1149, 9.90479e-1, 0), Mgx3D.Point(18.147, 0, 0), ["Som0389", "Som0380", "Som0371", "Som0362", "Som0353"])
# Modification de la position des sommets topologiques Som0341 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0341"], True, 18.7207, True, 1.55609, True, 0)
# Modification de la position des sommets topologiques Som0323 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0323"], True, 18.7333, True, 0, True, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(18.7207, 1.55609, 0), Mgx3D.Point(18.7333, 0, 0), ["Som0386", "Som0377", "Som0368", "Som0359", "Som0350"])
# Destruction des entités topologiques Fa0283 Fa0284 Fa0492 Fa0491 Fa0451 ... 
ctx.getTopoManager().destroy (["Fa0283", "Fa0284", "Fa0492", "Fa0491", "Fa0451", "Fa0204", "Fa0203"], True)
# Création d'une face topologique structurée sans projection (Surf0010)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0010")
# Translation de la topologie seule Ar1034
ctx.getTopoManager().translate (["Ar1034"], Mgx3D.Vector(-.2, 0, 0), False)
# Modification de la position des sommets topologiques Som0456 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0456"], True, 1.15, True, 1.85, False, 0)
# Découpage en O-grid des faces structurées Fa0503
ctx.getTopoManager().splitFacesWithOgrid (["Fa0503"], ["Ar1033", "Ar1034"], .4, 10)
# Changement de discrétisation pour Ar1036
emp = Mgx3D.EdgeMeshingPropertyUniform(20)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1036")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar1036", .35, .5)
# Changement de discrétisation pour Ar1045
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1045")
# Changement de discrétisation pour Ar1046
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1046")
# Destruction des entités topologiques Fa0209 Fa0210 Fa0455 Fa0496 Fa0495 ... 
ctx.getTopoManager().destroy (["Fa0209", "Fa0210", "Fa0455", "Fa0496", "Fa0495", "Fa0287", "Fa0288"], True)
# Création d'un bloc unitaire mis dans le groupe COMPOSITE
ctx.getTopoManager().newFreeTopoInGroup ("COMPOSITE", 2)
# Modification de la position des sommets topologiques Som0469 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0469"], True, 1.15, True, 1.85, False, 0)
# Modification de la position des sommets topologiques Som0468 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0468"], True, 1, True, 2, False, 0)
# Modification de la position des sommets topologiques Som0467 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0467"], True, 0, True, 0, False, 0)
# Modification de la position des sommets topologiques Som0466 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0466"], True, .2, True, 0, False, 0)
# Fusion entre arêtes Ar1032 et Ar1054
ctx.getTopoManager().fuse2Edges ("Ar1032","Ar1054")
# Affectation d'une projection vers Crb0000 pour les entités topologiques Ar1052
ctx.getTopoManager().setGeomAssociation (["Ar1052"], "Crb0000", True)
# Affectation d'une projection vers Crb0001 pour les entités topologiques Ar1032
ctx.getTopoManager().setGeomAssociation (["Ar1032"], "Crb0001", True)
# Affectation d'une projection vers Crb0001 pour les entités topologiques Ar1035
ctx.getTopoManager().setGeomAssociation (["Ar1035"], "Crb0001", True)
# Changement de discrétisation pour Ar1039
emp = Mgx3D.EdgeMeshingPropertyUniform(100)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1039")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar1041", Mgx3D.Point(3, 1.42053328406664, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar1055", Mgx3D.Point(3, 8.17599929199969e-1, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar1069", Mgx3D.Point(3, 6.15999921999967e-1, 0), .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar1083", Mgx3D.Point(3, 5.03999917999913e-1, 0), .5)
# Changement de discrétisation pour Ar1085
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1085")
# Changement de discrétisation pour Ar1100
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1100")
# Changement de discrétisation pour Ar1099
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1099")
# Changement de discrétisation pour Ar1071
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1071")
# Changement de discrétisation pour Ar1057
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1057")
# Fusion entre arêtes Ar0646 et Ar1048
ctx.getTopoManager().fuse2Edges ("Ar0646","Ar1048")
# Fusion entre arêtes Ar1049 et Ar0645
ctx.getTopoManager().fuse2Edges ("Ar1049","Ar0645")
# Fusion entre arêtes Ar1017 et Ar1056
ctx.getTopoManager().fuse2Edges ("Ar1017","Ar1056")
# Fusion entre arêtes Ar1016 et Ar1070
ctx.getTopoManager().fuse2Edges ("Ar1016","Ar1070")
# Fusion entre arêtes Ar0952 et Ar1084
ctx.getTopoManager().fuse2Edges ("Ar0952","Ar1084")
# Fusion entre arêtes Ar0440 et Ar1098
ctx.getTopoManager().fuse2Edges ("Ar0440","Ar1098")
# Fusion entre arêtes Ar0439 et Ar1097
ctx.getTopoManager().fuse2Edges ("Ar0439","Ar1097")
# Fusion entre arêtes Ar0033 et Ar1035
ctx.getTopoManager().fuse2Edges ("Ar0033","Ar1035")
# Affectation d'une projection vers Surf0010 pour les entités topologiques Fa0510 Fa0509 Fa0513 Fa0521 Fa0529 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0510", "Fa0509", "Fa0513", "Fa0521", "Fa0529", "Fa0537", "Fa0536"], "Surf0010", True)
# Modifie le groupe COMPOSITE
ctx.getTopoManager().removeFromGroup (["Fa0542", "Fa0543", "Fa0535", "Fa0527", "Fa0519"], 2, "COMPOSITE")
# Affectation d'une projection vers Surf0000 pour les entités topologiques Fa0519 Fa0527 Fa0535 Fa0543 Fa0542
ctx.getTopoManager().setGeomAssociation (["Fa0519", "Fa0527", "Fa0535", "Fa0543", "Fa0542"], "Surf0000", True)
# Modifie le groupe AIR
ctx.getGeomManager().setGroup (["Surf0009"], 2, "AIR")
# Modifie le groupe COMPOSITE
ctx.getTopoManager().removeFromGroup (["Fa0238", "Fa0240", "Fa0131", "Fa0133", "Fa0132", "Fa0241", "Fa0236", "Fa0226", "Fa0234", "Fa0235", "Fa0293", "Fa0294", "Fa0299", "Fa0301", "Fa0303", "Fa0304"], 2, "COMPOSITE")
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0069 Fa0171 Fa0172 Fa0073 Fa0071 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0069", "Fa0171", "Fa0172", "Fa0073", "Fa0071", "Fa0075", "Fa0077"], "Surf0014", True)
# Affectation d'une projection vers Surf0013 pour les entités topologiques Fa0305
ctx.getTopoManager().setGeomAssociation (["Fa0305"], "Surf0013", True)
# Affectation d'une projection vers Surf0014 pour les entités topologiques Fa0232
ctx.getTopoManager().setGeomAssociation (["Fa0232"], "Surf0014", True)
# Affectation d'une projection vers Surf0000 pour les entités topologiques Fa0542 Fa0543 Fa0535 Fa0527 Fa0519
ctx.getTopoManager().setGeomAssociation (["Fa0542", "Fa0543", "Fa0535", "Fa0527", "Fa0519"], "Surf0000", True)
# Modifie le groupe Hors_Groupe_2D
ctx.getTopoManager().removeFromGroup (["Fa0542", "Fa0543", "Fa0535", "Fa0527", "Fa0519"], 2, "Hors_Groupe_2D")
# Modifie le groupe Hors_Groupe_2D
ctx.getTopoManager().removeFromGroup (["Fa0278", "Fa0277", "Fa0265", "Fa0263", "Fa0273", "Fa0274", "Fa0417", "Fa0418"], 2, "Hors_Groupe_2D")
# Modifie le groupe Hors_Groupe_2D
ctx.getTopoManager().removeFromGroup (["Fa0407", "Fa0405", "Fa0409", "Fa0406", "Fa0419", "Fa0408", "Fa0420", "Fa0468", "Fa0467", "Fa0232"], 2, "Hors_Groupe_2D")
# Affectation d'une projection vers Crb0033 pour les entités topologiques Ar0498
ctx.getTopoManager().setGeomAssociation (["Ar0498"], "Crb0033", True)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0690", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0683", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0671", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0570", .5, .5)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0400", .5, .5)
# Modification de la position des sommets topologiques Som0535 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0535"], True, 5.10986, True, 2.5381, True, 0)
# Modification de la position des sommets topologiques Som0525 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0525"], True, 8.60295, True, 2.53115, True, 0)
# Modification de la position des sommets topologiques Som0284 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0284"], True, 9.78586, True, 2.15351, True, 4.48878e-8)
# Modification de la position des sommets topologiques Som0526 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0526"], True, 8.62186, True, 2.67566, True, 0)
# Modification de la position des sommets topologiques Som0525 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0525"], True, 8.56739, True, 2.54893, True, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(1.69010234954368e1, 9.70542391295203e-1, 0), Mgx3D.Point(16.88495, 0, 0), ["Som0513", "Som0514", "Som0515", "Som0512", "Som0511", "Som0510"])
# Homothétie de la topologie seule  Som0515 Som0514 Som0513 Som0512 Som0511 ... 
ctx.getTopoManager().scale (["Som0515", "Som0514", "Som0513", "Som0512", "Som0511", "Som0510"], .75, Mgx3D.Point(16.8946, 5.81562e-1, 0), False)
# Modification de la position des sommets topologiques Som0515 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0515"], True, 16.8889, True, 2.36999e-1, True, 0)
# Modification de la position des sommets topologiques Som0514 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0514"], True, 16.894, True, .54366, True, 0)
# Modification de la position des sommets topologiques Som0513 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0513"], True, 16.8973, True, 7.46059e-1, True, 0)
# Modification de la position des sommets topologiques Som0510 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0510"], True, 16.9002, True, .92057, True, 0)
# Modification de la position des sommets topologiques Som0512 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0512"], True, 16.899, True, 8.46289e-1, True, 0)
# Modification de la position des sommets topologiques Som0512 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0512"], True, 16.8989, True, 8.38271e-1, True, 0)
# Modification de la position des sommets topologiques Som0506 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0506"], True, 17.7946, True, 2.17633e-1, True, 0)
# Modification de la position des sommets topologiques Som0505 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0505"], True, 17.7899, True, 4.84356e-1, True, 0)
# Modification de la position des sommets topologiques Som0504 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0504"], True, 17.7857, True, 7.20624e-1, True, 0)
# Modification de la position des sommets topologiques Som0503 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0503"], True, 17.7839, True, 8.17578e-1, True, 0)
# Modification de la position des sommets topologiques Som0503 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0503"], True, 17.7841, True, 8.05944e-1, True, 0)
# Modification de la position des sommets topologiques Som0502 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0502"], True, 17.7837, True, 8.29717e-1, True, 0)
# Modification de la position des sommets topologiques Som0501 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0501"], True, 17.7842, True, 8.51774e-1, True, 0)
# Modification de la position des sommets topologiques Som0497 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0497"], True, 18.4348, True, 3.05879e-1, True, 0)
# Modification de la position des sommets topologiques Som0496 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0496"], True, 18.4282, True, 6.76988e-1, True, 0)
# Modification de la position des sommets topologiques Som0495 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0495"], True, 18.4227, True, 9.90544e-1, True, 0)
# Modification de la position des sommets topologiques Som0494 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0494"], True, 18.4203, True, 1.1292, True, 0)
# Modification de la position des sommets topologiques Som0493 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0493"], True, 18.4194, True, 1.18177, True, 0)
# Modification de la position des sommets topologiques Som0492 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0492"], True, 18.4192, True, 1.23052, True, 0)
# Modification de la position des sommets topologiques Som0325 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0325"], True, 16.7626, True, 1.61715, True, 0)
# Modification de la position des sommets topologiques Som0508 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0508"], True, 17.1217, True, 1.26866, True, 0)
# Modification de la position des sommets topologiques Som0242 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0242"], True, 17.4186, True, 8.40296e-1, True, 0)
# Modification de la position des sommets topologiques Som0033 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0033"], True, 17.4186, False, 0, False, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(17.4186, 8.40296e-1, 0), Mgx3D.Point(17.4186, 0, 0), ["Som0385", "Som0376", "Som0367", "Som0358", "Som0349", "Som0340"])
# Modification de la position des sommets topologiques Som0516 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0516"], True, 16.8284, True, 0, True, 0)
# Modification de la position des sommets topologiques Som0510 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0510"], True, 16.8791, True, 9.36378e-1, True, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(16.8791, 9.36378e-1, 0), Mgx3D.Point(16.8284, 0, 0), ["Som0515", "Som0514", "Som0513", "Som0512", "Som0511"])
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(1.80943245826608e1, 1.02166323466671, 0), Mgx3D.Point(1.81163785159687e1, 9.44857846784922e-1, 0), ["Som0344"])
# Modification de la position des sommets topologiques Som0504 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0504"], True, 17.792, True, 7.01718e-1, True, 0)
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(17.7985, 0, 0), Mgx3D.Point(17.792, 7.01718e-1, 0), ["Som0506", "Som0505"])
# Aligne des sommets topologiques sur une droite définie par 2 points
ctx.getTopoManager().alignVertices (Mgx3D.Point(1.77842279447542e1, 8.87547841179256e-1, 0), Mgx3D.Point(17.792, 7.01718e-1, 0), ["Som0501", "Som0502", "Som0503"])
# Modification de la position des sommets topologiques Som0503 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0503"], True, 17.7881, True, 7.95654e-1, True, 0)
# Modification de la position des sommets topologiques Som0503 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0503"], True, 17.7885, True, .78626, True, 0)
# Modification de la position des sommets topologiques Som0502 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0502"], True, 17.7873, True, .81328, True, 0)
# Modification de la position des sommets topologiques Som0385 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0385"], True, 17.4186, True, 1.57848e-1, True, 0)
# Modification de la position des sommets topologiques Som0376 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0376"], True, 17.4186, True, 3.76742e-1, True, 0)
# Modification de la position des sommets topologiques Som0367 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0367"], True, 17.4186, True, 5.75913e-1, True, 0)
# Modification de la position des sommets topologiques Som0358 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0358"], True, 17.4186, True, 6.73368e-1, True, 0)
# Modification de la position des sommets topologiques Som0349 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0349"], True, 17.4186, True, 7.20552e-1, True, 0)
# Modification de la position des sommets topologiques Som0340 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0340"], True, 17.4186, True, 7.83755e-1, True, 0)
# Modification de la position des sommets topologiques Som0514 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0514"], True, 16.8556, True, 5.02767e-1, True, 0)
# Modification de la position des sommets topologiques Som0513 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0513"], True, 16.8661, True, 6.96183e-1, True, 0)
# Modification de la position des sommets topologiques Som0512 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0512"], True, 16.872, True, .8052, True, 0)
# Modification de la position des sommets topologiques Som0511 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0511"], True, 16.8744, True, 8.49288e-1, True, 0)
# Modification de la position des sommets topologiques Som0510 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0510"], True, 16.8779, True, 9.14605e-1, True, 0)
# Modification de la position des sommets topologiques Som0505 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0505"], True, 17.7945, True, 4.28378e-1, True, 0)
# Modification de la position des sommets topologiques Som0506 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0506"], True, 17.7967, True, 1.98062e-1, True, 0)
# Modification de la position des sommets topologiques Som0504 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0504"], True, 17.7925, True, .64705, True, 0)
# Modification de la position des sommets topologiques Som0503 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0503"], True, 17.7896, True, 7.47281e-1, True, 0)
# Modification de la position des sommets topologiques Som0502 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0502"], True, 17.788, True, .79348, True, 0)
# Modification de la position des sommets topologiques Som0501 en coordonnées cartésiennes
ctx.getTopoManager().setVertexLocation (["Som0501"], True, 17.7862, True, 8.39583e-1, True, 0)
# Suppression d'entités géométriques et suppression des liens topologiques
ctx.getGeomManager().destroy(["Surf0014"], False)
# Création du segment Crb0098
ctx.getGeomManager().newSegment("Pt0027", "Pt0016")
# Création de la surface Surf0015
ctx.getGeomManager().newPlanarSurface( ["Crb0034", "Crb0072", "Crb0098", "Crb0071", "Crb0052", "Crb0064", "Crb0047", "Crb0070"] , "TITANE")
# Création du sommet Pt0107
ctx.getGeomManager().newVertex (Mgx3D.Point(15, 2.6, 0))
# Suppression d'entités géométriques et suppression des liens topologiques
ctx.getGeomManager().destroy(["Surf0006"], False)
# Suppression d'entités géométriques et suppression des liens topologiques
ctx.getGeomManager().destroy(["Crb0058"], True)
# Création du segment Crb0099
ctx.getGeomManager().newSegment("Pt0068", "Pt0107")
# Création du segment Crb0100
ctx.getGeomManager().newSegment("Pt0107", "Pt0066")
# Création du segment Crb0101
ctx.getGeomManager().newSegment("Pt0107", "Pt0037")
# Création de la surface Surf0016
ctx.getGeomManager().newPlanarSurface( ["Crb0075", "Crb0033", "Crb0076", "Crb0067", "Crb0055", "Crb0099", "Crb0101"] , "")
# Affectation d'une projection vers Surf0016 pour les entités topologiques Fa0234 Fa0235 Fa0293 Fa0576 Fa0577 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0234", "Fa0235", "Fa0293", "Fa0576", "Fa0577", "Fa0560", "Fa0561", "Fa0544", "Fa0545", "Fa0303", "Fa0304"], "Surf0016", True)
# Modifie le groupe TITANE
ctx.getGeomManager().addToGroup (["Surf0016"], 2, "TITANE")
# Création de la surface Surf0017
ctx.getGeomManager().newPlanarSurface( ["Crb0023", "Crb0101", "Crb0100", "Crb0060", "Crb0079", "Crb0012", "Crb0098", "Crb0083", "Crb0017", "Crb0018"] , "TITANE")
# Affectation d'une projection vers Surf0017 pour les entités topologiques Fa0226 Fa0236 Fa0241 Fa0240 Fa0238 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0226", "Fa0236", "Fa0241", "Fa0240", "Fa0238", "Fa0468", "Fa0467", "Fa0232", "Fa0424", "Fa0423", "Fa0258"], "Surf0017", True)
# Affectation d'une projection vers Surf0015 pour les entités topologiques Fa0172 Fa0171 Fa0441 Fa0481 Fa0482 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0172", "Fa0171", "Fa0441", "Fa0481", "Fa0482", "Fa0616", "Fa0617", "Fa0073", "Fa0071", "Fa0075", "Fa0594", "Fa0595", "Fa0483", "Fa0484", "Fa0444"], "Surf0015", True)
# Affectation d'une projection vers Crb0100 pour les entités topologiques Ar0477
ctx.getTopoManager().setGeomAssociation (["Ar0477"], "Crb0100", True)
# Affectation d'une projection vers Crb0100 pour les entités topologiques Ar0467
ctx.getTopoManager().setGeomAssociation (["Ar0467"], "Crb0100", True)
# Création de la surface Surf0018
ctx.getGeomManager().newPlanarSurface( ["Crb0100", "Crb0099", "Crb0056", "Crb0057", "Crb0059"] , "AIR")
# Affectation d'une projection vers Surf0018 pour les entités topologiques Fa0411 Fa0415 Fa0412 Fa0416 Fa0413 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0411", "Fa0415", "Fa0412", "Fa0416", "Fa0413", "Fa0414"], "Surf0018", True)
# Affectation d'une projection vers Crb0012 pour les entités topologiques Ar0905
ctx.getTopoManager().setGeomAssociation (["Ar0905"], "Crb0012", True)
# Suppression d'entités géométriques et suppression des liens topologiques
ctx.getGeomManager().destroy(["Surf0017"], False)
# Création du sommet Pt0108
ctx.getGeomManager().newVertex (Mgx3D.Point(10.8, 2, 0))
# Création du sommet Pt0109
ctx.getGeomManager().newVertex (Mgx3D.Point(11.2, 2.6, 0))
# Création du sommet Pt0110
ctx.getGeomManager().newVertex (Mgx3D.Point(10.8, 2.2, 0))
# Suppression d'entités géométriques et suppression des liens topologiques
ctx.getGeomManager().destroy(["Surf0007"], False)
# Suppression d'entités géométriques et suppression des liens topologiques
ctx.getGeomManager().destroy(["Crb0079"], False)
# Création de l'arc de cercle Crb0102
ctx.getGeomManager().newArcCircle("Pt0035", "Pt0110", "Pt0109", True)
# Création du segment Crb0103
ctx.getGeomManager().newSegment("Pt0109", "Pt0067")
# Création du segment Crb0104
ctx.getGeomManager().newSegment("Pt0110", "Pt0108")
# Création du segment Crb0105
ctx.getGeomManager().newSegment("Pt0108", "Pt0031")
# Création du segment Crb0106
ctx.getGeomManager().newSegment("Pt0108", "Pt0017")
# Création de la surface Surf0019
ctx.getGeomManager().newPlanarSurface( ["Crb0105", "Crb0017", "Crb0018", "Crb0023", "Crb0101", "Crb0100", "Crb0060", "Crb0103", "Crb0102", "Crb0104"] , "TITANE")
# Création de la surface Surf0020
ctx.getGeomManager().newPlanarSurface( ["Crb0105", "Crb0106", "Crb0012", "Crb0098", "Crb0083"] , "TITANE")
# Création de la surface Surf0021
ctx.getGeomManager().newPlanarSurface( ["Crb0012", "Crb0106", "Crb0104", "Crb0102", "Crb0103", "Crb0062", "Crb0063", "Crb0054", "Crb0071"] , "")
# Modifie le groupe AIR
ctx.getGeomManager().addToGroup (["Surf0021"], 2, "AIR")
# Fusion de courbes avec maj de la topologie
ctx.getGeomManager().joinCurves(["Crb0104", "Crb0102", "Crb0103"])
# Affectation d'une projection vers Surf0020 pour les entités topologiques Fa0258 Fa0423 Fa0424 Fa0232 Fa0467 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0258", "Fa0423", "Fa0424", "Fa0232", "Fa0467", "Fa0468"], "Surf0020", True)
# Affectation d'une projection vers Surf0019 pour les entités topologiques Fa0240 Fa0238 Fa0241 Fa0236 Fa0226
ctx.getTopoManager().setGeomAssociation (["Fa0240", "Fa0238", "Fa0241", "Fa0236", "Fa0226"], "Surf0019", True)
# Affectation d'une projection vers Surf0021 pour les entités topologiques Fa0408 Fa0406 Fa0409 Fa0405 Fa0499 ... 
ctx.getTopoManager().setGeomAssociation (["Fa0408", "Fa0406", "Fa0409", "Fa0405", "Fa0499", "Fa0501", "Fa0407", "Fa0498", "Fa0602", "Fa0604", "Fa0606", "Fa0497", "Fa0457", "Fa0500", "Fa0460", "Fa0502", "Fa0462", "Fa0603", "Fa0605", "Fa0607"], "Surf0021", True)
# Affectation d'une projection vers Crb0107 pour les entités topologiques Ar0545 Ar0876
ctx.getTopoManager().setGeomAssociation (["Ar0545", "Ar0876"], "Crb0107", True)
# Affectation d'une projection vers Crb0106 pour les entités topologiques Ar0978 Ar0979 Ar0967
ctx.getTopoManager().setGeomAssociation (["Ar0978", "Ar0979", "Ar0967"], "Crb0106", True)
# Affectation d'une projection vers Crb0012 pour les entités topologiques Ar0905
ctx.getTopoManager().setGeomAssociation (["Ar0905"], "Crb0012", True)
# Fusion entre arêtes Ar0349 et Ar0261
ctx.getTopoManager().fuse2Edges ("Ar0349","Ar0261")
# Fusion entre arêtes Ar0350 et Ar0269
ctx.getTopoManager().fuse2Edges ("Ar0350","Ar0269")
# Fusion entre arêtes Ar0518 et Ar0490
ctx.getTopoManager().fuse2Edges ("Ar0518","Ar0490")
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar0314", .5, .5)
# Destruction des entités topologiques Fa0644 Fa0640
ctx.getTopoManager().destroy (["Fa0644", "Fa0640"], True)
# Création d'une face topologique structurée sans projection (Surf0004)
ctx.getTopoManager().newFreeTopoOnGeometry ("Surf0004")
# Translation de la topologie seule Ar1369
ctx.getTopoManager().translate (["Ar1369"], Mgx3D.Vector(-.375, 0, 0), False)
# Translation de la topologie seule Ar1372
ctx.getTopoManager().translate (["Ar1372"], Mgx3D.Vector(0, -1.0395, 1e-7), False)
# Découpage de toutes les faces 2D structurées
ctx.getTopoManager().splitAllFaces ("Ar1370", Mgx3D.Point(12, .675, 0), .5)
# Changement de discrétisation pour Ar1373
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1373")
# Changement de discrétisation pour Ar1374
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1374")
# Changement de discrétisation pour Ar1377
emp = Mgx3D.EdgeMeshingPropertyUniform(5)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1377")
# Fusion entre arêtes Ar1275 et Ar1375
ctx.getTopoManager().fuse2Edges ("Ar1275","Ar1375")
# Fusion entre arêtes Ar1272 et Ar1369
ctx.getTopoManager().fuse2Edges ("Ar1272","Ar1369")
# Fusion entre arêtes Ar1270 et Ar1371
ctx.getTopoManager().fuse2Edges ("Ar1270","Ar1371")
# Affectation d'une projection vers Surf0004 pour les entités topologiques Fa0710
ctx.getTopoManager().setGeomAssociation (["Fa0710"], "Surf0004", True)
# Affectation d'une projection vers Surf0011 pour les entités topologiques Fa0709
ctx.getTopoManager().setGeomAssociation (["Fa0709"], "Surf0011", True)
# Affectation d'une projection vers Crb0041 pour les entités topologiques Ar1377
ctx.getTopoManager().setGeomAssociation (["Ar1377"], "Crb0041", True)
# Affectation d'une projection vers Crb0044 pour les entités topologiques Ar1373
ctx.getTopoManager().setGeomAssociation (["Ar1373"], "Crb0044", True)
# Destruction des entités topologiques Fa0680 Fa0683
ctx.getTopoManager().destroy (["Fa0680", "Fa0683"], True)
# Création d'un bloc unitaire mis dans le groupe 
ctx.getTopoManager().newFreeTopoInGroup ("", 2)
# Translation de la topologie seule Ar1381
ctx.getTopoManager().translate (["Ar1381"], Mgx3D.Vector(2, 0, 0), False)
# Translation de la topologie seule Ar1379
ctx.getTopoManager().translate (["Ar1379"], Mgx3D.Vector(.494, 0, 0), False)
# Affectation d'une projection vers Surf0010 pour les entités topologiques Fa0711
ctx.getTopoManager().setGeomAssociation (["Fa0711"], "Surf0010", True)
# Translation de la topologie seule Ar1380
ctx.getTopoManager().translate (["Ar1380"], Mgx3D.Vector(4.44089e-16, -.748, 0), False)
# Découpage des faces 2D structurées Fa0711 suivant l'arête Ar1378 avec comme point de départ Mgx3D.Point(1.10792496715, 1.8687499805e-1, 0)
ctx.getTopoManager().splitFaces (["Fa0711"], "Ar1378", Mgx3D.Point(1.10792496715, 1.8687499805e-1, 0), .5)
# Changement de discrétisation pour Ar1386
emp = Mgx3D.EdgeMeshingPropertyUniform(5)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1386")
# Changement de discrétisation pour Ar1385
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1385")
# Changement de discrétisation pour Ar1384
emp = Mgx3D.EdgeMeshingPropertyUniform(10)
ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar1384")
# Translation de la topologie seule Ar1386
ctx.getTopoManager().translate (["Ar1386"], Mgx3D.Vector(8.96837e-2, 0, 0), False)
# Collage entre 2 listes d'arêtes
ctx.getTopoManager().fuse2EdgeList (["Ar1335", "Ar1338", "Ar1337"], ["Ar1385", "Ar1384", "Ar1379"])
# Affectation d'une projection vers Surf0010 pour les entités topologiques Fa0713 Fa0712
ctx.getTopoManager().setGeomAssociation (["Fa0713", "Fa0712"], "Surf0010", True)
# Fusion entre arêtes Ar1381 et Ar1306
ctx.getTopoManager().fuse2Edges ("Ar1381","Ar1306")
# Affectation d'une projection vers Crb0001 pour les entités topologiques Som0014
ctx.getTopoManager().setGeomAssociation (["Som0014"], "Crb0001", True)
# Affectation d'une projection vers Crb0003 pour les entités topologiques Som0588 Som0464
ctx.getTopoManager().setGeomAssociation (["Som0588", "Som0464"], "Crb0003", True)
# Construction Topo et Geom 3D avec o-grid par révolution
ctx.getTopoManager().makeBlocksByRevolWithRatioOgrid (["Ar1335"], Mgx3D.Portion.QUART, 2.000000e-01)
# Changement de discrétisation pour les blocs Bl0000 Bl0001 Bl0002 Bl0003 Bl0004 ... 
ctx.getTopoManager().setMeshingProperty (Mgx3D.BlockMeshingPropertyTransfinite(),["Bl0000", "Bl0001", "Bl0002", "Bl0003", "Bl0004", "Bl0005", "Bl0006", "Bl0007", "Bl0008", "Bl0009", "Bl0010", "Bl0011", "Bl0012", "Bl0013", "Bl0014", "Bl0015", "Bl0016", "Bl0017", "Bl0018", "Bl0019", "Bl0020", "Bl0021", "Bl0022", "Bl0023", "Bl0024", "Bl0025", "Bl0026", "Bl0027", "Bl0028", "Bl0029", "Bl0030", "Bl0031", "Bl0032", "Bl0033", "Bl0034", "Bl0035", "Bl0036", "Bl0037", "Bl0038", "Bl0039", "Bl0040", "Bl0041", "Bl0042", "Bl0043", "Bl0044", "Bl0045", "Bl0046", "Bl0047", "Bl0048", "Bl0049", "Bl0050", "Bl0051", "Bl0052", "Bl0053", "Bl0054", "Bl0055", "Bl0056", "Bl0057", "Bl0058", "Bl0059", "Bl0060", "Bl0061", "Bl0062", "Bl0063", "Bl0064", "Bl0065", "Bl0066", "Bl0067", "Bl0068", "Bl0069", "Bl0070", "Bl0071", "Bl0072", "Bl0073", "Bl0074", "Bl0075", "Bl0076", "Bl0077", "Bl0078", "Bl0079", "Bl0080", "Bl0081", "Bl0082", "Bl0083", "Bl0084", "Bl0085", "Bl0086", "Bl0087", "Bl0088", "Bl0089", "Bl0090", "Bl0091", "Bl0092", "Bl0093", "Bl0094", "Bl0095", "Bl0096", "Bl0097", "Bl0098", "Bl0099", "Bl0100", "Bl0101", "Bl0102", "Bl0103", "Bl0104", "Bl0105", "Bl0106", "Bl0107", "Bl0108", "Bl0109", "Bl0110", "Bl0111", "Bl0112", "Bl0113", "Bl0114", "Bl0115", "Bl0116", "Bl0117", "Bl0118", "Bl0119", "Bl0120", "Bl0121", "Bl0122", "Bl0123", "Bl0124", "Bl0125", "Bl0126", "Bl0127", "Bl0128", "Bl0129", "Bl0130", "Bl0131", "Bl0132", "Bl0133", "Bl0134", "Bl0135", "Bl0136", "Bl0137", "Bl0138", "Bl0139", "Bl0140", "Bl0141", "Bl0142", "Bl0143", "Bl0144", "Bl0145", "Bl0146", "Bl0147", "Bl0148", "Bl0149", "Bl0150", "Bl0151", "Bl0152", "Bl0153", "Bl0154", "Bl0155", "Bl0156", "Bl0157", "Bl0158", "Bl0159", "Bl0160", "Bl0161", "Bl0162", "Bl0163", "Bl0164", "Bl0165", "Bl0166", "Bl0167", "Bl0168", "Bl0169", "Bl0170", "Bl0171", "Bl0172", "Bl0173", "Bl0174", "Bl0175", "Bl0176", "Bl0177", "Bl0178", "Bl0179", "Bl0180", "Bl0181", "Bl0182", "Bl0183", "Bl0184", "Bl0185", "Bl0186", "Bl0187", "Bl0188", "Bl0189", "Bl0190", "Bl0191", "Bl0192", "Bl0193", "Bl0194", "Bl0195", "Bl0196", "Bl0197", "Bl0198", "Bl0199", "Bl0200", "Bl0201", "Bl0202", "Bl0203", "Bl0204", "Bl0205", "Bl0206", "Bl0207", "Bl0208", "Bl0209", "Bl0210", "Bl0211", "Bl0212", "Bl0213", "Bl0214", "Bl0215", "Bl0216", "Bl0217", "Bl0218", "Bl0219", "Bl0220", "Bl0221", "Bl0222", "Bl0223", "Bl0224", "Bl0225", "Bl0226", "Bl0227", "Bl0228", "Bl0229", "Bl0230", "Bl0231", "Bl0232", "Bl0233", "Bl0234", "Bl0235", "Bl0236", "Bl0237", "Bl0238", "Bl0239", "Bl0240", "Bl0241", "Bl0242", "Bl0243", "Bl0244", "Bl0245", "Bl0246", "Bl0247", "Bl0248", "Bl0249", "Bl0250", "Bl0251", "Bl0252", "Bl0253", "Bl0254", "Bl0255", "Bl0256", "Bl0257", "Bl0258", "Bl0259", "Bl0260", "Bl0261", "Bl0262", "Bl0263", "Bl0264", "Bl0265", "Bl0266", "Bl0267", "Bl0268", "Bl0269", "Bl0270", "Bl0271", "Bl0272", "Bl0273", "Bl0274", "Bl0275", "Bl0276", "Bl0277", "Bl0278", "Bl0279", "Bl0280", "Bl0281", "Bl0282", "Bl0283", "Bl0284", "Bl0285", "Bl0286", "Bl0287", "Bl0288", "Bl0289", "Bl0290", "Bl0291", "Bl0292", "Bl0293", "Bl0294", "Bl0295", "Bl0296", "Bl0297", "Bl0298", "Bl0299", "Bl0300", "Bl0301", "Bl0302", "Bl0303", "Bl0304", "Bl0305", "Bl0306", "Bl0307", "Bl0308", "Bl0309", "Bl0310", "Bl0311", "Bl0312", "Bl0313", "Bl0314", "Bl0315", "Bl0316", "Bl0317", "Bl0318", "Bl0319", "Bl0320", "Bl0321", "Bl0322", "Bl0323", "Bl0324", "Bl0325", "Bl0326", "Bl0327", "Bl0328", "Bl0329", "Bl0330", "Bl0331", "Bl0332", "Bl0333", "Bl0334", "Bl0335", "Bl0336", "Bl0337", "Bl0338", "Bl0339", "Bl0340", "Bl0341", "Bl0342", "Bl0343", "Bl0344", "Bl0345", "Bl0346", "Bl0347", "Bl0348", "Bl0349", "Bl0350", "Bl0351", "Bl0352", "Bl0353", "Bl0354", "Bl0355", "Bl0356", "Bl0357", "Bl0358", "Bl0359", "Bl0360", "Bl0361", "Bl0362", "Bl0363", "Bl0364", "Bl0365", "Bl0366", "Bl0367", "Bl0368", "Bl0369", "Bl0370", "Bl0371", "Bl0372", "Bl0373", "Bl0374", "Bl0375", "Bl0376", "Bl0377", "Bl0378", "Bl0379", "Bl0380", "Bl0381", "Bl0382", "Bl0383", "Bl0384", "Bl0385", "Bl0386", "Bl0387", "Bl0388", "Bl0389", "Bl0390", "Bl0391", "Bl0392", "Bl0393", "Bl0394", "Bl0395", "Bl0396", "Bl0397", "Bl0398", "Bl0399", "Bl0400", "Bl0401", "Bl0402", "Bl0403", "Bl0404", "Bl0405", "Bl0406", "Bl0407", "Bl0408", "Bl0409", "Bl0410", "Bl0411", "Bl0412", "Bl0413", "Bl0414", "Bl0415", "Bl0416", "Bl0417", "Bl0418", "Bl0419", "Bl0420", "Bl0421", "Bl0422", "Bl0423", "Bl0424", "Bl0425", "Bl0426", "Bl0427", "Bl0428", "Bl0429", "Bl0430", "Bl0431", "Bl0432", "Bl0433", "Bl0434", "Bl0435", "Bl0436", "Bl0437", "Bl0438", "Bl0439", "Bl0440", "Bl0441", "Bl0442", "Bl0443", "Bl0444", "Bl0445", "Bl0446", "Bl0447", "Bl0448", "Bl0449", "Bl0450", "Bl0451", "Bl0452", "Bl0453", "Bl0454", "Bl0455", "Bl0456", "Bl0457", "Bl0458", "Bl0459", "Bl0460", "Bl0461", "Bl0462", "Bl0463", "Bl0464", "Bl0465", "Bl0466", "Bl0467", "Bl0468", "Bl0469", "Bl0470", "Bl0471", "Bl0472", "Bl0473", "Bl0474", "Bl0475", "Bl0476", "Bl0477", "Bl0478", "Bl0479", "Bl0480", "Bl0481", "Bl0482", "Bl0483", "Bl0484", "Bl0485", "Bl0486", "Bl0487", "Bl0488", "Bl0489", "Bl0490", "Bl0491", "Bl0492", "Bl0493", "Bl0494", "Bl0495", "Bl0496", "Bl0497", "Bl0498", "Bl0499", "Bl0500", "Bl0501", "Bl0502", "Bl0503", "Bl0504", "Bl0505", "Bl0506", "Bl0507", "Bl0508", "Bl0509", "Bl0510", "Bl0511", "Bl0512", "Bl0513", "Bl0514", "Bl0515", "Bl0516", "Bl0517", "Bl0518", "Bl0519", "Bl0520", "Bl0521", "Bl0522", "Bl0523", "Bl0524", "Bl0525", "Bl0526", "Bl0527", "Bl0528", "Bl0529", "Bl0530", "Bl0531", "Bl0532", "Bl0533", "Bl0534", "Bl0535", "Bl0536", "Bl0537", "Bl0538", "Bl0539", "Bl0540", "Bl0541", "Bl0542", "Bl0543", "Bl0544", "Bl0545", "Bl0546", "Bl0547", "Bl0548", "Bl0549", "Bl0550", "Bl0551", "Bl0552", "Bl0553", "Bl0554", "Bl0555", "Bl0556", "Bl0557", "Bl0558", "Bl0559", "Bl0560", "Bl0561", "Bl0562", "Bl0563", "Bl0564", "Bl0565", "Bl0566", "Bl0567", "Bl0568", "Bl0569", "Bl0570", "Bl0571", "Bl0572", "Bl0573", "Bl0574", "Bl0575", "Bl0576", "Bl0577", "Bl0578", "Bl0579", "Bl0580", "Bl0581", "Bl0582", "Bl0583", "Bl0584", "Bl0585", "Bl0586", "Bl0587", "Bl0588", "Bl0589", "Bl0590", "Bl0591", "Bl0592", "Bl0593", "Bl0594", "Bl0595", "Bl0596", "Bl0597", "Bl0598", "Bl0599", "Bl0600", "Bl0601", "Bl0602", "Bl0603", "Bl0604", "Bl0605", "Bl0606", "Bl0607", "Bl0608", "Bl0609", "Bl0610", "Bl0611", "Bl0612", "Bl0613", "Bl0614", "Bl0615", "Bl0616", "Bl0617", "Bl0618", "Bl0619", "Bl0620", "Bl0621", "Bl0622", "Bl0623", "Bl0624", "Bl0625", "Bl0626", "Bl0627", "Bl0628", "Bl0629", "Bl0630", "Bl0631", "Bl0632", "Bl0633", "Bl0634", "Bl0635", "Bl0636", "Bl0637", "Bl0638", "Bl0639", "Bl0640", "Bl0641", "Bl0642", "Bl0643", "Bl0644", "Bl0645"])
# Changement de discrétisation pour les faces Fa1597
ctx.getTopoManager().setMeshingProperty (Mgx3D.FaceMeshingPropertyTransfinite(),["Fa1597"])
# Changement de discrétisation pour les faces Fa1595
ctx.getTopoManager().setMeshingProperty (Mgx3D.FaceMeshingPropertyTransfinite(),["Fa1595"])
# Changement de discrétisation pour les faces Fa0010 Fa0051 Fa0055 Fa0059 Fa0061 ... 
ctx.getTopoManager().setMeshingProperty (Mgx3D.FaceMeshingPropertyTransfinite(),["Fa0010", "Fa0051", "Fa0055", "Fa0059", "Fa0061", "Fa0063", "Fa0070", "Fa0071", "Fa0072", "Fa0073", "Fa0074", "Fa0075", "Fa0109", "Fa0110", "Fa0113", "Fa0123", "Fa0131", "Fa0132", "Fa0133", "Fa0135", "Fa0137", "Fa0139", "Fa0141", "Fa0143", "Fa0144", "Fa0146", "Fa0148", "Fa0149", "Fa0150", "Fa0151", "Fa0153", "Fa0154", "Fa0155", "Fa0161", "Fa0163", "Fa0165", "Fa0169", "Fa0171", "Fa0173", "Fa0196", "Fa0198", "Fa0206", "Fa0213", "Fa0214", "Fa0217", "Fa0221", "Fa0222", "Fa0226", "Fa0232", "Fa0234", "Fa0235", "Fa0236", "Fa0238", "Fa0240", "Fa0241", "Fa0243", "Fa0253", "Fa0256", "Fa0258", "Fa0263", "Fa0265", "Fa0267", "Fa0273", "Fa0274", "Fa0275", "Fa0276", "Fa0277", "Fa0278", "Fa0279", "Fa0280", "Fa0285", "Fa0286", "Fa0293", "Fa0303", "Fa0304", "Fa0305", "Fa0307", "Fa0309", "Fa0313", "Fa0315", "Fa0321", "Fa0323", "Fa0325", "Fa0329", "Fa0331", "Fa0337", "Fa0339", "Fa0341", "Fa0345", "Fa0347", "Fa0353", "Fa0355", "Fa0357", "Fa0361", "Fa0363", "Fa0369", "Fa0371", "Fa0373", "Fa0377", "Fa0379", "Fa0385", "Fa0387", "Fa0389", "Fa0393", "Fa0395", "Fa0405", "Fa0406", "Fa0407", "Fa0408", "Fa0409", "Fa0411", "Fa0412", "Fa0413", "Fa0414", "Fa0415", "Fa0416", "Fa0417", "Fa0418", "Fa0419", "Fa0420", "Fa0423", "Fa0424", "Fa0427", "Fa0428", "Fa0432", "Fa0434", "Fa0436", "Fa0439", "Fa0441", "Fa0444", "Fa0446", "Fa0447", "Fa0449", "Fa0453", "Fa0457", "Fa0460", "Fa0462", "Fa0463", "Fa0464", "Fa0465", "Fa0466", "Fa0467", "Fa0468", "Fa0471", "Fa0472", "Fa0473", "Fa0474", "Fa0475", "Fa0476", "Fa0479", "Fa0480", "Fa0481", "Fa0482", "Fa0483", "Fa0484", "Fa0485", "Fa0486", "Fa0487", "Fa0488", "Fa0489", "Fa0490", "Fa0493", "Fa0494", "Fa0497", "Fa0498", "Fa0499", "Fa0500", "Fa0501", "Fa0502", "Fa0509", "Fa0510", "Fa0513", "Fa0514", "Fa0516", "Fa0519", "Fa0521", "Fa0522", "Fa0524", "Fa0527", "Fa0529", "Fa0530", "Fa0532", "Fa0535", "Fa0537", "Fa0538", "Fa0540", "Fa0543", "Fa0544", "Fa0545", "Fa0546", "Fa0547", "Fa0548", "Fa0549", "Fa0550", "Fa0551", "Fa0552", "Fa0553", "Fa0554", "Fa0555", "Fa0556", "Fa0557", "Fa0560", "Fa0561", "Fa0562", "Fa0563", "Fa0564", "Fa0565", "Fa0566", "Fa0567", "Fa0568", "Fa0569", "Fa0570", "Fa0571", "Fa0572", "Fa0573", "Fa0576", "Fa0577", "Fa0578", "Fa0579", "Fa0580", "Fa0581", "Fa0582", "Fa0583", "Fa0584", "Fa0585", "Fa0586", "Fa0587", "Fa0588", "Fa0589", "Fa0592", "Fa0593", "Fa0594", "Fa0595", "Fa0596", "Fa0597", "Fa0600", "Fa0601", "Fa0602", "Fa0603", "Fa0604", "Fa0605", "Fa0606", "Fa0607", "Fa0608", "Fa0609", "Fa0610", "Fa0611", "Fa0612", "Fa0613", "Fa0614", "Fa0615", "Fa0616", "Fa0617", "Fa0618", "Fa0619", "Fa0622", "Fa0623", "Fa0624", "Fa0625", "Fa0626", "Fa0627", "Fa0628", "Fa0629", "Fa0630", "Fa0631", "Fa0632", "Fa0633", "Fa0634", "Fa0635", "Fa0636", "Fa0637", "Fa0638", "Fa0639", "Fa0641", "Fa0642", "Fa0643", "Fa0645", "Fa0646", "Fa0647", "Fa0648", "Fa0649", "Fa0650", "Fa0651", "Fa0652", "Fa0653", "Fa0654", "Fa0655", "Fa0656", "Fa0657", "Fa0658", "Fa0659", "Fa0660", "Fa0661", "Fa0662", "Fa0663", "Fa0664", "Fa0665", "Fa0666", "Fa0667", "Fa0668", "Fa0669", "Fa0670", "Fa0671", "Fa0672", "Fa0673", "Fa0674", "Fa0675", "Fa0676", "Fa0677", "Fa0678", "Fa0679", "Fa0681", "Fa0682", "Fa0684", "Fa0685", "Fa0686", "Fa0687", "Fa0688", "Fa0689", "Fa0690", "Fa0691", "Fa0692", "Fa0693", "Fa0694", "Fa0695", "Fa0696", "Fa0697", "Fa0698", "Fa0699", "Fa0700", "Fa0701", "Fa0702", "Fa0703", "Fa0704", "Fa0705", "Fa0706", "Fa0707", "Fa0709", "Fa0710", "Fa0712", "Fa0713", "Fa0714", "Fa0715", "Fa0716", "Fa0717", "Fa0718", "Fa0719", "Fa0720", "Fa0721", "Fa0722", "Fa0723", "Fa0724", "Fa0725", "Fa0726", "Fa0727", "Fa0728", "Fa0729", "Fa0730", "Fa0731", "Fa0732", "Fa0733", "Fa0734", "Fa0735", "Fa0736", "Fa0737", "Fa0738", "Fa0739", "Fa0740", "Fa0741", "Fa0742", "Fa0743", "Fa0744", "Fa0745", "Fa0746", "Fa0747", "Fa0748", "Fa0749", "Fa0750", "Fa0751", "Fa0752", "Fa0753", "Fa0754", "Fa0755", "Fa0756", "Fa0757", "Fa0758", "Fa0759", "Fa0760", "Fa0761", "Fa0762", "Fa0763", "Fa0764", "Fa0765", "Fa0766", "Fa0767", "Fa0768", "Fa0769", "Fa0770", "Fa0771", "Fa0772", "Fa0773", "Fa0774", "Fa0775", "Fa0776", "Fa0777", "Fa0778", "Fa0779", "Fa0780", "Fa0781", "Fa0782", "Fa0783", "Fa0784", "Fa0785", "Fa0786", "Fa0787", "Fa0788", "Fa0789", "Fa0790", "Fa0791", "Fa0792", "Fa0793", "Fa0794", "Fa0795", "Fa0796", "Fa0797", "Fa0798", "Fa0799", "Fa0800", "Fa0801", "Fa0802", "Fa0803", "Fa0804", "Fa0805", "Fa0806", "Fa0807", "Fa0808", "Fa0809", "Fa0810", "Fa0811", "Fa0812", "Fa0813", "Fa0814", "Fa0815", "Fa0816", "Fa0817", "Fa0818", "Fa0819", "Fa0820", "Fa0821", "Fa0822", "Fa0823", "Fa0824", "Fa0825", "Fa0826", "Fa0827", "Fa0828", "Fa0829", "Fa0830", "Fa0831", "Fa0832", "Fa0833", "Fa0834", "Fa0835", "Fa0836", "Fa0837", "Fa0838", "Fa0839", "Fa0840", "Fa0841", "Fa0842", "Fa0843", "Fa0844", "Fa0845", "Fa0846", "Fa0847", "Fa0848", "Fa0849", "Fa0850", "Fa0851", "Fa0852", "Fa0853", "Fa0854", "Fa0855", "Fa0856", "Fa0857", "Fa0858", "Fa0859", "Fa0860", "Fa0861", "Fa0862", "Fa0863", "Fa0864", "Fa0865", "Fa0866", "Fa0867", "Fa0868", "Fa0869", "Fa0870", "Fa0871", "Fa0872", "Fa0873", "Fa0874", "Fa0875", "Fa0876", "Fa0877", "Fa0878", "Fa0879", "Fa0880", "Fa0881", "Fa0882", "Fa0883", "Fa0884", "Fa0885", "Fa0886", "Fa0887", "Fa0888", "Fa0889", "Fa0890", "Fa0891", "Fa0892", "Fa0893", "Fa0894", "Fa0895", "Fa0896", "Fa0897", "Fa0898", "Fa0899", "Fa0900", "Fa0901", "Fa0902", "Fa0903", "Fa0904", "Fa0905", "Fa0906", "Fa0907", "Fa0908", "Fa0909", "Fa0910", "Fa0911", "Fa0912", "Fa0913", "Fa0914", "Fa0915", "Fa0916", "Fa0917", "Fa0918", "Fa0919", "Fa0920", "Fa0921", "Fa0922", "Fa0923", "Fa0924", "Fa0925", "Fa0926", "Fa0927", "Fa0928", "Fa0929", "Fa0930", "Fa0931", "Fa0932", "Fa0933", "Fa0934", "Fa0935", "Fa0936", "Fa0937", "Fa0938", "Fa0939", "Fa0940", "Fa0941", "Fa0942", "Fa0943", "Fa0944", "Fa0945", "Fa0946", "Fa0947", "Fa0948", "Fa0949", "Fa0950", "Fa0951", "Fa0952", "Fa0953", "Fa0954", "Fa0955", "Fa0956", "Fa0957", "Fa0958", "Fa0959", "Fa0960", "Fa0961", "Fa0962", "Fa0963", "Fa0964", "Fa0965", "Fa0966", "Fa0967", "Fa0968", "Fa0969", "Fa0970", "Fa0971", "Fa0972", "Fa0973", "Fa0974", "Fa0975", "Fa0976", "Fa0977", "Fa0978", "Fa0979", "Fa0980", "Fa0981", "Fa0982", "Fa0983", "Fa0985", "Fa0987", "Fa0989", "Fa0990", "Fa0991", "Fa0993", "Fa0995", "Fa0997", "Fa0999", "Fa1001", "Fa1004", "Fa1006", "Fa1008", "Fa1010", "Fa1011", "Fa1013", "Fa1015", "Fa1017", "Fa1019", "Fa1021", "Fa1023", "Fa1025", "Fa1026", "Fa1027", "Fa1030", "Fa1031", "Fa1033", "Fa1035", "Fa1037", "Fa1039", "Fa1041", "Fa1043", "Fa1045", "Fa1047", "Fa1049", "Fa1055", "Fa1056", "Fa1057", "Fa1058", "Fa1059", "Fa1060", "Fa1061", "Fa1062", "Fa1063", "Fa1064", "Fa1065", "Fa1066", "Fa1067", "Fa1068", "Fa1069", "Fa1070", "Fa1071", "Fa1072", "Fa1073", "Fa1074", "Fa1075", "Fa1076", "Fa1077", "Fa1078", "Fa1079", "Fa1080", "Fa1081", "Fa1082", "Fa1083", "Fa1084", "Fa1085", "Fa1086", "Fa1087", "Fa1088", "Fa1089", "Fa1090", "Fa1091", "Fa1092", "Fa1093", "Fa1094", "Fa1095", "Fa1096", "Fa1097", "Fa1098", "Fa1099", "Fa1100", "Fa1101", "Fa1102", "Fa1103", "Fa1104", "Fa1105", "Fa1106", "Fa1107", "Fa1108", "Fa1109", "Fa1110", "Fa1111", "Fa1112", "Fa1113", "Fa1114", "Fa1115", "Fa1116", "Fa1117", "Fa1118", "Fa1119", "Fa1120", "Fa1121", "Fa1122", "Fa1123", "Fa1124", "Fa1125", "Fa1126", "Fa1127", "Fa1128", "Fa1129", "Fa1130", "Fa1131", "Fa1132", "Fa1133", "Fa1134", "Fa1135", "Fa1136", "Fa1137", "Fa1138", "Fa1139", "Fa1140", "Fa1141", "Fa1142", "Fa1143", "Fa1144", "Fa1145", "Fa1146", "Fa1147", "Fa1148", "Fa1149", "Fa1150", "Fa1151", "Fa1152", "Fa1153", "Fa1154", "Fa1155", "Fa1156", "Fa1157", "Fa1158", "Fa1159", "Fa1160", "Fa1161", "Fa1162", "Fa1163", "Fa1164", "Fa1165", "Fa1166", "Fa1167", "Fa1168", "Fa1169", "Fa1170", "Fa1171", "Fa1172", "Fa1173", "Fa1174", "Fa1175", "Fa1176", "Fa1177", "Fa1178", "Fa1179", "Fa1180", "Fa1181", "Fa1182", "Fa1183", "Fa1184", "Fa1185", "Fa1186", "Fa1187", "Fa1188", "Fa1189", "Fa1190", "Fa1191", "Fa1192", "Fa1193", "Fa1194", "Fa1195", "Fa1196", "Fa1197", "Fa1198", "Fa1199", "Fa1200", "Fa1201", "Fa1202", "Fa1203", "Fa1204", "Fa1205", "Fa1206", "Fa1207", "Fa1208", "Fa1209", "Fa1210", "Fa1211", "Fa1212", "Fa1213", "Fa1214", "Fa1215", "Fa1216", "Fa1217", "Fa1218", "Fa1219", "Fa1220", "Fa1221", "Fa1222", "Fa1223", "Fa1224", "Fa1225", "Fa1226", "Fa1227", "Fa1228", "Fa1229", "Fa1230", "Fa1231", "Fa1232", "Fa1233", "Fa1234", "Fa1235", "Fa1236", "Fa1237", "Fa1238", "Fa1239", "Fa1240", "Fa1241", "Fa1242", "Fa1243", "Fa1244", "Fa1245", "Fa1246", "Fa1247", "Fa1248", "Fa1249", "Fa1250", "Fa1251", "Fa1252", "Fa1253", "Fa1254", "Fa1255", "Fa1256", "Fa1257", "Fa1258", "Fa1259", "Fa1260", "Fa1261", "Fa1262", "Fa1263", "Fa1264", "Fa1265", "Fa1266", "Fa1267", "Fa1268", "Fa1269", "Fa1270", "Fa1271", "Fa1272", "Fa1273", "Fa1274", "Fa1275", "Fa1276", "Fa1277", "Fa1278", "Fa1279", "Fa1280", "Fa1281", "Fa1282", "Fa1283", "Fa1284", "Fa1285", "Fa1286", "Fa1287", "Fa1288", "Fa1289", "Fa1290", "Fa1291", "Fa1292", "Fa1293", "Fa1294", "Fa1295", "Fa1296", "Fa1297", "Fa1298", "Fa1299", "Fa1300", "Fa1301", "Fa1302", "Fa1303", "Fa1304", "Fa1305", "Fa1306", "Fa1307", "Fa1308", "Fa1309", "Fa1310", "Fa1311", "Fa1312", "Fa1313", "Fa1314", "Fa1315", "Fa1316", "Fa1317", "Fa1318", "Fa1319", "Fa1320", "Fa1321", "Fa1322", "Fa1323", "Fa1324", "Fa1325", "Fa1326", "Fa1327", "Fa1328", "Fa1329", "Fa1330", "Fa1331", "Fa1332", "Fa1333", "Fa1334", "Fa1335", "Fa1336", "Fa1337", "Fa1338", "Fa1339", "Fa1340", "Fa1341", "Fa1342", "Fa1343", "Fa1344", "Fa1345", "Fa1346", "Fa1347", "Fa1348", "Fa1349", "Fa1350", "Fa1351", "Fa1352", "Fa1353", "Fa1354", "Fa1355", "Fa1356", "Fa1357", "Fa1358", "Fa1359", "Fa1360", "Fa1361", "Fa1362", "Fa1363", "Fa1364", "Fa1365", "Fa1366", "Fa1367", "Fa1368", "Fa1369", "Fa1370", "Fa1371", "Fa1372", "Fa1373", "Fa1374", "Fa1375", "Fa1376", "Fa1377", "Fa1378", "Fa1379", "Fa1380", "Fa1381", "Fa1382", "Fa1383", "Fa1384", "Fa1385", "Fa1386", "Fa1387", "Fa1388", "Fa1389", "Fa1390", "Fa1391", "Fa1392", "Fa1393", "Fa1394", "Fa1395", "Fa1396", "Fa1397", "Fa1398", "Fa1399", "Fa1400", "Fa1401", "Fa1402", "Fa1403", "Fa1404", "Fa1405", "Fa1406", "Fa1407", "Fa1408", "Fa1409", "Fa1410", "Fa1411", "Fa1412", "Fa1413", "Fa1414", "Fa1415", "Fa1416", "Fa1417", "Fa1418", "Fa1419", "Fa1420", "Fa1421", "Fa1422", "Fa1423", "Fa1424", "Fa1425", "Fa1426", "Fa1427", "Fa1428", "Fa1429", "Fa1430", "Fa1431", "Fa1432", "Fa1433", "Fa1434", "Fa1435", "Fa1436", "Fa1437", "Fa1438", "Fa1439", "Fa1440", "Fa1441", "Fa1442", "Fa1443", "Fa1444", "Fa1445", "Fa1446", "Fa1447", "Fa1448", "Fa1449", "Fa1450", "Fa1451", "Fa1452", "Fa1453", "Fa1454", "Fa1455", "Fa1456", "Fa1457", "Fa1458", "Fa1459", "Fa1460", "Fa1461", "Fa1462", "Fa1463", "Fa1464", "Fa1465", "Fa1466", "Fa1467", "Fa1468", "Fa1469", "Fa1470", "Fa1471", "Fa1472", "Fa1473", "Fa1474", "Fa1475", "Fa1476", "Fa1477", "Fa1478", "Fa1479", "Fa1480", "Fa1481", "Fa1482", "Fa1483", "Fa1484", "Fa1485", "Fa1486", "Fa1487", "Fa1488", "Fa1489", "Fa1490", "Fa1491", "Fa1492", "Fa1493", "Fa1494", "Fa1495", "Fa1496", "Fa1497", "Fa1498", "Fa1499", "Fa1500", "Fa1501", "Fa1502", "Fa1503", "Fa1504", "Fa1505", "Fa1506", "Fa1507", "Fa1508", "Fa1509", "Fa1510", "Fa1511", "Fa1512", "Fa1513", "Fa1514", "Fa1515", "Fa1516", "Fa1517", "Fa1518", "Fa1519", "Fa1520", "Fa1521", "Fa1522", "Fa1523", "Fa1524", "Fa1525", "Fa1526", "Fa1527", "Fa1528", "Fa1529", "Fa1530", "Fa1531", "Fa1532", "Fa1533", "Fa1534", "Fa1535", "Fa1536", "Fa1537", "Fa1538", "Fa1539", "Fa1540", "Fa1541", "Fa1542", "Fa1543", "Fa1544", "Fa1545", "Fa1546", "Fa1547", "Fa1548", "Fa1549", "Fa1550", "Fa1551", "Fa1552", "Fa1553", "Fa1554", "Fa1555", "Fa1556", "Fa1557", "Fa1558", "Fa1559", "Fa1560", "Fa1561", "Fa1562", "Fa1563", "Fa1564", "Fa1565", "Fa1566", "Fa1567", "Fa1568", "Fa1569", "Fa1570", "Fa1571", "Fa1572", "Fa1573", "Fa1574", "Fa1575", "Fa1576", "Fa1577", "Fa1578", "Fa1579", "Fa1580", "Fa1581", "Fa1582", "Fa1583", "Fa1584", "Fa1585", "Fa1586", "Fa1587", "Fa1588", "Fa1589", "Fa1590", "Fa1591", "Fa1592", "Fa1593", "Fa1594", "Fa1595", "Fa1596", "Fa1597", "Fa1598", "Fa1599", "Fa1600", "Fa1601", "Fa1602", "Fa1603", "Fa1604", "Fa1605", "Fa1606", "Fa1607", "Fa1608", "Fa1609", "Fa1610", "Fa1611", "Fa1612", "Fa1613", "Fa1614", "Fa1615", "Fa1616", "Fa1617", "Fa1618", "Fa1619", "Fa1620", "Fa1621", "Fa1622", "Fa1623", "Fa1624", "Fa1625", "Fa1626", "Fa1627", "Fa1628", "Fa1629", "Fa1630", "Fa1631", "Fa1632", "Fa1633", "Fa1634", "Fa1635", "Fa1636", "Fa1637", "Fa1638", "Fa1639", "Fa1640", "Fa1641", "Fa1642", "Fa1643", "Fa1644", "Fa1645", "Fa1646", "Fa1647", "Fa1648", "Fa1649", "Fa1650", "Fa1651", "Fa1652", "Fa1653", "Fa1654", "Fa1655", "Fa1656", "Fa1657", "Fa1658", "Fa1659", "Fa1660", "Fa1661", "Fa1662", "Fa1663", "Fa1664", "Fa1665", "Fa1666", "Fa1667", "Fa1668", "Fa1669", "Fa1670", "Fa1671", "Fa1672", "Fa1673", "Fa1674", "Fa1675", "Fa1676", "Fa1677", "Fa1678", "Fa1679", "Fa1680", "Fa1681", "Fa1682", "Fa1683", "Fa1684", "Fa1685", "Fa1686", "Fa1687", "Fa1688", "Fa1689", "Fa1690", "Fa1691", "Fa1692", "Fa1693", "Fa1694", "Fa1695", "Fa1696", "Fa1697", "Fa1698", "Fa1699", "Fa1700", "Fa1701", "Fa1702", "Fa1703", "Fa1704", "Fa1705", "Fa1706", "Fa1707", "Fa1708", "Fa1709", "Fa1710", "Fa1711", "Fa1712", "Fa1713", "Fa1714", "Fa1715", "Fa1716", "Fa1717", "Fa1718", "Fa1719", "Fa1720", "Fa1721", "Fa1722", "Fa1723", "Fa1724", "Fa1725", "Fa1726", "Fa1727", "Fa1728", "Fa1729", "Fa1730", "Fa1731", "Fa1732", "Fa1733", "Fa1734", "Fa1735", "Fa1736", "Fa1737", "Fa1738", "Fa1739", "Fa1740", "Fa1741", "Fa1742", "Fa1743", "Fa1744", "Fa1745", "Fa1746", "Fa1747", "Fa1748", "Fa1749", "Fa1750", "Fa1751", "Fa1752", "Fa1753", "Fa1754", "Fa1755", "Fa1756", "Fa1757", "Fa1758", "Fa1759", "Fa1760", "Fa1761", "Fa1762", "Fa1763", "Fa1764", "Fa1765", "Fa1766", "Fa1767", "Fa1768", "Fa1769", "Fa1770", "Fa1771", "Fa1772", "Fa1773", "Fa1774", "Fa1775", "Fa1776", "Fa1777", "Fa1778", "Fa1779", "Fa1780", "Fa1781", "Fa1782", "Fa1783", "Fa1784", "Fa1785", "Fa1786", "Fa1787", "Fa1788", "Fa1789", "Fa1790", "Fa1791", "Fa1792", "Fa1793", "Fa1794", "Fa1795", "Fa1796", "Fa1797", "Fa1798", "Fa1799", "Fa1800", "Fa1801", "Fa1802", "Fa1803", "Fa1804", "Fa1805", "Fa1806", "Fa1807", "Fa1808", "Fa1809", "Fa1810", "Fa1811", "Fa1812", "Fa1813", "Fa1814", "Fa1815", "Fa1816", "Fa1817", "Fa1818", "Fa1819", "Fa1820", "Fa1821", "Fa1822", "Fa1823", "Fa1824", "Fa1825", "Fa1826", "Fa1827", "Fa1828", "Fa1829", "Fa1830", "Fa1831", "Fa1832", "Fa1833", "Fa1834", "Fa1835", "Fa1836", "Fa1837", "Fa1838", "Fa1839", "Fa1840", "Fa1841", "Fa1842", "Fa1843", "Fa1844", "Fa1845", "Fa1846", "Fa1847", "Fa1848", "Fa1849", "Fa1850", "Fa1851", "Fa1852", "Fa1853", "Fa1854", "Fa1855", "Fa1856", "Fa1857", "Fa1858", "Fa1859", "Fa1860", "Fa1861", "Fa1862", "Fa1863", "Fa1864", "Fa1865", "Fa1866", "Fa1867", "Fa1868", "Fa1869", "Fa1870", "Fa1871", "Fa1872", "Fa1873", "Fa1874", "Fa1875", "Fa1876", "Fa1877", "Fa1878", "Fa1879", "Fa1880", "Fa1881", "Fa1882", "Fa1883", "Fa1884", "Fa1885", "Fa1886", "Fa1887", "Fa1888", "Fa1889", "Fa1890", "Fa1891", "Fa1892", "Fa1893", "Fa1894", "Fa1895", "Fa1896", "Fa1897", "Fa1898", "Fa1899", "Fa1900", "Fa1901", "Fa1902", "Fa1903", "Fa1904", "Fa1905", "Fa1906", "Fa1907", "Fa1908", "Fa1909", "Fa1910", "Fa1911", "Fa1912", "Fa1913", "Fa1914", "Fa1915", "Fa1916", "Fa1917", "Fa1918", "Fa1919", "Fa1920", "Fa1921", "Fa1922", "Fa1923", "Fa1924", "Fa1925", "Fa1926", "Fa1927", "Fa1928", "Fa1929", "Fa1930", "Fa1931", "Fa1932", "Fa1933", "Fa1934", "Fa1935", "Fa1936", "Fa1937", "Fa1938", "Fa1939", "Fa1940", "Fa1941", "Fa1942", "Fa1943", "Fa1944", "Fa1945", "Fa1946", "Fa1947", "Fa1948", "Fa1949", "Fa1950", "Fa1951", "Fa1952", "Fa1953", "Fa1954", "Fa1955", "Fa1956", "Fa1957", "Fa1958", "Fa1959", "Fa1960", "Fa1961", "Fa1962", "Fa1963", "Fa1964", "Fa1965", "Fa1966", "Fa1967", "Fa1968", "Fa1969", "Fa1970", "Fa1971", "Fa1972", "Fa1973", "Fa1974", "Fa1975", "Fa1976", "Fa1977", "Fa1978", "Fa1979", "Fa1980", "Fa1981", "Fa1982", "Fa1983", "Fa1984", "Fa1985", "Fa1986", "Fa1987", "Fa1988", "Fa1989", "Fa1990", "Fa1991", "Fa1992", "Fa1993", "Fa1994", "Fa1995", "Fa1996", "Fa1997", "Fa1998", "Fa1999", "Fa2000", "Fa2001", "Fa2002", "Fa2003", "Fa2004", "Fa2005", "Fa2006", "Fa2007", "Fa2008", "Fa2009", "Fa2010", "Fa2011", "Fa2012", "Fa2013", "Fa2014", "Fa2015", "Fa2016", "Fa2017", "Fa2018", "Fa2019", "Fa2020", "Fa2021", "Fa2022", "Fa2023", "Fa2024", "Fa2025", "Fa2026", "Fa2027", "Fa2028", "Fa2029", "Fa2030", "Fa2031", "Fa2032", "Fa2033", "Fa2034", "Fa2035", "Fa2036", "Fa2037", "Fa2038", "Fa2039", "Fa2040", "Fa2041", "Fa2042", "Fa2043", "Fa2044", "Fa2045", "Fa2046", "Fa2047", "Fa2048", "Fa2049", "Fa2050", "Fa2051", "Fa2052", "Fa2053", "Fa2054", "Fa2055", "Fa2056", "Fa2057", "Fa2058", "Fa2059", "Fa2060", "Fa2061", "Fa2062", "Fa2063", "Fa2064", "Fa2065", "Fa2066", "Fa2067", "Fa2068", "Fa2069", "Fa2070", "Fa2071", "Fa2072", "Fa2073", "Fa2074", "Fa2075", "Fa2076", "Fa2077", "Fa2078", "Fa2079", "Fa2080", "Fa2081", "Fa2082", "Fa2083", "Fa2084", "Fa2085", "Fa2086", "Fa2087", "Fa2088", "Fa2089", "Fa2090", "Fa2091", "Fa2092", "Fa2093", "Fa2094", "Fa2095", "Fa2096", "Fa2097", "Fa2098", "Fa2099", "Fa2100", "Fa2101", "Fa2102", "Fa2103", "Fa2104", "Fa2105", "Fa2106", "Fa2107", "Fa2108", "Fa2109", "Fa2110", "Fa2111", "Fa2112", "Fa2113", "Fa2114", "Fa2115", "Fa2116", "Fa2117", "Fa2118", "Fa2119", "Fa2120", "Fa2121", "Fa2122", "Fa2123", "Fa2124", "Fa2125", "Fa2126", "Fa2127", "Fa2128", "Fa2129", "Fa2130", "Fa2131", "Fa2132", "Fa2133", "Fa2134", "Fa2135", "Fa2136", "Fa2137", "Fa2138", "Fa2139", "Fa2140", "Fa2141", "Fa2142", "Fa2143", "Fa2144", "Fa2145", "Fa2146", "Fa2147", "Fa2148", "Fa2149", "Fa2150", "Fa2151", "Fa2152", "Fa2153", "Fa2154", "Fa2155", "Fa2156", "Fa2157", "Fa2158", "Fa2159", "Fa2160", "Fa2161", "Fa2162", "Fa2163", "Fa2164", "Fa2165", "Fa2166", "Fa2167", "Fa2168", "Fa2169", "Fa2170", "Fa2171", "Fa2172", "Fa2173", "Fa2174", "Fa2175", "Fa2176", "Fa2177", "Fa2178", "Fa2179", "Fa2180", "Fa2181", "Fa2182", "Fa2183", "Fa2184", "Fa2185", "Fa2186", "Fa2187", "Fa2188", "Fa2189", "Fa2190", "Fa2191", "Fa2192", "Fa2193", "Fa2194", "Fa2195", "Fa2196", "Fa2197", "Fa2198", "Fa2199", "Fa2200", "Fa2201", "Fa2202", "Fa2203", "Fa2204", "Fa2205", "Fa2206", "Fa2207", "Fa2208", "Fa2209", "Fa2210", "Fa2211", "Fa2212", "Fa2213", "Fa2214", "Fa2215", "Fa2216", "Fa2217", "Fa2218", "Fa2219", "Fa2220", "Fa2221", "Fa2222", "Fa2223", "Fa2224", "Fa2225", "Fa2226", "Fa2227", "Fa2228", "Fa2229", "Fa2230", "Fa2231", "Fa2232", "Fa2233", "Fa2234", "Fa2235", "Fa2236", "Fa2237", "Fa2238", "Fa2239", "Fa2240", "Fa2241", "Fa2242", "Fa2243", "Fa2244", "Fa2245", "Fa2246", "Fa2247", "Fa2248", "Fa2249", "Fa2250", "Fa2251", "Fa2252", "Fa2253", "Fa2254", "Fa2255", "Fa2256", "Fa2257", "Fa2258", "Fa2259", "Fa2260", "Fa2261", "Fa2262", "Fa2263", "Fa2264", "Fa2265", "Fa2266", "Fa2267", "Fa2268", "Fa2269", "Fa2270", "Fa2271", "Fa2272", "Fa2273", "Fa2274", "Fa2275", "Fa2276", "Fa2277", "Fa2278", "Fa2279", "Fa2280", "Fa2281", "Fa2282", "Fa2283", "Fa2284", "Fa2285", "Fa2286", "Fa2287", "Fa2288", "Fa2289", "Fa2290", "Fa2291", "Fa2292", "Fa2293", "Fa2294", "Fa2295", "Fa2296", "Fa2297", "Fa2298", "Fa2299", "Fa2300", "Fa2301", "Fa2302", "Fa2303", "Fa2304", "Fa2305", "Fa2306", "Fa2307", "Fa2308", "Fa2309", "Fa2310", "Fa2311", "Fa2312", "Fa2313", "Fa2314", "Fa2315", "Fa2316", "Fa2317", "Fa2318", "Fa2319", "Fa2320", "Fa2321", "Fa2322", "Fa2323", "Fa2324", "Fa2325", "Fa2326", "Fa2327", "Fa2328", "Fa2329", "Fa2330", "Fa2331", "Fa2332", "Fa2333", "Fa2334", "Fa2335", "Fa2336", "Fa2337", "Fa2338", "Fa2339", "Fa2340", "Fa2341", "Fa2342", "Fa2343", "Fa2344", "Fa2345", "Fa2346", "Fa2347", "Fa2348", "Fa2349", "Fa2350", "Fa2351", "Fa2352", "Fa2353", "Fa2354", "Fa2355", "Fa2356", "Fa2357", "Fa2358", "Fa2359", "Fa2360", "Fa2361", "Fa2362", "Fa2363", "Fa2364", "Fa2365", "Fa2366", "Fa2367", "Fa2368", "Fa2369", "Fa2370", "Fa2371", "Fa2372", "Fa2373", "Fa2374", "Fa2375", "Fa2376", "Fa2377", "Fa2378", "Fa2379", "Fa2380", "Fa2381", "Fa2382", "Fa2383", "Fa2384", "Fa2385", "Fa2386", "Fa2387", "Fa2388", "Fa2389", "Fa2390", "Fa2391", "Fa2392", "Fa2393", "Fa2394", "Fa2395", "Fa2396", "Fa2397", "Fa2398", "Fa2399", "Fa2400", "Fa2401", "Fa2402", "Fa2403", "Fa2404", "Fa2405", "Fa2406", "Fa2407", "Fa2408", "Fa2409", "Fa2410", "Fa2411", "Fa2412", "Fa2413", "Fa2414", "Fa2415", "Fa2416", "Fa2417", "Fa2418", "Fa2419", "Fa2420", "Fa2421", "Fa2422", "Fa2423", "Fa2424", "Fa2425", "Fa2426", "Fa2427", "Fa2428", "Fa2429", "Fa2430", "Fa2431", "Fa2432", "Fa2433", "Fa2434", "Fa2435", "Fa2436", "Fa2437", "Fa2438", "Fa2439", "Fa2440", "Fa2441", "Fa2442", "Fa2443", "Fa2444", "Fa2445", "Fa2446", "Fa2447", "Fa2448", "Fa2449", "Fa2450", "Fa2451", "Fa2452", "Fa2453", "Fa2454", "Fa2455", "Fa2456", "Fa2457", "Fa2458", "Fa2459", "Fa2460", "Fa2461", "Fa2462", "Fa2463", "Fa2464", "Fa2465", "Fa2466", "Fa2467", "Fa2468", "Fa2469", "Fa2470", "Fa2471", "Fa2472", "Fa2473", "Fa2474", "Fa2475", "Fa2476", "Fa2477", "Fa2478", "Fa2479", "Fa2480", "Fa2481", "Fa2482", "Fa2483", "Fa2484", "Fa2485", "Fa2486", "Fa2487", "Fa2488", "Fa2489", "Fa2490", "Fa2491", "Fa2492", "Fa2493", "Fa2494", "Fa2495", "Fa2496", "Fa2497", "Fa2498", "Fa2499", "Fa2500", "Fa2501", "Fa2502", "Fa2503", "Fa2504", "Fa2505", "Fa2506", "Fa2507", "Fa2508", "Fa2509", "Fa2510", "Fa2511", "Fa2512", "Fa2513", "Fa2514", "Fa2515", "Fa2516", "Fa2517", "Fa2518", "Fa2519", "Fa2520", "Fa2521", "Fa2522", "Fa2523", "Fa2524", "Fa2525", "Fa2526", "Fa2527", "Fa2528", "Fa2529", "Fa2530", "Fa2531", "Fa2532", "Fa2533", "Fa2534", "Fa2535", "Fa2536", "Fa2537", "Fa2538", "Fa2539", "Fa2540", "Fa2541", "Fa2542", "Fa2543", "Fa2544", "Fa2545", "Fa2546", "Fa2547", "Fa2548", "Fa2549", "Fa2550", "Fa2551", "Fa2552", "Fa2553", "Fa2554", "Fa2555", "Fa2556", "Fa2557", "Fa2558", "Fa2559", "Fa2560", "Fa2561", "Fa2562", "Fa2563", "Fa2564", "Fa2565", "Fa2566", "Fa2567", "Fa2568", "Fa2569", "Fa2570", "Fa2571", "Fa2572", "Fa2573", "Fa2574", "Fa2575", "Fa2576", "Fa2577", "Fa2578", "Fa2579", "Fa2580", "Fa2581", "Fa2582", "Fa2583", "Fa2584", "Fa2585", "Fa2586", "Fa2587", "Fa2588", "Fa2589", "Fa2590", "Fa2591", "Fa2592", "Fa2593", "Fa2594", "Fa2595", "Fa2596", "Fa2597", "Fa2598", "Fa2599", "Fa2600", "Fa2601", "Fa2602", "Fa2603", "Fa2604", "Fa2605", "Fa2606", "Fa2607", "Fa2608", "Fa2609", "Fa2610", "Fa2611", "Fa2612", "Fa2613", "Fa2614", "Fa2615", "Fa2616", "Fa2617", "Fa2618", "Fa2619", "Fa2620", "Fa2621", "Fa2622", "Fa2623", "Fa2624", "Fa2625", "Fa2626", "Fa2627", "Fa2628", "Fa2629", "Fa2630", "Fa2631", "Fa2632", "Fa2633", "Fa2634", "Fa2635", "Fa2636", "Fa2637", "Fa2638", "Fa2639", "Fa2640", "Fa2641", "Fa2642", "Fa2643", "Fa2644", "Fa2645", "Fa2646", "Fa2647", "Fa2648", "Fa2649", "Fa2650", "Fa2651", "Fa2652", "Fa2653", "Fa2654", "Fa2655", "Fa2656", "Fa2657", "Fa2658", "Fa2659", "Fa2660", "Fa2661", "Fa2662", "Fa2663", "Fa2664", "Fa2665", "Fa2666", "Fa2667", "Fa2668", "Fa2669", "Fa2670", "Fa2671", "Fa2672", "Fa2673", "Fa2674", "Fa2675", "Fa2676", "Fa2677", "Fa2678", "Fa2679", "Fa2680", "Fa2681", "Fa2682", "Fa2683", "Fa2684", "Fa2685", "Fa2686", "Fa2687", "Fa2688", "Fa2689", "Fa2690", "Fa2691", "Fa2692", "Fa2693", "Fa2694", "Fa2695", "Fa2696", "Fa2697", "Fa2698", "Fa2699", "Fa2700", "Fa2701", "Fa2702", "Fa2703", "Fa2704", "Fa2705", "Fa2706", "Fa2707", "Fa2708", "Fa2709", "Fa2710", "Fa2711", "Fa2712", "Fa2713", "Fa2714", "Fa2715", "Fa2716", "Fa2717", "Fa2718", "Fa2719", "Fa2720", "Fa2721", "Fa2722", "Fa2723", "Fa2724", "Fa2725", "Fa2726", "Fa2727", "Fa2728", "Fa2729", "Fa2730", "Fa2731", "Fa2732", "Fa2733", "Fa2734", "Fa2735", "Fa2736", "Fa2737", "Fa2738", "Fa2739", "Fa2740"])
# Changement de discrétisation pour les faces Fa0010 Fa0051 Fa0055 Fa0059 Fa0061 ... 
ctx.getTopoManager().setMeshingProperty (Mgx3D.FaceMeshingPropertyTransfinite(),["Fa0010", "Fa0051", "Fa0055", "Fa0059", "Fa0061", "Fa0063", "Fa0070", "Fa0071", "Fa0072", "Fa0073", "Fa0074", "Fa0075", "Fa0109", "Fa0110", "Fa0113", "Fa0123", "Fa0131", "Fa0132", "Fa0133", "Fa0135", "Fa0137", "Fa0139", "Fa0141", "Fa0143", "Fa0144", "Fa0146", "Fa0148", "Fa0149", "Fa0150", "Fa0151", "Fa0153", "Fa0154", "Fa0155", "Fa0161", "Fa0163", "Fa0165", "Fa0169", "Fa0171", "Fa0173", "Fa0196", "Fa0198", "Fa0206", "Fa0213", "Fa0214", "Fa0217", "Fa0221", "Fa0222", "Fa0226", "Fa0232", "Fa0234", "Fa0235", "Fa0236", "Fa0238", "Fa0240", "Fa0241", "Fa0243", "Fa0253", "Fa0256", "Fa0258", "Fa0263", "Fa0265", "Fa0267", "Fa0273", "Fa0274", "Fa0275", "Fa0276", "Fa0277", "Fa0278", "Fa0279", "Fa0280", "Fa0285", "Fa0286", "Fa0293", "Fa0303", "Fa0304", "Fa0305", "Fa0307", "Fa0309", "Fa0313", "Fa0315", "Fa0321", "Fa0323", "Fa0325", "Fa0329", "Fa0331", "Fa0337", "Fa0339", "Fa0341", "Fa0345", "Fa0347", "Fa0353", "Fa0355", "Fa0357", "Fa0361", "Fa0363", "Fa0369", "Fa0371", "Fa0373", "Fa0377", "Fa0379", "Fa0385", "Fa0387", "Fa0389", "Fa0393", "Fa0395", "Fa0405", "Fa0406", "Fa0407", "Fa0408", "Fa0409", "Fa0411", "Fa0412", "Fa0413", "Fa0414", "Fa0415", "Fa0416", "Fa0417", "Fa0418", "Fa0419", "Fa0420", "Fa0423", "Fa0424", "Fa0427", "Fa0428", "Fa0432", "Fa0434", "Fa0436", "Fa0439", "Fa0441", "Fa0444", "Fa0446", "Fa0447", "Fa0449", "Fa0453", "Fa0457", "Fa0460", "Fa0462", "Fa0463", "Fa0464", "Fa0465", "Fa0466", "Fa0467", "Fa0468", "Fa0471", "Fa0472", "Fa0473", "Fa0474", "Fa0475", "Fa0476", "Fa0479", "Fa0480", "Fa0481", "Fa0482", "Fa0483", "Fa0484", "Fa0485", "Fa0486", "Fa0487", "Fa0488", "Fa0489", "Fa0490", "Fa0493", "Fa0494", "Fa0497", "Fa0498", "Fa0499", "Fa0500", "Fa0501", "Fa0502", "Fa0509", "Fa0510", "Fa0513", "Fa0514", "Fa0516", "Fa0519", "Fa0521", "Fa0522", "Fa0524", "Fa0527", "Fa0529", "Fa0530", "Fa0532", "Fa0535", "Fa0537", "Fa0538", "Fa0540", "Fa0543", "Fa0544", "Fa0545", "Fa0546", "Fa0547", "Fa0548", "Fa0549", "Fa0550", "Fa0551", "Fa0552", "Fa0553", "Fa0554", "Fa0555", "Fa0556", "Fa0557", "Fa0560", "Fa0561", "Fa0562", "Fa0563", "Fa0564", "Fa0565", "Fa0566", "Fa0567", "Fa0568", "Fa0569", "Fa0570", "Fa0571", "Fa0572", "Fa0573", "Fa0576", "Fa0577", "Fa0578", "Fa0579", "Fa0580", "Fa0581", "Fa0582", "Fa0583", "Fa0584", "Fa0585", "Fa0586", "Fa0587", "Fa0588", "Fa0589", "Fa0592", "Fa0593", "Fa0594", "Fa0595", "Fa0596", "Fa0597", "Fa0600", "Fa0601", "Fa0602", "Fa0603", "Fa0604", "Fa0605", "Fa0606", "Fa0607", "Fa0608", "Fa0609", "Fa0610", "Fa0611", "Fa0612", "Fa0613", "Fa0614", "Fa0615", "Fa0616", "Fa0617", "Fa0618", "Fa0619", "Fa0622", "Fa0623", "Fa0624", "Fa0625", "Fa0626", "Fa0627", "Fa0628", "Fa0629", "Fa0630", "Fa0631", "Fa0632", "Fa0633", "Fa0634", "Fa0635", "Fa0636", "Fa0637", "Fa0638", "Fa0639", "Fa0641", "Fa0642", "Fa0643", "Fa0645", "Fa0646", "Fa0647", "Fa0648", "Fa0649", "Fa0650", "Fa0651", "Fa0652", "Fa0653", "Fa0654", "Fa0655", "Fa0656", "Fa0657", "Fa0658", "Fa0659", "Fa0660", "Fa0661", "Fa0662", "Fa0663", "Fa0664", "Fa0665", "Fa0666", "Fa0667", "Fa0668", "Fa0669", "Fa0670", "Fa0671", "Fa0672", "Fa0673", "Fa0674", "Fa0675", "Fa0676", "Fa0677", "Fa0678", "Fa0679", "Fa0681", "Fa0682", "Fa0684", "Fa0685", "Fa0686", "Fa0687", "Fa0688", "Fa0689", "Fa0690", "Fa0691", "Fa0692", "Fa0693", "Fa0694", "Fa0695", "Fa0696", "Fa0697", "Fa0698", "Fa0699", "Fa0700", "Fa0701", "Fa0702", "Fa0703", "Fa0704", "Fa0705", "Fa0706", "Fa0707", "Fa0709", "Fa0710", "Fa0712", "Fa0713", "Fa0714", "Fa0715", "Fa0716", "Fa0717", "Fa0718", "Fa0719", "Fa0720", "Fa0721", "Fa0722", "Fa0723", "Fa0724", "Fa0725", "Fa0726", "Fa0727", "Fa0728", "Fa0729", "Fa0730", "Fa0731", "Fa0732", "Fa0733", "Fa0734", "Fa0735", "Fa0736", "Fa0737", "Fa0738", "Fa0739", "Fa0740", "Fa0741", "Fa0742", "Fa0743", "Fa0744", "Fa0745", "Fa0746", "Fa0747", "Fa0748", "Fa0749", "Fa0750", "Fa0751", "Fa0752", "Fa0753", "Fa0754", "Fa0755", "Fa0756", "Fa0757", "Fa0758", "Fa0759", "Fa0760", "Fa0761", "Fa0762", "Fa0763", "Fa0764", "Fa0765", "Fa0766", "Fa0767", "Fa0768", "Fa0769", "Fa0770", "Fa0771", "Fa0772", "Fa0773", "Fa0774", "Fa0775", "Fa0776", "Fa0777", "Fa0778", "Fa0779", "Fa0780", "Fa0781", "Fa0782", "Fa0783", "Fa0784", "Fa0785", "Fa0786", "Fa0787", "Fa0788", "Fa0789", "Fa0790", "Fa0791", "Fa0792", "Fa0793", "Fa0794", "Fa0795", "Fa0796", "Fa0797", "Fa0798", "Fa0799", "Fa0800", "Fa0801", "Fa0802", "Fa0803", "Fa0804", "Fa0805", "Fa0806", "Fa0807", "Fa0808", "Fa0809", "Fa0810", "Fa0811", "Fa0812", "Fa0813", "Fa0814", "Fa0815", "Fa0816", "Fa0817", "Fa0818", "Fa0819", "Fa0820", "Fa0821", "Fa0822", "Fa0823", "Fa0824", "Fa0825", "Fa0826", "Fa0827", "Fa0828", "Fa0829", "Fa0830", "Fa0831", "Fa0832", "Fa0833", "Fa0834", "Fa0835", "Fa0836", "Fa0837", "Fa0838", "Fa0839", "Fa0840", "Fa0841", "Fa0842", "Fa0843", "Fa0844", "Fa0845", "Fa0846", "Fa0847", "Fa0848", "Fa0849", "Fa0850", "Fa0851", "Fa0852", "Fa0853", "Fa0854", "Fa0855", "Fa0856", "Fa0857", "Fa0858", "Fa0859", "Fa0860", "Fa0861", "Fa0862", "Fa0863", "Fa0864", "Fa0865", "Fa0866", "Fa0867", "Fa0868", "Fa0869", "Fa0870", "Fa0871", "Fa0872", "Fa0873", "Fa0874", "Fa0875", "Fa0876", "Fa0877", "Fa0878", "Fa0879", "Fa0880", "Fa0881", "Fa0882", "Fa0883", "Fa0884", "Fa0885", "Fa0886", "Fa0887", "Fa0888", "Fa0889", "Fa0890", "Fa0891", "Fa0892", "Fa0893", "Fa0894", "Fa0895", "Fa0896", "Fa0897", "Fa0898", "Fa0899", "Fa0900", "Fa0901", "Fa0902", "Fa0903", "Fa0904", "Fa0905", "Fa0906", "Fa0907", "Fa0908", "Fa0909", "Fa0910", "Fa0911", "Fa0912", "Fa0913", "Fa0914", "Fa0915", "Fa0916", "Fa0917", "Fa0918", "Fa0919", "Fa0920", "Fa0921", "Fa0922", "Fa0923", "Fa0924", "Fa0925", "Fa0926", "Fa0927", "Fa0928", "Fa0929", "Fa0930", "Fa0931", "Fa0932", "Fa0933", "Fa0934", "Fa0935", "Fa0936", "Fa0937", "Fa0938", "Fa0939", "Fa0940", "Fa0941", "Fa0942", "Fa0943", "Fa0944", "Fa0945", "Fa0946", "Fa0947", "Fa0948", "Fa0949", "Fa0950", "Fa0951", "Fa0952", "Fa0953", "Fa0954", "Fa0955", "Fa0956", "Fa0957", "Fa0958", "Fa0959", "Fa0960", "Fa0961", "Fa0962", "Fa0963", "Fa0964", "Fa0965", "Fa0966", "Fa0967", "Fa0968", "Fa0969", "Fa0970", "Fa0971", "Fa0972", "Fa0973", "Fa0974", "Fa0975", "Fa0976", "Fa0977", "Fa0978", "Fa0979", "Fa0980", "Fa0981", "Fa0982", "Fa0983", "Fa0985", "Fa0987", "Fa0989", "Fa0990", "Fa0991", "Fa0993", "Fa0995", "Fa0997", "Fa0999", "Fa1001", "Fa1004", "Fa1006", "Fa1008", "Fa1010", "Fa1011", "Fa1013", "Fa1015", "Fa1017", "Fa1019", "Fa1021", "Fa1023", "Fa1025", "Fa1026", "Fa1027", "Fa1030", "Fa1031", "Fa1033", "Fa1035", "Fa1037", "Fa1039", "Fa1041", "Fa1043", "Fa1045", "Fa1047", "Fa1049", "Fa1055", "Fa1056", "Fa1057", "Fa1058", "Fa1059", "Fa1060", "Fa1061", "Fa1062", "Fa1063", "Fa1064", "Fa1065", "Fa1066", "Fa1067", "Fa1068", "Fa1069", "Fa1070", "Fa1071", "Fa1072", "Fa1073", "Fa1074", "Fa1075", "Fa1076", "Fa1077", "Fa1078", "Fa1079", "Fa1080", "Fa1081", "Fa1082", "Fa1083", "Fa1084", "Fa1085", "Fa1086", "Fa1087", "Fa1088", "Fa1089", "Fa1090", "Fa1091", "Fa1092", "Fa1093", "Fa1094", "Fa1095", "Fa1096", "Fa1097", "Fa1098", "Fa1099", "Fa1100", "Fa1101", "Fa1102", "Fa1103", "Fa1104", "Fa1105", "Fa1106", "Fa1107", "Fa1108", "Fa1109", "Fa1110", "Fa1111", "Fa1112", "Fa1113", "Fa1114", "Fa1115", "Fa1116", "Fa1117", "Fa1118", "Fa1119", "Fa1120", "Fa1121", "Fa1122", "Fa1123", "Fa1124", "Fa1125", "Fa1126", "Fa1127", "Fa1128", "Fa1129", "Fa1130", "Fa1131", "Fa1132", "Fa1133", "Fa1134", "Fa1135", "Fa1136", "Fa1137", "Fa1138", "Fa1139", "Fa1140", "Fa1141", "Fa1142", "Fa1143", "Fa1144", "Fa1145", "Fa1146", "Fa1147", "Fa1148", "Fa1149", "Fa1150", "Fa1151", "Fa1152", "Fa1153", "Fa1154", "Fa1155", "Fa1156", "Fa1157", "Fa1158", "Fa1159", "Fa1160", "Fa1161", "Fa1162", "Fa1163", "Fa1164", "Fa1165", "Fa1166", "Fa1167", "Fa1168", "Fa1169", "Fa1170", "Fa1171", "Fa1172", "Fa1173", "Fa1174", "Fa1175", "Fa1176", "Fa1177", "Fa1178", "Fa1179", "Fa1180", "Fa1181", "Fa1182", "Fa1183", "Fa1184", "Fa1185", "Fa1186", "Fa1187", "Fa1188", "Fa1189", "Fa1190", "Fa1191", "Fa1192", "Fa1193", "Fa1194", "Fa1195", "Fa1196", "Fa1197", "Fa1198", "Fa1199", "Fa1200", "Fa1201", "Fa1202", "Fa1203", "Fa1204", "Fa1205", "Fa1206", "Fa1207", "Fa1208", "Fa1209", "Fa1210", "Fa1211", "Fa1212", "Fa1213", "Fa1214", "Fa1215", "Fa1216", "Fa1217", "Fa1218", "Fa1219", "Fa1220", "Fa1221", "Fa1222", "Fa1223", "Fa1224", "Fa1225", "Fa1226", "Fa1227", "Fa1228", "Fa1229", "Fa1230", "Fa1231", "Fa1232", "Fa1233", "Fa1234", "Fa1235", "Fa1236", "Fa1237", "Fa1238", "Fa1239", "Fa1240", "Fa1241", "Fa1242", "Fa1243", "Fa1244", "Fa1245", "Fa1246", "Fa1247", "Fa1248", "Fa1249", "Fa1250", "Fa1251", "Fa1252", "Fa1253", "Fa1254", "Fa1255", "Fa1256", "Fa1257", "Fa1258", "Fa1259", "Fa1260", "Fa1261", "Fa1262", "Fa1263", "Fa1264", "Fa1265", "Fa1266", "Fa1267", "Fa1268", "Fa1269", "Fa1270", "Fa1271", "Fa1272", "Fa1273", "Fa1274", "Fa1275", "Fa1276", "Fa1277", "Fa1278", "Fa1279", "Fa1280", "Fa1281", "Fa1282", "Fa1283", "Fa1284", "Fa1285", "Fa1286", "Fa1287", "Fa1288", "Fa1289", "Fa1290", "Fa1291", "Fa1292", "Fa1293", "Fa1294", "Fa1295", "Fa1296", "Fa1297", "Fa1298", "Fa1299", "Fa1300", "Fa1301", "Fa1302", "Fa1303", "Fa1304", "Fa1305", "Fa1306", "Fa1307", "Fa1308", "Fa1309", "Fa1310", "Fa1311", "Fa1312", "Fa1313", "Fa1314", "Fa1315", "Fa1316", "Fa1317", "Fa1318", "Fa1319", "Fa1320", "Fa1321", "Fa1322", "Fa1323", "Fa1324", "Fa1325", "Fa1326", "Fa1327", "Fa1328", "Fa1329", "Fa1330", "Fa1331", "Fa1332", "Fa1333", "Fa1334", "Fa1335", "Fa1336", "Fa1337", "Fa1338", "Fa1339", "Fa1340", "Fa1341", "Fa1342", "Fa1343", "Fa1344", "Fa1345", "Fa1346", "Fa1347", "Fa1348", "Fa1349", "Fa1350", "Fa1351", "Fa1352", "Fa1353", "Fa1354", "Fa1355", "Fa1356", "Fa1357", "Fa1358", "Fa1359", "Fa1360", "Fa1361", "Fa1362", "Fa1363", "Fa1364", "Fa1365", "Fa1366", "Fa1367", "Fa1368", "Fa1369", "Fa1370", "Fa1371", "Fa1372", "Fa1373", "Fa1374", "Fa1375", "Fa1376", "Fa1377", "Fa1378", "Fa1379", "Fa1380", "Fa1381", "Fa1382", "Fa1383", "Fa1384", "Fa1385", "Fa1386", "Fa1387", "Fa1388", "Fa1389", "Fa1390", "Fa1391", "Fa1392", "Fa1393", "Fa1394", "Fa1395", "Fa1396", "Fa1397", "Fa1398", "Fa1399", "Fa1400", "Fa1401", "Fa1402", "Fa1403", "Fa1404", "Fa1405", "Fa1406", "Fa1407", "Fa1408", "Fa1409", "Fa1410", "Fa1411", "Fa1412", "Fa1413", "Fa1414", "Fa1415", "Fa1416", "Fa1417", "Fa1418", "Fa1419", "Fa1420", "Fa1421", "Fa1422", "Fa1423", "Fa1424", "Fa1425", "Fa1426", "Fa1427", "Fa1428", "Fa1429", "Fa1430", "Fa1431", "Fa1432", "Fa1433", "Fa1434", "Fa1435", "Fa1436", "Fa1437", "Fa1438", "Fa1439", "Fa1440", "Fa1441", "Fa1442", "Fa1443", "Fa1444", "Fa1445", "Fa1446", "Fa1447", "Fa1448", "Fa1449", "Fa1450", "Fa1451", "Fa1452", "Fa1453", "Fa1454", "Fa1455", "Fa1456", "Fa1457", "Fa1458", "Fa1459", "Fa1460", "Fa1461", "Fa1462", "Fa1463", "Fa1464", "Fa1465", "Fa1466", "Fa1467", "Fa1468", "Fa1469", "Fa1470", "Fa1471", "Fa1472", "Fa1473", "Fa1474", "Fa1475", "Fa1476", "Fa1477", "Fa1478", "Fa1479", "Fa1480", "Fa1481", "Fa1482", "Fa1483", "Fa1484", "Fa1485", "Fa1486", "Fa1487", "Fa1488", "Fa1489", "Fa1490", "Fa1491", "Fa1492", "Fa1493", "Fa1494", "Fa1495", "Fa1496", "Fa1497", "Fa1498", "Fa1499", "Fa1500", "Fa1501", "Fa1502", "Fa1503", "Fa1504", "Fa1505", "Fa1506", "Fa1507", "Fa1508", "Fa1509", "Fa1510", "Fa1511", "Fa1512", "Fa1513", "Fa1514", "Fa1515", "Fa1516", "Fa1517", "Fa1518", "Fa1519", "Fa1520", "Fa1521", "Fa1522", "Fa1523", "Fa1524", "Fa1525", "Fa1526", "Fa1527", "Fa1528", "Fa1529", "Fa1530", "Fa1531", "Fa1532", "Fa1533", "Fa1534", "Fa1535", "Fa1536", "Fa1537", "Fa1538", "Fa1539", "Fa1540", "Fa1541", "Fa1542", "Fa1543", "Fa1544", "Fa1545", "Fa1546", "Fa1547", "Fa1548", "Fa1549", "Fa1550", "Fa1551", "Fa1552", "Fa1553", "Fa1554", "Fa1555", "Fa1556", "Fa1557", "Fa1558", "Fa1559", "Fa1560", "Fa1561", "Fa1562", "Fa1563", "Fa1564", "Fa1565", "Fa1566", "Fa1567", "Fa1568", "Fa1569", "Fa1570", "Fa1571", "Fa1572", "Fa1573", "Fa1574", "Fa1575", "Fa1576", "Fa1577", "Fa1578", "Fa1579", "Fa1580", "Fa1581", "Fa1582", "Fa1583", "Fa1584", "Fa1585", "Fa1586", "Fa1587", "Fa1588", "Fa1589", "Fa1590", "Fa1591", "Fa1592", "Fa1593", "Fa1594", "Fa1595", "Fa1596", "Fa1597", "Fa1598", "Fa1599", "Fa1600", "Fa1601", "Fa1602", "Fa1603", "Fa1604", "Fa1605", "Fa1606", "Fa1607", "Fa1608", "Fa1609", "Fa1610", "Fa1611", "Fa1612", "Fa1613", "Fa1614", "Fa1615", "Fa1616", "Fa1617", "Fa1618", "Fa1619", "Fa1620", "Fa1621", "Fa1622", "Fa1623", "Fa1624", "Fa1625", "Fa1626", "Fa1627", "Fa1628", "Fa1629", "Fa1630", "Fa1631", "Fa1632", "Fa1633", "Fa1634", "Fa1635", "Fa1636", "Fa1637", "Fa1638", "Fa1639", "Fa1640", "Fa1641", "Fa1642", "Fa1643", "Fa1644", "Fa1645", "Fa1646", "Fa1647", "Fa1648", "Fa1649", "Fa1650", "Fa1651", "Fa1652", "Fa1653", "Fa1654", "Fa1655", "Fa1656", "Fa1657", "Fa1658", "Fa1659", "Fa1660", "Fa1661", "Fa1662", "Fa1663", "Fa1664", "Fa1665", "Fa1666", "Fa1667", "Fa1668", "Fa1669", "Fa1670", "Fa1671", "Fa1672", "Fa1673", "Fa1674", "Fa1675", "Fa1676", "Fa1677", "Fa1678", "Fa1679", "Fa1680", "Fa1681", "Fa1682", "Fa1683", "Fa1684", "Fa1685", "Fa1686", "Fa1687", "Fa1688", "Fa1689", "Fa1690", "Fa1691", "Fa1692", "Fa1693", "Fa1694", "Fa1695", "Fa1696", "Fa1697", "Fa1698", "Fa1699", "Fa1700", "Fa1701", "Fa1702", "Fa1703", "Fa1704", "Fa1705", "Fa1706", "Fa1707", "Fa1708", "Fa1709", "Fa1710", "Fa1711", "Fa1712", "Fa1713", "Fa1714", "Fa1715", "Fa1716", "Fa1717", "Fa1718", "Fa1719", "Fa1720", "Fa1721", "Fa1722", "Fa1723", "Fa1724", "Fa1725", "Fa1726", "Fa1727", "Fa1728", "Fa1729", "Fa1730", "Fa1731", "Fa1732", "Fa1733", "Fa1734", "Fa1735", "Fa1736", "Fa1737", "Fa1738", "Fa1739", "Fa1740", "Fa1741", "Fa1742", "Fa1743", "Fa1744", "Fa1745", "Fa1746", "Fa1747", "Fa1748", "Fa1749", "Fa1750", "Fa1751", "Fa1752", "Fa1753", "Fa1754", "Fa1755", "Fa1756", "Fa1757", "Fa1758", "Fa1759", "Fa1760", "Fa1761", "Fa1762", "Fa1763", "Fa1764", "Fa1765", "Fa1766", "Fa1767", "Fa1768", "Fa1769", "Fa1770", "Fa1771", "Fa1772", "Fa1773", "Fa1774", "Fa1775", "Fa1776", "Fa1777", "Fa1778", "Fa1779", "Fa1780", "Fa1781", "Fa1782", "Fa1783", "Fa1784", "Fa1785", "Fa1786", "Fa1787", "Fa1788", "Fa1789", "Fa1790", "Fa1791", "Fa1792", "Fa1793", "Fa1794", "Fa1795", "Fa1796", "Fa1797", "Fa1798", "Fa1799", "Fa1800", "Fa1801", "Fa1802", "Fa1803", "Fa1804", "Fa1805", "Fa1806", "Fa1807", "Fa1808", "Fa1809", "Fa1810", "Fa1811", "Fa1812", "Fa1813", "Fa1814", "Fa1815", "Fa1816", "Fa1817", "Fa1818", "Fa1819", "Fa1820", "Fa1821", "Fa1822", "Fa1823", "Fa1824", "Fa1825", "Fa1826", "Fa1827", "Fa1828", "Fa1829", "Fa1830", "Fa1831", "Fa1832", "Fa1833", "Fa1834", "Fa1835", "Fa1836", "Fa1837", "Fa1838", "Fa1839", "Fa1840", "Fa1841", "Fa1842", "Fa1843", "Fa1844", "Fa1845", "Fa1846", "Fa1847", "Fa1848", "Fa1849", "Fa1850", "Fa1851", "Fa1852", "Fa1853", "Fa1854", "Fa1855", "Fa1856", "Fa1857", "Fa1858", "Fa1859", "Fa1860", "Fa1861", "Fa1862", "Fa1863", "Fa1864", "Fa1865", "Fa1866", "Fa1867", "Fa1868", "Fa1869", "Fa1870", "Fa1871", "Fa1872", "Fa1873", "Fa1874", "Fa1875", "Fa1876", "Fa1877", "Fa1878", "Fa1879", "Fa1880", "Fa1881", "Fa1882", "Fa1883", "Fa1884", "Fa1885", "Fa1886", "Fa1887", "Fa1888", "Fa1889", "Fa1890", "Fa1891", "Fa1892", "Fa1893", "Fa1894", "Fa1895", "Fa1896", "Fa1897", "Fa1898", "Fa1899", "Fa1900", "Fa1901", "Fa1902", "Fa1903", "Fa1904", "Fa1905", "Fa1906", "Fa1907", "Fa1908", "Fa1909", "Fa1910", "Fa1911", "Fa1912", "Fa1913", "Fa1914", "Fa1915", "Fa1916", "Fa1917", "Fa1918", "Fa1919", "Fa1920", "Fa1921", "Fa1922", "Fa1923", "Fa1924", "Fa1925", "Fa1926", "Fa1927", "Fa1928", "Fa1929", "Fa1930", "Fa1931", "Fa1932", "Fa1933", "Fa1934", "Fa1935", "Fa1936", "Fa1937", "Fa1938", "Fa1939", "Fa1940", "Fa1941", "Fa1942", "Fa1943", "Fa1944", "Fa1945", "Fa1946", "Fa1947", "Fa1948", "Fa1949", "Fa1950", "Fa1951", "Fa1952", "Fa1953", "Fa1954", "Fa1955", "Fa1956", "Fa1957", "Fa1958", "Fa1959", "Fa1960", "Fa1961", "Fa1962", "Fa1963", "Fa1964", "Fa1965", "Fa1966", "Fa1967", "Fa1968", "Fa1969", "Fa1970", "Fa1971", "Fa1972", "Fa1973", "Fa1974", "Fa1975", "Fa1976", "Fa1977", "Fa1978", "Fa1979", "Fa1980", "Fa1981", "Fa1982", "Fa1983", "Fa1984", "Fa1985", "Fa1986", "Fa1987", "Fa1988", "Fa1989", "Fa1990", "Fa1991", "Fa1992", "Fa1993", "Fa1994", "Fa1995", "Fa1996", "Fa1997", "Fa1998", "Fa1999", "Fa2000", "Fa2001", "Fa2002", "Fa2003", "Fa2004", "Fa2005", "Fa2006", "Fa2007", "Fa2008", "Fa2009", "Fa2010", "Fa2011", "Fa2012", "Fa2013", "Fa2014", "Fa2015", "Fa2016", "Fa2017", "Fa2018", "Fa2019", "Fa2020", "Fa2021", "Fa2022", "Fa2023", "Fa2024", "Fa2025", "Fa2026", "Fa2027", "Fa2028", "Fa2029", "Fa2030", "Fa2031", "Fa2032", "Fa2033", "Fa2034", "Fa2035", "Fa2036", "Fa2037", "Fa2038", "Fa2039", "Fa2040", "Fa2041", "Fa2042", "Fa2043", "Fa2044", "Fa2045", "Fa2046", "Fa2047", "Fa2048", "Fa2049", "Fa2050", "Fa2051", "Fa2052", "Fa2053", "Fa2054", "Fa2055", "Fa2056", "Fa2057", "Fa2058", "Fa2059", "Fa2060", "Fa2061", "Fa2062", "Fa2063", "Fa2064", "Fa2065", "Fa2066", "Fa2067", "Fa2068", "Fa2069", "Fa2070", "Fa2071", "Fa2072", "Fa2073", "Fa2074", "Fa2075", "Fa2076", "Fa2077", "Fa2078", "Fa2079", "Fa2080", "Fa2081", "Fa2082", "Fa2083", "Fa2084", "Fa2085", "Fa2086", "Fa2087", "Fa2088", "Fa2089", "Fa2090", "Fa2091", "Fa2092", "Fa2093", "Fa2094", "Fa2095", "Fa2096", "Fa2097", "Fa2098", "Fa2099", "Fa2100", "Fa2101", "Fa2102", "Fa2103", "Fa2104", "Fa2105", "Fa2106", "Fa2107", "Fa2108", "Fa2109", "Fa2110", "Fa2111", "Fa2112", "Fa2113", "Fa2114", "Fa2115", "Fa2116", "Fa2117", "Fa2118", "Fa2119", "Fa2120", "Fa2121", "Fa2122", "Fa2123", "Fa2124", "Fa2125", "Fa2126", "Fa2127", "Fa2128", "Fa2129", "Fa2130", "Fa2131", "Fa2132", "Fa2133", "Fa2134", "Fa2135", "Fa2136", "Fa2137", "Fa2138", "Fa2139", "Fa2140", "Fa2141", "Fa2142", "Fa2143", "Fa2144", "Fa2145", "Fa2146", "Fa2147", "Fa2148", "Fa2149", "Fa2150", "Fa2151", "Fa2152", "Fa2153", "Fa2154", "Fa2155", "Fa2156", "Fa2157", "Fa2158", "Fa2159", "Fa2160", "Fa2161", "Fa2162", "Fa2163", "Fa2164", "Fa2165", "Fa2166", "Fa2167", "Fa2168", "Fa2169", "Fa2170", "Fa2171", "Fa2172", "Fa2173", "Fa2174", "Fa2175", "Fa2176", "Fa2177", "Fa2178", "Fa2179", "Fa2180", "Fa2181", "Fa2182", "Fa2183", "Fa2184", "Fa2185", "Fa2186", "Fa2187", "Fa2188", "Fa2189", "Fa2190", "Fa2191", "Fa2192", "Fa2193", "Fa2194", "Fa2195", "Fa2196", "Fa2197", "Fa2198", "Fa2199", "Fa2200", "Fa2201", "Fa2202", "Fa2203", "Fa2204", "Fa2205", "Fa2206", "Fa2207", "Fa2208", "Fa2209", "Fa2210", "Fa2211", "Fa2212", "Fa2213", "Fa2214", "Fa2215", "Fa2216", "Fa2217", "Fa2218", "Fa2219", "Fa2220", "Fa2221", "Fa2222", "Fa2223", "Fa2224", "Fa2225", "Fa2226", "Fa2227", "Fa2228", "Fa2229", "Fa2230", "Fa2231", "Fa2232", "Fa2233", "Fa2234", "Fa2235", "Fa2236", "Fa2237", "Fa2238", "Fa2239", "Fa2240", "Fa2241", "Fa2242", "Fa2243", "Fa2244", "Fa2245", "Fa2246", "Fa2247", "Fa2248", "Fa2249", "Fa2250", "Fa2251", "Fa2252", "Fa2253", "Fa2254", "Fa2255", "Fa2256", "Fa2257", "Fa2258", "Fa2259", "Fa2260", "Fa2261", "Fa2262", "Fa2263", "Fa2264", "Fa2265", "Fa2266", "Fa2267", "Fa2268", "Fa2269", "Fa2270", "Fa2271", "Fa2272", "Fa2273", "Fa2274", "Fa2275", "Fa2276", "Fa2277", "Fa2278", "Fa2279", "Fa2280", "Fa2281", "Fa2282", "Fa2283", "Fa2284", "Fa2285", "Fa2286", "Fa2287", "Fa2288", "Fa2289", "Fa2290", "Fa2291", "Fa2292", "Fa2293", "Fa2294", "Fa2295", "Fa2296", "Fa2297", "Fa2298", "Fa2299", "Fa2300", "Fa2301", "Fa2302", "Fa2303", "Fa2304", "Fa2305", "Fa2306", "Fa2307", "Fa2308", "Fa2309", "Fa2310", "Fa2311", "Fa2312", "Fa2313", "Fa2314", "Fa2315", "Fa2316", "Fa2317", "Fa2318", "Fa2319", "Fa2320", "Fa2321", "Fa2322", "Fa2323", "Fa2324", "Fa2325", "Fa2326", "Fa2327", "Fa2328", "Fa2329", "Fa2330", "Fa2331", "Fa2332", "Fa2333", "Fa2334", "Fa2335", "Fa2336", "Fa2337", "Fa2338", "Fa2339", "Fa2340", "Fa2341", "Fa2342", "Fa2343", "Fa2344", "Fa2345", "Fa2346", "Fa2347", "Fa2348", "Fa2349", "Fa2350", "Fa2351", "Fa2352", "Fa2353", "Fa2354", "Fa2355", "Fa2356", "Fa2357", "Fa2358", "Fa2359", "Fa2360", "Fa2361", "Fa2362", "Fa2363", "Fa2364", "Fa2365", "Fa2366", "Fa2367", "Fa2368", "Fa2369", "Fa2370", "Fa2371", "Fa2372", "Fa2373", "Fa2374", "Fa2375", "Fa2376", "Fa2377", "Fa2378", "Fa2379", "Fa2380", "Fa2381", "Fa2382", "Fa2383", "Fa2384", "Fa2385", "Fa2386", "Fa2387", "Fa2388", "Fa2389", "Fa2390", "Fa2391", "Fa2392", "Fa2393", "Fa2394", "Fa2395", "Fa2396", "Fa2397", "Fa2398", "Fa2399", "Fa2400", "Fa2401", "Fa2402", "Fa2403", "Fa2404", "Fa2405", "Fa2406", "Fa2407", "Fa2408", "Fa2409", "Fa2410", "Fa2411", "Fa2412", "Fa2413", "Fa2414", "Fa2415", "Fa2416", "Fa2417", "Fa2418", "Fa2419", "Fa2420", "Fa2421", "Fa2422", "Fa2423", "Fa2424", "Fa2425", "Fa2426", "Fa2427", "Fa2428", "Fa2429", "Fa2430", "Fa2431", "Fa2432", "Fa2433", "Fa2434", "Fa2435", "Fa2436", "Fa2437", "Fa2438", "Fa2439", "Fa2440", "Fa2441", "Fa2442", "Fa2443", "Fa2444", "Fa2445", "Fa2446", "Fa2447", "Fa2448", "Fa2449", "Fa2450", "Fa2451", "Fa2452", "Fa2453", "Fa2454", "Fa2455", "Fa2456", "Fa2457", "Fa2458", "Fa2459", "Fa2460", "Fa2461", "Fa2462", "Fa2463", "Fa2464", "Fa2465", "Fa2466", "Fa2467", "Fa2468", "Fa2469", "Fa2470", "Fa2471", "Fa2472", "Fa2473", "Fa2474", "Fa2475", "Fa2476", "Fa2477", "Fa2478", "Fa2479", "Fa2480", "Fa2481", "Fa2482", "Fa2483", "Fa2484", "Fa2485", "Fa2486", "Fa2487", "Fa2488", "Fa2489", "Fa2490", "Fa2491", "Fa2492", "Fa2493", "Fa2494", "Fa2495", "Fa2496", "Fa2497", "Fa2498", "Fa2499", "Fa2500", "Fa2501", "Fa2502", "Fa2503", "Fa2504", "Fa2505", "Fa2506", "Fa2507", "Fa2508", "Fa2509", "Fa2510", "Fa2511", "Fa2512", "Fa2513", "Fa2514", "Fa2515", "Fa2516", "Fa2517", "Fa2518", "Fa2519", "Fa2520", "Fa2521", "Fa2522", "Fa2523", "Fa2524", "Fa2525", "Fa2526", "Fa2527", "Fa2528", "Fa2529", "Fa2530", "Fa2531", "Fa2532", "Fa2533", "Fa2534", "Fa2535", "Fa2536", "Fa2537", "Fa2538", "Fa2539", "Fa2540", "Fa2541", "Fa2542", "Fa2543", "Fa2544", "Fa2545", "Fa2546", "Fa2547", "Fa2548", "Fa2549", "Fa2550", "Fa2551", "Fa2552", "Fa2553", "Fa2554", "Fa2555", "Fa2556", "Fa2557", "Fa2558", "Fa2559", "Fa2560", "Fa2561", "Fa2562", "Fa2563", "Fa2564", "Fa2565", "Fa2566", "Fa2567", "Fa2568", "Fa2569", "Fa2570", "Fa2571", "Fa2572", "Fa2573", "Fa2574", "Fa2575", "Fa2576", "Fa2577", "Fa2578", "Fa2579", "Fa2580", "Fa2581", "Fa2582", "Fa2583", "Fa2584", "Fa2585", "Fa2586", "Fa2587", "Fa2588", "Fa2589", "Fa2590", "Fa2591", "Fa2592", "Fa2593", "Fa2594", "Fa2595", "Fa2596", "Fa2597", "Fa2598", "Fa2599", "Fa2600", "Fa2601", "Fa2602", "Fa2603", "Fa2604", "Fa2605", "Fa2606", "Fa2607", "Fa2608", "Fa2609", "Fa2610", "Fa2611", "Fa2612", "Fa2613", "Fa2614", "Fa2615", "Fa2616", "Fa2617", "Fa2618", "Fa2619", "Fa2620", "Fa2621", "Fa2622", "Fa2623", "Fa2624", "Fa2625", "Fa2626", "Fa2627", "Fa2628", "Fa2629", "Fa2630", "Fa2631", "Fa2632", "Fa2633", "Fa2634", "Fa2635", "Fa2636", "Fa2637", "Fa2638", "Fa2639", "Fa2640", "Fa2641", "Fa2642", "Fa2643", "Fa2644", "Fa2645", "Fa2646", "Fa2647", "Fa2648", "Fa2649", "Fa2650", "Fa2651", "Fa2652", "Fa2653", "Fa2654", "Fa2655", "Fa2656", "Fa2657", "Fa2658", "Fa2659", "Fa2660", "Fa2661", "Fa2662", "Fa2663", "Fa2664", "Fa2665", "Fa2666", "Fa2667", "Fa2668", "Fa2669", "Fa2670", "Fa2671", "Fa2672", "Fa2673", "Fa2674", "Fa2675", "Fa2676", "Fa2677", "Fa2678", "Fa2679", "Fa2680", "Fa2681", "Fa2682", "Fa2683", "Fa2684", "Fa2685", "Fa2686", "Fa2687", "Fa2688", "Fa2689", "Fa2690", "Fa2691", "Fa2692", "Fa2693", "Fa2694", "Fa2695", "Fa2696", "Fa2697", "Fa2698", "Fa2699", "Fa2700", "Fa2701", "Fa2702", "Fa2703", "Fa2704", "Fa2705", "Fa2706", "Fa2707", "Fa2708", "Fa2709", "Fa2710", "Fa2711", "Fa2712", "Fa2713", "Fa2714", "Fa2715", "Fa2716", "Fa2717", "Fa2718", "Fa2719", "Fa2720", "Fa2721", "Fa2722", "Fa2723", "Fa2724", "Fa2725", "Fa2726", "Fa2727", "Fa2728", "Fa2729", "Fa2730", "Fa2731", "Fa2732", "Fa2733", "Fa2734", "Fa2735", "Fa2736", "Fa2737", "Fa2738", "Fa2739", "Fa2740"])
# Changement de discrétisation pour les arêtes Ar2801
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar2804"], ["Ar2806"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar2801"])
# Changement de discrétisation pour les arêtes Ar2815
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar2975"], ["Ar2804"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar2815"])
# Changement de discrétisation pour les arêtes Ar3150
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar3153"], ["Ar3224"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar3150"])
# Changement de discrétisation pour les arêtes Ar3164
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar3324"], ["Ar3226"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar3164"])
# Changement de discrétisation pour les arêtes Ar3164
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar3324"], ["Ar3153"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar3164"])
# Changement de discrétisation pour les arêtes Ar2818 Ar2820 Ar2833 Ar2834
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar2819"], ["Ar2824"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar2818", "Ar2820", "Ar2833", "Ar2834"])
# Changement de discrétisation pour les arêtes Ar3167 Ar3169 Ar3182 Ar3183
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar3168"], ["Ar3173"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar3167", "Ar3169", "Ar3182", "Ar3183"])
# Changement de discrétisation pour les arêtes Ar3179 Ar3181 Ar3156 Ar3154
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar3155"], ["Ar3172"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar3179", "Ar3181", "Ar3156", "Ar3154"])
# Changement de discrétisation pour les arêtes Ar2830 Ar2832 Ar2807 Ar2805
emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(5, ["Ar2806"], ["Ar2823"])
ctx.getTopoManager().setMeshingProperty (emp,["Ar2830", "Ar2832", "Ar2807", "Ar2805"])
