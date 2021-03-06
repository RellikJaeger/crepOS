#include <port.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t data)
{
    asm volatile(   "outb %0,%1"
                    :: "a"(data),
                    "Nd"(port)
                );
}

void outw(uint16_t port, uint16_t data)
{
    asm volatile(   "outw %0,%1"
                    ::"a"(data),
                    "Nd"(port)
                );
}

void outl(uint16_t port, uint32_t data)
{
    asm volatile(   "outl %0, %1"
                    ::"a"(data),
                    "Nd"(port)
                );
}

uint8_t inb(uint16_t port)
{
    uint8_t data;
    asm volatile(   "inb %1, %0"
                    :"=a"(data)
                    :"Nd"(port)
                );
    return data;
}

uint16_t inw(uint16_t port)
{
    uint16_t data;
    asm volatile(   "inw %1, %0"
                    :"=a"(data)
                    :"Nd"(port)
                );
    return data;
}

uint32_t inl(uint16_t port)
{
    uint32_t data;
    asm volatile(   "inl %1, %0"
                    :"=a"(data)
                    :"Nd"(port)
                );
    return data;
}

void serial_init()
{
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

inline uint8_t serial_read()
{
    while((inb(COM1 + 5) & 1) == 0);
    return inb(COM1);
}

void serial_write(uint8_t data)
{
    while((inb(COM1 + 5) & 0x20) == 0);
    outb(COM1, data);
}

uint64_t get_msr(uint64_t msr)
{
    uint64_t rax, rdx;
    asm volatile ("rdmsr" : "=a"(rax), "=d"(rdx) : "c"(msr));
    return (rdx << 32) | rax;
}

void write_msr(uint64_t msr, uint64_t data)
{
    uint64_t rax = data & 0xffffffff, rdx = data >> 32;
    asm volatile("wrmsr" :: "a"(rax), "d"(rdx), "c"(msr));
}
