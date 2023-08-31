#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include "MyMacros.h"


int		writeMarketToFile(const SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	if (!fp) {
		/*printf("Error open supermarket file to write\n");
		return 0;*/
		CHECK_MSG_RETURN_0(fp);
	}

	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
	{
		/*fclose(fp);
		return 0; */
		CLOSE_RETURN_0(fp);
	}

	if (!saveAddressToFile(&pMarket->location, fp))
	{
		/*fclose(fp);
		return 0;*/
		CLOSE_RETURN_0(fp);

	}
	int count = getNumOfProductsInList(pMarket);

	if (!writeIntToFile(count, fp, "Error write product count\n"))
	{
		/*fclose(fp);
		return 0;*/
		CLOSE_RETURN_0(fp);
	}

	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!saveProductToFile(pTemp, fp))
		{
			/*fclose(fp);
			return 0;*/
			CLOSE_RETURN_0(fp);
		}
		pN = pN->next;
	}

	fclose(fp);
	saveCustomerToTextFile(pMarket->customerArr,pMarket->customerCount, customersFileName);

	return 1;
}

int		writeMarketToCompressedFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_RETURN_0(fp);
	
	//2.1 compress prod count & name length:
	BYTE data[2] = { 0 };
	int nameLen = strlen(pMarket->name);
	int count = getNumOfProductsInList(pMarket);
	data[0] = count >> 2;
	data[1] = (count &0x3) << 6 | nameLen & 0x3F;

	if (fwrite(data, sizeof(BYTE), 2, fp) != 2)
	{
		fclose(fp);
		return 0;
	}

	//2.2 write the actual name, char by char:
	char* tav;
	tav = pMarket->name;
	
	for(int i=0; i < nameLen; i++)
	{
		if (fwrite(tav, sizeof(char), 1, fp) != 1)
		{
			fclose(fp);
			return 0;
		}
		tav++;
	}

	//2.3 write the num of location 
	BYTE dataAddres[1] = { 0 };
	int numAddres = pMarket->location.num;
	dataAddres[0] = numAddres;
	if (fwrite(dataAddres, sizeof(BYTE), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}

	//2.3 write the street and the city
	//city:
	int lenghtCity = strlen(pMarket->location.city);
	if(fwrite(&lenghtCity,sizeof(int),1,fp)!=1)
	{
		fclose(fp);
		return 0;
	}
	if (fwrite(pMarket->location.city, sizeof(char), lenghtCity, fp) != lenghtCity)
	{
		fclose(fp);
		return 0;
	}
	//street:
	int lenghtStreet = strlen(pMarket->location.street);
	if (fwrite(&lenghtStreet, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}
	if (fwrite(pMarket->location.street, sizeof(char), lenghtStreet, fp) != lenghtStreet)
	{
		fclose(fp);
		return 0;
	}
	
	//2.4 write all products:
	/*if (!writeCompressedProduct(pMarket, fp))
	{
		fclose(fp);
		return 0;
	}*/
	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!writedProductsToCompressedFile(pTemp, fp)) // + 2.5 & 2.6
		{
			/*fclose(fp);
			return 0;*/
			CLOSE_RETURN_0(fp);
		}
		pN = pN->next;
	}

	fclose(fp);
	return 1;

}
int		writedProductsToCompressedFile(Product* pProd, FILE* fp)
{

	BYTE dataProduct1[3] = { 0 };

	//inserting the 1st part of barcode characters:
	dataProduct1[0] = castToByte(pProd->barcode[0]) << 2 | castToByte(pProd->barcode[1]) >> 4;
	dataProduct1[1] = castToByte(pProd->barcode[1]) << 4 | castToByte(pProd->barcode[2]) >> 2;
	dataProduct1[2] = castToByte(pProd->barcode[2]) << 6 | castToByte(pProd->barcode[3]);
	
	//wrtiting them into the file:
	if (fwrite(&dataProduct1, sizeof(BYTE), 3, fp) != 3)
	{
		fclose(fp);
		return 0;
	}

	int nameLen = strlen(pProd->name) ;
	int type = pProd->type & 0x3;
	BYTE dataProduct2[3] = { 0 };

	//inserting the 2nd part, including the length of the product's name & it's type:
	dataProduct2[0] = castToByte(pProd->barcode[4]) << 2 | castToByte(pProd->barcode[5]) >> 4;
	dataProduct2[1] = castToByte(pProd->barcode[5]) << 4 | castToByte(pProd->barcode[6]) >> 2;
	dataProduct2[2] = castToByte(pProd->barcode[6]) << 6 | (nameLen & 0xF) << 2 | type;
	
	//wrtiting them into the file:
	if (fwrite(&dataProduct2, sizeof(BYTE), 3, fp) != 3)
	{
		fclose(fp);
		return 0;
	}
	
	//2.5 write the name of the product, char by char:
	char* tav;
	tav = pProd->name;

	for (int i = 0; i < nameLen; i++)
	{
		if (fwrite(tav, sizeof(char), 1, fp) != 1)
		{
			fclose(fp);
			return 0;
		}
		tav++;
	}
	
	//2.6 write the amount & price:
	int shekels = (int)pProd->price;
	int agorot = (pProd->price - shekels) / 0.01;
	BYTE dataAmountPrice[3] = { 0 };

	dataAmountPrice[0] = pProd->count & 0xFF;
	dataAmountPrice[1] = agorot << 1 | shekels >> 8;
	dataAmountPrice[2] = shekels & 0xFF;
	if (fwrite(dataAmountPrice, sizeof(BYTE), 3, fp) != 3)
	{
		fclose(fp);
		return 0;
	}
	return 1;

}



int		readMarketFromCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	BYTE data[2];
	if (fread(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;

	// reading the bits of the number products
	int productCount=data[0] ;
	productCount = productCount << 2 | data[1] >> 6;
	
	// reading the name of the supermarket 
	int nameLen = data[1] & 0x3F;
	pMarket->name = (char*)malloc(sizeof(char) * (nameLen + 1));
	if (!pMarket->name) return 0;
	if (fread(pMarket->name, sizeof(char), nameLen, fp) != nameLen)
	{
		fclose(fp);
		return 0;
	}
	pMarket->name[nameLen] = '\0';
	//reading the adress of the store
	readAddressCompressed(pMarket, fileName, fp);
	
	// creating the list of the products. 
	
	for (int i = 0; i < productCount; i++)
	{
		Product* pProd = (Product*)malloc(sizeof(Product));
		if (!pProd) return 0;
		readProductCompressed(pProd, &fp);
		insertNewProductToList(&pMarket->productList, pProd);
	}
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	return 1;
	
}
int		readAddressCompressed(SuperMarket* pMarket, const char* fileName, FILE* fp)
{
	BYTE hoseNum;
	if (fread(&hoseNum, sizeof(BYTE), 1, fp) != 1)
		return 0;
	pMarket->location.num = hoseNum;

	int lengthCity=0 ;
	int lengthStreet=0;
	if (fread(&lengthCity, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}
	pMarket->location.city = (char*)malloc(sizeof(char) * (lengthCity + 1));
	if (!pMarket->location.city) return 0;
	if (fread(pMarket->location.city, sizeof(char), lengthCity, fp) != lengthCity)
	{
		fclose(fp);
		return 0;
	}
	pMarket->location.city[lengthCity] = '\0';
	if (fread(&lengthStreet, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}
	pMarket->location.street = (char*)malloc(sizeof(char) * (lengthStreet + 1));
	if (!pMarket->location.street) return 0;
	if (fread(pMarket->location.street, sizeof(char), lengthStreet, fp) != lengthStreet)
	{
		fclose(fp);
		return 0;
	}
	pMarket->location.street[lengthStreet] = '\0';
	return 1;
}
int		readProductCompressed(Product* pProd, FILE** fp)
{
	BYTE byteBarcode;

	BYTE dataProduct1[3] = { 0 };
	if (fread(&dataProduct1, sizeof(BYTE), 3, *fp) != 3)
	{
		fclose(fp);
		return 0;
	}
	byteBarcode = dataProduct1[0] >> 2;
	pProd->barcode[0] = castToChar(byteBarcode);

	byteBarcode = (dataProduct1[0] & 0x3) << 4 | dataProduct1[1] >> 4;
	pProd->barcode[1] = castToChar(byteBarcode);

	byteBarcode = (dataProduct1[1] & 0xf) << 2 | dataProduct1[2] >> 6;
	pProd->barcode[2] = castToChar(byteBarcode);

	byteBarcode = (dataProduct1[2] & 0x3f);
	pProd->barcode[3] = castToChar(byteBarcode);


	BYTE dataProduct2[3] = { 0 };
	if (fread(&dataProduct2, sizeof(BYTE), 3, *fp) != 3)
	{
		fclose(fp);
		return 0;
	}
	byteBarcode = (dataProduct2[0] & 0xf) >> 2;
	pProd->barcode[4] = castToChar(byteBarcode);

	byteBarcode = (dataProduct2[0] & 0x3) << 4 | dataProduct2[1] >> 4;
	pProd->barcode[5] = castToChar(byteBarcode);

	byteBarcode = (dataProduct2[1] & 0xf) << 2 | dataProduct2[2] >> 6;
	pProd->barcode[6] = castToChar(byteBarcode);

	pProd->barcode[7] = '\0';

	int nameLen = (dataProduct2[2] >> 2) & 0xf;
	pProd->type = dataProduct2[2] & 0x3;

	char* tempName = (char*)malloc(sizeof(char) * (nameLen + 1));
	if (!tempName) return 0;

	if (fread(tempName, sizeof(char), nameLen, *fp) != nameLen)
	{
		fclose(fp);
		return 0;
	}
	strcpy(pProd->name, tempName);
	pProd->name[nameLen] = '\0';
	BYTE prodPrice[3] = { 0 };
	if (fread(prodPrice, sizeof(BYTE), 3, *fp) != 3)
	{
		fclose(fp);
		return 0;
	}

	pProd->count = prodPrice[0];

	double agorot = (prodPrice[1] >> 1) * 0.01;
	int price = (prodPrice[1] & 0x1);
	price = price << 8 | prodPrice[2];
	pProd->price = (float)(agorot + price);
	return 1;
}

char	castToChar(BYTE check)
{
	if (check <= 9)
	{
		return  check + '0'; 
	}
	else
	{
		return check - 10 + 'A';
	}
}
BYTE	castToByte(char tav)
{
	if (tav >= '0' && tav <= '9')
	{
		return  tav - '0';
	}
	else
	{
		return tav + 10 - 'A';
	}
}





//Ariel's Note on reading from file: 
// The difference I saw between these two functions is that:
// on the first function - we read the products from binary file, and customers from text file.
// on the second function - we read both products & customers, from the text file.

#define BIN
#ifdef BIN

int		readMarketFromFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		/*printf("Error open company file\n");
		return 0;*/
		CHECK_MSG_RETURN_0(fp);
	}

	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		/*fclose(fp);
		return 0;*/
		CLOSE_RETURN_0(fp);
	}
	
	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		/*free(pMarket->name);
		fclose(fp);
		return 0;*/
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}

	int count;
	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		/*free(pMarket->name);
		fclose(fp);
		return 0;*/
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}

	if (count > 0)
	{
		Product* pTemp;
		for (int i = 0; i < count; i++)
		{
			//1. allocate space for product:
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				/*free(pMarket->name);
				fclose(fp);
				return 0;*/
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}

			//2. get the product from file:
			if (!loadProductFromFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				/*free(pMarket->name);
				fclose(fp);
				return 0;*/
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}

			//3. insert the product to list:
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				/*free(pMarket->name);
				fclose(fp);
				return 0;*/
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
		}
	}


	fclose(fp);

	//reading the customers from the text file:
	
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;
	

	return	1;

}

#else
int		readMarketFromFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		/*printf("Error open company file\n");
		return 0;*/
		CHECK_MSG_RETURN_0(fp);
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		/*fclose(fp);
		return 0;*/
		CLOSE_RETURN_0(fp);
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		/*free(pMarket->name);
		fclose(fp);
		return 0;*/
		FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
int		readProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	if (!fp)
	{
		/*printf("Error open company file\n");
		return 0;*/
		CHECK_MSG_RETURN_0(fp);
	}

	int count;
	fscanf(fp, "%d\n", &count);

	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		//allocate space for product:
		pTemp = (Product*)calloc(1, sizeof(Product));
		
		//read the dynamic string to the product:
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		
		//read regular data to the product:
		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		
		//insert product to sorted array:
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}

#endif
