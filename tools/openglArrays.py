#!BPY
# coding: utf-8
""" 
Name: 'OpenGL Arrays (.h)...'
Blender: 248
Group: 'Export'
Tooltip: 'OpenGl Arrays (.h).'
"""

__author__ = ["Romulo Fernandes"]
__url__ = ("http://code.google.com/p/knowledge")
__version__ = "0.1"
__bpydoc__ = """\

This script export blender meshes to opengl
arrays (GL_TRIANGLES arrays only).

"""

######################################################
# Importing modules
######################################################

import Blender
import bpy
from BPyMesh import getMeshFromObject
from BPyObject import getDerivedObjects

def findVec(list, vector):
	for i in range(0, len(list)):
		if (list[i] == vector):
			return i
	
	return None

def saveMesh(filename):
	for obj in Blender.Object.Get():
		objName = obj.getName().upper()
		file = open(filename, "w")
		file.write("#ifndef _%s_H_\n" % objName.upper())
		file.write("#define _%s_H_\n\n" % objName.upper())

		mesh = obj.getData(False, True)
		meshVertices = []
		meshNormals = []
		meshUvs = []
		meshIndices = []
		for face in mesh.faces:

			# convert to triangles
			for v in range(1, len(face.v)-1):
			
				# check if we have the vertices on main list
				index = findVec(meshVertices, face.v[0].co)
				exists = False
				if (index and meshUvs[index] == face.uv[0]):
					exists = True

				if (exists):
					meshIndices.append(index)
				else:
				 	meshVertices.append(face.v[0].co)
					meshNormals.append(face.v[0].no)
					meshUvs.append(face.uv[0])
					meshIndices.append(len(meshVertices) - 1)

				index = findVec(meshVertices, face.v[v].co)
				exists = False
				if (index and meshUvs[index] == face.uv[v]):
					exists = True

				if (exists):
					meshIndices.append(index)
				else:
				 	meshVertices.append(face.v[v].co)
					meshNormals.append(face.v[v].no)
					meshUvs.append(face.uv[v])
					meshIndices.append(len(meshVertices) - 1)

				index = findVec(meshVertices, face.v[v + 1].co)
				exists = False
				if (index and meshUvs[index] == face.uv[v + 1]):
					exists = True

				if (exists):
					meshIndices.append(index)
				else:
				 	meshVertices.append(face.v[v + 1].co)
					meshNormals.append(face.v[v + 1].no)
					meshUvs.append(face.uv[v + 1])
					meshIndices.append(len(meshVertices) - 1)

		file.write("#define %s_VERTEX_COUNT %d\n" % (objName.upper(), len(meshVertices)))
		file.write("#define %s_INDEX_COUNT %d\n\n" % (objName.upper(), len(meshIndices)))

		file.write("const float %s_vertices[] = {\n" % objName)
		for vIndex in range(0, len(meshVertices)):
			vertex = meshVertices[vIndex]
			if (vIndex == len(meshVertices) - 1):
				file.write("%f, %f, %f\n};\n\n" % (vertex.x, -vertex.z, -vertex.y))
			else:
				file.write("%f, %f, %f,\n" % (vertex.x, -vertex.z, -vertex.y))

		file.write("const float %s_uvs[] = {\n" % objName)
		for uvIndex in range(0, len(meshUvs)):
			uv = meshUvs[uvIndex]
			if (uvIndex == len(meshUvs) - 1):
				file.write("%f, %f\n};\n\n" % (uv.x, uv.y))
			else:
				file.write("%f, %f,\n" % (uv.x, uv.y))

		file.write("const float %s_normals[] = {\n" % objName)
		for noIndex in range(0, len(meshNormals)):
			normal = meshNormals[noIndex]
			if (noIndex == len(meshNormals) - 1):
				file.write("%f, %f, %f\n};\n\n" % (normal.x, -normal.z, -normal.y))
			else:
				file.write("%f, %f, %f,\n" % (normal.x, -normal.z, -normal.y))

		file.write("const int %s_indices[] = {\n" % objName)
		count = 0
		for index in range(0, len(meshIndices)):
			i = meshIndices[index]
			if (index == len(meshIndices) - 1):
				file.write("%d\n};\n" % i)
			else:
				file.write("%d, " % i)

			count += 1
			if (count == 8):
				count = 0
				file.write("\n")

		file.write("#endif\n")
		file.close()

if __name__=='__main__':
	Blender.Window.FileSelector(saveMesh, "Export OpenGl Header")
