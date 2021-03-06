#include "tunalloc.h"


// int recopie(int src, int dest) {
//     int nb_read;
//     char buffer[1024];
//     while (42) {
//         nb_read = read(src, buffer, 1024);
//         write(dest, buffer, nb_read);
//     }
//     return -1;
// }

int tun_alloc(char *dev) {
    struct ifreq ifr;
    int fd, err;
    if((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }
    memset(&ifr, 0, sizeof(ifr));
    /* Flags:         IFF_TUN     - TUN device (no Ethernet headers)
     *                IFF_TAP     - TAP device
     *
     *                IFF_NO_PI   - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    if(*dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    if((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0) {
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_name);
    return fd;
}

// int main (int argc, char** argv){
//     if (argc != 2) {
//         printf("usage: %s tunname\n", argv[0]);
//         return 1;
//     }
//     int fd = tun_alloc(argv[1]);
//     printf("Tunnel créé.\n");
//     system("./configure-tun.sh");
//     printf("Tunnel configuré.\n");
//     recopie(fd, 1);
//     printf("Press Any Key to Continue\n");
//     getchar();
//     close(fd);
//     return 0;
// }
