//
// Copyright (c) 2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//


#include <python2.7/Python.h>
#include <stdio.h>
#include "fmAPI.h"
#include "fmAlarmUtils.h"


static const size_t DEF_MAX_ALARMS (1000);
static const size_t MAXSTRINGSIZE (500);
static PyObject *logging = NULL;

enum { error, warning, info, debug, max_level };


#define LOG_MSG(level,data,...) \
        log_msg(level, "fm_python_extension: "\
        data, ## __VA_ARGS__ )

#define ERROR_LOG(data,...) \
		LOG_MSG(error, data, ## __VA_ARGS__)

#define WARNING_LOG(data,...) \
		LOG_MSG(warning, data, ## __VA_ARGS__)

#define INFO_LOG(data,...) \
		LOG_MSG(info, data, ## __VA_ARGS__)

#define DEBUG_LOG(data,...) \
		LOG_MSG(debug, data, ## __VA_ARGS__)


static void log_msg(int type, const char *data,...)
{
    static PyObject *str = NULL;
    const char* methods[] = {"error", "warning", "info", "debug"};

    if (logging == NULL) {
        logging = PyImport_ImportModuleNoBlock("logging");
        if (logging == NULL) {
            PyErr_SetString(PyExc_ImportError,
                "Could not import python module 'logging'");
        }
    }

    va_list ap;
    char buff[MAXSTRINGSIZE];
    va_start(ap, data );
    vsnprintf(buff, sizeof(buff), data, ap);
    va_end(ap);

    str = Py_BuildValue((char *)"s", buff);

    if (type < max_level) {
        PyObject_CallMethod(logging, (char *)methods[type], (char *)"O", str);
    }

    Py_DECREF(str);
}

static PyObject * _fm_set(PyObject * self, PyObject *args) {

	SFmAlarmDataT alm_data;
	std::string alarm;
	fm_uuid_t tmp_uuid;
	const char *alm_str;
	EFmErrorT rc;

	if (!PyArg_ParseTuple(args, "s", &alm_str)) {
		ERROR_LOG("Failed to parse args.");
		Py_RETURN_NONE;
	}

    alarm.assign(alm_str);
	if (!fm_alarm_from_string(alarm, &alm_data)) {
		ERROR_LOG("Failed to convert string to alarm.");
		Py_RETURN_NONE;
	}

	rc = fm_set_fault(&alm_data, &tmp_uuid);
	if (rc == FM_ERR_OK) {
		return PyString_FromString(&(tmp_uuid[0]));
	}

	if (rc == FM_ERR_NOCONNECT){
		// when the fm-manager process has not been started by SM
		WARNING_LOG("Failed to connect to FM manager");
	} else {
		ERROR_LOG("Failed to generate an alarm: (%s) (%s)",
				alm_data.alarm_id, alm_data.entity_instance_id);
	}

	Py_RETURN_NONE;
}

static PyObject * _fm_get(PyObject * self, PyObject *args) {

	const char *filter;
	std::string alm_str, filter_str;
	AlarmFilter af;
	SFmAlarmDataT ad;
	EFmErrorT rc;

	if (!PyArg_ParseTuple(args, "s", &filter)) {
		ERROR_LOG("Failed to parse args");
		Py_RETURN_FALSE;
	}

	filter_str.assign(filter);
	if (!fm_alarm_filter_from_string(filter_str, &af)) {
		ERROR_LOG("Invalid alarm filter: (%s)", filter_str.c_str());
		Py_RETURN_FALSE;
	}

	rc = fm_get_fault(&af,&ad);
	if (rc == FM_ERR_OK) {
		fm_alarm_to_string(&ad,alm_str);
		return PyString_FromString(alm_str.c_str());
	}

	if (rc == FM_ERR_ENTITY_NOT_FOUND) {
		DEBUG_LOG("Alarm id (%s), Entity id:(%s) not found",
				af.alarm_id, af.entity_instance_id);
		Py_RETURN_NONE;
	} else if (rc == FM_ERR_NOCONNECT) {
		WARNING_LOG("Failed to connect to FM manager");
	} else {
		ERROR_LOG("Failed to get alarm by filter: (%s) (%s), error code: (%d)",
				af.alarm_id, af.entity_instance_id, rc);
	}
	Py_RETURN_FALSE;
}


static PyObject * _fm_get_by_aid(PyObject * self, PyObject *args, PyObject* kwargs) {
	const char *aid;
	fm_alarm_id alm_id;
	unsigned int max = DEF_MAX_ALARMS;
	char* keywords[] = {"alarm_id", "max", (char*)NULL};

	memset(alm_id, 0 , sizeof(alm_id));
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i", keywords, &aid, &max)) {
		ERROR_LOG("Failed to parse args");
		Py_RETURN_FALSE;
	}
	strncpy(alm_id, aid, sizeof(alm_id)-1);

	std::vector< SFmAlarmDataT > lst;
	try {
		lst.resize(max);
	} catch(...) {
		ERROR_LOG("Failed to allocate memory");
		Py_RETURN_FALSE;
	}
	unsigned int max_alarms_to_get = max;
	EFmErrorT rc = fm_get_faults_by_id(&alm_id, &(lst[0]), &max_alarms_to_get);
	if (rc == FM_ERR_OK) {
		PyObject *__lst = PyList_New(0);
		for ( size_t ix = 0 ; ix < max_alarms_to_get ; ++ix ) {
			std::string s;
			fm_alarm_to_string(&lst[ix],s);
			if (s.size() > 0) {
				if (PyList_Append(__lst, PyString_FromString(s.c_str())) != 0) {
					ERROR_LOG("Failed to append alarm to the list");
				}
			}
		}
		/* python will garbage collect if the reference count is correct
		   (it should be 1 at this point) */
		return __lst;
	}

	if (rc == FM_ERR_ENTITY_NOT_FOUND) {
		DEBUG_LOG("No alarm found for alarm id (%s)", alm_id);
		Py_RETURN_NONE;
	} else if (rc == FM_ERR_NOCONNECT) {
		WARNING_LOG("Failed to connect to FM manager");
	} else {
		ERROR_LOG("Failed to get alarm list for alarm id (%s), error code: (%d)", alm_id, rc);
	}
	Py_RETURN_FALSE;
}

static PyObject * _fm_get_by_eid(PyObject * self, PyObject *args, PyObject* kwargs) {
	const char *eid;
	fm_ent_inst_t inst_id;
	std::vector< SFmAlarmDataT > lst;
	unsigned int max= DEF_MAX_ALARMS;
	char* keywords[] = {"entity_instance_id", "max", (char*)NULL};

	memset(inst_id, 0 , sizeof(inst_id));
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|i", keywords, &eid, &max)) {
		ERROR_LOG("Failed to parse args");
		Py_RETURN_FALSE;
	}
	strncpy(inst_id, eid ,sizeof(inst_id)-1);

	try {
		lst.resize(max);
	} catch(...) {
		ERROR_LOG("Failed to allocate memory");
		Py_RETURN_FALSE;
	}
	unsigned int max_alarms_to_get = max;
	EFmErrorT rc = fm_get_faults(&inst_id, &(lst[0]), &max_alarms_to_get);
	if (rc == FM_ERR_OK) {
		PyObject *__lst = PyList_New(0);
		for ( size_t ix = 0; ix < max_alarms_to_get; ++ix ) {
			std::string s;
			fm_alarm_to_string(&lst[ix], s);
			if (s.size() > 0) {
				if (PyList_Append(__lst,PyString_FromString(s.c_str())) != 0) {
					ERROR_LOG("Failed to append alarm to the list");
				}
			}
		}
		/* python will garbage collect if the reference count is correct
		   (it should be 1 at this point) */
		return __lst;
	}

	if (rc == FM_ERR_ENTITY_NOT_FOUND) {
		DEBUG_LOG("No alarm found for entity id (%s)", inst_id);
		Py_RETURN_NONE;
	} else if (rc == FM_ERR_NOCONNECT) {
		WARNING_LOG("Failed to connect to FM manager");
	} else {
		ERROR_LOG("Failed to get alarm list for entity id (%s), error code: (%d)", inst_id, rc);
	}
	Py_RETURN_FALSE;
}

static PyObject * _fm_clear(PyObject * self, PyObject *args) {

	const char *filter;
	std::string alm_str, filter_str;
	AlarmFilter af;
	EFmErrorT rc;

	if (!PyArg_ParseTuple(args, "s", &filter)) {
		ERROR_LOG("Failed to parse args");
		Py_RETURN_FALSE;
	}

	filter_str.assign(filter);
	if (!fm_alarm_filter_from_string(filter_str, &af)) {
		ERROR_LOG("Invalid alarm filter: (%s)", filter_str.c_str());
		Py_RETURN_FALSE;
	}

	rc = fm_clear_fault(&af);
	if (rc == FM_ERR_OK) {
		Py_RETURN_TRUE;
	}

	if (rc == FM_ERR_ENTITY_NOT_FOUND) {
		DEBUG_LOG("No alarm found to clear: (%s) (%s)", af.alarm_id, af.entity_instance_id);
        Py_RETURN_NONE;
	} else if (rc == FM_ERR_NOCONNECT) {
		WARNING_LOG("Failed to connect to FM manager");
	} else {
		ERROR_LOG("Failed to clear alarm by filter: (%s) (%s), error code: (%d)",
				af.alarm_id, af.entity_instance_id, rc);
	}
	Py_RETURN_FALSE;
}

static PyObject * _fm_clear_all(PyObject * self, PyObject *args) {

	fm_ent_inst_t inst_id;
	const char *eid;
	EFmErrorT rc;

	memset(inst_id, 0 , sizeof(inst_id));
	if (!PyArg_ParseTuple(args,"s", &eid)) {
		ERROR_LOG("Failed to parse args");
		Py_RETURN_FALSE;
	}

	strncpy(inst_id, eid ,sizeof(inst_id)-1);
	rc = fm_clear_all(&inst_id);
	if (rc == FM_ERR_OK) {
		Py_RETURN_TRUE;
	}

	if (rc == FM_ERR_ENTITY_NOT_FOUND) {
		DEBUG_LOG("No alarm found to clear with entity id (%s)", inst_id);
        Py_RETURN_NONE;
	} else if (rc == FM_ERR_NOCONNECT) {
		WARNING_LOG("Failed to connect to FM manager");
    } else {
		ERROR_LOG("Failed to clear alarms with entity id (%s), error code: (%d)",
				inst_id, rc);
	}
	Py_RETURN_FALSE;
}

static PyMethodDef _methods [] = {
		{ "set", _fm_set, METH_VARARGS, "Set or update an alarm" },
		{ "get", _fm_get, METH_VARARGS, "Get alarms by filter" },
		{ "clear", _fm_clear, METH_VARARGS, "Clear an alarm by filter" },
		{ "clear_all", _fm_clear_all, METH_VARARGS,
				"Clear alarms that match the entity instance id"},
		{ "get_by_aid", (PyCFunction)_fm_get_by_aid, METH_VARARGS | METH_KEYWORDS,
				"Get alarms by alarm id" },
		{ "get_by_eid", (PyCFunction)_fm_get_by_eid, METH_VARARGS | METH_KEYWORDS,
				"Get alarms by entity instance id" },
	    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initfm_core() {
	PyObject *m = Py_InitModule("fm_core", _methods);
	if (m == NULL){
		PySys_WriteStderr("Failed to initialize fm_core");
		return;
	}
}
