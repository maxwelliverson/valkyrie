//
// Created by maxwe on 2021-04-26.
//

#ifndef VALKYRIE_AGENT_AGENCY_HPP
#define VALKYRIE_AGENT_AGENCY_HPP

namespace valkyrie{


  class agency{
    /// Provides system-wide callbacks for several actor operations.
    actor_profiler* profiler_;

    /// Used to generate ascending actor IDs.
    std::atomic<size_t> ids_;

    /// Manages all metrics collected by the system.
    telemetry::metric_registry metrics_;

    /// Stores all metrics that the actor system collects by default.
    base_metrics_t base_metrics_;

    /// Identifies this actor system in a distributed setting.
    node_id node_;

    /// Manages log output.
    intrusive_ptr<caf::logger> logger_;

    /// Maps well-known actor names to actor handles.
    actor_registry registry_;

    /// Maps well-known group names to group handles.
    group_manager groups_;

    /// Stores optional actor system components.
    module_array modules_;

    /// Provides pseudo scheduling context to actors.
    scoped_execution_unit dummy_execution_unit_;

    /// Stores whether the system should wait for running actors on shutdown.
    bool await_actors_before_shutdown_;

    /// Stores config parameters.
    strong_actor_ptr config_serv_;

    /// Allows fully dynamic spawning of actors.
    strong_actor_ptr spawn_serv_;

    /// The system-wide, user-provided configuration.
    actor_system_config& cfg_;

    /// Stores whether the logger has run its destructor and stopped any thread,
    /// file handle, etc.
    std::atomic<bool> logger_dtor_done_;

    /// Guards `logger_dtor_done_`.
    mutable std::mutex logger_dtor_mtx_;

    /// Allows waiting on specific values for `logger_dtor_done_`.
    mutable std::condition_variable logger_dtor_cv_;

    /// Stores the system-wide factory for deserializing tracing data.
    tracing_data_factory* tracing_context_;

    /// Caches the configuration parameter `caf.metrics-filters.actors.includes`
    /// for faster lookups at runtime.
    std::vector<std::string> metrics_actors_includes_;

    /// Caches the configuration parameter `caf.metrics-filters.actors.excludes`
    /// for faster lookups at runtime.
    std::vector<std::string> metrics_actors_excludes_;

    /// Caches families for optional actor metrics.
    actor_metric_families_t actor_metric_families_;

    /// Manages threads for detached actors.
    detail::private_thread_pool private_threads_;

    /// Ties the lifetime of the meta objects table to the actor system.
    detail::global_meta_objects_guard_type meta_objects_guard_;
  };


}

#endif  //VALKYRIE_AGENT_AGENCY_HPP
