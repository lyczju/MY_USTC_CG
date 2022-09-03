#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh) 
	// TODO
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
	// default settings
	SetParaShape(Para::square);
	SetParaWeight(Para::uniform);
}

void Paramaterize::Clear() {
	// TODO
	heMesh->Clear();
	triMesh = nullptr;
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	// TODO
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	// init half-edge structure
	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	// triangle mesh's positions ->  half-edge structure's positions
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
		v->coord = triMesh->GetTexcoords()[i].cast_to<vecf2>();
	}

	this->triMesh = triMesh;
	return true;
}

bool Paramaterize::Run() {
	// TODO
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::Paramaterize::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	DoParamaterize();

	// half-edge structure -> triangle mesh
	vector<pointf2> texcoords;
	for (auto vertex : heMesh->Vertices())
		texcoords.push_back(vertex->coord.cast_to<pointf2>());
	triMesh->Update(texcoords);

	return true;
}

void Paramaterize::DoParamaterize() {
	auto boundaries = heMesh->Boundaries()[0];
	double theta = 0;
	double delta = 360 / static_cast<double>(boundaries.size());

	// set boundaries
	if (this->paraShape == Para::square) {
		theta = -45;
		for (auto he : boundaries) {
			auto vertex = he->Origin();
			if (theta >= -45 && theta < 45)
				vertex->coord = { 1, 0.5 * tan(theta * PI<double> / 180) + 0.5 };
			else if (theta >= 45 && theta < 135)
				vertex->coord = { 0.5 / tan(theta * PI<double> / 180) + 0.5, 1 };
			else if (theta >= 135 && theta < 225)
				vertex->coord = { 0, -0.5 * tan(theta * PI<double> / 180) + 0.5 };
			else if (theta >= 225 && theta < 315)
				vertex->coord = { -0.5 / tan(theta * PI<double> / 180) + 0.5, 0 };
			theta += delta;
		}
	}
	else if (this->paraShape == Para::circle) {
		for (auto he : boundaries) {
			auto vertex = he->Origin();
			vertex->coord = { 0.5 + 0.5 * cos(theta * PI<double> / 180), 0.5 + 0.5 * sin(theta * PI<double> / 180) };
			theta += delta;
		}
	}

	std::vector<Triplet<double>> triplet_list;
	size_t vert_num = heMesh->NumVertices();
	VectorXd bx = VectorXd::Zero(vert_num);
	VectorXd by = VectorXd::Zero(vert_num);

	for (auto vertex : heMesh->Vertices()) {
		int i = static_cast<int>(heMesh->Index(vertex));
		if (vertex->IsBoundary()) {
			bx(i) += vertex->coord[0];
			by(i) += vertex->coord[1];
			triplet_list.push_back(Triplet<double>(i, i, 1));
		}
		else {
			vector<double> weight_list;
			double weight_sum = 0;
			auto adjacent_list = vertex->AdjVertices();

			// set weights
			if (this->paraWeight == Para::uniform) {
				for (int j = 0; j < adjacent_list.size(); j++) {
					weight_list.push_back(1);
					weight_sum += 1;
				}
			}
			else if (this->paraWeight == Para::cotangent) {
				for (int j = 0; j < adjacent_list.size(); j++) {
					int prev = j - 1 < 0 ? adjacent_list.size() - 1 : j - 1;
					int next = j + 1 >= adjacent_list.size() ? 0 : j + 1;
					auto adj_prev = adjacent_list[prev];
					auto adj_curr = adjacent_list[j];
					auto adj_next = adjacent_list[next];
					double cos_alpha = vecf3::cos_theta(vertex->pos - adj_prev->pos, adj_curr->pos - adj_prev->pos);
					double cos_beta = vecf3::cos_theta(vertex->pos - adj_next->pos, adj_curr->pos - adj_next->pos);
					double cotangent_weight = sqrt(cos_alpha * cos_alpha / (1 - cos_alpha * cos_alpha)) + sqrt(cos_beta * cos_beta / (1 - cos_beta * cos_beta));
					weight_list.push_back(cotangent_weight);
					weight_sum += cotangent_weight;
				}
			}

			for (int j = 0; j < adjacent_list.size(); j++) {
				if (adjacent_list[j]->IsBoundary()) {
					bx(i) += weight_list[j] * adjacent_list[j]->coord[0];
					by(i) += weight_list[j] * adjacent_list[j]->coord[1];
				}
				else {
					int i_adj = static_cast<int>(heMesh->Index(adjacent_list[j]));
					triplet_list.push_back(Triplet<double>(i, i_adj, weight_list[j]));
				}
			}

			triplet_list.push_back(Triplet<double>(i, i, weight_sum));
		}
	}

	SparseMatrix<double> A(vert_num, vert_num);
	SimplicialLLT< SparseMatrix<double>> solver;
	A.setFromTriplets(triplet_list.begin(), triplet_list.end());
	solver.compute(A);
	VectorXd cx = solver.solve(bx);
	VectorXd cy = solver.solve(by);
	for (auto vertex : heMesh->Vertices()) {
		size_t i = heMesh->Index(vertex);
		vertex->coord = { cx(i), cy(i) };
	}
}

void Paramaterize::SetParaShape(Para::ParaShape paraShape) {
	this->paraShape = paraShape;
}

void Paramaterize::SetParaWeight(Para::ParaWeight paraWeight) {
	this->paraWeight = paraWeight;
}
