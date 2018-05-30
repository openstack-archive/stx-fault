/*
 * Copyright (c) 2017 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
 */



#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/callback.h>
#include <net-snmp/library/snmp_transport.h>
#include <net-snmp/library/snmp_api.h>
#include <net-snmp/library/mib.h>
#include <net-snmp/library/snmp.h>
#include <net-snmp/library/vacm.h>
#include <net-snmp/library/snmpUDPDomain.h>
#include <net-snmp/library/tools.h>
#include <net-snmp/agent/agent_callbacks.h>
#include <net-snmp/agent/agent_handler.h>
#include <net-snmp/agent/agent_registry.h>

#define LOG_BUF_STR (256)

#define AUDIT_TAG "snmp-auditor"

/* Used to keep track of the first handler call for a transaction */
typedef struct s_audit_req_t {
    long transid;
    long reqid;
} audit_req_t;

typedef struct s_enum_to_string_t {
    const int enumval;
    const char *str;
} enum_to_string_t;


/* Logs IP session information, in the format: "remote IP:port ==> local IP:port" */
static inline char* fmtaddr(const char *prefix, int af,
                            void *remote_addr, unsigned short remote_port,
                            char*buf, size_t buflen)
{
    char remote_addr_str[LOG_BUF_STR+1];

    if (NULL == inet_ntop(af, remote_addr, remote_addr_str, sizeof(remote_addr_str))) {
        strncpy(remote_addr_str, "UNKNOWN", LOG_BUF_STR+1);
    }
    remote_addr_str[LOG_BUF_STR] = 0;

    snprintf(buf, buflen, "transport:%s remote:%s", prefix, remote_addr_str);
    return buf;
}


#ifdef NETSNMP_ENABLE_IPV6
static char* ipv6_fmtaddr(const char *prefix, netsnmp_addr_pair *addr_pair, char*buf, size_t buflen)
{
    return fmtaddr(prefix, AF_INET6,
                   (void *)&addr_pair->remote_addr.sin6.sin6_addr, addr_pair->remote_addr.sin6.sin6_port,
                   buf, buflen);
}
#endif


static char* ipv4_fmtaddr(const char *prefix, netsnmp_addr_pair *addr_pair, char*buf, size_t buflen)
{
    return fmtaddr(prefix, AF_INET,
                   (void *)&addr_pair->remote_addr.sin.sin_addr, addr_pair->remote_addr.sin.sin_port,
                   buf, buflen);
}


/* Logs IP session information */
static char* log_session_addresses(const oid* tDomain, netsnmp_addr_pair *addr_pair, char*buf, size_t buflen)
{
    if (tDomain == netsnmpUDPDomain) {
        return ipv4_fmtaddr("udp", addr_pair, buf, buflen);
    }

    if (tDomain == netsnmp_snmpTCPDomain) {
        return ipv4_fmtaddr("tcp", addr_pair, buf, buflen);
    }

#ifdef NETSNMP_ENABLE_IPV6
    if (tDomain == netsnmp_UDPIPv6Domain) {
        return ipv6_fmtaddr("udpv6", addr_pair, buf, buflen);
    }

    if (tDomain == netsnmp_TCPIPv6Domain) {
        return ipv6_fmtaddr("tcpv6", addr_pair, buf, buflen);
    }
#endif
    strncpy(buf, "IP FMT ERROR", buflen);
    buf[buflen-1] = 0;
    return buf;
}


/* SNMP OID formatting (a wrapper around the 'standard' function */
static inline char* fmtoid(const oid * theoid, size_t len, int* no_overflow)
{
    u_char *buf = NULL;
    size_t buf_len = 0;
    size_t out_len = 0;

    *no_overflow = sprint_realloc_objid(&buf, &buf_len, &out_len, 1, theoid, len);
    if (NULL == buf) {
        *no_overflow = 0;
    }

    return (char*)buf;
}


/* SNMP var bind formatting (a convenience function) - formats the OID (variable name)
   This function is always called with var != NULL */
static inline char* fmtmsg_var(netsnmp_variable_list * var, int* no_overflow)
{
    return fmtoid(var->name, var->name_length, no_overflow);
}

static const char* get_version(long version)
{
    switch (version) {
    case 0:  return "v1";
    case 1:  return "v2c";
    case 2:  return "v3";
    }
    return "error";
}


static const char *get_str_from_enum(int enumval, const enum_to_string_t* table, const char* defval)
{
    const enum_to_string_t* ptr = table;

    for ( ; ptr->str != NULL; ++ptr) {
        if (ptr->enumval == enumval) {
            return ptr->str;
        }
    }
    if (NULL == defval) {
        return "unknown";
    }
    return defval;
}


static const char *get_auth_error(int errorcode)
{
    static enum_to_string_t errorcodes_str[] = {
        { VACM_SUCCESS,        "Success", },
        { VACM_NOSECNAME,      "InvalidCommunityName" },
        { VACM_NOGROUP,        "NoGroup" },
        { VACM_NOACCESS,       "NoAccess" },
        { VACM_NOVIEW,         "NoViewAccess" },
        { VACM_NOTINVIEW,      "NotInView" },
        { VACM_NOSUCHCONTEXT,  "NoSuchContext" },
        { VACM_SUBTREE_UNKNOWN,"SubtreeUnknown" },
        {0, NULL}
    };
    return get_str_from_enum(errorcode, errorcodes_str, "unknown err");
}

static const char *get_result_error(int errorcode)
{
    static enum_to_string_t errorcodes_str[] = {
        { 0, "pass" },
        { SNMP_NOSUCHOBJECT,   "NoSuchObject" },
        { SNMP_NOSUCHINSTANCE, "NoSuchInstance" },
        { SNMP_ENDOFMIBVIEW,   "EndOfMIBView" },
        {0, NULL}
    };
    return get_str_from_enum(errorcode, errorcodes_str, "pass");
}


/* Logs all var-binds in PDU (only variable names, aka OID's) */
static void log_var_list(netsnmp_pdu *pdu)
{
    netsnmp_variable_list * var;

    for (var = pdu->variables; var != NULL; var = var->next_variable) {
        int no_overflow_var = 0;
        char* var_str = fmtmsg_var(var, &no_overflow_var);

        snmp_log(LOG_INFO, AUDIT_TAG"    reqid:%ld oid:%s%s\n",
                 pdu->reqid,
                 (var_str != NULL) ? var_str : "INVALID",
                 (no_overflow_var) ? "" : " [TRUNCATED]");
        free(var_str);
    }
}

/* Logs the 'header' of a PDU/request (IP addresses, reqid, msg type, version) */
static void log_pdu_header(netsnmp_pdu *pdu, const char *status)
{
    char buf[LOG_BUF_STR];
    netsnmp_addr_pair *addr_pair = (netsnmp_addr_pair *)pdu->transport_data;

    snmp_log(LOG_INFO,  AUDIT_TAG" %s reqid:%ld msg-type:%s version:%s%s\n",
             log_session_addresses(pdu->tDomain, addr_pair, buf, sizeof(buf)),
             pdu->reqid, snmp_pdu_type(pdu->command), get_version(pdu->version), status);
}

/* Logs the results of a request, namely results obtained from actual processing handlers */
static void log_results(long reqid, netsnmp_request_info *requests)
{
    netsnmp_request_info *req;

    for (req = requests; req != NULL; req = req->next) {
        netsnmp_variable_list *var = req->requestvb;

        if (NULL == var) {
          continue;
        }
        if (var->type != ASN_NULL) { /* NULL means no result, so skip */
            int no_overflow_var = 0;
            char* var_str = fmtmsg_var(var, &no_overflow_var);

            /* Print only first variable: this is the request that we get a result for */
            snmp_log(LOG_INFO,  AUDIT_TAG"    reqid:%ld oid:%s%s status:%s\n", reqid,
                     (var_str != NULL) ? var_str : "INVALID",
                     (no_overflow_var) ? "" : " [TRUNCATED]",   get_result_error(var->type));
            free(var_str);
        }
    }
}


static void log_invalid_oid_trees(netsnmp_pdu *pdu)
{
    char buf[LOG_BUF_STR];
    netsnmp_variable_list *var;
    netsnmp_addr_pair *addr_pair = (netsnmp_addr_pair *)pdu->transport_data;
    int first_time = 1;

    for (var = pdu->variables; var != NULL; var = var->next_variable) {
        netsnmp_subtree *tp = netsnmp_subtree_find(var->name, var->name_length,
                                                   NULL, pdu->contextName);
        if (tp != NULL) {
            int prefix_len = netsnmp_oid_find_prefix(tp->start_a,
                                                     tp->start_len,
                                                     tp->end_a, tp->end_len);
            while (prefix_len < 1) {
                tp = tp->next;
                if (NULL == tp) {
                    break;
                }
                prefix_len = netsnmp_oid_find_prefix(tp->start_a,
                                                     tp->start_len,
                                                     tp->end_a, tp->end_len);
            }
            DEBUGMSGTL(("helper:snmpAudit", "var=%p tp=%p prefix_len=%d\n", var, tp, prefix_len ));
        }
        else {
            DEBUGMSGTL(("helper:snmpAudit", "tp NOT found var=%p\n", var));
        }
        if (NULL == tp) {
            int no_overflow_var = 0;
            char* var_str = fmtmsg_var(var, &no_overflow_var);

            if (first_time) {
                first_time = 0;
                snmp_log(LOG_INFO,  AUDIT_TAG" %s reqid:%ld msg-type:%s version:%s\n",
                         log_session_addresses(pdu->tDomain, addr_pair, buf, sizeof(buf)), pdu->reqid,
                         snmp_pdu_type(pdu->command),
                         get_version(pdu->version));
                log_var_list(pdu);
            }

            snmp_log(LOG_INFO,  AUDIT_TAG"    reqid:%ld oid:%s%s status:%s\n",
                     pdu->reqid,
                     (var_str != NULL) ? var_str : "INVALID",
                     (no_overflow_var) ? "" : " [TRUNCATED]",
                     get_result_error(SNMP_ENDOFMIBVIEW));
            free(var_str);
        }
    }
}

/*
 * Register with 'SNMPD_CALLBACK_ACM_CHECK_INITIAL == minorId'
 * This function is used to log authorization errors and invalid OID's errors,
 * for GET BULK and GET NEXT requests
 */
static int audit_callback_acm_check_initial(int majorID, int minorID, void *serverarg,
                              void *clientarg)
{
    struct view_parameters *view_parms =
        (struct view_parameters *) serverarg;
    netsnmp_pdu    *pdu = view_parms->pdu;

    DEBUGMSGTL(("helper:snmpAudit", "%s  msg-type: %s errcode=%d minorID=%d\n",
                __FUNCTION__, snmp_pdu_type(pdu->command), view_parms->errorcode, minorID));

    if (view_parms->errorcode != VACM_SUCCESS) {
        /* Log Authentication errors */
        char buf[LOG_BUF_STR];
        netsnmp_addr_pair *addr_pair = (netsnmp_addr_pair *)pdu->transport_data;

        snmp_log(LOG_INFO, AUDIT_TAG" %s reqid:%ld msg-type:%s version:%s status:%s\n",
                 log_session_addresses(pdu->tDomain, addr_pair, buf, sizeof(buf)), pdu->reqid,
                 snmp_pdu_type(pdu->command), get_version(pdu->version),
                 get_auth_error(view_parms->errorcode));
        log_var_list(pdu);
        return 0;
    }

    if (SNMP_MSG_GETBULK == pdu->command ||
        SNMP_MSG_GETNEXT == pdu->command) {
        /* Log possible invalid OID subtrees for GETNEXT and GETBULK request
         *  (e.g. "1.10" - outside the normal ISO MIB subtree)
         */
        log_invalid_oid_trees(pdu);
    }
    return 0;
}

/*
 * Register with 'SNMPD_CALLBACK_ACM_CHECK == minorId'
 * This function is used to log SET requests (which are normally rejected)
 */
static int audit_callback_acm_check(int majorID, int minorID, void *serverarg,
                       void *clientarg)
{
    struct view_parameters *view_parms =
        (struct view_parameters *) serverarg;
    netsnmp_pdu    *pdu = view_parms->pdu;

    DEBUGMSGTL(("helper:snmpAudit", "%s  msg-type: %s errcode=%d minorID=%d\n",
                __FUNCTION__, snmp_pdu_type(pdu->command), view_parms->errorcode, minorID));
    if (SNMP_MSG_SET == pdu->command) {
        char status_buf[LOG_BUF_STR];
        snprintf(status_buf, LOG_BUF_STR,
                 " status:%s", get_auth_error(view_parms->errorcode));
        log_pdu_header(pdu, status_buf);
        log_var_list(pdu);
    }
    return 0;
}


/* Main log handler function: logs 'normal' requests:
 *  everything except SET operations, authentication errors and GETBULK/GETNEXT for invalid OIDs */
static int audit_log_handler(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info *reqinfo,
                          netsnmp_request_info *requests)
{
    static audit_req_t req = {
        .transid = 0,
        .reqid   = 0
    };
    netsnmp_pdu    *orig_pdu = reqinfo->asp->orig_pdu;
    int ret;

    /* Note. Assumes single-threaded processing. */
    if ((req.transid != orig_pdu->transid) &&
        (req.reqid != orig_pdu->reqid)) {

        /* New transaction */
        req.transid = orig_pdu->transid;
        req.reqid   = orig_pdu->reqid;

        /*  Logs session information (e.g. IP addresses, version...) */
        log_pdu_header(orig_pdu, "");
        /*  Logs the variables names in the request */
        log_var_list(orig_pdu);
    }
    /* Calls the next handlers, to obtain processing results */
    ret = netsnmp_call_next_handler(handler, reginfo, reqinfo, requests);
    /* Logs the variables names in the results
     *   resulted from the calls to 'netsnmp_call_next_handler' above
     *      which invokes all other handlers in the chain.
    */
    log_results(orig_pdu->reqid, requests);
    return ret;
}



/*
 * Initialization routine, automatically called by the agent
 * (to get called, the function name must match init_FILENAME())
 */
extern "C" void init_snmpAuditPlugin(void) {

    netsnmp_mib_handler *audit_handler = NULL;

    snmp_log(LOG_INFO, "init_snmpAuditPlugin\n");
    audit_handler = netsnmp_create_handler("snmpAudit",
                                           audit_log_handler);
    if (audit_handler != NULL) {
        netsnmp_register_handler_by_name("snmpAudit", audit_handler);
    }

    netsnmp_register_callback(SNMP_CALLBACK_APPLICATION, SNMPD_CALLBACK_ACM_CHECK,
                              audit_callback_acm_check,
                              NULL, NETSNMP_CALLBACK_LOWEST_PRIORITY );
    netsnmp_register_callback(SNMP_CALLBACK_APPLICATION, SNMPD_CALLBACK_ACM_CHECK_INITIAL,
                              audit_callback_acm_check_initial,
                              NULL, NETSNMP_CALLBACK_LOWEST_PRIORITY);
}

extern "C" void deinit_snmpAuditPlugin(void)
{
    snmp_log(LOG_INFO, "deinit_snmpAuditPlugin\n");
    snmp_unregister_callback(SNMP_CALLBACK_APPLICATION, SNMPD_CALLBACK_ACM_CHECK,
                             audit_callback_acm_check, NULL, 1);
    snmp_unregister_callback(SNMP_CALLBACK_APPLICATION, SNMPD_CALLBACK_ACM_CHECK_INITIAL,
                             audit_callback_acm_check_initial, NULL, 1);
}
