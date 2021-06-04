//
// Created by maxwe on 2021-05-15.
//

#ifndef VALKYRIE_WRITE_STREAM_HPP
#define VALKYRIE_WRITE_STREAM_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie {

  class write_stream{

  protected:

    virtual void do_write(const byte* bytes, u64 length) noexcept = 0;
  };

  template <typename GraphType>
  struct dot_graph_traits{


    using graph_type = typename graph_traits<GraphType>::graph_type;
    using node_ref   = typename graph_traits<GraphType>::node_ref;
    using node_type  = typename graph_traits<GraphType>::node_type;
    using edge_ref   = typename graph_traits<GraphType>::edge_ref;
    using edge_type  = typename graph_traits<GraphType>::edge_type;

    using nodes_iterator      = typename graph_traits<GraphType>::nodes_iterator;
    using child_iterator      = typename graph_traits<GraphType>::child_iterator;
    using child_edge_iterator = typename graph_traits<GraphType>::child_edge_iterator;



    /// getGraphName - Return the label for the graph as a whole.  Printed at the
    /// top of the graph.
    ///
    static std::string get_graph_name(const graph_type& ) { return ""; }

    /// getGraphProperties - Return any custom properties that should be included
    /// in the top level graph structure for dot.
    ///
    static std::string get_graph_properties(const graph_type &) {
      return "";
    }

    /// renderGraphFromBottomUp - If this function returns true, the graph is
    /// emitted bottom-up instead of top-down.  This requires graphviz 2.0 to work
    /// though.
    static bool render_graph_from_bottom_up() {
      return false;
    }

    /// isNodeHidden - If the function returns true, the given node is not
    /// displayed in the graph.
    static bool is_node_hidden(node_ref, const graph_type &) {
      return false;
    }

    /// getNodeLabel - Given a node and a pointer to the top level graph, return
    /// the label to print in the node.
    static std::string get_node_label(node_ref, const GraphType &) {
      return "";
    }


    // getNodeIdentifierLabel - Returns a string representing the
    // address or other unique identifier of the node. (Only used if
    // non-empty.)
    static std::string get_node_identifier_label(node_ref, const graph_type& ) {
      return "";
    }

    static std::string get_node_description(node_ref, const graph_type& ) {
      return "";
    }

    /// If you want to specify custom node attributes, this is the place to do so
    ///
    static std::string get_node_attributes(node_ref, const graph_type&) {
      return "";
    }

    /// If you want to override the dot attributes printed for a particular edge,
    /// override this method.
    static std::string get_edge_attributes(node_ref, in_param_t<child_edge_iterator>, const graph_type&) {
      return "";
    }

    /// getEdgeSourceLabel - If you want to label the edge source itself,
    /// implement this method.
    static std::string get_edge_source_label(node_ref, in_param_t<child_edge_iterator>) {
      return "";
    }

    /// edgeTargetsEdgeSource - This method returns true if this outgoing edge
    /// should actually target another edge source, not a node.  If this method is
    /// implemented, getEdgeTarget should be implemented.
    static bool edge_targets_edge_source(node_ref, in_param_t<child_edge_iterator>) {
      return false;
    }

    /// getEdgeTarget - If edgeTargetsEdgeSource returns true, this method is
    /// called to determine which outgoing edge of Node is the target of this
    /// edge.
    static child_edge_iterator get_edge_target(node_ref, in_param_t<child_edge_iterator> I) noexcept {
      return I;
    }

    /// hasEdgeDestLabels - If this function returns true, the graph is able
    /// to provide labels for edge destinations.
    static bool has_edge_dest_labels() noexcept {
      return false;
    }

    /// numEdgeDestLabels - If hasEdgeDestLabels, this function returns the
    /// number of incoming edge labels the given node has.
    static u32 num_edge_dest_labels(node_ref node) noexcept {
      return 0;
    }

    /// getEdgeDestLabel - If hasEdgeDestLabels, this function returns the
    /// incoming edge label with the given index in the given node.
    static std::string get_edge_dest_label(node_ref, u32) noexcept {
      return "";
    }

    /// addCustomGraphFeatures - If a graph is made up of more than just
    /// straight-forward nodes and edges, this is the place to put all of the
    /// custom stuff necessary.  The GraphWriter object, instantiated with your
    /// GraphType is passed in as an argument.  You may call arbitrary methods on
    /// it to add things to the output graph.
    ///
    template<typename GraphWriter>
    static void add_custom_graph_features(const graph_type&, GraphWriter &) {}
  };
}

#endif  //VALKYRIE_WRITE_STREAM_HPP
