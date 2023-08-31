#pragma once

#include <stdio.h>
#include "Supermarket.h"

typedef unsigned char BYTE;

int		writeMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName);

int		readMarketFromFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName);
int		readProductFromTextFile(SuperMarket* pMarket, const char* fileName);


int		writeMarketToCompressedFile(SuperMarket* pMarket, const char* fileName);
int		writedProductsToCompressedFile(Product* pProd, FILE* fp);

int		readMarketFromCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName);
int		readAddressCompressed(SuperMarket* pMarket, const char* fileName, FILE* fp);
int		readProductCompressed(Product* pProd, FILE** fp);

char	castToChar(BYTE check);
BYTE	castToByte(char tav);
