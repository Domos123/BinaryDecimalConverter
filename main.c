#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

int l, n, mode;
char *binaryIn;

int useArgs( int argc, char *argv[] );
void decimalToBinary(int decimal, int firstBit, int bits, char binary[bits]);
int binaryToDecimal(int firstBit, int bits, char binary[bits]);
void invert(int firstBit, int bits, char binary[bits]);
void signedMagnitude(int size, char result[size+1]);
int reverseSignedMagnitude();
void onesComplement(int size, char result[size+1]);
int reverseOnesComplement();
void twosComplement(int size, char result[size+1]);
int reverseTwosComplement();
void excess(int size, char result[size+1]);
int reverseExcess();

int main( int argc, char *argv[] ) {
    if (!useArgs( argc, argv ))
        return EXIT_FAILURE;
    if (mode) {
        char result[l + 1];
        signedMagnitude(l, result);
        printf("Signed Magnitude: %s\n", result);
        onesComplement(l, result);
        printf("One's Complement: %s\n", result);
        twosComplement(l, result);
        printf("Two's Complement: %s\n", result);
        excess(l, result);
        printf("Excess Representation: %s\n", result);
        return EXIT_SUCCESS;
    }
    printf("Signed Magnitude: %d\n",reverseSignedMagnitude());
    printf("One's Complement: %d\n",reverseOnesComplement());
    printf("Two's Complement: %d\n",reverseTwosComplement());
    printf("Excess Representation: %d\n",reverseExcess());
    return EXIT_SUCCESS;
}

void signedMagnitude(int size, char result[size+1]){
    result[size]='\0';
    int num;
    if (n<0){
        num = n * -1;
        result[0] = '1';
    } else {
        num = n;
        result[0] = '0';
    }
    decimalToBinary(num, 1, size, result);
}

int reverseSignedMagnitude(){
    int result = binaryToDecimal(1,l,binaryIn);
    if (binaryIn[0] == '1')
        result *= -1;
    return result;
}

void onesComplement(int size, char result[size+1]){
    result[size]='\0';
    int num;
    if (n<0)
        num = n * -1;
    else
        num = n;
    result[0] = '0';
    decimalToBinary(num, 1, size, result);
    if (n < 0){
        invert(0,size,result);
    }
}

int reverseOnesComplement(){
    if (binaryIn[0] == '0')
        return binaryToDecimal(0,l,binaryIn);
    char inverse[l];
    strcpy(inverse,binaryIn);
    invert(0,l,inverse);
    int result = binaryToDecimal(0,l,inverse);
    result *= -1;
    return result;
}


void twosComplement(int size, char result[size+1]){
    result[size]='\0';
    int num;
    if (n<0)
        num = n * -1;
    else
        num = n;
    result[0] = '0';
    decimalToBinary(num, 1, size, result);
    if (n < 0){
        invert(0,size,result);
        int pos = size-1;//Add one after inverting
        while (result[pos] == '1'){
            result[pos] = '0';
            pos--;
        }
        result[pos] = '1';
    }
}

int reverseTwosComplement(){
    if (binaryIn[0] == '0')
        return binaryToDecimal(0,l,binaryIn);
    char inverse[l];
    strcpy(inverse,binaryIn);
    invert(0,l,inverse);
    int pos = l-1;//Add one after inverting
    while (inverse[pos] == '1'){
        inverse[pos] = '0';
        pos--;
    }
    inverse[pos] = '1';
    int result = binaryToDecimal(0,l,inverse);
    result *= -1;
    return result;
}

void excess(int size, char result[size+1]){
    int bias = (int)pow(2,l-1)- 1;
    int num = n + bias;
    result[size] = '\0';
    decimalToBinary(num, 0, size, result);
}

int reverseExcess(){
    int bias = (int)pow(2,l-1)- 1;
    int num = binaryToDecimal(0,l,binaryIn);
    return num - bias;
}

void decimalToBinary(int decimal, int firstBit, int bits, char binary[bits]){
    for (int i=firstBit; i<bits; i++) {
        int divisor = (int) (pow(2, bits - (i + 1)) + 0.5);
        if (decimal / divisor)
            binary[i] = '1';
        else
            binary[i] = '0';
        decimal = decimal % divisor;
    }
}

int binaryToDecimal(int firstBit, int bits, char binary[bits]){
    int sum = 0;
    for (int i=firstBit; i<bits; i++) {
        int divisor = (int) (pow(2, bits - 1 - i ));
        if (binary[i] == '1')
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

int useArgs( int argc, char *argv[] ){
    if (argc > 3 || argc < 2){
        fprintf(stderr,"Usage Error: Expected two arguments, Number and Length, or a binary bit-string\n");
        return 0;
    }
    if (argc == 3) {
        mode = 1;
        l = atoi(argv[2]);
        if (l < 1 || l > 64) {
            fprintf(stderr, "Length must be an integer between 1 and 64\n");
            return 0;
        }
        n = atoi(argv[1]);
        int largestInt = (int) pow(2, l - 1) - 1;
        int smallestInt = (int) (-1 * pow(2, l - 1)) + 1;
        if (n > largestInt || n < smallestInt) {
            fprintf(stderr, "Can only represent numbers between %d and %d in %d bits\n", smallestInt, largestInt, l);
            return 0;
        }
        return 1;
    }
    mode = 0;
    for (int i=0; i<strlen(argv[1]);i++){
        if (argv[1][i]!='1' && argv[1][i]!='0'){
            fprintf(stderr,"Expected binary string to convert\n");
            return 0;
        }
    }
    binaryIn = (char *)malloc(sizeof(argv[1]));
    strcpy(binaryIn,argv[1]);
    if (strlen(binaryIn) > 64 || strlen(binaryIn) < 2){
        fprintf(stderr,"Binary string must be between 2 and 64 characters long\n");
        return 0;
    }
    l = strlen(binaryIn);
    return 1;
}