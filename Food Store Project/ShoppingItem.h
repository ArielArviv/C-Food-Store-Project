#pragma once
#include "Product.h"

typedef struct
{
	char		barcode[BARCODE_LENGTH + 1]; //change
	float		price;
	int			count;
}ShoppingItem;


void			printItem(const ShoppingItem* pItem);
void			freeItemV(void* val);

ShoppingItem*	createItem(const char* barcode, float price, int count);

