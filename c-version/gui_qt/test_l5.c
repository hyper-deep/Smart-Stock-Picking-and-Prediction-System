#include <stdio.h>
#include <string.h>
#include "stock.h"
#include "portable.h"
#include "stock_maintain.h"
#include "stock_seek.h"
int main(void) {
    static Stock stocks[MAX_STOCK];
    int count = read_stock_file((char*)"stocks.txt", stocks);
    printf("count=%d\n", count);
    FILE* f = fopen("_l5in.txt", "w");
    fprintf(f, "Kweichow Moutai\n");
    fclose(f);
    freopen("_l5in.txt", "r", stdin);
    BST_Search_stock(stocks, count);
    printf("L5 done\n");
    return 0;
}
