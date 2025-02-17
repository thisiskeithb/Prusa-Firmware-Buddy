#pragma once

#include <concepts>
#include <cstdint>
#include <FreeRTOS.h>
#include <event_groups.h>
#include <stdint.h>
#include <option/has_puppies.h>
#include <option/has_embedded_esp32.h>
#include "utility_extensions.hpp"

namespace TaskDeps {

/**
 * Code in this module handles task/component dependencies
 *
 * Provided tools enable definition of dependency masks, waiting for dependencies being satisfied and marking dependencies as provided.
 */

using dependency_t = EventBits_t;

/// Definition of different dependencies
enum class Dependency {
    puppies_ready,
    resources_ready,
    default_task_ready,
    usbserial_ready,
    esp_flashed,
    lwip_initialized,
    manufacture_report_sent,
#ifdef USE_ASYNCIO
    async_io_ready,
#endif
    power_panic_initialized,

    _count
};

// Check dependency mask fits into the dependency_t integer
static_assert(ftrstd::to_underlying(Dependency::_count) <= sizeof(dependency_t) * 8);

#define NETWORK_DEPENDS_ON_ASYNCIO     USE_ASYNCIO
#define NETWORK_DEPENDS_ON_ESP_FLASHED (HAS_EMBEDDED_ESP32() && BOARD_VER_HIGHER_OR_EQUAL_TO(0, 5, 0))

// Create dependency mask from the dependencies enum
constexpr dependency_t make(std::same_as<Dependency> auto... dependencies) {
    // Feel free to lift the assert in case some build configuration results in empty list
#if NETWORK_DEPENDS_ON_ASYNCIO || NETWORK_DEPENDS_ON_ESP_FLASHED
    static_assert(sizeof...(dependencies) > 0, "No dependencies, is this intended?");
#endif
    return ((1 << ftrstd::to_underlying(dependencies)) | ... | 0);
}

/// Definitions of dependencies for different tasks/components
namespace Tasks {
    inline constexpr dependency_t default_start = make(
#if HAS_PUPPIES()
        Dependency::puppies_ready,
#endif
        Dependency::usbserial_ready);
    inline constexpr dependency_t puppy_start = make(Dependency::resources_ready, Dependency::manufacture_report_sent);
    inline constexpr dependency_t puppy_run = make(Dependency::default_task_ready);
    inline constexpr dependency_t espif = make(Dependency::esp_flashed);
    inline constexpr dependency_t connect = make(Dependency::lwip_initialized);
    inline constexpr dependency_t network = make(
#if NETWORK_DEPENDS_ON_ESP_FLASHED
        // This is temporary, remove once everyone has compatible hardware.
        // Requires new sandwich rev. 06 or rev. 05 with R83 removed.
        Dependency::esp_flashed
#endif
#if NETWORK_DEPENDS_ON_ESP_FLASHED && NETWORK_DEPENDS_ON_ASYNCIO
        ,
#endif
#if NETWORK_DEPENDS_ON_ASYNCIO
        Dependency::async_io_ready
#endif
    );

} // namespace Tasks

// Needed for inline methods being embedded to different compilation modules
extern EventGroupHandle_t components_ready;

/// Initialize component dependency resolution
void components_init();

/**
 * Check whether all dependencies are fulfilled already
 */
inline bool check(dependency_t dependencies) {
    return xEventGroupGetBits(components_ready) & dependencies;
}

/**
 * Check whether a dependency is fulfilled
 */
inline bool check(Dependency dependency) {
    return check(make(dependency));
}

/**
 * Wait for dependencies of the task/component
 */
inline void wait(dependency_t dependencies) {
    if (dependencies) {
        xEventGroupWaitBits(components_ready, dependencies, pdFALSE, pdTRUE, portMAX_DELAY);
    }
}

/**
 * Mark particular dependency as satisfied
 */
inline void provide(Dependency dependency) {
    xEventGroupSetBits(components_ready, 1 << ftrstd::to_underlying(dependency));
}

} // namespace TaskDeps
