#ifndef FRACTION_H
#define FRACTION_H

typedef struct {
    int num, den;
} Fraction;

extern void add_fraction(Fraction *dest, Fraction *f1, Fraction *f2);

extern void simplify_fraction(Fraction* self);

#endif