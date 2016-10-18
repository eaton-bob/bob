#include <malamute.h>

int
main (int argc, char **argv) {

    if (argc < 2) {
        zsys_warning ("USAGE: %s <client_name>", argv [0]);
    }


    static char *endpoint = "ipc://helloworld";
    int r;

    mlm_client_t *user = mlm_client_new ();
    r = mlm_client_connect (user, endpoint, 1000, argv[1]);
    assert (r != -1);

    r = mlm_client_sendtox (user, "provider", "SUBJECT", "HELLO", NULL);
    assert (r != -1);

    zmsg_t *msg = mlm_client_recv (user);
    assert (msg);
    zmsg_print (msg);

    zmsg_destroy (&msg);
    mlm_client_destroy (&user);

}
