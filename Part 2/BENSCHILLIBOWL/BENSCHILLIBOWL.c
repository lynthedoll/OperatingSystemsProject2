// Authors: Caitlyn Lynch and Kristian Morgan

#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Check if the restaurant queue is empty
bool IsEmpty(BENSCHILLIBOWL* bcb);
 
// Check if the restaurant queue is full
bool IsFull(BENSCHILLIBOWL* bcb);

// Adds an order to the back of the queue
void AddOrderToBack(Order **orders, Order *order);

// Restaurant menu items
MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

/* Selects a random item from the menu and returns it */
MenuItem PickRandomMenuItem() {
    int random_index = rand() % BENSCHILLIBOWLMenuLength; 
    return BENSCHILLIBOWLMenu[random_index];
}

/* Opens the restaurant by allocating memory, initializing variables, and creating mutex/condition variables */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL* bcb = malloc(sizeof(BENSCHILLIBOWL));
    if (!bcb) {
        perror("Failed to allocate memory for restaurant");
        exit(1);
    }
    bcb->max_size = max_size;
    bcb->expected_num_orders = expected_num_orders;
    bcb->orders = NULL; // No orders initially
    bcb->current_size = 0;
    bcb->orders_handled = 0;
    bcb->orders_received = 0;

    // Initialize synchronization objects
    pthread_mutex_init(&(bcb->mutex), NULL);
    pthread_cond_init(&(bcb->can_add_orders), NULL);
    pthread_cond_init(&(bcb->can_get_orders), NULL);

    printf("Restaurant is now open!\n");
    return bcb;
}

/* Closes the restaurant, ensuring all orders are fulfilled and cleaning up resources */
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
    if (bcb->orders_received != bcb->orders_handled) {
        printf("Warning: Not all orders were handled.\n");
    }
    pthread_mutex_unlock(&(bcb->mutex));

    // Destroy synchronization objects
    pthread_mutex_destroy(&(bcb->mutex));
    pthread_cond_destroy(&(bcb->can_add_orders));
    pthread_cond_destroy(&(bcb->can_get_orders));

    // Free memory
    free(bcb);

    printf("Restaurant is now closed!\n");
}

/* Adds an order to the queue and signals cooks when a new order is available */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&(bcb->mutex));

    // Wait until there is space to add an order
    while (bcb->current_size == bcb->max_size) {
        pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }

    AddOrderToBack(&(bcb->orders), order);
    bcb->current_size++;
    bcb->orders_received++;

    // Signal cooks that a new order is ready
    pthread_cond_signal(&(bcb->can_get_orders));
    pthread_mutex_unlock(&(bcb->mutex));

    return 0; 
}

/* Retrieves an order from the queue and signals customers when space becomes available */
Order* GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));

    // Wait until there are orders to retrieve
    while (bcb->current_size == 0) {
        pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
    }

    Order* order = bcb->orders; 
    bcb->orders = bcb->orders->next; 
    bcb->current_size--;
    bcb->orders_handled++;

    // Signal customers that there is now space
    pthread_cond_signal(&(bcb->can_add_orders));
    pthread_mutex_unlock(&(bcb->mutex));

    return order;
}

/* Checks if the restaurant queue is empty */
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == 0;
}

/* Checks if the restaurant queue is full */
bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == bcb->max_size;
}

/* Adds an order to the back of the linked list representing the queue */
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL) {
        *orders = order; 
    } else {
        Order *temp = *orders;
        while (temp->next != NULL) {
            temp = temp->next; 
        }
        temp->next = order; 
    }
}
