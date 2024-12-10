# Project #2 - Synchronization

### Course: Operating Systems CSCI-401  
### Student Name(s): Caitlyn Lynch (@02995610), Kristian Morgan (@02937236)

---

## Overview

**Project #2** explores key synchronization concepts in operating systems by implementing two interrelated tasks:

- **Part 1: Process Synchronization using Semaphores**  
  A parent and child process coordinate to manage a shared bank account. Semaphores ensure mutual exclusion and prevent race conditions when accessing the shared resource.

- **Part 2: Thread Synchronization using Monitors and Condition Variables**  
  A multithreaded simulation of a restaurant, where customer threads place orders and cook threads fulfill them. Mutexes and condition variables are used to synchronize access to the shared order queue.

---

## Key Features

### Part 1: Process Synchronization
- **Semaphores:** Used to enforce mutual exclusion during deposit and withdrawal operations.
- **Dynamic Behavior:** Randomized delays simulate real-world process interaction.
- **Race Condition Prevention:** Ensures thread-safe access to the shared bank account.

### Part 2: Thread Synchronization
- **Thread Management:** Customers place orders while cooks fulfill them in parallel.
- **Order Queue:** A FIFO (First-In, First-Out) queue ensures proper order processing.
- **Synchronization:** Mutexes and condition variables regulate access to the shared queue.

---

## Acknowledgments
Special thanks to Professor Lee Burge, Ph.D. and Howard University College of Engineering and Architecture for their guidance and support.
