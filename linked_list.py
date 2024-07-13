import argparse
import csv
import random


def toHex(address):
    return f'{address:#x}'


def generate_unique_heap_addresses(heap_base, heap_size, num_iterations, alignment=16):
    # Calculate the range of addresses
    max_address = heap_base + heap_size

    # Generate all possible aligned addresses within the range
    possible_addresses = list(range(heap_base, max_address, alignment))

    # Check if the number of iterations is feasible
    if num_iterations > len(possible_addresses):
        raise ValueError("Number of iterations exceeds the number of possible unique addresses.")

    # Shuffle the list to randomize
    random.shuffle(possible_addresses)

    # Select the first num_iterations addresses
    heap_addresses = possible_addresses[:num_iterations]

    return heap_addresses


def simulate_memory_accesses(num_iterations, sp_base, heap_base, heap_size, filename):
    memory_accesses = []

    # Initial addresses for stack and heap
    sp = sp_base

    stack = {}
    # Step 1: Initialization
    head_address = sp + 24
    ptr_address = sp + 16
    sum_address = sp + 12
    zero_register = 0

    # generates num_iterations heap addresses that are unique,
    # 16 byte aligned and spread randomly over heap_size bytes of memory
    heap_addresses = generate_unique_heap_addresses(heap_base, heap_size, num_iterations)

    # Initialize the heap data: each node contains data = 1 and next = random address
    heap = {}
    for i in range(num_iterations):
        node_data_address = heap_addresses[i]
        node_next_address = heap_addresses[i + 1] if i + 1 < num_iterations else 0
        heap[node_data_address] = 1
        heap[node_data_address + 8] = node_next_address

    # 1. Initialisierung
    # write head to stack
    memory_accesses.append(['W', toHex(head_address), toHex(heap_addresses[0])])
    stack[head_address] = heap_addresses[0]
    # read head from stack `ptr = head`
    memory_accesses.append(['R', toHex(head_address)])
    # write ptr to stack
    memory_accesses.append(['W', toHex(ptr_address), toHex(heap_addresses[0])])
    stack[ptr_address] = stack[head_address]
    # initialize sum = 0
    memory_accesses.append(['W', toHex(sum_address), toHex(zero_register)])
    stack[sum_address] = zero_register

    # Step 2: Loop
    while True:
        # read `ptr` from stack
        memory_accesses.append(['R', toHex(ptr_address)])
        current_ptr = stack[ptr_address]
        if current_ptr == 0:
            break

        ptr_data_address = current_ptr

        # read `ptr->data` from heap
        memory_accesses.append(['R', toHex(ptr_data_address)])
        data = heap[ptr_data_address]
        # read `sum` from stack
        memory_accesses.append(['R', toHex(sum_address)])
        sum = stack[sum_address]

        # write `sum = sum + ptr->data` to stack
        new_sum = (sum + data)
        memory_accesses.append(['W', toHex(sum_address), toHex(new_sum)])
        stack[sum_address] = new_sum

        ptr_next_address = current_ptr + 8

        # read `ptr->next` from heap
        memory_accesses.append(['R', toHex(ptr_next_address)])
        # write `ptr = ptr->next` to stack
        memory_accesses.append(['W', toHex(ptr_address), toHex(heap[ptr_next_address])])
        stack[ptr_address] = heap[ptr_next_address]

    # Step 3: Loop end
    memory_accesses.append(['R', toHex(sum_address)])

    # Write to CSV file
    with open(f'./examples/{filename}', 'w', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        for access in memory_accesses:
            csv_writer.writerow(access)


def main():
    parser = argparse.ArgumentParser(description="Simulate memory accesses in a linked list.")
    parser.add_argument("--list-size", type=int, default=1000, help="The size of the linked list to simulate.")
    parser.add_argument("--sp-base", type=lambda x: int(x, 0), default=0x0, help="Base address of the stack pointer.")
    # Default heap base is default sp_base + 32 bytes = 0x20
    parser.add_argument("--heap-base", type=lambda x: int(x, 0), default=0x20, help="Base address of the heap.")
    # Default heap size is 256 MiB
    parser.add_argument("--heap-size", type=lambda x: int(x, 0), default=0x10000000, help="Size of the heap in bytes.")
    parser.add_argument("--filename", type=str, default='linked_list',
                        help="Output filename for the CSV (default: 'linked_list').")

    args = parser.parse_args()

    # Check for valid values
    if args.list_size <= 0:
        raise ValueError("num_iterations must be a positive integer.")
    if args.sp_base < 0:
        raise ValueError("sp_base must be a non-negative integer.")
    if args.heap_base < 0:
        raise ValueError("heap_base must be a non-negative integer.")
    if args.heap_size <= 0:
        raise ValueError("heap_size must be a positive integer.")

    # Check that stack and heap do not overlap
    sp_end = args.sp_base + 32  # stack size is 32 bytes
    heap_end = args.heap_base + args.heap_size
    if args.sp_base < heap_end and args.heap_base < sp_end:
        raise ValueError("Stack and heap memory regions overlap.")

    simulate_memory_accesses(args.list_size, args.sp_base, args.heap_base, args.heap_size, args.filename)


if __name__ == "__main__":
    main()
