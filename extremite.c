#include "extremite.h"
#include "tunalloc.h"

void recopy(int src, int dest) {
    int nb_read;
    char buffer[1024];
    while(42) {
        nb_read = read(src, buffer, 1024);
        write(dest, buffer, nb_read);
    }
}

int create_socket(struct addrinfo* servinfo) {
    int soc;
    if ((soc = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Created Socket.\n");
    return soc;
}

struct addrinfo* get_info(char* node, char* service, struct addrinfo* hints) {
    int status;
    struct addrinfo* servinfo;
    if ((status = getaddrinfo(node, service, hints, &servinfo)) < 0) {
        fprintf(stderr, "Getaddrinfo: %s...\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    printf("Get infos.\n");
    return servinfo;
}

void make_multi_socket(int soc) {
    int opt = 1;
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("Multi socket.\n");
}

void try_connect(int soc, struct sockaddr* addr, socklen_t size) {
    int cpt = 2;
    printf("Connect attempt: 1");
    while (connect(soc, addr, size) < 0) {
        printf(" %d", cpt++);
        if (cpt > 30) {
            fprintf(stderr, "Fail connect...\n");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
    printf("\nConnected.\n");
}

void bind_listen(int soc, struct sockaddr* addr, socklen_t size) {
    if (bind(soc, addr, size) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Binded.\n");
    if (listen(soc, SOMAXCONN) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening.\n");
}

void ext_out(char* port, int tun) {
    struct sockaddr_in6 address;
    int addresslenght = sizeof address;
    struct addrinfo indic = {
        AI_PASSIVE, AF_INET6, SOCK_STREAM, 0, 0, NULL, NULL, NULL
    };
    struct addrinfo * servinfo = get_info(NULL, port, &indic);
    int srv_soc = create_socket(servinfo);
    // rendre la socket multi connexion
    bind_listen(srv_soc, servinfo->ai_addr, sizeof (struct sockaddr_in6));
    freeaddrinfo(servinfo);
    int new_soc;
    while (42) {
        if ((new_soc = accept(srv_soc, (struct sockaddr*)&address, (socklen_t*)&addresslenght)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        recopy(new_soc, tun);
    }
}

void ext_in(char* hote, char* port, int tun) {
    struct addrinfo * servinfo = get_info(hote, port, NULL);
    int srv_soc = create_socket(servinfo);
    try_connect(srv_soc, servinfo->ai_addr, sizeof (struct sockaddr_in6));
    freeaddrinfo(servinfo);
    recopy(tun, srv_soc);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("usage: %s tunname ipdest portdest portsrc\n", argv[0]);
        return (EXIT_SUCCESS);
    }
    int tun = tun_alloc(argv[1]);
    printf("Created tunnel.\n");
    system("/mnt/partage/configure-tun.sh");
    printf("Configured tunnel.\n");
    printf("Fork.\n");
    int pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // Le fils
        ext_in(argv[2], argv[3], tun);
        exit(EXIT_SUCCESS);
    }
    //Le pere
    ext_out(argv[4], tun);

    return 0;
}
