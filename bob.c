#include <malamute.h>

int main () {

    static char *endpoint = "inproc://bob";
    int r;

    zactor_t *server = zactor_new (mlm_server, "Malamute");
    zstr_sendx (server, "BIND", endpoint, NULL);

    mlm_client_t *producer = mlm_client_new ();
    r = mlm_client_connect (producer, endpoint, 1000, "producer");
    assert (r != -1);

    mlm_client_t *consumer = mlm_client_new ();
    r = mlm_client_connect (consumer, endpoint, 1000, "consumer");
    assert (r != -1);

    mlm_client_sendtox (producer, "consumer", "SUBJECT", "Hello", "Bob", NULL);

    zmsg_t *msg = mlm_client_recv (consumer);
    zsys_info ("subject=%s", mlm_client_subject (consumer));
    zsys_info ("command=%s", mlm_client_command (consumer));
    zmsg_print (msg);
    zmsg_destroy (&msg);


    mlm_client_destroy (&consumer);
    mlm_client_destroy (&producer);
    zactor_destroy (&server);

}
