# Concurrent Store

## Introduction and Motivation:
A key-value store is a storage system in which data (a value) is addressed by a key; common examples are Python's dictionaries or Java/C++ hash maps. Unlike file systems or other structures where objects are arranged in a hierarchical directory structure, keys map directly to their values. Key-value stores are popular for large-scale web applications because they are particularly amenable to performance and scalability optimizations, such as concurrent access/modification and sharding. Such optimizations are paramount to the success of Internet-scale applications like Facebook, AirBnB, and Twitter, which must handle thousands of connection requests and petabytes of data every day.

Concurrency is useful because it allows processes to do multiple things seemingly at the same time: for example, a process that is waiting for a slow operation (such as fetching data over the internet) can use the processor for something else until it completes. When a computer has multiple processors, concurrency also allows for parallelism, i.e., the computer actually does multiple things at the same time.

However, concurrency also introduces a new category of undefined behavior: race conditions. A primary goal for any concurrent program is to avoid undefined behavior due to race conditions between threads. A program that is free of such race conditions is known as thread-safe, which means any data shared between threads is protected by appropriate synchronization, in case threads try to access or modify it simultaneously. Some of the most common synchronization primitives are mutexes and condition variables.

## Overview:
This program implements multiple synchronized data structures for a multithreaded backend server to use, first via a simple synchronized key-value store, then a key-value store with fine-grained synchronization. Concurrent Store also features a queue data structure to support synchronized access, which is used by the backend server to accept multiple connections concurrently.

## File Structure and Layout:
- `build/`: Folder for running and compiling the program. To compile, `cd` into the `build/` directory and run `make`. This creates 2 executables: `simple_client` and `server`.
- `tests/` and `test_utils/`: Folders used for testing. To run tests, you must be in the `build/` directory. Then, run either `make check concurrent_store` or `./run_tests concurrent_store` from inside the `build/` directory, which will run all of the Concurrent Store tests.
- `client/` and `repl/`: Contains the clients and relevant support code for interacting with `KvServer` and `Shardmaster`s
- `cmd/`: Contains the runner code that starts the client, server, and shardmaster
- `common/`: Contains the definition of the `Shard` struct, as well as some useful helpers for managing or operating on them. Additionally, this folder contains miscellaneous utility functions, such as color printing and string hashing
- `kvstore/`: Contains the definition of the abstract `KvStore` interface, as well as a simple synchronized key-value store implementation
- `net/`: Contains the networking support code used by the clients and servers
- `server/`: Contains the definition of the `KvServer` class, and supporting data structures
- `shardmaster/`: Contains the definition of the `Shardmaster` class, and support code to process persistent connections

## Key Features:
**- Synchronized Queue:** The synchronized allows multiple threads to add and remove elements safely, and wait for new elements to arrive. It is also useful to distribute requests from different clients across multiple threads.
**- Concurrent, mutlithreaded bucket-based KV store:** Provides safe and concurrent access to key-value pairs through the use of fine-grained locking (via a readers-writer lock) to prevent deadlock. This reduces lock contention by splitting the data structure or code into smaller, potentially disjoint pieces that each have thei rown lock. Thus, multiple threads can then operate on separate pieces in parallel.

## Code:
Available upon request (patrick_li@brown.edu or patrickli2021@gmail.com)
