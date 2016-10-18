#include <malamute.h>

int main () {

    static char *endpoint = "ipc://helloworld";
    int r;

    mlm_client_t *provider = mlm_client_new ();
    r = mlm_client_connect (provider, endpoint, 1000, "provider");
    assert (r != -1);

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

    mlm_client_destroy (&provider);
    return EXIT_SUCCESS;
}
