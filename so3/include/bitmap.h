#ifndef BITMAP_H
#define BITMAP_H

#ifndef __ASSEMBLY__

#include <common.h>
#include <types.h>
#include <bitops.h>
#include <string.h>

/*
 * bitmaps provide bit arrays that consume one or more unsigned
 * longs.  The bitmap interface and available operations are listed
 * here, in bitmap.h
 *
 * Function implementations generic to all architectures are in
 * lib/bitmap.c.  Functions implementations that are architecture
 * specific are in various include/asm-<arch>/bitops.h headers
 * and other arch/<arch> specific files.
 *
 * See lib/bitmap.c for more details.
 */

/*
 * The available bitmap operations and their rough meaning in the
 * case that the bitmap is a single unsigned long are thus:
 *
 * bitmap_zero(dst, nbits)			*dst = 0UL
 * bitmap_fill(dst, nbits)			*dst = ~0UL
 * bitmap_copy(dst, src, nbits)			*dst = *src
 * bitmap_and(dst, src1, src2, nbits)		*dst = *src1 & *src2
 * bitmap_or(dst, src1, src2, nbits)		*dst = *src1 | *src2
 * bitmap_xor(dst, src1, src2, nbits)		*dst = *src1 ^ *src2
 * bitmap_andnot(dst, src1, src2, nbits)	*dst = *src1 & ~(*src2)
 * bitmap_complement(dst, src, nbits)		*dst = ~(*src)
 * bitmap_equal(src1, src2, nbits)		Are *src1 and *src2 equal?
 * bitmap_intersects(src1, src2, nbits) 	Do *src1 and *src2 overlap?
 * bitmap_subset(src1, src2, nbits)		Is *src1 a subset of *src2?
 * bitmap_empty(src, nbits)			Are all bits zero in *src?
 * bitmap_full(src, nbits)			Are all bits set in *src?
 * bitmap_weight(src, nbits)			Hamming Weight: number set bits
 * bitmap_shift_right(dst, src, n, nbits)	*dst = *src >> n
 * bitmap_shift_left(dst, src, n, nbits)	*dst = *src << n
 * bitmap_scnprintf(buf, len, src, nbits)	Print bitmap src to buf
 * bitmap_scnlistprintf(buf, len, src, nbits)	Print bitmap src as list to buf
 */

/*
 * Also the following operations in asm/bitops.h apply to bitmaps.
 *
 * set_bit(bit, addr)			*addr |= bit
 * clear_bit(bit, addr)			*addr &= ~bit
 * change_bit(bit, addr)		*addr ^= bit
 * test_bit(bit, addr)			Is bit set in *addr?
 * test_and_set_bit(bit, addr)		Set bit and return old value
 * test_and_clear_bit(bit, addr)	Clear bit and return old value
 * test_and_change_bit(bit, addr)	Change bit and return old value
 * find_first_zero_bit(addr, nbits)	Position first zero bit in *addr
 * find_first_bit(addr, nbits)		Position first set bit in *addr
 * find_next_zero_bit(addr, nbits, bit)	Position next zero bit in *addr >= bit
 * find_next_bit(addr, nbits, bit)	Position next set bit in *addr >= bit
 */

/*
 * The DECLARE_BITMAP(name,bits) macro, in types.h, can be used
 * to declare an array named 'name' of just enough unsigned longs to
 * contain all bit positions from 0 to 'bits' - 1.
 */

/*
 * lib/bitmap.c provides these functions:
 */

extern int __bitmap_empty(const unsigned long *bitmap, int bits);
extern int __bitmap_full(const unsigned long *bitmap, int bits);
extern int __bitmap_equal(const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern void __bitmap_complement(unsigned long *dst, const unsigned long *src, int bits);
extern void __bitmap_shift_right(unsigned long *dst, const unsigned long *src, int shift, int bits);
extern void __bitmap_shift_left(unsigned long *dst, const unsigned long *src, int shift, int bits);
extern void __bitmap_and(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern void __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern int __bitmap_intersects(const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern int __bitmap_subset(const unsigned long *bitmap1, const unsigned long *bitmap2, int bits);
extern int __bitmap_weight(const unsigned long *bitmap, int bits);

extern int bitmap_scnprintf(char *buf, unsigned int len, const unsigned long *src, int nbits);
extern int bitmap_scnlistprintf(char *buf, unsigned int len, const unsigned long *src, int nbits);
extern int bitmap_find_free_region(unsigned long *bitmap, int bits, int order);
extern void bitmap_release_region(unsigned long *bitmap, int pos, int order);
extern int bitmap_allocate_region(unsigned long *bitmap, int pos, int order);

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

static inline void bitmap_zero(unsigned long *dst, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = 0UL;
	else {
		int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
		memset(dst, 0, len);
	}
}

static inline void bitmap_fill(unsigned long *dst, int nbits)
{
	size_t nlongs = BITS_TO_LONGS(nbits);
	if (nlongs > 1) {
		int len = (nlongs - 1) * sizeof(unsigned long);
		memset(dst, 0xff, len);
	}
	dst[nlongs - 1] = BITMAP_LAST_WORD_MASK(nbits);
}

static inline void bitmap_copy(unsigned long *dst, const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src;
	else {
		int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
		memcpy(dst, src, len);
	}
}

static inline void bitmap_and(unsigned long *dst, const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 & *src2;
	else
		__bitmap_and(dst, src1, src2, nbits);
}

static inline void bitmap_or(unsigned long *dst, const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 | *src2;
	else
		__bitmap_or(dst, src1, src2, nbits);
}

static inline void bitmap_xor(unsigned long *dst, const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 ^ *src2;
	else
		__bitmap_xor(dst, src1, src2, nbits);
}

static inline void bitmap_andnot(unsigned long *dst, const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src1 & ~(*src2);
	else
		__bitmap_andnot(dst, src1, src2, nbits);
}

static inline void bitmap_complement(unsigned long *dst, const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = ~(*src) & BITMAP_LAST_WORD_MASK(nbits);
	else
		__bitmap_complement(dst, src, nbits);
}

static inline int bitmap_equal(const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return !((*src1 ^ *src2) & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_equal(src1, src2, nbits);
}

static inline int bitmap_intersects(const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return ((*src1 & *src2) & BITMAP_LAST_WORD_MASK(nbits)) != 0;
	else
		return __bitmap_intersects(src1, src2, nbits);
}

static inline int bitmap_subset(const unsigned long *src1, const unsigned long *src2, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return !((*src1 & ~(*src2)) & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_subset(src1, src2, nbits);
}

static inline int bitmap_empty(const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return !(*src & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_empty(src, nbits);
}

static inline int bitmap_full(const unsigned long *src, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		return !(~(*src) & BITMAP_LAST_WORD_MASK(nbits));
	else
		return __bitmap_full(src, nbits);
}

static inline int bitmap_weight(const unsigned long *src, int nbits)
{
	return __bitmap_weight(src, nbits);
}

static inline void bitmap_shift_right(unsigned long *dst, const unsigned long *src, int n, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = *src >> n;
	else
		__bitmap_shift_right(dst, src, n, nbits);
}

static inline void bitmap_shift_left(unsigned long *dst, const unsigned long *src, int n, int nbits)
{
	if (nbits <= BITS_PER_LONG)
		*dst = (*src << n) & BITMAP_LAST_WORD_MASK(nbits);
	else
		__bitmap_shift_left(dst, src, n, nbits);
}

void bitmap_long_to_byte(uint8_t *bp, const unsigned long *lp, int nbits);
void bitmap_byte_to_long(unsigned long *lp, const uint8_t *bp, int nbits);

#endif /* __ASSEMBLY__ */

#endif /* BITMAP_H */
