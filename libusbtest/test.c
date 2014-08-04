#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define OR_DIE(expr) { \
    int err = (expr); \
    if (err < 0) { \
        fprintf(stderr, "%s: error %d: %s\n", #expr, err, libusb_strerror(err)); \
        exit(1); \
    } \
}

int main(int argc, char **argv)
{
    libusb_context *ctx;
    OR_DIE(libusb_init(&ctx));

    libusb_device **devs;
    ssize_t ndevs = libusb_get_device_list(ctx, &devs);

    uint8_t epoutaddr, epinaddr;
    for (int i = 0; i < ndevs; ++i) {
        printf("bus %d port %d\n", libusb_get_bus_number(devs[i]),
               libusb_get_port_number(devs[i]));

        struct libusb_device_descriptor dsc;
        OR_DIE(libusb_get_device_descriptor(devs[i], &dsc));
        printf(" vend 0x%04x prod 0x%04x\n", dsc.idVendor, dsc.idProduct);

        struct libusb_config_descriptor *cfg;
        OR_DIE(libusb_get_active_config_descriptor(devs[i], &cfg));

        printf(" nint %d\n", cfg->bNumInterfaces);
        for (int i = 0; i < cfg->bNumInterfaces; ++i) {
            struct libusb_interface intf = cfg->interface[i];
            for (int i = 0; i < intf.num_altsetting; ++i) {
                struct libusb_interface_descriptor intfdsc = intf.altsetting[i];
                printf("  cls %d sub %d pro %d nep %d\n", intfdsc.bInterfaceClass,
                       intfdsc.bInterfaceSubClass, intfdsc.bInterfaceProtocol,
                       intfdsc.bNumEndpoints);

                for (int i = 0; i < intfdsc.bNumEndpoints; ++i) {
                    struct libusb_endpoint_descriptor epd = intfdsc.endpoint[i];
                    printf("   adr 0x%x\n", epd.bEndpointAddress);

                    if (dsc.idVendor == 0x03eb && dsc.idProduct == 0x2040) {
                        printf("    ^^^ this is it\n");
                        if (epd.bEndpointAddress & 0x80)
                            epinaddr = epd.bEndpointAddress;
                        else
                            epoutaddr = epd.bEndpointAddress;
                    }
                }
            }
        }

        if (epinaddr && epoutaddr) {
            libusb_device_handle *handle;
            OR_DIE(libusb_open(devs[i], &handle));

            int xfrd;
            char sndbuf[] = "derpherpherp";
            int nsnd = strlen(sndbuf);
            uint8_t rdbuf[64];
            OR_DIE(libusb_bulk_transfer(handle, epoutaddr, (unsigned char *)sndbuf, nsnd, &xfrd, 1000));
            printf("%d xfrd to dev:\n", xfrd);
            for (int i = 0; i < xfrd; ++i)
                printf(" %02x", sndbuf[i]);
            printf("\n");
            OR_DIE(libusb_bulk_transfer(handle, epinaddr, rdbuf, sizeof rdbuf, &xfrd, 1000));
            printf("%d xfrd from dev:\n", xfrd);
            for (int i = 0; i < xfrd; ++i)
                printf(" %02x", rdbuf[i]);
            printf("\n");

            libusb_close(handle);

            epinaddr = 0;
            epoutaddr = 0;
        }

        libusb_free_config_descriptor(cfg);
    }
    libusb_free_device_list(devs, 0);

    libusb_exit(ctx);
    return 0;
}
