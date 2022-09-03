#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
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
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	Minimize();

	// half-edge structure -> triangle mesh
	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) { // f is triangle
		for (auto v : f->BoundaryVertice()) // vertices of the triangle
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	// TODO
	std::vector<Triplet<double>> triplet_list;
	size_t vert_num = heMesh->NumVertices();
	VectorXd bx = VectorXd::Zero(vert_num);
	VectorXd by = VectorXd::Zero(vert_num);
	VectorXd bz = VectorXd::Zero(vert_num);
	for (auto vertex : heMesh->Vertices()) {
		int i = static_cast<int>(heMesh->Index(vertex));
		// boundary is fixed
		if (vertex->IsBoundary()) {
			bx(i) += (vertex->pos[0]);
			by(i) += (vertex->pos[1]);
			bz(i) += (vertex->pos[2]);
			triplet_list.push_back(Triplet<double>(i, i, 1));
		}
		else {
			for (auto adjacent : vertex->AdjVertices()) {
				if (adjacent->IsBoundary()) {
					bx(i) += (adjacent->pos[0]);
					by(i) += (adjacent->pos[1]);
					bz(i) += (adjacent->pos[2]);
				}
				else {
					int i_adj = static_cast<int>(heMesh->Index(adjacent));
					triplet_list.push_back(Triplet<double>(i, i_adj, -1));
				}
			}
			triplet_list.push_back(Triplet<double>(i, i, double(vertex->Degree())));
		}
	}

	SparseMatrix<double> A(vert_num, vert_num);
	SimplicialLLT< SparseMatrix<double>> solver;
	A.setFromTriplets(triplet_list.begin(), triplet_list.end());
	solver.compute(A);
	VectorXd px = solver.solve(bx);
	VectorXd py = solver.solve(by);
	VectorXd pz = solver.solve(bz);
	for (auto vertex : heMesh->Vertices()) {
		size_t i = heMesh->Index(vertex);
		vertex->pos = { px(i), py(i), pz(i) };
	}
}
