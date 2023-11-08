/**
 * @file addons/cpp/mixins/system/builder_i.hpp
 * @brief System builder interface.
 */

#pragma once

#include "../query/builder_i.hpp"

namespace flecs 
{

/** System builder interface.
 * 
 * \ingroup cpp_addons_systems
 */
template<typename Base, typename ... Components>
struct system_builder_i : query_builder_i<Base, Components ...> {
private:
    using BaseClass = query_builder_i<Base, Components ...>;

public:
    system_builder_i(ecs_system_desc_t *desc) 
        : BaseClass(&desc->query)
        , m_desc(desc) { }

    /** Specify that the system depends on another system or phase.
     *
     * @param phase The system or phase to depend on.
     */
    Base& depends_on(entity_t other) {
        if (other) {
            ecs_add_id(world_v(), m_desc->entity, ecs_dependson(other));
        }
        return *this;
    }

    /** Specify that the system depends on another system or phase.
     *
     * @tparam Phase The system or phase to depend on.
     */
    template <typename Other>
    Base& depends_on() {
        return this->depends_on(_::cpp_type<Other>::id(world_v()));
    }

    /** Specify that the system should run after another system or phase.
     * 
     * @param other The system or phase to run after.
     */
    Base& after(entity_t other) {
        if (other) {
            ecs_add_id(world_v(), m_desc->entity, ecs_after(other));
        }
        return *this;
    }

    /** Specify that the system should run after another system or phase.
     * 
     * @tparam Other The system or phase to run after.
     */
    template <typename Other>
    Base& after() {
        return this->after(_::cpp_type<Other>::id(world_v()));
    }

    /** Specify that the system should run before another system or phase.
     * 
     * @param other The system or phase to run before.
     */
    Base& before(entity_t other) {
        if (other) {
            ecs_add_id(world_v(), other, ecs_after(m_desc->entity));
        }
        return *this;
    }

    /** Specify that the system should run before another system or phase.
     * 
     * @tparam Other The system or phase to run before.
     */
    template <typename Other>
    Base& before() {
        return this->before(_::cpp_type<Other>::id(world_v()));
    }

    /** Specify whether system can run on multiple threads.
     *
     * @param value If false system will always run on a single thread.
     */
    Base& multi_threaded(bool value = true) {
        m_desc->multi_threaded = value;
        return *this;
    }

    /** Specify whether system should be ran in staged context.
     *
     * @param value If false system will always run staged.
     */
    Base& no_readonly(bool value = true) {
        m_desc->no_readonly = value;
        return *this;
    }

    /** Set system interval.
     * This operation will cause the system to be ran at the specified interval.
     *
     * The timer is synchronous, and is incremented each frame by delta_time.
     *
     * @param interval The interval value.
     */
    Base& interval(ecs_ftime_t interval) {
        m_desc->interval = interval;
        return *this;
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * provided tick source. The tick source may be any entity, including
     * another system.
     *
     * @param tick_source The tick source.
     * @param rate The multiple at which to run the system.
     */
    Base& rate(const entity_t tick_source, int32_t rate) {
        m_desc->rate = rate;
        m_desc->tick_source = tick_source;
        return *this;
    }

    /** Set system rate.
     * This operation will cause the system to be ran at a multiple of the 
     * frame tick frequency. If a tick source was provided, this just updates
     * the rate of the system.
     *
     * @param rate The multiple at which to run the system.
     */
    Base& rate(int32_t rate) {
        m_desc->rate = rate;
        return *this;
    }

    /** Set tick source.
     * This operation sets a shared tick source for the system.
     *
     * @param tick_source The tick source to use for the system.
     */
    Base& tick_source(flecs::entity_t tick_source) {
        m_desc->tick_source = tick_source;
        return *this;
    }

    /** Set system context */
    Base& ctx(void *ptr) {
        m_desc->ctx = ptr;
        return *this;
    }

    /** Set system run callback */
    Base& run(ecs_iter_action_t action) {
        m_desc->run = action;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_system_desc_t *m_desc;
};

/** @} */

}
