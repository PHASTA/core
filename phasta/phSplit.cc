#include "phSplit.h"
#include "phInput.h"
#include <PCU.h>
#include <parma.h>
#include <apfZoltan.h>
#include <apfMDS.h>
#include <apfMesh2.h>

namespace ph {

void split(Input& in, apf::Mesh2* m, void (*runAfter)(apf::Mesh2*))
{
  assert(in.recursivePtn <= 1);
  int factor = in.numTotParts / PCU_Comm_Peers();
  assert(in.numTotParts % PCU_Comm_Peers() == 0);
  std::map<std::string, int> methodMap;
  methodMap["rib"] = apf::RIB;
  methodMap["graph"] = apf::GRAPH;
  methodMap["hypergraph"] = apf::HYPERGRAPH;
  int method = methodMap[in.partitionMethod];
  apf::Splitter* splitter = apf::makeZoltanSplitter(m, method, apf::REPARTITION);
  apf::MeshTag* weights = Parma_WeighByMemory(m);
  apf::Migration* plan = splitter->split(weights, 1.03, factor);
  apf::removeTagFromDimension(m, weights, m->getDimension());
  m->destroyTag(weights);
  delete splitter;
  apf::splitMdsMesh(m, plan, factor, runAfter);
}

}
