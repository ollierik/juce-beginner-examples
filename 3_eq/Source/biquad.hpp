// This file demonstrates how to implement a biquad-based equaliser.
// A biquad is a typical way of implementing recursive IIR filter equations.
// For simple use, an explanation and design formulas written by Robert Bristow-Johnson are often used.
// It is available for example here:
// https://www.musicdsp.org/en/latest/_downloads/3e1dc886e7849251d6747b194d482272/Audio-EQ-Cookbook.txt

// In case you're not familiar with templates in C++:
//
// In C++ a class (or struct) or a function can be declared as template
// This way, if we for example need the class, like the Biquad, to be able to work with doubles or floats, we
// wouldn't have to write to two separate classes, like BiquadFloat and BiquadDouble.
//
// The templates parameters are deduced at compile time, so it can't be used to change the type at runtime.
// For that, we have to use patterns such as polymorphism and dynamic dispatch via virtual funtions or function pointers.
//
// As an added bonus, templates also make managing file includes a bit easier, because templates are implemented in the header file.
// The tradeoff is the more complicated syntax, and somewhat longer comilation times, but such is the price to pay for  the convenience.
//
// By writing template <typename FloatType> before a struct, the struct becomes a template class. In the code we can (and have to) define, what the
// actual type is that we implement it with. Audio IIR filters are a rare exception where double vs float difference is actually audible and where
// using "just" the 32-bit accuracy of the floats can cause problems such as the filter blowing up.

template <typename FloatType>
struct Biquad
{
    // Generally a pretty inefficients way to do the iir one sample at a time, but with modern CPU's
    // there's little reason to optimise this further - compiler is likely to do that anyway.
    // If the utmost performance is needed, we could resort to a more complex and specific implementation anyway.
    FloatType performFilter(FloatType inputSample)
    {
        FloatType v0 = inputSample - fb1 * v1 - fb2 * v2;
        FloatType y0 = G * v0 + ff1 * v1 + ff2 * v2;
        
        v2 = v1;
        v1 = v0;
        
        return y0;
    }
    
    void clearState()
    {
        v2 = 0.0;
        v1 = 0.0;
    }

    FloatType G, fb1, fb2, ff1, ff2; // coeffs
    FloatType v1, v2;  // internal state

    // Design a lowpass filter and replace biquad's coefficients with them
    // This is a template function. When you pass in the Biquad-reference bq, the compiler is able to
    // deduce the type of the FloatType, so you can basically use it just like a normal funtion.
    void design_lowpass_filter(FloatType f0, FloatType Q, FloatType Fs)
    {
        const FloatType pi = 3.14159265359;
        const FloatType w0 = 2 * pi * f0/Fs;
        const FloatType alpha = sin(w0)/(2*Q);

        // IIR is just a bunch of coefficients that are used to calculate the next sample from previous ones
        // Coefs b0 - b2 and a0 - a2 are the mathematical coefficients straight from RBJ's Audio EQ Cookbook.
        const FloatType b0 =  (1 - cos(w0))/2;
        const FloatType b1 =   1 - cos(w0);
        const FloatType b2 =  (1 - cos(w0))/2;
        const FloatType a0 =   1 + alpha;
        const FloatType a1 =  -2*cos(w0);
        const FloatType a2 =   1 - alpha;
    
        // IIR filters are often normalised to get rid of the a0 coefficient. This is a bit lighter for the processor.
        // So when you copy the implementations from the cookbook, just add this block to get the coeffs to the format that the
        // Biquad struct above is expecting them to be in.
        G   = b0 / a0;
        ff1 = b1 / a0;
        ff2 = b2 / a0;
        fb1 = a1 / a0;
        fb2 = a2 / a0;
    }

    // Another filter type, the most common one in EQs: peaking EQ band.
    void design_peaking_filter(FloatType f0, FloatType dBgain, FloatType Q, FloatType Fs)
    {
        const FloatType A = pow(10, (dBgain/40));
        const FloatType pi = 3.14159265359;
        const FloatType w0 = 2 * pi * f0/Fs;
        const FloatType alpha = sin(w0)/(2*Q);

        const FloatType b0 =   1 + alpha*A;
        const FloatType b1 =  -2*cos(w0);
        const FloatType b2 =   1 - alpha*A;
        const FloatType a0 =   1 + alpha/A;
        const FloatType a1 =  -2*cos(w0);
        const FloatType a2 =   1 - alpha/A;
    
        G   = b0 / a0;
        ff1 = b1 / a0;
        ff2 = b2 / a0;
        fb1 = a1 / a0;
        fb2 = a2 / a0;
    }


};

