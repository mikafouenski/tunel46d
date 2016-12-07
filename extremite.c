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

void try_connect(int soc, struct sockaddr* addr, socklen_t size) {
    int cpt = 2;
    printf("Connect attempt: 1");
    while (connect(soc, addr, size) < 0) {
        printf(" %d", cpt++);
        if (cpt > 30) {
            fprintf(stderr, "Fail connect...\n");
            exit (EXIT_FAILURE);
        }
        sleep(2);
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
    bind_listen(srv_soc, servinfo->ai_addr, sizeof (struct sockaddr_in6));
    freeaddrinfo(servinfo);
    int new_soc;
    if ((new_soc = accept(srv_soc, (struct sockaddr*)&address, (socklen_t*)&addresslenght)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    recopy(new_soc, tun);
}

void ext_in(char* hote, char* port, int tun) {
    struct addrinfo * servinfo = get_info(hote, port, NULL);
    int srv_soc = create_socket(servinfo);
    try_connect(srv_soc, servinfo->ai_addr, sizeof (struct sockaddr_in6));
    freeaddrinfo(servinfo);
    recopy(tun, srv_soc);
}

const char* getfield(char* line, int num) {
    const char* tok;
    for (tok = strtok(line, "="); tok && *tok; tok = strtok(NULL, "=\n"))
        if (!--num)
            return tok;
    return NULL;
}

int son;
void int_handler(int blop) {
    printf("Ctrl+c detected.\n");
    kill(son, SIGINT);
    exit (EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: %s configfile\n", argv[0]);
        return (EXIT_FAILURE);
    }

    char* tun_name;
    char* in_ip;
    char* in_port;
    char* out_ip;
    char* out_port;

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("fopen");
        return (EXIT_FAILURE);
    }
    char line[128];
    while (fgets(line, 128, f)) {
        if (!strchr(line, '#')) {
            char * pline1 = strdup(line);
            char * pline2 = strdup(line);
            const char * key = getfield(pline1, 1);
            const char * value = getfield(pline2, 2);
            if (strcmp(key, "tun") == 0) {
                tun_name = strdup(value);
            } else if (strcmp(key, "inip") == 0) {
                in_ip = strdup(value);
            } else if (strcmp(key, "inport") == 0) {
                in_port = strdup(value);
            } else if (strcmp(key, "outip") == 0) {
                out_ip = strdup(value);
            } else if (strcmp(key, "outport") == 0) {
                out_port = strdup(value);
            } else {
                printf("unknown config\n");
            }
            free(pline1);
            free(pline2);
        }
    }
    fclose(f);

    int tun = tun_alloc(tun_name);
    printf("Created tunnel.\n");
    system("/mnt/partage/configure-tun.sh");
    printf("Configured tunnel.\n");
    printf("Fork.\n");
    son = fork();
    if (son < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (son == 0) { // Le fils
        ext_in(out_ip, out_port, tun);
    }
    //Le pere
    signal(SIGINT, int_handler);
    ext_out(in_port, tun);

    free(tun_name);
    free(in_ip);
    free(in_port);
    free(out_ip);
    free(out_port);

    return (EXIT_SUCCESS);
}
