#include <malamute.h>
    
static char *endpoint = "inproc://bob";

void
publisher_server (zsock_t *pipe, void* args) {

    char *name = strdup (args);

    mlm_client_t *client = mlm_client_new ();
    mlm_client_connect (client, endpoint, 1000, name);
    mlm_client_set_producer (client, "STREAM");

    zpoller_t *poller = zpoller_new (pipe, mlm_client_msgpipe (client), NULL);

    zsock_signal (pipe, 0);
    while (!zsys_interrupted) {

        void *which = zpoller_wait (poller, 500);

        if (zpoller_expired (poller))
            mlm_client_sendx (client, "SUBJECT", "Hello", "world", NULL);

        if (which == pipe) {
            char *command = zstr_recv (pipe);
            zsys_debug ("%s:\tpipe, command=%s", name, command);
            if (streq (command, "$TERM"))
                break;
            else
                zsys_warning ("unknown command");
            zstr_free (&command);
            continue;
        }
        else
        if (which == mlm_client_msgpipe (client)) {
            zmsg_t *msg = mlm_client_recv (client);
            zmsg_print (msg);
            zmsg_destroy (&msg);
        }

    }

    zpoller_destroy (&poller);
    mlm_client_destroy (&client);
    zstr_free (&name);

}

int main () {

    int r;

    zactor_t *server = zactor_new (mlm_server, "Malamute");
    zstr_sendx (server, "BIND", endpoint, NULL);

    zactor_t *publisher = zactor_new (publisher_server, (void*)"publisher-1");
    
    mlm_client_t *client = mlm_client_new ();
    mlm_client_connect (client, endpoint, 1000, "reader");
    mlm_client_set_consumer (client, "STREAM", ".*");

    while (!zsys_interrupted) {
        zmsg_t *msg = mlm_client_recv (client);
        zmsg_print (msg);
        zmsg_destroy (&msg);
    }

    mlm_client_destroy (&client);
    zactor_destroy (&publisher);
    zactor_destroy (&server);

}
