//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_DIRECTED_ACYCLIC_GRAPH_HPP
#define VALKYRIE_CORE_ADT_DIRECTED_ACYCLIC_GRAPH_HPP

namespace valkyrie::Core{

  template <typename T, template <typename> typename EdgeType>
  class DAGNode {
  public:
    using edge_type = EdgeType<DAGNode>;
    using value_type = T;
  };


  template <typename NodeType>
  class DirectedAcyclicGraph{
  public:
    using node_type = NodeType;
    using edge_type = typename node_type::edge_type;
  };
}

#endif //VALKYRIE_CORE_ADT_DIRECTED_ACYCLIC_GRAPH_HPP
