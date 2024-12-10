// Authors: Caitlyn Lynch and Kristian Morgan

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "BENSCHILLIBOWL.h"

// Global constants defining the restaurant size, number of threads, and orders
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// Global variable representing the restaurant
BENSCHILLIBOWL *bcb;

/* Customer thread function that places orders at the restaurant */
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *(int*)tid;
    free(tid); 

    Order* order = malloc(sizeof(Order));
    if (!order) {
        perror("Failed to allocate memory for order");
        pthread_exit(NULL);
    }

    order->menu_item = PickRandomMenuItem(); // Random menu selection
    order->customer_id = customer_id;
    order->next = NULL;

    printf("Customer %d: Placing order for %s\n", customer_id, order->menu_item);
    AddOrder(bcb, order); // Add the order to the restaurant queue

    pthread_exit(NULL);
}

/* Cook thread function that retrieves and fulfills orders from the restaurant */
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *(int*)tid;
    free(tid);

    while (1) {
        Order* order = GetOrder(bcb);
        if (!order) {
            printf("Cook %d: No more orders. Exiting.\n", cook_id);
            break;
        }

        printf("Cook %d: Fulfilling order for customer %d (%s)\n",
               cook_id, order->customer_id, order->menu_item);
        free(order); // Free memory after fulfilling the order
    }

    pthread_exit(NULL);
}

/* Main function to manage the restaurant lifecycle, create threads, and clean up resources */
int main() {
    int num_customers = NUM_CUSTOMERS;
    int num_cooks = NUM_COOKS;

    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);

    pthread_t customers[num_customers];
    pthread_t cooks[num_cooks];

    // Create customer threads
    for (int i = 0; i < num_customers; i++) {
        int* customer_id = malloc(sizeof(int));
        *customer_id = i + 1;
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, customer_id);
    }

    // Create cook threads
    for (int i = 0; i < num_cooks; i++) {
        int* cook_id = malloc(sizeof(int));
        *cook_id = i + 1;
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, cook_id);
    }

    // Wait for all customers to finish
    for (int i = 0; i < num_customers; i++) {
        pthread_join(customers[i], NULL);
    }

    // Wait for all cooks to finish
    for (int i = 0; i < num_cooks; i++) {
        pthread_join(cooks[i], NULL);
    }

    CloseRestaurant(bcb); // Clean up and close the restaurant
    return 0;
}
