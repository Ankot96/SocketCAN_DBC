/*
 * Native Linux SocketCAN transmitter for KAPEXLab DBC assignment.
 * Usage: ./can_transmitter [interface]
 * Default interface: vcan0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static volatile sig_atomic_t running = 1;

static void stop_running(int sig) {
    (void)sig;
    running = 0;
}

static void put_u16_le(struct can_frame *frame, uint16_t raw) {
    frame->data[0] = (uint8_t)(raw & 0xFF);
    frame->data[1] = (uint8_t)((raw >> 8) & 0xFF);
    frame->can_dlc = 8;
}

static int send_signal(int sock, canid_t id, uint16_t raw) {
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id = id;
    put_u16_le(&frame, raw);
    if (write(sock, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("CAN write");
        return -1;
    }
    return 0;
}

static double clamp(double value, double low, double high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

int main(int argc, char *argv[]) {
    const char *ifname = (argc > 1) ? argv[1] : "vcan0";
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX (is the CAN interface up?)");
        close(sock);
        return 1;
    }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    signal(SIGINT, stop_running);
    signal(SIGTERM, stop_running);
    srand((unsigned int)time(NULL));

    double speed = 0.0;
    double rpm = 900.0;
    double coolant = 75.0;
    double fuel = 85.0;
    double battery = 12.6;
    double ambient = 28.0;
    int speed_direction = 1;
    int fuel_tick = 0;

    printf("Transmitting vehicle signals on %s. Press Ctrl+C to stop.\n", ifname);

    while (running) {
        /* Smooth, bounded changes make the generated values look plausible. */
        speed += speed_direction * (1.0 + (rand() % 30) / 10.0);
        if (speed >= 120.0) { speed = 120.0; speed_direction = -1; }
        if (speed <= 0.0) { speed = 0.0; speed_direction = 1; }

        rpm = 800.0 + (speed / 120.0) * 4200.0 + (rand() % 121) - 60;
        rpm = clamp(rpm, 800.0, 5000.0);

        coolant += ((speed > 20.0) ? 0.04 : -0.015);
        coolant = clamp(coolant, 20.0, 105.0);

        if (++fuel_tick >= 50) {
            fuel -= 0.5;
            if (fuel < 0.0) fuel = 100.0;
            fuel_tick = 0;
        }

        battery = 14.2 - (speed < 2.0 ? 0.15 : 0.0) + ((rand() % 21) - 10) / 100.0;
        battery = clamp(battery, 11.0, 15.0);
        ambient += ((rand() % 3) - 1) * 0.1;
        ambient = clamp(ambient, 15.0, 40.0);

        /* DBC raw = (physical - offset) / factor. */
        uint16_t speed_raw = (uint16_t)(speed / 0.1 + 0.5);
        uint16_t rpm_raw = (uint16_t)(rpm + 0.5);
        uint16_t coolant_raw = (uint16_t)((coolant + 40.0) / 0.1 + 0.5);
        uint16_t fuel_raw = (uint16_t)(fuel / 0.5 + 0.5);
        uint16_t battery_raw = (uint16_t)(battery / 0.01 + 0.5);
        uint16_t ambient_raw = (uint16_t)((ambient + 40.0) / 0.1 + 0.5);

        if (send_signal(sock, 0x100, speed_raw) ||
            send_signal(sock, 0x101, rpm_raw) ||
            send_signal(sock, 0x102, coolant_raw) ||
            send_signal(sock, 0x103, fuel_raw) ||
            send_signal(sock, 0x104, battery_raw) ||
            send_signal(sock, 0x105, ambient_raw)) {
            running = 0;
            break;
        }

        printf("\rSpeed:%5.1f km/h | RPM:%4.0f | Coolant:%5.1f C | Fuel:%5.1f%% | Battery:%4.2f V | Ambient:%4.1f C",
               speed, rpm, coolant, fuel, battery, ambient);
        fflush(stdout);
        usleep(100000);
    }

    printf("\nTransmitter stopped.\n");
    close(sock);
    return 0;
}
