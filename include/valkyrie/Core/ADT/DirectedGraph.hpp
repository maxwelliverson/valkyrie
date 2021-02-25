//
// Created by Maxwell on 2021-01-12.
//

#ifndef VALKYRIE_CORE_ADT_DIRECTED_GRAPH_HPP
#define VALKYRIE_CORE_ADT_DIRECTED_GRAPH_HPP

#include <valkyrie/Core/ADT/FlatSet.hpp>
//#include <valkyrie/Core/ADT/TempArray.hpp>
#include <memory>
//#include <llvm/ADT/DirectedGraph.h>

namespace valkyrie::Core{


  // Code borrowed from LLVM DirectedGraph implementation. For details see <llvm/ADT/DirectedGraph.h> and <llvm/ADT/GraphTraits.h>

  template<class GraphType>
  struct GraphTraits {
    // Elements to provide:

    using graph_type = GraphType;
    
    
    
    // typedef NodeRef           - Type of Node token in the graph, which should
    //                             be cheap to copy.
    // typedef ChildIteratorType - Type used to iterate over children in graph,
    //                             dereference to a NodeRef.

    // static NodeRef getEntryNode(const GraphType &)
    //    Return the entry node of the graph

    // static ChildIteratorType child_begin(NodeRef)
    // static ChildIteratorType child_end  (NodeRef)
    //    Return iterators that point to the beginning and ending of the child
    //    node list for the specified node.

    // typedef  ...iterator nodes_iterator; - dereference to a NodeRef
    // static nodes_iterator nodes_begin(GraphType *G)
    // static nodes_iterator nodes_end  (GraphType *G)
    //    nodes_iterator/begin/end - Allow iteration over all nodes in the graph

    // typedef EdgeRef           - Type of Edge token in the graph, which should
    //                             be cheap to copy.
    // typedef ChildEdgeIteratorType - Type used to iterate over children edges in
    //                             graph, dereference to a EdgeRef.

    // static ChildEdgeIteratorType child_edge_begin(NodeRef)
    // static ChildEdgeIteratorType child_edge_end(NodeRef)
    //     Return iterators that point to the beginning and ending of the
    //     edge list for the given callgraph node.
    //
    // static NodeRef edge_dest(EdgeRef)
    //     Return the destination node of an edge.

    // static unsigned       size       (GraphType *G)
    //    Return total number of nodes in the graph

    // If anyone tries to use this class without having an appropriate
    // specialization, make an error.  If you get this error, it's because you
    // need to include the appropriate specialization of GraphTraits<> for your
    // graph, or you need to define it for a new graph type. Either that or
    // your argument to XXX_begin(...) is unknown or needs to have the proper .h
    // file #include'd.
    using node_ref_type = typename GraphType::UnknownGraphTypeError;
  };




  template <typename NodeType, typename EdgeType>
  class DGEdge{
  public:
    using node_type = NodeType;
    using edge_type = EdgeType;

    DGEdge() = delete;
    /// Create an edge pointing to the given node \p N.
    explicit DGEdge(node_type &N) : targetNode(N) {}
    explicit DGEdge(const DGEdge& E) : targetNode(E.targetNode) {}
    DGEdge& operator=(const DGEdge& E) {
      targetNode = E.targetNode;
      return *this;
    }

    /// Static polymorphism: delegate implementation (via isEqualTo) to the
    /// derived class.
    bool operator==(const edge_type& E) const { return getDerived().isEqualTo(E); }

    /// Retrieve the target node this edge connects to.
    const node_type& getTargetNode() const noexcept{ return targetNode; }
    node_type& getTargetNode() noexcept {
      return const_cast<node_type &>(static_cast<const DGEdge<node_type, edge_type> &>(*this).getTargetNode());
    }

    /// Set the target node this edge connects to.
    void setTargetNode(const node_type &N) { targetNode = N; }

  protected:
    // As the default implementation use address comparison for equality.
    bool isEqualTo(const edge_type& E) const { return this == &E; }

    // Cast the 'this' pointer to the derived type and return a reference.
    edge_type& getDerived() { return *static_cast<edge_type *>(this); }
    const edge_type& getDerived() const {
      return *static_cast<const edge_type *>(this);
    }

    // The target node this edge connects to.
    node_type& targetNode;
  };
  template <typename NodeType, typename EdgeType>
  class DGNode{
  public:
    using node_type = NodeType;
    using edge_type = EdgeType;
    using edge_list_type = FlatSet<edge_type*>;
    using iterator = typename edge_list_type::iterator;
    using const_iterator = typename edge_list_type::const_iterator;

    /// Create a node with a single outgoing edge \p E.
    explicit DGNode(edge_type& E) : edges() { edges.insert(&E); }
    DGNode() = default;

    explicit DGNode(const DGNode& N) : edges(N.edges) {}
    DGNode(DGNode&& N) noexcept : edges(std::move(N.edges)) {}

    DGNode& operator=(const DGNode& N) {
      edges = N.edges;
      return *this;
    }
    DGNode& operator=(const DGNode&& N) noexcept {
      edges = std::move(N.edges);
      return *this;
    }

    /// Static polymorphism: delegate implementation (via isEqualTo) to the
    /// derived class.
    bool operator==(const node_type &N) const { return getDerived().isEqualTo(N); }
    bool operator!=(const node_type &N) const { return !operator==(N); }

    const_iterator begin() const { return edges.begin(); }
    const_iterator end() const { return edges.end(); }
    iterator begin() { return edges.begin(); }
    iterator end() { return edges.end(); }
    /*const edge_type &front() const { return *edges.front(); }
    edge_type &front() { return *edges.front(); }
    const edge_type &back() const { return *edges.back(); }
    edge_type &back() { return *edges.back(); }*/

    /// Collect in \p EL, all the edges from this node to \p N.
    /// Return true if at least one edge was found, and false otherwise.
    /// Note that this implementation allows more than one edge to connect
    /// a given pair of nodes.
    /*bool findEdgesTo(const node_type &N, SmallVectorImpl<edge_type *> &EL) const {
      assert(EL.empty() && "Expected the list of edges to be empty.");
      for (auto *E : edges)
        if (E->getTargetNode() == N)
          EL.push_back(E);
      return !EL.empty();
    }*/

    /// Add the given edge \p E to this node, if it doesn't exist already. Returns
    /// true if the edge is added and false otherwise.
    bool addEdge(edge_type &E) { return edges.insert(&E); }

    /// Remove the given edge \p E from this node, if it exists.
    void removeEdge(edge_type &E) { edges.remove(&E); }

    /// Test whether there is an edge that goes from this node to \p N.
    bool hasEdgeTo(const node_type &N) const {
      return (findEdgeTo(N) != edges.end());
    }

    /// Retrieve the outgoing edges for the node.
    const edge_list_type &getEdges() const { return edges; }
    edge_list_type &getEdges() {
      return const_cast<edge_list_type &>(
          static_cast<const DGNode<node_type, edge_type> &>(*this).edges);
    }

    /// Clear the outgoing edges.
    void clear() { edges.clear(); }

  protected:
    // As the default implementation use address comparison for equality.
    bool isEqualTo(const node_type &N) const { return this == &N; }

    // Cast the 'this' pointer to the derived type and return a reference.
    node_type &getDerived() { return *static_cast<node_type *>(this); }
    const node_type &getDerived() const {
      return *static_cast<const node_type *>(this);
    }

    /// Find an edge to \p N. If more than one edge exists, this will return
    /// the first one in the list of edges.
    const_iterator findEdgeTo(const node_type &N) const {
      return std::ranges::find_if(edges, [&N](const edge_type *E) { return E->getTargetNode() == N; });
      /*return llvm::find_if(
          edges, [&N](const edge_type *E) { return E->getTargetNode() == N; });*/
    }

    // The list of outgoing edges.
    edge_list_type edges;
  };


  template <typename NodeType, typename EdgeType = typename NodeType::edge_type>
  class DirectedGraph{
  public:
    using node_type = NodeType;
    using edge_type = EdgeType;
  protected:
    using node_list_type = SmallArray<NodeType*, 10>;
    using edge_list_type = SmallArray<EdgeType*, 10>;
  public:
    using iterator = typename node_list_type::iterator;
    using const_iterator = typename node_list_type::const_iterator;
    using graph_type = DirectedGraph<NodeType, EdgeType>;

    DirectedGraph() = default;
    explicit DirectedGraph(node_type& N) : node_list() { addNode(N); }
    DirectedGraph(const graph_type& G) : node_list(G.node_list) {}
    DirectedGraph(graph_type&& RHS) : node_list(std::move(RHS.node_list)) {}

    graph_type& operator=(const graph_type &G) {
      node_list = G.node_list;
      return *this;
    }
    graph_type& operator=(const graph_type &&G) {
      node_list = std::move(G.node_list);
      return *this;
    }

    const_iterator begin() const { return node_list.begin(); }
    const_iterator end() const { return node_list.end(); }
    iterator begin() { return node_list.begin(); }
    iterator end() { return node_list.end(); }
    const node_type &front() const { return *node_list.front(); }
    node_type &front() { return *node_list.front(); }
    const node_type &back() const { return *node_list.back(); }
    node_type &back() { return *node_list.back(); }

    size_t size() const { return node_list.size(); }

    /// Find the given node \p N in the table.
    const_iterator findNode(const node_type &N) const {
      return std::ranges::find_if(node_list, [&N](const node_type *Node) { return *Node == N; });
    }
    iterator findNode(const node_type &N) {
      return const_cast<iterator>(static_cast<const graph_type &>(*this).findNode(N));
    }

    /// Add the given node \p N to the graph if it is not already present.
    bool addNode(node_type &N) {
      if (findNode(N) != node_list.end())
        return false;
      node_list.push_back(&N);
      return true;
    }

    /// Collect in \p EL all edges that are coming into node \p N. Return true
    /// if at least one edge was found, and false otherwise.
    /*bool findIncomingEdgesToNode(const node_type &N, SmallVectorImpl<edge_type*> &EL) const {
      assert(EL.empty() && "Expected the list of edges to be empty.");
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
    bool removeNode(node_type &N) {
      iterator IT = findNode(N);
      if (IT == node_list.end())
        return false;
      // Remove incoming edges.
      edge_list_type EL;
      for (auto *Node : node_list) {
        if (*Node == N)
          continue;
        Node->findEdgesTo(N, EL);
        for (auto *E : EL)
          Node->removeEdge(*E);
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
      assert(findNode(Src) != node_list.end() && "Src node should be present.");
      assert(findNode(Dst) != node_list.end() && "Dst node should be present.");
      assert((E.getTargetNode() == Dst) && "Target of the given edge does not match Dst.");
      return Src.addEdge(E);
    }

  protected:
    // The list of nodes in the graph.
    node_list_type node_list;
  };



  class ExecutableGraphNode;
  class GraphResourceDesc{};
  class ExecutableGraphNodeInput{};
  class ExecutableGraphNodeOutput{};
  class ExecutableGraphEdge : public DGEdge<ExecutableGraphNode, ExecutableGraphEdge>{
  public:

  private:
  };
  class ExecutableGraphNode : public DGNode<ExecutableGraphNode, ExecutableGraphEdge> {
  public:


  private:
    DynamicArray<GraphResourceDesc*> resourceDescriptions;
  };

  class ExecutableGraph : public DirectedGraph<ExecutableGraphNode>{
  public:


  private:
  };
  
  
  
  
  
  
  
  
  
  

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





#endif //VALKYRIE_CORE_ADT_DIRECTED_GRAPH_HPP
