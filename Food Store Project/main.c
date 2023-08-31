#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"





int menu();


int main(int  argc, char* argv[])
{
	if (argc != 3) {
		printf("Error in params\n");
		system("pause");
		return -1;
	}
	int fileLen = 0;
	int isCompressed=0;
	SuperMarket	market;
	char* fileName = NULL;
	fileLen = strlen(argv[2]);
	fileName = (char*)malloc(sizeof(char) * (fileLen + 1));
	if (strcmp(argv[1],"1")==0)
	{
		isCompressed = 1;
	}
	else
	{
		isCompressed = 0;
	}
	strcpy(fileName, argv[2]);

	if (!initSuperMarket(&market, fileName,CUSTOMER_FILE_NAME,isCompressed))
	{
		printf("error init  Super Market");
		return 0;
	}

	int option;
	int stop = 0;
	
	do
	{
		option = menu();
		switch (option)
		{
		case eShowSuperMarket:
			printSuperMarket(&market);
			break;


		case eAddProduct:
			if (!addProduct(&market))
				printf("Error adding product\n");
			break;

		case eAddCustomer:
			if (!addCustomer(&market))
				printf("Error adding customer\n");
			break;

		case eCustomerDoShopping:
			if(!doShopping(&market))
				printf("Error in shopping\n");
			break;

		case ePrintCart:
			doPrintCart(&market);
			break;

		case eCustomerPay:
			if(!doPayment(&market))
				printf("Error in payment\n");
			break;
		
		case eSortCustomer:
			sortCustomers(&market);
			break;
			
		case eSearchCustomer:
			findCustomer(&market);
			break;
			
		case ePrintProductByType:
			printProductByType(&market);
			break;

		case EXIT:
			printMessage("Thank", "You", "For", "Shopping", "With", "Us", NULL);
			stop = 1;
			break;

		default:
			printf("Wrong option\n");
			break;
		}
	} while (!stop);

	handleCustomerStillShoppingAtExit(&market);
	if (isCompressed == 1) {
		if (!writeMarketToCompressedFile(&market, fileName))
			printf("Error saving supermarket to file\n");
		saveCustomerToTextFile(market.customerArr, market.customerCount, CUSTOMER_FILE_NAME);
	}
	else
	{
		if (!writeMarketToFile(&market, fileName, CUSTOMER_FILE_NAME))
			printf("Error saving supermarket to file\n");
	}

	freeMarket(&market);

	system("pause");
	return 1;
}

int menu()
{
	int option;
	printf("\n\n");
	printf("Please choose one of the following options\n");
	for(int i = 0 ; i < eNofOptions ; i++)
		printf("%d - %s\n",i, menuStrings[i]);
	printf("%d - Quit\n", EXIT);
	scanf("%d", &option);
	//clean buffer
	char tav;
	scanf("%c", &tav);
	return option;
}

