#include "extremite.h"
#include "tunalloc.h"

int ext_out(char* port) {
    int srv_soc;
    struct addrinfo * resol; /* résolution */
    struct addrinfo indic = {AI_PASSIVE, /* Toute interface */
                             AF_INET6,SOCK_STREAM,0, /* IP mode connecté */
                             0,NULL,NULL,NULL};
    struct sockaddr_in6 address;
    char buffer[BUFFSIZE];

    // récupérer les infos
    fprintf(stderr,"Ecoute sur le port %s\n", port);
    int err = getaddrinfo(NULL, port, &indic, &resol);
    if (err < 0){
        fprintf(stderr, "Résolution: %s\n", gai_strerror(err));
        printf("1\n");
        return (EXIT_FAILURE);
    }

    // création du la socket serveur
    if ((srv_soc = socket(resol->ai_family, resol->ai_socktype, resol->ai_protocol)) < 0) {
        perror("socket");
        printf("2\n");
        return (EXIT_FAILURE);
    }
    fprintf(stderr,"le n° de la socket est : %i\n", srv_soc);

    // rendre la socket multi connexion
    // int opt = 1;
    // if (setsockopt(srv_soc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0) {
    //     perror("setsockopt");
    //     return (EXIT_FAILURE);
    // }
    // fprintf(stderr,"Option(s) OK!\n");

    // bind
    if (bind(srv_soc, resol->ai_addr, sizeof (struct sockaddr_in6)) < 0) {
        perror("bind");
        printf("3\n");
        return (EXIT_FAILURE);
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    fprintf(stderr,"bind!\n");

    // listen
    if (listen(srv_soc, SOMAXCONN) < 0) {
        perror("listen");
        printf("4\n");
        return (EXIT_FAILURE);
    }
    fprintf(stderr,"listen!\n");

    int new_soc, nb_read;
    while (42) {
        int addrlen = sizeof address;
        if ((new_soc = accept(srv_soc, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            return (EXIT_FAILURE);
        }
        while (42) {
          nb_read = read(new_soc, buffer, BUFFSIZE);
          write(1, buffer, nb_read);
        }
    }
    return -1;
}

int ext_in(char* hote, char* port, int tun) {
    int srv_soc;
    struct addrinfo * resol; /* résolution */
    struct sockaddr_in6 address;
    char buffer[BUFFSIZE];

    // récupérer les infos
    fprintf(stderr,"Ecoute sur le port %s\n", port);
    int err = getaddrinfo(hote, port, NULL, &resol);
    if (err < 0){
        fprintf(stderr, "Résolution: %s\n", gai_strerror(err));
        return (EXIT_FAILURE);
    }

    // création du la socket serveur
    if ((srv_soc = socket(resol->ai_family, resol->ai_socktype, resol->ai_protocol)) < 0) {
        perror("socket");
        return (EXIT_FAILURE);
    }
    fprintf(stderr,"le n° de la socket est : %i\n", srv_soc);

    // connect
    if (connect(srv_soc, resol->ai_addr, sizeof (struct sockaddr_in6)) < 0) {
        perror("connect");
        return (EXIT_FAILURE);
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    fprintf(stderr,"connect!\n");
    int nb_read;
    while (42) {
        nb_read = read(tun, buffer, BUFFSIZE);
        write(srv_soc, buffer, nb_read);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Tapez 1: ext_out\nTapez 2: ext_in\nSinon faites l'étoile !\n");
        return (EXIT_SUCCESS);
    }
    if (atoi(argv[1]) == 1)
        ext_out("1234");
    else if (atoi(argv[1]) == 2) {
        int tun = tun_alloc(argv[2]);
        printf("Tunnel créé.\n");
        system("./configure-tun.sh");
        printf("Tunnel configuré.\n");
        ext_in("fc00:1234:2::36", "1234", tun);
    }
    else
        printf("Vous avez fait l'étoile !\n");

    return 0;
}
