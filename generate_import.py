import os
import sys
import pyMagix3D as Mgx3D
from collections import defaultdict

def generer_script(ctx, export_folder):
    script = """# Script généré automatiquement pour recréer les entités Magix3D
# Ne pas modifier manuellement

"""
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    script += "import pyMagix3D as Mgx3D\n"
    script += "import itertools\n"
    script += "from collections import defaultdict\n\n"

    script += "ctx = Mgx3D.getStdContext()\n"
    script += "ctx.clearSession()\n"
    script += "gm = ctx.getGeomManager()\n"
    script += "tm = ctx.getTopoManager()\n"
    script += "gpm = ctx.getGroupManager()\n"
    script += "mm = ctx.getMeshManager()\n"
    script += "\n"

    script += "# Création des points\n"
    script += "points_mapping = dict()\n"
    for vertex in gm.getVertices():
        script += f'# Création du sommet {vertex}\n'
        groups = gm.getInfos(vertex, 0).groups()
        if 'Hors_Groupe_0D' in groups:
            groups.remove('Hors_Groupe_0D')
        point_file_name = f"{brep_folder}/{vertex + '.brep'}"
        script += f'cmd = gm.newVertexFromBrep("{point_file_name}")\n'
        script += 'point = cmd.getVertices()[0]\n'
        for group in groups:
            script += f"gm.addToGroup ([point], 0, '{group}')\n"
        script += f'points_mapping["{vertex}"] = point\n'
    script += "\n"
    script += "print(\"Correspondance des points:\")\n"
    script += "print(points_mapping, '\\n')\n\n"

    script += "# Création des courbes\n"
    script += "curves_mapping = dict()\n"
    for curve in gm.getCurves():
        script += f'# Création de la courbe {curve}\n'
        groups = gm.getInfos(curve, 1).groups()
        if 'Hors_Groupe_1D' in groups:
            groups.remove('Hors_Groupe_1D')
        curve_file_name = f"{brep_folder}/{curve + '.brep'}"
        extrema_first_point = gm.getInfos(curve, 1).vertices()[0]
        extrema_last_point = gm.getInfos(curve, 1).vertices()[-1]
        script += f'cmd = gm.newCurveFromBrep("{curve_file_name}", points_mapping["{extrema_first_point}"], points_mapping["{extrema_last_point}"])\n'
        script += 'curve = cmd.getCurves()[0]\n'
        for group in groups:
            script += f"gm.addToGroup ([curve], 1, '{group}')\n"
        script += f'curves_mapping["{curve}"] = curve\n'
    script += "\n"
    script += "print(\"Correspondance des courbes:\")\n"
    script += "print(curves_mapping, '\\n')\n\n"

    script += "# Création des surfaces\n"
    script += "surfaces_mapping = dict()\n"
    for surface in gm.getSurfaces():
        script += f'# Création de la surface {surface}\n'
        groups = gm.getInfos(surface, 2).groups()
        if 'Hors_Groupe_2D' in groups:
            groups.remove('Hors_Groupe_2D')
        surface_file_name = f"{brep_folder}/{surface + '.brep'}"
        script += f'cmd = gm.newSurfaceFromBrep("{surface_file_name}")\n'
        script += 'surface = cmd.getSurfaces()[0]\n'
        for group in groups:
            script += f"gm.addToGroup ([surface], 2, '{group}')\n"
        script += f'surfaces_mapping["{surface}"] = surface\n'
    script += "\n"
    script += "print(\"Correspondance des surfaces:\")\n"
    script += "print(surfaces_mapping, '\\n')\n"

    script += "# Création des volumes\n"
    script += "volumes_mapping = dict()\n"
    for volume in gm.getVolumes():
        script += f'# Création du volume {volume}\n'
        groups = gm.getInfos(volume, 3).groups()
        if 'Hors_Groupe_3D' in groups:
            groups.remove('Hors_Groupe_3D')
        volume_file_name = f"{brep_folder}/{volume + '.brep'}"
        script += f'cmd = gm.newVolumeFromBrep("{volume_file_name}")\n'
        script += 'volume = cmd.getVolumes()[0]\n'
        for group in groups:
            script += f"gm.addToGroup ([volume], 3, '{group}')\n"
        script += f'volumes_mapping["{volume}"] = volume\n'
    script += "\n"
    script += "print(\"Correspondance des volumes:\")\n"
    script += "print(volumes_mapping, '\\n')\n\n"

    script += "def find_in_dicts(key):\n"
    script += "    for d in points_mapping, curves_mapping, surfaces_mapping, volumes_mapping:\n"
    script += "            if key in d:\n"
    script += "                return d[key]\n"
    script += "    # Si la clé n'est pas trouvée dans aucun dictionnaire\n"
    script += "    return None\n\n"

    script += "# Création des sommets\n"
    script += "vertices_mapping = dict()\n"
    for vertex in tm.getVertices():
        script += f'# Création du sommet {vertex}\n'
        coord = tm.getCoord(vertex)
        script += f'cmd = tm.newTopoVertex(Mgx3D.Point({coord.getX()}, {coord.getY()}, {coord.getZ()}), "")\n'
        script += 'v = cmd.getTopoVertices()[0]\n'
        groups = tm.getInfos(vertex, 0).groups()
        if 'Hors_Groupe_0D' in groups:
            groups.remove('Hors_Groupe_0D')
        if groups != []:
            for group in groups:
                script += f"tm.addToGroup ([v], 0, \"{group}\")\n"
        geom_entity = tm.getInfos(vertex, 0).geom_entity
        if geom_entity:
            script += f"# Le sommet {vertex} est associé à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([v], find_in_dicts('{geom_entity}'), False)\n"
        script += f'vertices_mapping["{vertex}"] = v\n'
    script += "print(\"Correspondance des sommets:\")\n"
    script += "print(vertices_mapping, '\\n')\n\n"
    script += "\n"

    script += "# Création des arêtes\n"
    script += "coedges_mapping = dict()\n"
    for coedge in tm.getCoEdges():
        script += f'# Création de l\'arête {coedge}\n'
        start_vertex = tm.getInfos(coedge, 1).vertices()[0]
        end_vertex = tm.getInfos(coedge, 1).vertices()[1]
        script += f'cmd = tm.newTopoEntity([vertices_mapping["{start_vertex}"], vertices_mapping["{end_vertex}"]], 1, "", True)\n'
        script += 'e = cmd.getEdges()[0]\n'
        groups = tm.getInfos(coedge, 1).groups()
        if 'Hors_Groupe_1D' in groups:
            groups.remove('Hors_Groupe_1D')
        if groups != []:
            for group in groups:
                script += f"tm.addToGroup ([e], 1, \"{group}\")\n"
        geom_entity = tm.getInfos(coedge, 1).geom_entity
        if geom_entity:
            script += "try:\n"
            script += f"    # L'arête {coedge} est associée à l'entité géométrique {geom_entity}\n"
            script += f"    tm.setGeomAssociation([e], find_in_dicts('{geom_entity}'), True)\n"
            script += "except:\n"
            script += "    pass\n"
        script += f'coedges_mapping["{coedge}"] = e\n'
    script += "print(\"Correspondance des arêtes:\")\n"
    script += "print(coedges_mapping, '\\n')\n\n"
    script += "\n"

    script += "# Application des propriétés de maillage sur les arêtes\n"
    for coedge in tm.getCoEdges():
        script += f'# Discrétisation de l\'arête {coedge}\n'
        emp = tm.getEdgeMeshingProperty(coedge)
        law = emp.getMeshLaw()
        if (law == Mgx3D.CoEdgeMeshingProperty.uniforme):
            uemp = Mgx3D.EdgeMeshingPropertyUniform(emp)
            script += f'emp = Mgx3D.EdgeMeshingPropertyUniform({uemp.getNbEdges()})\n'
        elif (law == Mgx3D.CoEdgeMeshingProperty.globalinterpolate):
            giemp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(emp)
            script += f'emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate({giemp.getNbEdges()}, [", ".join([coedges_mapping[coedge] for coedge in {giemp.getFirstCoEdges()}])], [", ".join([coedges_mapping[coedge] for coedge in {giemp.getSecondCoEdges()}])])\n'
        else:
            raise ValueError(f"Loi de maillage pour les arêtes non traitée: {law}")
        script += f"tm.setMeshingProperty (emp, [coedges_mapping['{coedge}']])\n"

    script += "# Création des cofaces\n"
    script += "cofaces_mapping = dict()\n"
    edge_id = 0
    faces_cofaces_map = defaultdict(list)
    faces_vertices_map = defaultdict(list)
    for coface in tm.getCoFaces():
        script += f'# Création de la coface {coface}\n'
        edges_list = tm.getInfos(coface, 2).edges()
        script += "coface_edges = []\n"
        hasHole = False
        for edge in edges_list:
            v1 = tm.getInfos(edge, 1).vertices()[0]
            v2 = tm.getInfos(edge, 1).vertices()[1]
            if (v1 == v2):
                hasHole = True
            coedges = []
            for coedge in tm.getInfos(edge, 1).coedges():
                coedges.append(f'coedges_mapping["{coedge}"]')
            script += f"cmd = tm.newEdge(vertices_mapping['{v1}'], vertices_mapping['{v2}'], [{', '.join(coedges)}])\n"
            script += f"coface_edges.append('Edge{edge_id:04d}')\n"
            edge_id += 1
        coface_vertices = [f'vertices_mapping["{vertex}"]' for vertex in tm.getInfos(coface, 2).vertices()]
        isStructured = "False" if coface in tm.getUnstructuredFaces() else "True"
        script += f'cmd = tm.newCoFace(coface_edges, [{", ".join(coface_vertices)}],{isStructured}, {hasHole})\n'
        script += 'f = cmd.getFaces()[0]\n'
        groups = tm.getInfos(coface, 2).groups()
        if 'Hors_Groupe_2D' in groups:
            groups.remove('Hors_Groupe_2D')
        if groups != []:
            for group in groups:
                script += f"tm.addToGroup ([f], 2, \"{group}\")\n"
        geom_entity = tm.getInfos(coface, 2).geom_entity
        if geom_entity:
            script += f"# La coface {coface} est associée à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([f], find_in_dicts('{geom_entity}'), False)\n"
        script += f'cofaces_mapping["{coface}"] = f\n'
        # On remplit la liste des faces
        for face in tm.getInfos(coface, 2).faces():
            faces_cofaces_map[face].append(coface)
            faces_vertices_map['-'.join(sorted(tm.getInfos(face, 2).vertices()))].append(face)
    script += "\n"

    if tm.getTransfiniteMeshLawFaces():
        script += "emp = Mgx3D.FaceMeshingPropertyTransfinite()\n"
        script += f"tm.setMeshingProperty(emp, [cofaces_mapping[coface] for coface in {tm.getTransfiniteMeshLawFaces()}])\n"
    
    script += "print(\"Correspondance des cofaces:\")\n"
    script += "print(cofaces_mapping, '\\n')\n\n"
    script += "\n"
    
    script += "# Création des faces\n"
    script += "faces_mapping = dict()\n"

    face_id = 0
    blocks_faces_0 = defaultdict(list)
    for face, cofaces in faces_cofaces_map.items():       
        script += f'# Création de la face {face}\n'
        face_cofaces = [f'cofaces_mapping["{coface}"]' for coface in cofaces]
        face_vertices = [f'vertices_mapping["{vertex}"]' for vertex in tm.getInfos(face, 2).vertices()]
        script += f'cmd = tm.newFace([{", ".join(face_cofaces)}], [{", ".join(face_vertices)}], {"False" if face in tm.getUnstructuredFaces() else "True"})\n'
        script += f'faces_mapping["{face}"] = "Face{face_id:04d}"\n'
        face_id += 1
        for block in tm.getInfos(face, 2).blocks():
            blocks_faces_0[block].append(face)
    script += "print(\"Correspondance des faces:\")\n"
    script += "print(faces_mapping, '\\n')\n\n"
    script += "\n"

    # On cherche ici à obtenir les faces de chaque bloc dans le bon ordre 
    # (on ne peut pas l'obtenir avec l'API Python)
    blocks_faces = defaultdict(list)
    tabIndVtxByFaceOnBlock = ((4, 0, 2, 6),
    (5, 1, 3, 7),
    (4, 0, 1, 5),
    (6, 2, 3, 7),
    (2, 0, 1, 3),
    (6, 4, 5, 7)
    )
    for block in tm.getBlocks():
        block_vertices = tm.getInfos(block, 3).vertices()
        if (len(block_vertices) == 8):
            for idxs in tabIndVtxByFaceOnBlock:
                face_vertices_key = '-'.join(sorted([block_vertices[i] for i in idxs]))
                # Il faut que la face appartienne au bloc
                faces = list(set(faces_vertices_map[face_vertices_key]) & set(blocks_faces_0[block]))
                if len(faces) == 1:
                    blocks_faces[block].append(faces[0])
            if len(blocks_faces[block]) != 6:
                print(f"Erreur pour le bloc {block} : nombre de faces trouvées = {len(blocks_faces[block])} au lieu de 6.")
                print(f"Faces trouvées : {blocks_faces[block]}\n")

    script += "# Création des blocs\n"
    script += "blocks_mapping = dict()\n"
    for block in tm.getBlocks():
        block_vertices = [f'vertices_mapping["{vertex}"]' for vertex in tm.getInfos(block, 3).vertices()]    
        if blocks_faces[block]:
            script += f'# Création du bloc {block} ayant pour faces {blocks_faces[block]} et pour sommets {tm.getInfos(block, 3).vertices()}\n'
            block_faces = [f'faces_mapping["{face}"]' for face in blocks_faces[block]]
            script += f'cmd = ctx.getTopoManager().newBlock([{", ".join(block_faces)}], [{", ".join(block_vertices)}], {"False" if block in tm.getUnstructuredBlocks() else "True"}, "")\n'
        else:
            script += f'# Création du bloc {block} ayant pour faces {blocks_faces_0[block]} et pour sommets {tm.getInfos(block, 3).vertices()}\n'
            block_faces = [f'faces_mapping["{face}"]' for face in blocks_faces_0[block]]
            script += f'cmd = ctx.getTopoManager().newBlock([{", ".join(block_faces)}], [{", ".join(block_vertices)}], {"False" if block in tm.getUnstructuredBlocks() else "True"}, "")\n'
        script += 'b = cmd.getBlocks()[0]\n'
        geom_entity = tm.getInfos(block, 3).geom_entity
        if geom_entity:
            script += f"# Le bloc {block} est associé à l'entité géométrique {geom_entity}\n"
            script += f"tm.setGeomAssociation([b], find_in_dicts('{geom_entity}'), False)\n"
        script += f'blocks_mapping["{block}"] = b\n'

    if tm.getTransfiniteMeshLawBlocks():
        script += "emp = Mgx3D.BlockMeshingPropertyTransfinite()\n"
        script += f"tm.setMeshingProperty(emp, [blocks_mapping[block] for block in {tm.getTransfiniteMeshLawBlocks()}])\n"

    script += "print(\"Correspondance des blocs:\")\n"
    script += "print(blocks_mapping, '\\n')\n"

    script += "\n# Création du maillage pour tous les blocs\n"
    script += "mm.newAllBlocksMesh()\n"
    script += "nbRegions = mm.getNbRegions()\n"
    script += f"if nbRegions == {mm.getNbRegions()}:\n"
    script += "    print('Nombre de mailles identique.')\n"
    script += "else:\n"
    script += f"    print('Différence du nombre de mailles : ' + str(nbRegions) + ' vs {mm.getNbRegions()}.')\n"

    return script

if __name__ == "__main__":
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    
    if len(sys.argv) < 2:
        print("Usage: python export.py <nom_du_script>")
        sys.exit(1)

    script = sys.argv[1]  # Récupère le nom du script à exécuter
    script_file = os.path.abspath(script)
    path = os.path.dirname(script_file)
    file = os.path.basename(script_file)
    script_name = os.path.splitext(file)[0]

    try:
        with open(script_file, "r") as script_file:
            code = script_file.read()
            # Exécuter le script dans le contexte global actuel
            exec(code)

            # création du répertoire de sortie s'il n'existe pas
            export_folder = os.path.join(path, script_name + "_generate_import")
            brep_folder = os.path.join(export_folder, "brep")
            os.makedirs(export_folder, exist_ok=True)
            os.makedirs(brep_folder, exist_ok=True) 
            
            for vertex in gm.getVertices():
                brep_file_name = f"{brep_folder}/{vertex + '.brep'}"
                print(brep_file_name)
                gm.exportBREP ([vertex], brep_file_name)

            for curve in gm.getCurves():
                brep_file_name = f"{brep_folder}/{curve + '.brep'}"
                print(brep_file_name)
                gm.exportBREP ([curve], brep_file_name)

            for surface in gm.getSurfaces():
                brep_file_name = f"{brep_folder}/{surface + '.brep'}"
                print(brep_file_name)
                gm.exportBREP ([surface], brep_file_name)

            for volume in gm.getVolumes():
                brep_file_name = f"{brep_folder}/{volume + '.brep'}"
                print(brep_file_name)
                gm.exportBREP ([volume], brep_file_name)
            
            generated_script = generer_script(ctx, export_folder)
            generated_script_file_name = f"{export_folder}/{file.replace('.py', '_generated.py')}"
            # Sauvegarde du script généré
            with open(generated_script_file_name, "w") as f:
                f.write(generated_script)

            print(f"Script généré avec succès : {generated_script_file_name}")

    except FileNotFoundError:
        print(f"Erreur : Le fichier {script_file} n'existe pas.")
    except Exception as e:
        print(f"Erreur lors de l'exécution : {e}")