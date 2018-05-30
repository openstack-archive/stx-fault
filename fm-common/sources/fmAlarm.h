//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

/*  This file contains CGTS alarm definitions. The alarm ids that used by
 *  C/C++ applications are defined here. For a completed alarm id list see
 *  the alarm ids that used by the Python applications defined in
 *  fm-api/fm_api/constants.py
 */

#ifndef _FM_ALARM_H
#define _FM_ALARM_H

/* alarm sub entity types*/
#define FM_ENTITY_TYPE_SYSTEM       "system"
#define FM_ENTITY_TYPE_HOST         "host"
#define FM_ENTITY_TYPE_PORT         "port"
#define FM_ENTITY_TYPE_INTERFACE    "interface"
#define FM_ENTITY_TYPE_DISK         "disk"
#define FM_ENTITY_TYPE_SERVICE      "service"
#define FM_ENTITY_TYPE_AGENT        "agent"
#define FM_ENTITY_TYPE_PROVIDERNET  "providernet"
#define FM_ENTITY_TYPE_INSTANCE     "instance"

/* alarm_id = <Alarm Group ID>.<Alarm Event ID> */
/* <Alarm Group ID> = 000 - 999 */
/* <Alarm Event ID> = 000  999 */

#define ALARM_GROUP_GENERAL      "100"
#define ALARM_GROUP_MAINTENANCE  "200"
#define ALARM_GROUP_NETWORK      "300"
#define ALARM_GROUP_HA           "400"
#define ALARM_GROUP_SECURITY     "500"
#define ALARM_GROUP_LICENSING    "600"
#define ALARM_GROUP_VM           "700"
#define ALARM_GROUP_STORAGE      "800"
#define ALARM_GROUP_SW_MGMT      "900"


/* Maintenance group alarm id */
#define FM_ALARM_ID_MTC_LOCK      (ALARM_GROUP_MAINTENANCE ".001")
#define FM_ALARM_ID_MTC_CONFIG    (ALARM_GROUP_MAINTENANCE ".004")
#define FM_ALARM_ID_MTC_HB        (ALARM_GROUP_MAINTENANCE ".005")
#define FM_ALARM_ID_MTC_PMOND     (ALARM_GROUP_MAINTENANCE ".006")
#define FM_ALARM_ID_MTC_RESMON    (ALARM_GROUP_MAINTENANCE ".007")

/* HA alarm id */
#define FM_ALARM_ID_SERVICE_GROUP_STATE       (ALARM_GROUP_HA ".001")
#define FM_ALARM_ID_SERVICE_GROUP_REDUNDANCY  (ALARM_GROUP_HA ".002")

/* Patching alarm id */
#define FM_ALARM_ID_PATCH_IN_PROGRESS          (ALARM_GROUP_SW_MGMT ".001")
#define FM_ALARM_ID_PATCH_HOST_INSTALL_FAILED  (ALARM_GROUP_SW_MGMT ".002")
#define FM_ALARM_ID_PATCH_OBS_IN_SYSTEM        (ALARM_GROUP_SW_MGMT ".003")

#endif /* _FM_ALARM_H */
