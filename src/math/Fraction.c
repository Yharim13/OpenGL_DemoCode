#include "Fraction.h"

void add_fraction(Fraction *dest, Fraction *f1, Fraction *f2)
{
    dest->num = f1->num * f2->den + f2->num * f1->den;
    dest->den = f1->den * f2->den;
}

void simplify_fraction(Fraction* self)
{
    int gcd = 0;

    int factor = 2;

    while (factor <= self->num && factor <= self->den)
    {
        if (self->num % factor == 0 && self->den % factor == 0)
        {
            gcd = factor;
        }
        factor++;
    }

    self->num /= gcd;
    self->den /= gcd;
}