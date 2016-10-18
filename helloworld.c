#include <malamute.h>

static char *endpoint = "inproc://bob";

void
provider_server (zsock_t *pipe, void* args) {

    char *name = strdup (args);

    int r;
    mlm_client_t *provider = mlm_client_new ();
    r = mlm_client_connect (provider, endpoint, 1000, name);
    assert (r != -1);

    zsock_signal (pipe, 0);
    while (!zsys_interrupted) {
        zmsg_t *msg = mlm_client_recv (provider);
        zsys_info ("Message received:");
        zsys_info ("subject=%s", mlm_client_subject (provider));
        zsys_info ("command=%s", mlm_client_command (provider));
        zmsg_print (msg);
        char *frame = zmsg_popstr (msg);
        zmsg_t *reply = zmsg_new ();
        if (frame && streq (frame, "HELLO")) {
            zmsg_addstr (reply, "HI");
            zmsg_addstr (reply, mlm_client_sender (provider));

        }
        else {
            zmsg_addstr (reply, "ERROR");
        }
        zstr_free (&frame);
        zmsg_destroy (&msg);

        mlm_client_sendto (
                provider,
                mlm_client_sender (provider),
                mlm_client_subject (provider),
                NULL,
                1000,
                &reply);
        assert (reply == NULL);
    }

    zstr_free(&name);
    mlm_client_destroy (&provider);
}

void
user_server (zsock_t *pipe, void* args) {
    char *name = strdup (args);

    int r;
    mlm_client_t *user = mlm_client_new ();
    r = mlm_client_connect (user, endpoint, 1000, name);
    assert (r != -1);

    zsock_signal (pipe, 0);
    while (!zsys_interrupted) {
        mlm_client_sendtox (user, "provider-1", "SUBJECT", "HELLO", NULL);

        zmsg_t *msg = mlm_client_recv (user);
        assert (msg);
        zmsg_print (msg);

        zmsg_destroy (&msg);
    }
    mlm_client_destroy (&user);
}

int main () {
    zactor_t *server = zactor_new (mlm_server, "Malamute");
    zstr_sendx (server, "BIND", endpoint, NULL);

    zactor_t *user = zactor_new (user_server, (void*)"user-1");

    zactor_t *provider = zactor_new (provider_server, (void*)"provider-1");

    zactor_destroy(&provider);
    zactor_destroy (&user);
    zactor_destroy (&server);

}
