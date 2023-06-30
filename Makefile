BASE=/usr/local/src/freeswitch
include $(BASE)/build/modmake.rules
MODNAME=mod_healthcheck

mod_LTLIBRARIES = mod_healthcheck.la
mod_healthcheck_la_SOURCES  = mod_healthcheck.c
mod_healthcheck_la_LIBADD   = $(switch_builddir)/libfreeswitch.la
mod_healthcheck_la_LDFLAGS  = -avoid-version -module -no-undefined -shared

MAKEFLAGS=-j1
XMLRPC_DIR=$(switch_srcdir)/libs/xmlrpc-c
XMLRPC_BUILDDIR=$(switch_builddir)/libs/xmlrpc-c
mod_healthcheck_la_CFLAGS  = -I$(XMLRPC_DIR)/lib/expat/xmlparse -I$(XMLRPC_DIR)/lib/expat/xmltok -I$(XMLRPC_DIR) -I$(XMLRPC_DIR)/include
mod_healthcheck_la_CFLAGS  += -I$(XMLRPC_DIR)/lib/abyss/src -I$(XMLRPC_DIR)/lib/util/include -D_THREAD -D__EXTENSIONS__ -D_GNU_SOURCE
mod_healthcheck_la_CFLAGS  += -I$(XMLRPC_BUILDDIR)/include 
mod_healthcheck_la_SOURCES += $(switch_srcdir)/libs/xmlrpc-c/src/version.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/asprintf.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/util/casprintf.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/channel.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/chanswitch.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/util/cmdline_parser.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/conf.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/conn.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/data.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/date.c\
$(switch_srcdir)/libs/xmlrpc-c/src/double.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/error.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/file.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/expat/gennmtab/gennmtab.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/util/getoptx.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/handler.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/http.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/init.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/make_printable.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/memblock.c\
$(switch_srcdir)/libs/xmlrpc-c/src/method.c\
$(switch_srcdir)/libs/xmlrpc-c/src/parse_value.c\
$(switch_srcdir)/libs/xmlrpc-c/src/parse_datetime.c\
$(switch_srcdir)/libs/xmlrpc-c/src/registry.c\
$(switch_srcdir)/libs/xmlrpc-c/src/resource.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/response.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/select.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/server.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/session.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/sleep.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/socket.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/socket_unix.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/util/string_parser.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/util/stripcaseeq.c\
$(switch_srcdir)/libs/xmlrpc-c/src/system_method.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/thread_pthread.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/time.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/token.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xml_trace.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/abyss/src/trace.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/utf8.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/string_number.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/libutil/base64.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/expat/xmlparse/xmlparse.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/expat/xmltok/xmlrole.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_array.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_authcookie.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_base64.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_build.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_data.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_datetime.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_decompose.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_expat.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_parse.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_serialize.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_server_abyss.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_server_cgi.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_string.c\
$(switch_srcdir)/libs/xmlrpc-c/src/xmlrpc_struct.c\
$(switch_srcdir)/libs/xmlrpc-c/lib/expat/xmltok/xmltok.c

mod_healthcheck_la_CFLAGS  += $(AM_CFLAGS) -I. -w

MOD_CFLAGS = $(mod_healthcheck_la_CFLAGS)
