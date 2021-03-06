#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

int useArgs( int argc, char *argv[], int *l, long *n, float *f, int *mode, char **binaryIn);
void decimalToBinary(long decimal, int firstBit, int bits, char binary[bits]);
long binaryToDecimal(int firstBit, int bits, char binary[bits]);
void floatToBinary(float f, int firstBit, int bits, char binary[bits]);
float binaryToFloat(int firstBit, int bits, char binary[bits]);
void invert(int firstBit, int bits, char binary[bits]);
void signedMagnitude(long n, int size, char result[size+1]);
long reverseSignedMagnitude(int size, char binaryIn[size]);
void onesComplement(long n, int size, char result[size+1]);
long reverseOnesComplement(int size, char binaryIn[size]);
void twosComplement(long n, int size, char result[size+1]);
long reverseTwosComplement(int size, char binaryIn[size]);
void excess(long n, int size, char result[size+1]);
long reverseExcess(int size, char binaryIn[size]);
void toIEEESingle(float f, char result[33]);
float fromIEEESingle(char binaryIn[32]);

int main( int argc, char *argv[] ) {
    int l, mode;
    long n;
    float f = 0;
    char *binaryIn;
    if (!useArgs( argc, argv, &l, &n, &f, &mode, &binaryIn ))
        return EXIT_FAILURE;
    if (mode) {
        char result[l + 1];
        if (f){
            toIEEESingle(f, result);
            printf("IEEE-754 Single: %s\n", result);
        } else {
            signedMagnitude(n, l, result);
            printf("Signed Magnitude: %s\n", result);
            onesComplement(n, l, result);
            printf("One's Complement: %s\n", result);
            twosComplement(n, l, result);
            printf("Two's Complement: %s\n", result);
            excess(n, l, result);
            printf("Excess Representation: %s\n", result);
            if (l == 32){
                toIEEESingle(n, result);
                printf("IEEE-754 Single: %s\n", result);
            }
        }
        return EXIT_SUCCESS;
    }
    printf("Signed Magnitude: %li\n",reverseSignedMagnitude(l, binaryIn));
    printf("One's Complement: %li\n",reverseOnesComplement(l, binaryIn));
    printf("Two's Complement: %li\n",reverseTwosComplement(l, binaryIn));
    printf("Excess Representation: %li\n",reverseExcess(l, binaryIn));
    if (l == 32){
        if (!strncmp(&binaryIn[1],"1111111",7) && strcmp(&binaryIn[8],"00000000000000000000000")) {
            printf("IEEE-754 Single: NaN");
        }else{
            float result = fromIEEESingle(binaryIn);
            if ((result < 0.00001 && result > 0) || (result < -0.0 && result > -0.00001))//Numbers which will truncate to 0
                printf("IEEE-754 Single: %.60lf\n",result);
            else
                printf("IEEE-754 Single: %lf\n",result);
        }
    }
    return EXIT_SUCCESS;
}

void signedMagnitude(long n, int size, char result[size+1]){
    result[size]='\0';
    if (n<0){
        n *= -1;
        result[0] = '1';
    } else {
        result[0] = '0';
    }
    decimalToBinary(n, 1, size, result);
}

long reverseSignedMagnitude(int size, char binaryIn[size]){
    long result = binaryToDecimal(1,size,binaryIn);
    if (binaryIn[0] == '1')
        result *= -1;
    return result;
}

void onesComplement(long n, int size, char result[size+1]){
    result[size]='\0';
    int toInvert = 0;
    if (n<0) {
        n *= -1;
        toInvert = 1;
    }
    result[0] = '0';
    decimalToBinary(n, 1, size, result);
    if (toInvert){
        invert(0,size,result);
    }
}

long reverseOnesComplement(int size, char binaryIn[size]){
    if (binaryIn[0] == '0')
        return binaryToDecimal(0,size,binaryIn);
    char inverse[size];
    strcpy(inverse,binaryIn);
    invert(0,size,inverse);
    long result = binaryToDecimal(0,size,inverse);
    result *= -1;
    return result;
}


void twosComplement(long n, int size, char result[size+1]){
    result[size]='\0';
    int toInvert = 0;
    if (n<0) {
        n *= -1;
        toInvert = 1;
    }
    result[0] = '0';
    decimalToBinary(n, 1, size, result);
    if (toInvert){
        invert(0,size,result);
        int pos = size-1;//Add one after inverting
        while (result[pos] == '1'){
            result[pos] = '0';
            pos--;
        }
        result[pos] = '1';
    }
}

long reverseTwosComplement(int size, char binaryIn[size]){
    if (binaryIn[0] == '0')
        return binaryToDecimal(0,size,binaryIn);
    char inverse[size];
    strcpy(inverse,binaryIn);
    invert(0,size,inverse);
    int pos = size-1;//Add one after inverting
    while (inverse[pos] == '1'){
        inverse[pos] = '0';
        pos--;
    }
    inverse[pos] = '1';
    long result = binaryToDecimal(0,size,inverse);
    result *= -1;
    return result;
}

void excess(long n, int size, char result[size+1]){
    long bias = (long)pow(2,size-1)- 1;
    n += bias;
    result[size] = '\0';
    decimalToBinary(n, 0, size, result);
}

long reverseExcess(int size, char binaryIn[size]){
    long bias = (long)pow(2,size-1)- 1;
    long num = binaryToDecimal(0,size,binaryIn);
    return num - bias;
}

void toIEEESingle(float f, char result[32]){
    result[32] = '\0';
    if (f < 0){
        result[0] = '1';
        f *= -1;
    } else {
        result[0] = '0';
    }
    long exponent = 0;
    if (f == 0){//zero will result in infinite loop so we better catch it
        for (int i=1; i<32; i++){
            result[i] = '0';
        }
        return;
    }
    if (f > 2){
        while (f > 2){
            f /= 2;
            exponent++;
        }
    } else if (f < 1){
        while (f < 1){
            f *= 2;
            exponent--;
        }
    }
    excess(exponent,8,&result[1]);
    floatToBinary(f-1,9,23,result);
}

float fromIEEESingle(char binaryIn[32]){
    int negative = binaryIn[0] == '1' ? 1 : 0;
    int denormalised = strncmp(&binaryIn[1],"00000000",8);
    long exponent = reverseExcess(8,&binaryIn[1]);
    float significand = binaryToFloat(9,23,binaryIn);
    if (denormalised)
        significand += 1;
    float result = significand * pow(2,exponent);
    if (negative)
        result *= -1;
    return result;
}

void decimalToBinary(long decimal, int firstBit, int bits, char binary[bits]){
    for (int i=firstBit; i<bits; i++) {
        long divisor = (long) (pow(2, bits - (i + 1)) + 0.5);
        if (decimal / divisor)
            binary[i] = '1';
        else
            binary[i] = '0';
        decimal = decimal % divisor;
    }
}

void floatToBinary(float f, int firstBit, int bits, char binary[bits]){
    for (int i=1; i<=bits; i++){
        float divisor = (pow(2, -1*i));
        if (f >= divisor) {
            binary[i + (firstBit - 1)] = '1';
            f -= divisor;
        } else {
            binary[i + (firstBit - 1)] = '0';
        }
    }
}

long binaryToDecimal(int firstBit, int bits, char binary[bits]){
    long sum = 0;
    for (int i=firstBit; i<bits; i++) {
        long divisor = (long) (pow(2, bits - 1 - i ));
        if (binary[i] == '1')
            sum += divisor;
    }
    return sum;
}

float binaryToFloat(int firstBit, int bits, char binary[bits]){
    float sum = 0;
    for (int i=1; i<bits; i++) {
        float divisor = (pow(2, -1*i));
        if (binary[i + (firstBit - 1)] == '1')
            sum += divisor;
    }
    return sum;
}

void invert(int firstBit, int bits, char binary[bits]){
    for (int i=firstBit; i<bits; i++) {
        if (binary[i] == '1')
            binary[i] = '0';
        else
            binary[i] = '1';
    }
}

int useArgs( int argc, char *argv[], int *l, long *n, float *f, int *mode, char **binaryIn ){
    if (argc > 3 || argc < 2){
        fprintf(stderr,"Usage Error: Expected two arguments, Number and Length; a single float; or a binary bit-string\n");
        return 0;
    }
    if (argc == 3) {
        *mode = 1;
        *l = atoi(argv[2]);
        if (*l < 2 || *l > 32) {
            fprintf(stderr, "Length must be an integer between 2 and 32\n");
            return 0;
        }
        *n = atol(argv[1]);
        long largestInt = (long) pow(2, *l - 1) - 1;
        long smallestInt = (long) (-1 * pow(2, *l - 1)) + 1;
        if (*n > largestInt || *n < smallestInt) {
            fprintf(stderr, "Can only represent numbers between %li and %li in %i bits\n", smallestInt, largestInt, *l);
            return 0;
        }
        return 1;
    }

    *mode = 0;
    for (int i=0; i<strlen(argv[1]);i++){
        if (argv[1][i]!='1' && argv[1][i]!='0'){
            *n = atol(argv[1]);//check for float that parses
            *f = atof(argv[1]);
            if (*f != *n){
                *l = 32;
                *mode = 1;
                return 1;
            }else {
                fprintf(stderr, "Expected binary string to convert\n");
                return 0;
            }
        }
    }
    *binaryIn = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(*binaryIn,argv[1]);
    if (strlen(*binaryIn) > 32 || strlen(*binaryIn) < 2){
        fprintf(stderr,"Binary string must be between 2 and 32 characters long\n");
        return 0;
    }
    *l = strlen(*binaryIn);
    return 1;
}