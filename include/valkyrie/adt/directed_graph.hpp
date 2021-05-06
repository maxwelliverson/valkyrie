//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_ADT_DIRECTED_GRAPH_HPP
#define VALKYRIE_ADT_DIRECTED_GRAPH_HPP

#include "flat_set.hpp"
#include "iterator.hpp"

#include <memory>

namespace valkyrie{


  // code borrowed from LLVM directed_graph implementation. For details see <llvm/adt/directed_graph.h> and <llvm/adt/GraphTraits.h>


  template <typename G>
  struct inverse_graph{
    const G& graph;

    inline inverse_graph(const G& graph) noexcept : graph(graph){}
  };

  template <graph_c G>
  struct traits::graph<inverse_graph<inverse_graph<G>>> : traits::graph<G>{};


  template <graph_c G>
  range_view<typename graph_traits<G>::nodes_iterator,
             typename graph_traits<G>::nodes_sentinel>
  nodes(const G& graph) noexcept {
    return { graph_traits<G>::nodes_begin(graph), graph_traits<G>::nodes_end(graph) };
  }

  template <graph_c G>
  range_view<typename graph_traits<inverse_graph<G>>::nodes_iterator,
             typename graph_traits<inverse_graph<G>>::nodes_sentinel>
  inverse_nodes(const G& graph) noexcept {
    return { graph_traits<inverse_graph<G>>::nodes_begin(graph),
             graph_traits<inverse_graph<G>>::nodes_end(graph) };
  }

  template <graph_c G>
  range_view<typename graph_traits<G>::child_iterator,
             typename graph_traits<G>::child_sentinel>
  children(typename graph_traits<G>::node_ref node) noexcept {
    return { graph_traits<G>::child_begin(node), graph_traits<G>::child_end(node) };
  }

  template <graph_c G>
  range_view<typename graph_traits<inverse_graph<G>>::child_iterator,
             typename graph_traits<inverse_graph<G>>::child_sentinel>
  inverse_children(typename graph_traits<G>::node_ref node) noexcept {
    return { graph_traits<inverse_graph<G>>::child_begin(node),
             graph_traits<inverse_graph<G>>::child_end(node) };
  }

  template <graph_c G>
  range_view<typename graph_traits<G>::child_edge_iterator,
             typename graph_traits<G>::child_edge_sentinel>
  children_edges(typename graph_traits<G>::node_ref node) noexcept {
    return { graph_traits<G>::child_edge_begin(node), graph_traits<G>::child_edge_end(node) };
  }



  template <typename NodeType, typename EdgeType>
  class dg_edge {
  public:
    using node_type = NodeType;
    using edge_type = EdgeType;

    dg_edge() = delete;
    /// Create an edge pointing to the given node \p N.
    explicit dg_edge(node_type &N) : targetNode(N) {}
    explicit dg_edge(const dg_edge& E) : targetNode(E.targetNode) {}
    dg_edge& operator=(const dg_edge& E) {
      targetNode = E.targetNode;
      return *this;
    }

    /// Static polymorphism: delegate implementation (via is_equal_to) to the
    /// derived class.
    bool operator==(const edge_type& E) const { return derived().is_equal_to(E); }

    /// Retrieve the target node this edge connects to.
    const node_type& get_target_node() const noexcept{ return targetNode; }
    node_type& get_target_node() noexcept {
      return const_cast<node_type &>(static_cast<const dg_edge<node_type, edge_type> &>(*this).get_target_node());
    }

    /// Set the target node this edge connects to.
    void set_target_node(const node_type &N) { targetNode = N; }

  protected:
    // As the default implementation use address comparison for equality.
    bool is_equal_to(const edge_type& E) const { return this == &E; }

    // Cast the 'this' pointer to the derived type and return a reference.
    edge_type& derived() { return *static_cast<edge_type *>(this); }
    const edge_type& derived() const {
      return *static_cast<const edge_type *>(this);
    }

    // The target node this edge connects to.
    node_type& targetNode;
  };
  template <typename NodeType, typename EdgeType>
  class dg_node {
  public:
    using node_type = NodeType;
    using edge_type = EdgeType;
    using edge_list_type = flat_set<edge_type*>;
    using iterator = typename edge_list_type::iterator;
    using const_iterator = typename edge_list_type::const_iterator;

    /// Create a node with a single outgoing edge \p E.
    explicit dg_node(edge_type& E) : edges() { edges.insert(&E); }
    dg_node() = default;

    explicit dg_node(const dg_node& N) : edges(N.edges) {}
    dg_node(dg_node&& N) noexcept : edges(std::move(N.edges)) {}

    dg_node& operator=(const dg_node& N) {
      edges = N.edges;
      return *this;
    }
    dg_node& operator=(const dg_node&& N) noexcept {
      edges = std::move(N.edges);
      return *this;
    }

    /// Static polymorphism: delegate implementation (via is_equal_to) to the
    /// derived class.
    bool operator==(const node_type &N) const { return derived().is_equal_to(N); }
    bool operator!=(const node_type &N) const { return !operator==(N); }

    const_iterator begin() const { return edges.begin(); }
    const_iterator end() const { return edges.end(); }
    iterator       begin() { return edges.begin(); }
    iterator       end() { return edges.end(); }
    /*const edge_type &front() const { return *edges.front(); }
    edge_type &front() { return *edges.front(); }
    const edge_type &back() const { return *edges.back(); }
    edge_type &back() { return *edges.back(); }*/

    /// Collect in \p EL, all the edges from this node to \p N.
    /// Return true if at least one edge was found, and false otherwise.
    /// Note that this implementation allows more than one edge to connect
    /// a given pair of nodes.
    /*bool findEdgesTo(const node_type &N, SmallVectorImpl<edge_type *> &EL) const {
      VK_assert(EL.empty() && "Expected the list of edges to be empty.");
      for (auto *E : edges)
        if (E->get_target_node() == N)
          EL.push_back(E);
      return !EL.empty();
    }*/

    /// Add the given edge \p E to this node, if it doesn't exist already. Returns
    /// true if the edge is added and false otherwise.
    bool add_edge(edge_type &E) { return edges.insert(&E); }

    /// Remove the given edge \p E from this node, if it exists.
    void remove_edge(edge_type &E) { edges.remove(&E); }

    /// Test whether there is an edge that goes from this node to \p N.
    bool has_edge_to(const node_type &N) const {
      return (find_edge_to(N) != edges.end());
    }

    /// Retrieve the outgoing edges for the node.
    const edge_list_type &get_edges() const { return edges; }
    edge_list_type &get_edges() {
      return const_cast<edge_list_type &>(
          static_cast<const dg_node<node_type, edge_type> &>(*this).edges);
    }

    /// Clear the outgoing edges.
    void clear() { edges.clear(); }

  protected:
    // As the default implementation use address comparison for equality.
    bool is_equal_to(const node_type &N) const { return this == &N; }

    // Cast the 'this' pointer to the derived type and return a reference.
    node_type &derived() { return *static_cast<node_type *>(this); }
    const node_type &derived() const {
      return *static_cast<const node_type *>(this);
    }

    /// Find an edge to \p N. If more than one edge exists, this will return
    /// the first one in the list of edges.
    const_iterator find_edge_to(const node_type &N) const {
      return std::ranges::find_if(edges, [&N](const edge_type *E) { return E->get_target_node() == N; });
      /*return llvm::find_if(
          edges, [&N](const edge_type *E) { return E->get_target_node() == N; });*/
    }

    // The list of outgoing edges.
    edge_list_type edges;
  };


  template <typename NodeType, typename EdgeType = typename NodeType::edge_type>
  class directed_graph {
  public:
    using node_type = NodeType;
    using edge_type = EdgeType;
  protected:
    using node_list_type = small_vector<NodeType*, 10>;
    using edge_list_type = small_vector<EdgeType*, 10>;
  public:
    using iterator = typename node_list_type::iterator;
    using const_iterator = typename node_list_type::const_iterator;
    using graph_type = directed_graph<NodeType, EdgeType>;

    directed_graph() = default;
    explicit directed_graph(node_type& N) : node_list() { this->add(N); }
    directed_graph(const graph_type& G) : node_list(G.node_list) {}
    directed_graph(graph_type&& RHS) : node_list(std::move(RHS.node_list)) {}

    graph_type& operator=(const graph_type &G) {
      node_list = G.node_list;
      return *this;
    }
    graph_type& operator=(const graph_type &&G) {
      node_list = std::move(G.node_list);
      return *this;
    }

    const_iterator   begin() const noexcept { return node_list.begin(); }
    const_iterator   end() const noexcept { return node_list.end(); }
    iterator         begin() noexcept { return node_list.begin(); }
    iterator         end() noexcept { return node_list.end(); }
    const node_type& front() const noexcept { return *node_list.front(); }
    node_type &      front() noexcept { return *node_list.front(); }
    const node_type &back() const noexcept { return *node_list.back(); }
    node_type &      back() noexcept { return *node_list.back(); }

    size_t size() const noexcept { return node_list.size(); }

    /// Find the given node \p N in the table.
    const_iterator find(const node_type &N) const {
      return std::ranges::find_if(node_list, [&N](const node_type *Node) { return *Node == N; });
    }
    iterator find(const node_type &N) {
      return const_cast<iterator>(static_cast<const graph_type &>(*this).find(N));
    }

    /// Add the given node \p N to the graph if it is not already present.
    bool add(node_type &N) {
      if (this->find(N) != node_list.end())
        return false;
      node_list.push_back(&N);
      return true;
    }

    /// Collect in \p EL all edges that are coming into node \p N. Return true
    /// if at least one edge was found, and false otherwise.
    /*bool findIncomingEdgesToNode(const node_type &N, SmallVectorImpl<edge_type*> &EL) const {
      VK_assert(EL.empty() && "Expected the list of edges to be empty.");
      EdgeListTy TempList;
      for (auto *Node : node_list) {
        if (*Node == N)
          continue;
        Node->findEdgesTo(N, TempList);
        EL.insert(EL.end(), TempList.begin(), TempList.end());
        TempList.clear();
      }
      return !EL.empty();
    }*/

    /// Remove the given node \p N from the graph. If the node has incoming or
    /// outgoing edges, they are also removed. Return true if the node was found
    /// and then removed, and false if the node was not found in the graph to
    /// begin with.
    bool remove(node_type &N) {
      iterator IT = this->find(N);
      if (IT == node_list.end())
        return false;
      // Remove incoming edges.
      edge_list_type EL;
      for (auto *Node : node_list) {
        if (*Node == N)
          continue;
        Node->find_edges_to(N, EL);
        for (auto *E : EL)
          Node->remove_edge(*E);
        EL.clear();
      }
      N.clear();
      node_list.erase(IT);
      return true;
    }

    /// Assuming nodes \p Src and \p Dst are already in the graph, connect node \p
    /// Src to node \p Dst using the provided edge \p E. Return true if \p Src is
    /// not already connected to \p Dst via \p E, and false otherwise.
    bool connect(node_type& Src, node_type& Dst, edge_type& E) {
      VK_assert(this->find(Src) != node_list.end() && "Src node should be present.");
      VK_assert(this->find(Dst) != node_list.end() && "Dst node should be present.");
      VK_assert((E.get_target_node() == Dst)       && "Target of the given edge does not match Dst.");
      return Src.add_edge(E);
    }

  protected:
    // The list of nodes in the graph.
    node_list_type node_list;
  };

  /*class ExecutableGraphNode;
  class GraphResourceDesc{};
  class ExecutableGraphNodeInput{};
  class ExecutableGraphNodeOutput{};
  class ExecutableGraphEdge : public dg_edge<ExecutableGraphNode, ExecutableGraphEdge>{
  public:

  private:
  };
  class ExecutableGraphNode : public dg_node<ExecutableGraphNode, ExecutableGraphEdge> {
  public:


  private:
    vector<GraphResourceDesc*> resourceDescriptions;
  };

  class ExecutableGraph : public directed_graph<ExecutableGraphNode>{
  public:


  private:
  };*/

  /*namespace Graph{
    class ExVisitor;
    class ExOperand;
    class ExOperandDescriptor;
    class ExNode;
    class ExEdge;
    class ExNodeInputPort;
    class ExNodeOutputPort;
    class ExGraph;


    
    class ExOperandDescriptor{
      
    public:
      
      virtual bool isCompatibleWith(const ExOperandDescriptor& other) const noexcept = 0;
    };

    class ExNodeInputPort{
      
      
    private:
      std::unique_ptr<ExOperandDescriptor> operandDescriptor;
      
    };
    class ExNodeOutputPort{};
    
    
    class ExNode{
      
      
    private:
      std::vector<ExNodeInputPort>     inputPorts;
      std::vector<ExNodeOutputPort>    outputPorts;
      std::vector<ExOperandDescriptor> operandDescriptors;
    };
    
    
    class UserGraph{
    public:
      
      virtual ~UserGraph() = default;
      
      virtual void addNode(ExNode* pNode) = 0;
      virtual void addOperand(ExOperandDescriptor* pOperand) = 0;
      virtual void traverseGraph(ExVisitor& visitor, void* pUserData) = 0;
    };
    
    
    
    

    class RenderPass{};
    class SubPass{};

    class GraphicsOpNode{};
    class ComputeOpNode{};
    class TransferOpNode{};
    class RayTracingOpNode{};
    class HostOpNode{};
    class EmptyOpNode{};
  }


  class ExecutionGraphVisitor{};
  class ExecutionGraphOperand{};
  class ExecutionGraphNode;
  class ExecutionGraphEdge{

  };
  class ExecutionGraphNodePort{



  private:
    ExecutionGraphNode* pFromNode;
    ExecutionGraphNode* pToNode;
  };
  class ExecutionGraphNode{


  private:
    std::vector<ExecutionGraphNodePort> inPorts;
    std::vector<ExecutionGraphNodePort> outPorts;
  };
  class ExecutionGraph{};*/
}





#endif //VALKYRIE_ADT_DIRECTED_GRAPH_HPP
