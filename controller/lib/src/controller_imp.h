/*
 * Licensed under the MIT License (MIT)
 *
 * Copyright (c) 2013 AudioScience Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * controller_imp.h
 *
 * Controller implementation class
 */

#pragma once

#include "controller.h"

#include <memory>

namespace avdecc_lib
{
class aecp_controller_state_machine;
class acmp_controller_state_machine;
class adp_discovery_state_machine;
class net_interface_imp;
class notification_imp;
class notification_acmp_imp;
class end_stations;

class controller_imp : public virtual controller
{
private:

    system & m_system_ref;
    net_interface_imp * net_interface_ref;

    std::unique_ptr<notification_imp> notification_imp_ref;
    std::unique_ptr<notification_acmp_imp> notification_acmp_imp_ref;
    std::unique_ptr<aecp_controller_state_machine> aecp_controller_state_machine_ref;
    std::unique_ptr<acmp_controller_state_machine> acmp_controller_state_machine_ref;
    std::unique_ptr<adp_discovery_state_machine> adp_discovery_state_machine_ref;
    std::unique_ptr<end_stations> end_station_array;

    uint32_t m_entity_capabilities_flags;
    uint32_t m_talker_capabilities_flags;
    uint32_t m_listener_capabilities_flags;

    ///
    /// Find an end station that matches the entity and controller IDs
    ///
    int find_in_end_station(struct jdksavdecc_eui64 & entity_entity_id, bool isUnsolicited, const uint8_t * frame);

public:
    ///
    /// A constructor for controller_imp used for constructing an object with notification, and post_log_msg callback functions.
    ///
    controller_imp(system & system_ref, net_interface_imp * netif,
                   void (*notification_callback)(void *, int32_t, uint64_t, uint16_t, uint16_t, uint16_t, uint32_t, void *),
                   void (*acmp_notification_callback)(void *, int32_t, uint16_t, uint64_t, uint16_t, uint64_t,
                                                      uint16_t, uint32_t, void *),
                   void (*log_callback)(void *, int32_t, const char *, int32_t));

    virtual ~controller_imp();

    ///
    /// Call destructor for Controller used for destroying objects
    ///
    void STDCALL destroy();

    const char * STDCALL get_version() const;
    size_t STDCALL get_end_station_count();
    end_station * STDCALL get_end_station_by_index(size_t end_station_index);

    aecp_controller_state_machine & get_aecp_controller_state_machine() { return *aecp_controller_state_machine_ref; }
    acmp_controller_state_machine & get_acmp_controller_state_machine() { return *acmp_controller_state_machine_ref; }
    adp_discovery_state_machine   & get_adp_discovery_state_machine()   { return *adp_discovery_state_machine_ref; }
    notification_imp              & get_notification()                  { return *notification_imp_ref; }

    ///
    /// Check if the corresponding End Station with the Entity ID exists.
    ///
    bool STDCALL is_end_station_found_by_entity_id(uint64_t entity_entity_id, uint32_t & end_station_index);

    ///
    /// Check if the corresponding End Station with the Mac Address exists.
    ///
    bool STDCALL is_end_station_found_by_mac_addr(uint64_t mac_addr, uint32_t & end_station_index);

    configuration_descriptor * STDCALL get_current_config_desc(size_t end_station_index, bool report_error = true);

    configuration_descriptor * STDCALL get_config_desc_by_entity_id(uint64_t entity_entity_id, uint16_t entity_index, uint16_t config_index);

    ///
    /// Check if the command with the corresponding notification id is in the inflight list.
    ///
    bool is_inflight_cmd_with_notification_id(void * notification_id);

    bool is_active_operation_with_notification_id(void * notification_id);

    void STDCALL set_logging_level(int32_t new_log_level);

    void STDCALL apply_end_station_capabilities_filters(uint32_t entity_capabilities_flags,
                                                        uint32_t talker_capabilities_flags,
                                                        uint32_t listener_capabilities_flags);

    uint32_t STDCALL missed_notification_count();
    uint32_t STDCALL missed_log_count();

    ///
    /// Check for End Station connection, command packet, and response packet timeouts.
    ///
    void time_tick_event();

    ///
    /// Lookup and process packet received.
    ///
    void rx_packet_event(void *& notification_id, bool & is_notification_id_valid, const uint8_t * frame, size_t frame_len, int & status, uint16_t & operation_id, bool & is_operation_id_valid);

    ///
    /// Send queued packet to the AEM Controller State Machine.
    ///
    void tx_packet_event(void * notification_id, uint32_t notification_flag, uint8_t * frame, size_t frame_len);

    int STDCALL send_controller_avail_cmd(void * notification_id, uint32_t end_station_index);

    int STDCALL send_controller_avail_response(const uint8_t * frame, size_t frame_len);

    ///
    /// Process a CONTROLLER_AVAILABLE response for the CONTROLLER_AVAILABLE command.
    ///
    int proc_controller_avail_resp(void *& notification_id, const uint8_t * frame, size_t frame_len, int & status);

    size_t STDCALL system_queue_tx(void * notification_id, uint32_t notification_flag, uint8_t * frame, size_t frame_len);

};

}
