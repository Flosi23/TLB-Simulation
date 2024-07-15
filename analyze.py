import argparse
import subprocess

# Create the parser
parser = argparse.ArgumentParser(description='Run the C program with varying TLB sizes')

# Add the arguments
parser.add_argument('--cycles', type=int)
parser.add_argument('--blocksize', type=int)
parser.add_argument('--v2b-block-offset', type=int)
parser.add_argument('--tlb-latency', type=int)
parser.add_argument('--memory-latency', type=int)
parser.add_argument('--tf', type=str)
parser.add_argument('--lf', type=str)
parser.add_argument('--rf', type=str)
parser.add_argument('filename', type=str)  # filename is now a positional argument
parser.add_argument('--debug', action='store_true')
parser.add_argument('--from-tlb-size', type=int, required=True)
parser.add_argument('--to-tlb-size', type=int, required=True)

# Parse the arguments
args = parser.parse_args()

# Call the C program with varying TLB sizes
for tlb_size in range(args.from_tlb_size, args.to_tlb_size + 1):
    command = ['./main']
    if args.cycles is not None:
        command.extend(['--cycles', str(args.cycles)])
    if args.blocksize is not None:
        command.extend(['--blocksize', str(args.blocksize)])
    if args.v2b_block_offset is not None:
        command.extend(['--v2b-block-offset', str(args.v2b_block_offset)])
    command.extend(['--tlb-size', str(tlb_size)])
    if args.tlb_latency is not None:
        command.extend(['--tlb-latency', str(args.tlb_latency)])
    if args.memory_latency is not None:
        command.extend(['--memory-latency', str(args.memory_latency)])
    if args.tf is not None:
        command.extend(['--tf', args.tf])
    if args.lf is not None:
        command.extend(['--lf', args.lf])
    if args.rf is not None:
        command.extend(['--rf', args.rf])
    command.append(args.filename)  # filename is a positional argument
    if args.debug:
        command.append('--debug')
    subprocess.run(command)
