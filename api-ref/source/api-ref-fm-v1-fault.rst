====================================================
Fault Management API v1
====================================================

The API supports alarm and event collection of the cloud platform
itself.

The typical port used for the FM REST API is 18002. However, proper
technique would be to look up the FM service endpoint in Keystone.

-------------
API versions
-------------

**************************************************************************
Lists information about Fault Management API versions
**************************************************************************

.. rest_method:: GET /

**Normal response codes**

200, 300

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

::

   {
      "default_version":{
         "id":"v1",
         "links":[
            {
               "href":"http://128.224.150.54:18002/v1/",
               "rel":"self"
            }
         ]
      },
      "versions":[
         {
            "id":"v1",
            "links":[
               {
                  "href":"http://128.224.150.54:18002/v1/",
                  "rel":"self"
               }
            ]
         }
      ],
      "description":"StarlingX Fault API allows for the management of physical servers.  This includes inventory collection and configuration of hosts, ports, interfaces, CPUs, disk, memory, and system configuration.  The API also supports the configuration of the cloud's SNMP interface. ",
      "name":"stx-fault API"
   }

This operation does not accept a request body.

*******************************************
Shows details for Fault Management API v1
*******************************************

.. rest_method:: GET /v1

**Normal response codes**

200, 203

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

::

   {
       "media_types": [
           {
               "base": "application/json",
               "type": "application/vnd.openstack.fm.v1+json"
           }
       ],
       "links": [
           {
               "href": "http://127.168.204.2:18002/v1/",
               "rel": "self"
           },
           {
               "href": "http://www.windriver.com/developer/fm/dev/api-spec-v1.html",
               "type": "text/html",
               "rel": "describedby"
           }
       ],
       "event_log": [
           {
               "href": "http://127.168.204.2:18002/v1/event_log/",
               "rel": "self"
           },
           {
               "href": "http://127.168.204.2:18002/event_log/",
               "rel": "bookmark"
           }
       ],
       "alarms": [
           {
               "href": "http://127.168.204.2:18002/v1/alarms/",
               "rel": "self"
           },
           {
               "href": "http://127.168.204.2:18002/alarms/",
               "rel": "bookmark"
           }
       ],
       "event_suppression": [
           {
               "href": "http://127.168.204.2:18002/v1/event_suppression/",
               "rel": "self"
           },
           {
               "href": "http://127.168.204.2:18002/event_suppression/",
               "rel": "bookmark"
           }
       ],
       "id": "v1"
   }

This operation does not accept a request body.

-------
Alarms
-------

These APIs allow the display of the Active Alarms in the system.

**************************************************
Lists all active alarms based on specified query
**************************************************

.. rest_method:: GET /v1/alarms

The supported query options are alarm_id, entity_type_id,
entity_instance_id, severity and alarm_type.

**Normal response codes**

200

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "include_suppress (Optional)", "query", "xsd:boolean", "This optional parameter when set to true (include_suppress=true) specifies to include suppressed alarms in output."

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "ialarms (Optional)", "plain", "xsd:list", "The list of active alarms based on the specified query."
   "alarm_id (Optional)", "plain", "xsd:string", "The alarm ID; each type of alarm has a unique ID. Note the alarm_id and the entity_instance_id uniquely identify an alarm instance."
   "entity_instance_id (Optional)", "plain", "xsd:string", "The instance of the object raising alarm. A . separated list of sub-entity-type=instance-value pairs, representing the containment structure of the overall entity instance. Note the alarm_id and the entity_instance_id uniquely identify an alarm instance."
   "reason_text (Optional)", "plain", "xsd:string", "The text description of the alarm."
   "severity (Optional)", "plain", "xsd:string", "The severity of the alarm; ``critical``, ``major``, ``minor``, or ``warning``."
   "timestamp (Optional)", "plain", "xsd:dateTime", "The time in UTC at which the alarm has last been updated."
   "uuid (Optional)", "plain", "csapi:UUID", "The unique identifier of the alarm."

::

   http://192.168.204.2:18002/v1/alarms?q.field=severity&q.op=eq&q.type=&q.value=major&include_suppress=True

::

   {
      "ialarms":[
         {
            "severity":"major",
            "timestamp":"2016-05-12T12:11:10.405609",
            "uuid":"25d28c97-70e4-45c7-a896-ba8e71a81f26",
            "alarm_id":"400.002",
            "entity_instance_id":"service_domain=controller.service_group=oam-services",
            "suppression_status":"suppressed",
            "reason_text":"Service group oam-services loss of redundancy; expected 1 standby member but no standby members available",
            "mgmt_affecting": "warning"
         },
         {
            "severity":"major",
            "timestamp":"2016-05-12T12:08:12.850730",
            "uuid":"63c94239-2d16-4a30-a910-60198de1c0a8",
            "alarm_id":"200.001",
            "entity_instance_id":"host=controller-1",
            "suppression_status":"unsuppressed",
            "reason_text":"controller-1 was administratively locked to take it out-of-service.",
            "mgmt_affecting": "warning"
         }
      ]
   }

******************************************
Shows information about a specific alarm
******************************************

.. rest_method:: GET /v1/alarms/​{alarm_uuid}​

**Normal response codes**

200

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "alarm_uuid", "URI", "csapi:UUID", "The unique identifier of an existing active alarm."

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "alarm_id (Optional)", "plain", "xsd:string", "The alarm ID; each type of alarm has a unique ID. Note the alarm_id and the entity_instance_id uniquely identify an alarm instance."
   "entity_instance_id (Optional)", "plain", "xsd:string", "The instance of the object raising alarm. A . separated list of sub-entity-type=instance-value pairs, representing the containment structure of the overall entity instance. Note the alarm_id and the entity_instance_id uniquely identify an alarm instance."
   "reason_text (Optional)", "plain", "xsd:string", "The text description of the alarm."
   "severity (Optional)", "plain", "xsd:string", "The severity of the alarm; ``critical``, ``major``, ``minor``, or ``warning``."
   "timestamp (Optional)", "plain", "xsd:dateTime", "The time in UTC at which the alarm has last been updated."
   "uuid (Optional)", "plain", "csapi:UUID", "The unique identifier of the alarm."
   "alarm_state (Optional)", "plain", "xsd:string", "The state of the alarm; ``set`` or ``clear``"
   "service_affecting (Optional)", "plain", "xsd:string", "Indicates whether the alarm affects the service."
   "proposed_repair_action (Optional)", "plain", "xsd:string", "The proposed action to clear the alarm."
   "alarm_type (Optional)", "plain", "xsd:string", "The type of the alarm."
   "entity_type_id (Optional)", "plain", "xsd:string", "The type of the object raising the alarm. A . separated list of sub-entity-type, representing the containment structure of the overall entity type."
   "probable_cause (Optional)", "plain", "xsd:string", "The probable cause of the alarm."
   "suppression (Optional)", "plain", "xsd:string", "Indicates whether suppression of the specific alarm is allowed."
   "uuid (Optional)", "plain", "csapi:UUID", "The universally unique identifier for this object."
   "links (Optional)", "plain", "xsd:list", "For convenience, resources contain links to themselves. This allows a client to easily obtain rather than construct resource URIs. The following types of link relations are associated with resources: a self link containing a versioned link to the resource, and a bookmark link containing a permanent link to a resource that is appropriate for long term storage."
   "created_at (Optional)", "plain", "xsd:dateTime", "The time when the object was created."
   "updated_at (Optional)", "plain", "xsd:dateTime", "The time when the object was last updated."

::

   {
       "alarm_state": "set",
       "service_affecting": "True",
       "proposed_repair_action": "contact next level of support",
       "alarm_type": "processing-error",
       "severity": "minor",
       "created_at": "2014-09-23T00:38:16.797155+00:00",
       "entity_type_id": "host",
       "probable_cause": "key-expired",
       "updated_at": "2014-09-24T00:38:17.403135+00:00",
       "alarm_id": "400.003",
       "entity_instance_id": "system=big_lab.host=controller-0",
       "suppression": "False",
       "timestamp": "2014-09-24T00:38:17.400169+00:00",
       "uuid": "2a88acd3-e9eb-432e-bc0a-4276e3537a40",
       "reason_text": "evaluation license key will exprire on 31-dec-2014",
       "mgmt_affecting": "critical"
   }

This operation does not accept a request body.

**************************
Deletes a specific alarm
**************************

.. rest_method:: DELETE /v1/alarms/​{alarm_uuid}​

NOTE Typically this command should NOT be used. I.e typically alarms
will be and should be cleared by the system when the alarm condition
clears. This command is only provided in the event that the alarm has
cleared but for some reason the system has not removed the alarm.

**Normal response codes**

204

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "alarm_uuid", "URI", "csapi:UUID", "The unique identifier of an existing active alarm."

This operation does not accept a request body.

*****************************************
Summarize all active alarms by severity
*****************************************

.. rest_method:: SUMMARY /v1/alarms/summary

**Normal response codes**

200

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413)

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "include_suppress (Optional)", "query", "xsd:boolean", "This optional parameter when set to true (include_suppress=true) specifies to include suppressed alarms in the summations (default false)."

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "system_uuid (Optional)", "plain", "csapi:UUID", "UUID of the system."
   "status (Optional)", "plain", "xsd:string", "Overall system status based on alarms present; ``critical``, ``degraded``, or ``OK``."
   "critical (Optional)", "plain", "xsd:integer", "Count of critical alarms on the system"
   "major (Optional)", "plain", "xsd:integer", "Count of major alarms on the system"
   "minor (Optional)", "plain", "xsd:integer", "Count of minor alarms on the system"
   "warnings (Optional)", "plain", "xsd:integer", "Count of warnings on the system"

::

   {
       "status": "OK",
       "major": 0,
       "warnings": 0,
       "system_uuid": "6a314f63-4969-46f8-9221-b69b6b50424c",
       "critical": 0,
       "minor": 0
   }

----------
Event Log
----------

These APIs allow the display of the Event Log in the system. The Event
log contains both historical alarms and customer logs.

*******************************************************************************************************************************************
Lists all event logs (historical alarms and customer logs) based on specified query. The logs are returned in reverse chronological order
*******************************************************************************************************************************************

.. rest_method:: GET /v1/event_log

The supported query options are event_log_id, entity_type_id,
entity_instance_id, severity, event_log_type, start and end.

**Normal response codes**

200

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "q (Optional)", "query", "xsd:list", "This parameter specifies filter rules for the logs to be returned."
   "limit (Optional)", "query", "xsd:int", "This parameter specifies the maximum number of event logs to be returned."
   "alarms (Optional)", "query", "xsd:boolean", "This optional parameter when set to true (alarms=true) specifies that only alarm event log records should be returned."
   "logs (Optional)", "query", "xsd:boolean", "This optional parameter when set to true (logs=true) specifies that only customer log records should be returned."
   "include_suppress (Optional)", "query", "xsd:boolean", "This optional parameter when set to true (include_suppress=true) specifies to include suppressed alarms in output."

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "event_log (Optional)", "plain", "xsd:list", "The list of events log based on the specified query."
   "event_log_id (Optional)", "plain", "xsd:string", "The event log ID; each type of event log has a unique ID. Note the event_log_id and the entity_instance_id uniquely identify an event log instance."
   "state (Optional)", "plain", "xsd:string", "The state of the event; ``set``, ``clear`` or ``log``"
   "entity_instance_id (Optional)", "plain", "xsd:string", "The instance of the object generating the event log. A . separated list of sub-entity-type=instance-value pairs, representing the containment structure of the overall entity instance. Note the event_log_id and the entity_instance_id uniquely identify an event log instance."
   "reason_text (Optional)", "plain", "xsd:string", "The text description of the event log."
   "severity (Optional)", "plain", "xsd:string", "The severity of the event log; ``critical``, ``major``, ``minor`` or ``warning``."
   "timestamp (Optional)", "plain", "xsd:dateTime", "The time in UTC at which the event log has last been updated."
   "uuid (Optional)", "plain", "csapi:UUID", "The unique identifier of the event log."
   "next (Optional)", "plain", "xsd:string", "The next attribute is the request to use to get the next n items. It is used to paginate the event log list."

::

   http://192.168.204.2:18002/v1/event_log?q.field=start&q.field=end&q.op=eq&q.op=eq&q.type=&q.type=&q.value=2014-11-28T16%3A56%3A44&q.value=2014-11-28T16%3A56%3A45&limit=2

::

   {
       "event_log": [
       {
           "severity": "critical",
           "timestamp": "2014-11-28T16:56:44.814747",
           "uuid": "67f88971-d769-450e-9e8a-126dd8585187",
           "event_log_id": "400.005",
           "entity_instance_id": "host=controller-0.network=mgmt",
           "reason_text": "Communication failure detected with peer over port eth1 on host controller-0",
           "state": "log"
       },
       {
           "severity": "major",
           "timestamp": "2014-11-28T16:56:44.808965",
           "uuid": "1a259ab9-8ea2-4177-8053-ad7596509c66",
           "log_id": "400.002",
           "entity_instance_id": "service_domain=controller.service_group=cloud-services",
           "reason_text": "Service group cloud-services loss of redundancy; expected 1 standby member but no standby members available",
           "state": "set"
       }],
           "next": "http://192.168.204.2:18002/v1/event_log?sort_key=timestamp&sort_dir=desc&limit=2&marker=1a259ab9-8ea2-4177-8053-ad7596509c66"
   }

**********************************************
Shows information about a specific event log
**********************************************

.. rest_method:: GET /v1/event_log/​{log_uuid}​

**Normal response codes**

200

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "log_uuid", "URI", "csapi:UUID", "The unique identifier of an event log."

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "event_log_id (Optional)", "plain", "xsd:string", "The event log ID; each type of event log has a unique ID. Note the event_log_id and the entity_instance_id uniquely identify an event log instance."
   "state (Optional)", "plain", "xsd:string", "The state of the event; ``set``, ``clear`` or ``log``"
   "entity_instance_id (Optional)", "plain", "xsd:string", "The instance of the object generating the event log. A . separated list of sub-entity-type=instance-value pairs, representing the containment structure of the overall entity instance. Note the event_log_id and the entity_instance_id uniquely identify an event log instance."
   "reason_text (Optional)", "plain", "xsd:string", "The text description of the event log."
   "severity (Optional)", "plain", "xsd:string", "The severity of the event log; ``critical``, ``major``, ``minor`` or ``warning``."
   "timestamp (Optional)", "plain", "xsd:dateTime", "The time in UTC at which the event log has last been updated."
   "uuid (Optional)", "plain", "csapi:UUID", "The unique identifier of the event log."
   "next (Optional)", "plain", "xsd:string", "The next attribute is the request to use to get the next n items. It is used to paginate the event log list."
   "uuid (Optional)", "plain", "csapi:UUID", "The universally unique identifier for this object."
   "links (Optional)", "plain", "xsd:list", "For convenience, resources contain links to themselves. This allows a client to easily obtain rather than construct resource URIs. The following types of link relations are associated with resources: a self link containing a versioned link to the resource, and a bookmark link containing a permanent link to a resource that is appropriate for long term storage."
   "created_at (Optional)", "plain", "xsd:dateTime", "The time when the object was created."
   "updated_at (Optional)", "plain", "xsd:dateTime", "The time when the object was last updated."

::

   {
       "state": "set",
       "service_affecting": "True",
       "proposed_repair_action":
       "Contact next level of support.",
       "event_log_type": "processing-error",
       "severity": "critical",
       "created_at": "2014-11-28T20:00:56.116251+00:00",
       "entity_type_id": "service_domain.service_group.host",
       "probable_cause": "underlying-resource-unavailable",
       "updated_at": null,
       "event_log_id": "400.001",
       "entity_instance_id":"system=1bcd6f11-8152-45f7-9d93-0960e1887afe.service_domain=controller.service_group=patching-services.host=controller-0",
       "suppression": "True",
       "timestamp": "2014-11-28T20:00:56.114060+00:00",
       "uuid": "8701d806-cae7-4f34-be8d-17fd11a0d25d",
       "reason_text": "Service group failure; patch-alarm-manager(disabled, failed)."
   }

This operation does not accept a request body.

------------------
Event Suppression
------------------

These APIs allow the display of the Event Suppression state in the
system.

*****************************
Lists suppressed event id's
*****************************

.. rest_method:: GET /v1/event_suppression

**Normal response codes**

200

**Error response codes**

computeFault (400, 500, ...), serviceUnavailable (503), badRequest (400),
unauthorized (401), forbidden (403), badMethod (405), overLimit (413),
itemNotFound (404)

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "event_suppression (Optional)", "plain", "xsd:list", "The list of suppressed event types."
   "alarm_id (Optional)", "plain", "xsd:string", "The alarm ID type (event ID type) that can be suppressed or unsuppressed."
   "description (Optional)", "plain", "xsd:string", "The text description of the event type."
   "suppression_status (Optional)", "plain", "xsd:string", "The suppression status for the event ID type; ``suppressed`` or ``unsuppressed``"
   "uuid (Optional)", "plain", "csapi:UUID", "The universally unique identifier for this object."
   "links (Optional)", "plain", "xsd:list", "For convenience, resources contain links to themselves. This allows a client to easily obtain rather than construct resource URIs. The following types of link relations are associated with resources: a self link containing a versioned link to the resource, and a bookmark link containing a permanent link to a resource that is appropriate for long term storage."
   "created_at (Optional)", "plain", "xsd:dateTime", "The time when the object was created."
   "updated_at (Optional)", "plain", "xsd:dateTime", "The time when the object was last updated."

::

   {
      "event_suppression":[
         {
            "alarm_id":"100.101",
            "suppression_status":"unsuppressed",
            "description":"Platform CPU threshold exceeded; threshold x%, actual y% .",
            "links":[
               {
                  "href":"http://192.168.204.2:6385/v1/event_suppression/494af09d-6810-4cf2-a57b-528f570f0511",
                  "rel":"self"
               },
               {
                  "href":"http://192.168.204.2:6385/event_suppression/494af09d-6810-4cf2-a57b-528f570f0511",
                  "rel":"bookmark"
               }
            ],
            "uuid":"494af09d-6810-4cf2-a57b-528f570f0511"
         },
         {
            "alarm_id":"100.102",
            "suppression_status":"unsuppressed",
            "description":"VSwitch CPU threshold exceeded; threshold x%, actual y% .",
            "links":[
               {
                  "href":"http://192.168.204.2:6385/v1/event_suppression/b140b479-10a3-430f-9aba-a9a46b3fc03f",
                  "rel":"self"
               },
               {
                  "href":"http://192.168.204.2:6385/event_suppression/b140b479-10a3-430f-9aba-a9a46b3fc03f",
                  "rel":"bookmark"
               }
            ],
            "uuid":"b140b479-10a3-430f-9aba-a9a46b3fc03f"
         },

         ...

         {
            "alarm_id":"900.003",
            "suppression_status":"suppressed",
            "description":"Patch host install failure.",
            "links":[
               {
                  "href":"http://192.168.204.2:6385/v1/event_suppression/72285cc6-07ad-47ef-8b39-2e4482895533",
                  "rel":"self"
               },
               {
                  "href":"http://192.168.204.2:6385/event_suppression/72285cc6-07ad-47ef-8b39-2e4482895533",
                  "rel":"bookmark"
               }
            ],
            "uuid":"72285cc6-07ad-47ef-8b39-2e4482895533"
         }
      ]
   }

This operation does not accept a request body.

********************************************
Modifies the value of an event suppression
********************************************

.. rest_method:: PATCH /v1/event_suppression/​{event_suppression_uuid}​

**Normal response codes**

200

**Error response codes**

badMediaType (415)

**Request parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "event_suppression_uuid", "URI", "csapi:UUID", "The unique identifier of an event suppression."
   "suppression_status (Optional)", "plain", "xsd:string", "The suppression status of an event suppression; ``suppressed`` or ``unsuppressed``"

**Response parameters**

.. csv-table::
   :header: "Parameter", "Style", "Type", "Description"
   :widths: 20, 20, 20, 60

   "event_suppression (Optional)", "plain", "xsd:list", "URIs to the modified event suppression."
   "alarm_id (Optional)", "plain", "xsd:string", "The alarm ID type (event ID type) that can be suppressed or unsuppressed."
   "description (Optional)", "plain", "xsd:string", "The text description of the event type."
   "suppression_status (Optional)", "plain", "xsd:string", "The suppression status for the event ID type; ``suppressed`` or ``unsuppressed``"
   "uuid (Optional)", "plain", "csapi:UUID", "The universally unique identifier for this object."
   "links (Optional)", "plain", "xsd:list", "For convenience, resources contain links to themselves. This allows a client to easily obtain rather than construct resource URIs. The following types of link relations are associated with resources: a self link containing a versioned link to the resource, and a bookmark link containing a permanent link to a resource that is appropriate for long term storage."
   "created_at (Optional)", "plain", "xsd:dateTime", "The time when the object was created."
   "updated_at (Optional)", "plain", "xsd:dateTime", "The time when the object was last updated."

::

   [
      {
         "path":"/suppression_status",
         "value":"unsuppressed",
         "op":"replace"
      }
   ]

::

   {
      "description":"Service group loss of redundancy; expected <num> standby member<s> but only <num> standby member<s> available.ORService group loss of redundancy; expected <num> standby member<s> but only <num> standby member<s> available.ORService group loss of redu ...",
      "links":[
         {
            "href":"http://192.168.204.2:6385/v1/event_suppression/eb54eb1a-6314-4818-8fe7-83bdb6fe5b80",
            "rel":"self"
         },
         {
            "href":"http://192.168.204.2:6385/event_suppression/eb54eb1a-6314-4818-8fe7-83bdb6fe5b80",
            "rel":"bookmark"
         }
      ],
      "created_at":"2016-05-09T11:25:22.436412+00:00",
      "updated_at":"2016-05-12T12:38:51.524402+00:00",
      "alarm_id":"400.002",
      "suppression_status":"unsuppressed",
      "id":37,
      "uuid":"eb54eb1a-6314-4818-8fe7-83bdb6fe5b80"
   }







