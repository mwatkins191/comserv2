#ifndef MD5_H
#define MD5_H

typedef unsigned long uint32;

struct MD5Context {
        uint32 buf[4];
        uint32 bits[2];
        unsigned char in[64];
};


extern void MD5Init(struct MD5Context *ctc);
extern void MD5Update(struct MD5Context *ctc,unsigned char *buf, unsigned len);
extern void MD5Final(unsigned char digest[16], struct MD5Context *ctc);
extern void MD5Transform(uint32 buf[4],uint32 in[16]);

#endif 
