#include <fastcgi2/component.h>
#include <fastcgi2/component_factory.h>
#include <fastcgi2/handler.h>
#include <fastcgi2/request.h>
#include <cassandra.h>
#include <stdio.h>
#include <exception>

#include <iostream>
#include <sstream>

//#include <QDebug>

class HelloFastCGI : virtual public fastcgi::Component, virtual public fastcgi::Handler {
    CassFuture* connect_future;
    CassCluster* cluster;
    CassSession* session;

    void cassandra_init() {
        connect_future = NULL;
        cluster = cass_cluster_new();
        session = cass_session_new();

        char* hosts = "127.0.0.1";

        cass_cluster_set_contact_points(cluster, hosts);
        connect_future = cass_session_connect(session, cluster);

        if (cass_future_error_code(connect_future) != CASS_OK) {
            const char* message;
            size_t message_length;
            cass_future_error_message(connect_future, &message, &message_length);
            fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length, message);
            throw std::exception();
        }
        const char* query = "USE highload;";
        CassStatement* statement = cass_statement_new(query, 0);

        CassFuture* result_future = cass_session_execute(session, statement);

        cass_statement_free(statement);

        if (cass_future_error_code(result_future) == CASS_OK) {
            const CassResult* result = cass_future_get_result(result_future);
            cass_result_free(result);
        }
    }

    void cassandra_destroy() {
        CassFuture* close_future = cass_session_close(session);
        cass_future_wait(close_future);
        cass_future_free(close_future);
        cass_future_free(connect_future);
        cass_cluster_free(cluster);
        cass_session_free(session);
    }

    public:
        HelloFastCGI(fastcgi::ComponentContext *context) :
                fastcgi::Component(context) {
        }

        virtual void onLoad() {
            cassandra_init();
        }

        virtual void onUnload() {
            cassandra_destroy();
        }

        virtual void handleRequest(fastcgi::Request *request, fastcgi::HandlerContext *context) {
                std::cerr << "here" << std::endl;
                request->setContentType("text/plain");
                std::string method = request->getRequestMethod();
                std::stringbuf to_add;

                if (method == "GET") {
                    if (request->hasArg("id")) {
                        std::string id = request->getArg("id");
                        char query[100];
                        snprintf(query, sizeof(query), "SELECT text FROM jokes WHERE id = %s", id.c_str());

                        CassStatement* statement = cass_statement_new(query, 0);
                        CassFuture* result_future = cass_session_execute(session, statement);

                        if (cass_future_error_code(result_future) == CASS_OK) {
                            const CassResult* result = cass_future_get_result(result_future);

                            const CassRow* row = cass_result_first_row(result);

                            if (row != NULL) {
                                const CassValue* value = cass_row_get_column_by_name(row, "text");

                                const char* text;
                                size_t text_length;
                                cass_value_get_string(value, &text, &text_length);
                                to_add.str(to_add.str() + text);
                            }
                            cass_result_free(result);
                        }

                        cass_future_free(result_future);
                        cass_statement_free(statement);
                    }
                }
                request->write(&to_add);
        }
};

FCGIDAEMON_REGISTER_FACTORIES_BEGIN()
FCGIDAEMON_ADD_DEFAULT_FACTORY("HelloFastCGIFactory", HelloFastCGI)
FCGIDAEMON_REGISTER_FACTORIES_END()
