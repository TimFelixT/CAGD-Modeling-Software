//#pragma once
//#include <fstream>
//#include <sstream>
//#include <iostream>
//#include <cstdio>
//#include <iterator>
//#include <glm/gtx/vector_angle.hpp>
//
//
//
//
//struct ObjParser{
//	
//};
//
//ObjParser::ObjParser() {
//	logfile.open("ObjectLOG.txt");
//}
//
//ObjParser::~ObjParser(){
//	logfile.close();
//}
//
//
//bool ObjParser::parseObj(const std::string &path, Mesh& mesh) {
//	std::ifstream File(path.c_str());
//
//	if (!File.good()) {
//		logfile << "Datei konnte nicht gelesen werden." << std::endl;
//		return false;
//	}
//
//	std::string Line;
//	std::string Name;
//
//	std::cout << "Starte Einlesen..." << std::endl;
//	while (std::getline(File, Line)) {
//		if (Line == "" || Line[0] == '#') continue; //Kommentar oder leere Zeile
//		std::istringstream LineStream(Line);
//		LineStream >> Name;
//
//		if (Name == "v") {// Vertex
//			Vertex* v = new Vertex();
//			float x, y, z;
//			sscanf_s(Line.c_str(), "%*s %f %f %f", &x, &y, &z);
//			v->position = glm::vec3(x, y, z);
//			mesh.vertices.push_back(v);
//			logfile << "Vertex eingelesen: " << x << " " << y << " " << z << std::endl;
//		}
//
//		else if (Name == "vt") {// Textur
//			glm::vec2 vt;
//			sscanf_s(Line.c_str(), "%*s %f %f", &vt.x, &vt.y);
//			mesh.textures.push_back(vt);
//			logfile << "Textur eingelesen: " << vt.x << " " << vt.y << std::endl;
//		}
//
//		else if (Name == "vn") {// Normals
//			glm::vec3 normal;
//			sscanf_s(Line.c_str(), "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
//			normal = glm::normalize(normal); // Normale muss ggf. nochmal normalisiert werden
//			mesh.normals.push_back(normal);
//			logfile << "Normale eingelesen: " << normal.x << " " << normal.y << " " << normal.z << std::endl;
//		}
//
//		else if (Name == "f") {// Face
//			Face* f = new Face();
//
//			std::istringstream tokenizer(Line);
//			std::vector<std::string> tokens;
//			std::copy(std::istream_iterator<std::string>(tokenizer),
//				std::istream_iterator<std::string>(),
//				std::back_inserter<std::vector<std::string> >(tokens));
//
//			logfile << "Neues Face:" << std::endl;
//
//			unsigned int v=0, vt=0, vn=0;
//			for (unsigned int i = 1; i < tokens.size(); i++) {
//				if (sscanf_s(tokens[i].c_str(), "%d/%d/%d", &v, &vt, &vn) == 3) {
//					f->v.push_back(v);
//					f->vt.push_back(vt);
//					f->vn.push_back(vn);
//				}
//				else if (sscanf_s(tokens[i].c_str(), "%d//%d", &v, &vn) == 2) {
//					f->v.push_back(v);
//					f->vt.push_back(0);
//					f->vn.push_back(vn);
//				}
//				else if (sscanf_s(tokens[i].c_str(), "%d/%d", &v, &vt) == 2) {
//					f->v.push_back(v);
//					f->vt.push_back(vt);
//					f->vn.push_back(0); 
//				}
//				else if (sscanf_s(tokens[i].c_str(), "%d", &v) == 1) {
//					f->v.push_back(v);
//					f->vt.push_back(0);
//					f->vn.push_back(0);
//				} else {
//					logfile << "Token " << tokens[i] << " in Zeile: " << Line << " konnte nicht geparsed werden.." << std::endl;
//				}
//				logfile << "FaceToken eingelesen: " << v << " " << vt << " " << vn << std::endl;
//			}
//
//			mesh.faces.push_back(f);
//		}
//
//		else if (Name == "s") {// Face
//			logfile << "Smoothing gropus werden aktuell ignoriert!" << std::endl;
//		}
//	}
//	std::cout << "Einlesen beendet." << std::endl;
//
//	return triangulateObj(mesh);
//}
//
//bool ObjParser::triangulateObj(Mesh& mesh) {
//	//Im Grunde müssen nur die Faces im Mesh verändert werden, da man keine neuen Punkte hinzufügen muss. Dazu gehört dann auch die Indexe auf die Vertices, Normalen und, wenn dann verwendet, Texturen zu verändern
//
//	std::cout << "Beginne Triangulierung..." << std::endl;
//
//	//Referenz auf die Faces
//	std::vector<Face*>& faces = mesh.faces;
//
//	for (int i = 0; i < faces.size(); i++) {
//		Face& face = *(faces.at(i));
//		
//		if (face.v.size() == 3) continue; //Face besteht schon aus nur 3 Punkten
//		else {
//			while (face.v.size() != 3) {
//				Face* f = new Face();
//				f->v.push_back(face.v[0]);
//				f->vt.push_back(face.vt[0]);
//				f->vn.push_back(face.vn[0]);
//
//				f->v.push_back(face.v[1]);
//				f->vt.push_back(face.vt[1]);
//				f->vn.push_back(face.vn[1]);
//
//				f->v.push_back(face.v[2]);
//				f->vt.push_back(face.vt[2]);
//				f->vn.push_back(face.vn[2]);
//
//				face.v.erase(face.v.begin() + 1);
//				face.vt.erase(face.vt.begin() + 1);
//				face.vn.erase(face.vn.begin() + 1);
//
//				faces.push_back(f);
//			}
//		}
//	}
//
//	//DEBUG
//	/*for (int i = 0; i < faces.size(); i++) {
//		logfile << "Face:" << std::endl;
//		for (int j = 0; j < faces[i]->v.size(); j++) {
//			logfile << "V:" << faces[i]->v[j] << std::endl;
//			logfile << "T:" << faces[i]->vt[j] << std::endl;
//			logfile << "N:" << faces[i]->vn[j] << std::endl;
//		}
//		logfile << std::endl;
//		logfile << std::endl;
//	}*/
//
//	std::cout << "Triangulierung beendet." << std::endl;
//
//
//	return true;
//}