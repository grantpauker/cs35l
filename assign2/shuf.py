#!/usr/bin/env python3.10
import random
import argparse

def main():
    # setup argparse
    parser = argparse.ArgumentParser()
    group = parser.add_mutually_exclusive_group()
    group.add_argument(
        "-e", "--echo", action="store_true", help="treat each ARG as an input line"
    )
    group.add_argument(
        "-i",
        "--input-range",
        metavar="LO-HI",
        help="treat each number LO through HI as an input line",
    )
    parser.add_argument(
        "-n", "--head-count", metavar="COUNT", help="output at most COUNT lines"
    )
    parser.add_argument(
        "-r", "--repeat", action="store_true", help="output lines can be repeated"
    )

    args, extra_args = parser.parse_known_args()

    # get head count
    count = -1
    if args.head_count:
        try:
            count = int(args.head_count)
        except ValueError:
            parser.error(f"invalid line count: '{args.head_count}'")

    # get input
    if args.echo:
        lines = extra_args
    elif args.input_range:
        if len(extra_args) > 0:
            parser.error(f"extra operand {extra_args[0]}")
        try:
            match args.input_range.split("-"):
                case [str_low, str_high]:
                    low = int(str_low)
                    high = int(str_high) + 1
                case _:
                    raise ValueError
            if low > high:
                raise ValueError
        except ValueError:
            parser.error(f"invalid input range: '{args.input_range}'")
        lines = [str(i) for i in range(low, high)]
    else:
        if len(extra_args) > 1:
            parser.error(f"extra operand {extra_args[1]}")
        elif not extra_args or extra_args[0] == "-":
            filename = "/dev/stdin"
        else:
            filename = extra_args[0]
        try:
            f = open(filename, "r")
        except FileNotFoundError:
            parser.error(f"{filename}: No such file or directory")
        lines = f.readlines()
        f.close()

    # print out lines
    if args.repeat:
        while count >= 1 or not args.head_count:
            print(random.choice(lines).rstrip())
            count -= 1
    else:
        if not args.head_count or count > len(lines):
            count = len(lines)
        lines = random.sample(lines, k=count)
        for line in lines:
            print(line.rstrip())

if __name__ == "__main__":
    main()
