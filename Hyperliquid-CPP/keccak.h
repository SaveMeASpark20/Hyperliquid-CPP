#ifndef KECCAK_H
#define KECCAK_H

#include <vector>
#include <stdint.h>
#include <string.h>

class Keccak {
    static uint64_t rotl(uint64_t x, int n) { return (x << n) | (x >> (64 - n)); }

    static void f(uint64_t* st) {
        static const uint64_t rc[24] = {
            0x0000000000000001, 0x0000000000008082, 0x800000000000808a, 0x8000000080008000,
            0x000000000000808b, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
            0x000000000000008a, 0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
            0x000000008000808b, 0x800000000000008b, 0x8000000000008089, 0x8000000000008003,
            0x8000000000008002, 0x8000000000000080, 0x000000000000800a, 0x800000008000000a,
            0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008
        };
        static const int r[24] = { 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 44, 31, 23, 39, 56, 8, 25, 43, 62, 18, 39 };
        static const int p[24] = { 10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1 };

        for (int i = 0; i < 24; i++) {
            uint64_t bc[5], t;
            for (int j = 0; j < 5; j++) bc[j] = st[j] ^ st[j + 5] ^ st[j + 10] ^ st[j + 15] ^ st[j + 20];
            for (int j = 0; j < 5; j++) {
                t = bc[(j + 4) % 5] ^ rotl(bc[(j + 1) % 5], 1);
                for (int k = 0; k < 25; k += 5) st[j + k] ^= t;
            }
            t = st[1];
            for (int j = 0; j < 24; j++) {
                int k = p[j];
                bc[0] = st[k];
                st[k] = rotl(t, r[j]);
                t = bc[0];
            }
            for (int j = 0; j < 25; j += 5) {
                for (int k = 0; k < 5; k++) bc[k] = st[j + k];
                for (int k = 0; k < 5; k++) st[j + k] ^= (~bc[(k + 1) % 5]) & bc[(k + 2) % 5];
            }
            st[0] ^= rc[i];
        }
    }

public:
    static std::vector<uint8_t> hash(const std::vector<uint8_t>& data) {
        uint64_t st[25] = { 0 };
        uint8_t* byte_st = (uint8_t*)st;
        size_t rsiz = 136;

        // ABSORPTION: Manually XOR bytes into the state
        for (size_t i = 0; i < data.size(); i++) {
            byte_st[i % rsiz] ^= data[i];
            if ((i + 1) % rsiz == 0) f(st);
        }

        // PADDING: Ethereum Keccak-256 specific
        byte_st[data.size() % rsiz] ^= 0x01;
        byte_st[rsiz - 1] ^= 0x80;
        f(st);

        std::vector<uint8_t> out(32);
        memcpy(out.data(), st, 32);
        return out;
    }
};

#endif
