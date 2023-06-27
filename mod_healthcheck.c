/*
* FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
* Copyright (C) 2005-2014, Anthony Minessale II <anthm@freeswitch.org>
*
* Version: MPL 1.1
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
*
* The Initial Developer of the Original Code is
* Anthony Minessale II <anthm@freeswitch.org>
* Portions created by the Initial Developer are Copyright (C)
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Anthony Minessale II <anthm@freeswitch.org>
*
* mod_healthcheck.c -- Health Checker
*
*/
#include <switch.h>
#ifdef _MSC_VER
#pragma warning(disable:4142)
#endif

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>
#include <xmlrpc-c/base64_int.h>
#include <../lib/abyss/src/token.h>
#include <../lib/abyss/src/http.h>
#include <../lib/abyss/src/session.h>

SWITCH_MODULE_LOAD_FUNCTION(mod_healthcheck_load);
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_healthcheck_shutdown);
SWITCH_MODULE_RUNTIME_FUNCTION(mod_healthcheck_runtime);
SWITCH_MODULE_DEFINITION(mod_healthcheck, mod_healthcheck_load, mod_healthcheck_shutdown, mod_healthcheck_runtime);

static abyss_bool HTTPWrite(TSession * s, const char *buffer, const uint32_t len);

static struct {
   uint16_t port;
   uint8_t running;
   TServer abyssServer;
   xmlrpc_registry *registryP;
} globals;

static switch_status_t do_config(void)
{
   char *cf = "healthcheck.conf";
   switch_xml_t cfg, xml, settings, param;

   if (!(xml = switch_xml_open_cfg(cf, &cfg, NULL))) {
	   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Open of %s failed\n", cf);
	   return SWITCH_STATUS_TERM;
   }

   if ((settings = switch_xml_child(cfg, "settings"))) {
	   for (param = switch_xml_child(settings, "param"); param; param = param->next) {
		   char *var = (char *) switch_xml_attr_soft(param, "name");
		   char *val = (char *) switch_xml_attr_soft(param, "value");

		   if (!zstr(var) && !zstr(val)) {
			   if (!strcasecmp(var, "http-port")) {
				   globals.port = (uint16_t) atoi(val);
			   }
		   }
	   }
   }

   if (!globals.port) {
	   globals.port = 62589;
   }
   switch_xml_free(xml);

   return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_LOAD_FUNCTION(mod_healthcheck_load)
{

   /* connect my internal structure to the blank pointer passed to me */
   *module_interface = switch_loadable_module_create_module_interface(pool, modname);

   memset(&globals, 0, sizeof(globals));

   do_config();

   /* indicate that the module should continue to be loaded */
   return SWITCH_STATUS_SUCCESS;
}

static abyss_bool HTTPWrite(TSession * s, const char *buffer, const uint32_t len)
{
   if (s->chunkedwrite && s->chunkedwritemode) {
	   char t[16];

	   if (ConnWrite(s->connP, t, sprintf(t, "%x" CRLF, len)))
		   if (ConnWrite(s->connP, buffer, len))
			   return ConnWrite(s->connP, CRLF, 2);

	   return FALSE;
   }

   return ConnWrite(s->connP, buffer, len);
}

abyss_bool handler_hook(TSession * r)
{
   int i;
   abyss_bool ret = TRUE;
   const char *uri = 0;
   TRequestInfo *info = 0;
   char v[256] = "";

   if (!r || !(info = &r->requestInfo) || !(uri = info->uri)) {
	   return FALSE;
   }

   /* Default to 500 status to avoid assert. It should be
	  overridden later if we actually handle it or if the
	  default handler in abyss handles it. */
   ResponseStatus(r, 500);

   if (!strcmp(uri, "/health/check")) {
		// {"status":"UP"}
	   /* We made it this far, always OK */
	   if (!HTTPWrite(r, "HTTP/1.1 200 OK\r\n", (uint32_t) strlen("HTTP/1.1 200 OK\r\n"))) {
		   return TRUE;
	   }
	   ResponseAddField(r, "Connection", "close");

	   const char *dateValue;
	   DateToString(r->date, &dateValue);
	   if (dateValue) {
		   ResponseAddField(r, "Date", dateValue);
		   free((void *)dateValue);
	   }

	   /* Generation of the server field */
	   switch_snprintf(v, sizeof(v), "healthcheck", switch_version_full());
	   ResponseAddField(r, "Server", v);

	   ResponseAddField(r, "Content-Type", "application/json");

	   for (i = 0; i < r->responseHeaderFields.size; i++) {
		   TTableItem *ti = &r->responseHeaderFields.item[i];
		   char *header = switch_mprintf("%s: %s\r\n", ti->name, ti->value);
		   if (!ConnWrite(r->connP, header, (uint32_t) strlen(header))) {
			   switch_safe_free(header);
			   return TRUE;
		   }
		   switch_safe_free(header);
	   }

	   if (!ConnWrite(r->connP, CRLF, 2)) {
		   return TRUE;
	   }

	   /* We made it this far, always OK */
	   if (!HTTPWrite(r, "{\"status\":\"UP\"}\r\n", (uint32_t) strlen("{\"status\":\"UP\"}\r\n"))) {
		   return TRUE;
	   }
   } else {
	   return FALSE; /* 404 */
   }

   r->responseStarted = TRUE;
   ResponseStatus(r, 200);     /* we don't want an assertion failure */
   r->requestInfo.keepalive = 0;

end:

   return ret;
}

SWITCH_MODULE_RUNTIME_FUNCTION(mod_healthcheck_runtime)
{
   xmlrpc_env env;
   char logfile[512];
   switch_hash_index_t *hi;
   const void *var;
   void *val;

   globals.running = 1;

   xmlrpc_env_init(&env);

   globals.registryP = xmlrpc_registry_new(&env);

   MIMETypeInit();

   for (hi = switch_core_mime_index(); hi; hi = switch_core_hash_next(&hi)) {
	   switch_core_hash_this(hi, &var, NULL, &val);
	   if (var && val) {
		   MIMETypeAdd((char *) val, (char *) var);
	   }
   }

   switch_snprintf(logfile, sizeof(logfile), "%s%s%s", SWITCH_GLOBAL_dirs.log_dir, SWITCH_PATH_SEPARATOR, "freeswitch_healthcheck.log");
   ServerCreate(&globals.abyssServer, "XmlRpcServer", globals.port, SWITCH_GLOBAL_dirs.htdocs_dir, logfile);

   xmlrpc_env_clean(&env);

   if (ServerInit(&globals.abyssServer) != TRUE) {
	   globals.running = 0;
	   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "Failed to start HTTP Port %d\n", globals.port);
	   xmlrpc_registry_free(globals.registryP);
	   MIMETypeTerm();

	   return SWITCH_STATUS_TERM;
   }

   ServerAddHandler(&globals.abyssServer, handler_hook);
   ServerSetKeepaliveTimeout(&globals.abyssServer, 5);

   switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_NOTICE, "Starting HealthCheck HTTP Port %d\n", globals.port);
   ServerRun(&globals.abyssServer);

   switch_yield(1000000);

   globals.running = 0;

   return SWITCH_STATUS_TERM;
}

/* upon module unload */
SWITCH_MODULE_SHUTDOWN_FUNCTION(mod_healthcheck_shutdown)
{

   /* this makes the worker thread (ServerRun) stop */
   ServerTerminate(&globals.abyssServer);

   do {
	   switch_yield(100000);
   } while (globals.running);

   ServerFree(&globals.abyssServer);
   xmlrpc_registry_free(globals.registryP);
   MIMETypeTerm();

   return SWITCH_STATUS_SUCCESS;
}

/* For Emacs:
* Local Variables:
* mode:c
* indent-tabs-mode:t
* tab-width:4
* c-basic-offset:4
* End:
* For VIM:
* vim:set softtabstop=4 shiftwidth=4 tabstop=4 noet:
*/

