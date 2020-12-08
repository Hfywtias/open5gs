/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PCF_CONTEXT_H
#define PCF_CONTEXT_H

#include "ogs-app.h"
#include "ogs-crypt.h"
#include "ogs-sbi.h"

#include "pcf-sm.h"
#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NUM_OF_SERVED_GUAMI     8

extern int __pcf_log_domain;

#undef OGS_LOG_DOMAIN
#define OGS_LOG_DOMAIN __pcf_log_domain

typedef struct pcf_context_s {
    OpenAPI_nf_type_e   nf_type;

    ogs_list_t      pcf_ue_list;
    ogs_hash_t      *suci_hash;
    ogs_hash_t      *supi_hash;

} pcf_context_t;

struct pcf_ue_s {
    ogs_sbi_object_t sbi;
    ogs_fsm_t sm;

    OpenAPI_auth_event_t *auth_event;
    OpenAPI_amf3_gpp_access_registration_t *amf_3gpp_access_registration;

    char *ctx_id;
    char *suci;
    char *supi;
    char *serving_network_name;

    char *ausf_instance_id;
    char *amf_instance_id;

    char *dereg_callback_uri;

    uint8_t k[OGS_KEY_LEN];
    uint8_t opc[OGS_KEY_LEN];
    uint8_t amf[OGS_AMF_LEN];
    uint8_t rand[OGS_RAND_LEN];
    uint8_t sqn[OGS_SQN_LEN];

    ogs_guami_t guami;

    OpenAPI_auth_type_e auth_type;

#define PCF_NF_INSTANCE_CLEAR(_cAUSE, _nFInstance) \
    do { \
        ogs_assert(_nFInstance); \
        if ((_nFInstance)->reference_count == 1) { \
            ogs_info("[%s] (%s) NF removed", (_nFInstance)->id, (_cAUSE)); \
            pcf_nf_fsm_fini((_nFInstance)); \
        } else { \
            /* There is an assocation with other context */ \
            ogs_info("[%s:%d] (%s) NF suspended", \
                    _nFInstance->id, _nFInstance->reference_count, (_cAUSE)); \
            OGS_FSM_TRAN(&_nFInstance->sm, pcf_nf_state_de_registered); \
            ogs_fsm_dispatch(&_nFInstance->sm, NULL); \
        } \
        ogs_sbi_nf_instance_remove(_nFInstance); \
    } while(0)
};

void pcf_context_init(void);
void pcf_context_final(void);
pcf_context_t *pcf_self(void);

int pcf_context_parse_config(void);

pcf_ue_t *pcf_ue_add(char *suci);
void pcf_ue_remove(pcf_ue_t *pcf_ue);
void pcf_ue_remove_all(void);
pcf_ue_t *pcf_ue_find_by_suci(char *suci);
pcf_ue_t *pcf_ue_find_by_supi(char *supi);
pcf_ue_t *pcf_ue_find_by_suci_or_supi(char *suci_or_supi);
pcf_ue_t *pcf_ue_find_by_ctx_id(char *ctx_id);

pcf_ue_t *pcf_ue_cycle(pcf_ue_t *pcf_ue);

#ifdef __cplusplus
}
#endif

#endif /* PCF_CONTEXT_H */