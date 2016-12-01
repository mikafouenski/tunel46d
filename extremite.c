#include "extremite.h"

int ext_out(char* port) {
    int srv_soc;
    struct addrinfo * resol; /* résolution */
    struct addrinfo indic = {AI_PASSIVE, /* Toute interface */
                             PF_INET,SOCK_STREAM,0, /* IP mode connecté */
                             0,NULL,NULL,NULL};
    struct sockaddr_in address;
    char buffer[1024];

    // récupérer les infos
    fprintf(stderr,"Ecoute sur le port %s\n", port);
    int err = getaddrinfo(NULL, port, &indic, &resol);
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

    // rendre la socket multi connexion
    // int opt = 1;
    // if (setsockopt(srv_soc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0) {
    //     perror("setsockopt");
    //     return (EXIT_FAILURE);
    // }
    // fprintf(stderr,"Option(s) OK!\n");

    // bind
    if (bind(srv_soc, resol->ai_addr, sizeof (struct sockaddr_in)) < 0) {
        perror("bind");
        return (EXIT_FAILURE);
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    fprintf(stderr,"bind!\n");

    // listen
    if (listen(srv_soc, SOMAXCONN) < 0) {
        perror("listen");
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
        do {
            nb_read = recv(new_soc, buffer, 1024, 0);
            if (nb_read <= 0) break;
            //  buffer[nb_read] = '\0';
            fprintf(stderr, "%s\n", buffer);
        } while (42);
    }
    return -1;
}

int ext_in(char* port, int tun) {
int srv_soc;
    struct addrinfo * resol; /* résolution */
    struct addrinfo indic = {AI_PASSIVE, /* Toute interface */
                             PF_INET,SOCK_STREAM,0, /* IP mode connecté */
                             0,NULL,NULL,NULL};
    struct sockaddr_in address;
    char buffer[1024];

    // récupérer les infos
    fprintf(stderr,"Ecoute sur le port %s\n", port);
    int err = getaddrinfo(NULL, port, &indic, &resol);
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
    if (connect(srv_soc, resol->ai_addr, sizeof (struct sockaddr_in)) < 0) {
        perror("connect");
        return (EXIT_FAILURE);
    }
    freeaddrinfo(resol); /* /!\ Libération mémoire */
    fprintf(stderr,"connect!\n");
    int nb_read;
    while (42) {
        nb_read = read(tun, buffer, 1024);
        write(srv_soc, buffer, nb_read);
    }
}

int main(int argc, char const *argv[]) {
    printf("COUCOU\n");
    //ext_out("1234");
    ext_in("1234", 1);
    return 0;
}
