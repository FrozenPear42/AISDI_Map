#!/usr/bin/python

import sys
import os
import csv
from matplotlib import pyplot as plt

if len(sys.argv) < 2:
    print "You have to specify file(s)\n"
else:
    for n in xrange(1, len(sys.argv)):
        title = "a"
        results = {}
        current_row = ''
        csv_file = sys.argv[n]
        pre, ext = os.path.splitext(csv_file)
        png_file = pre + ".png"
        plt.clf()
        with open(csv_file, mode='rb') as csv_file:
            reader = csv.reader(csv_file, delimiter=',')
            head = next(reader)
            title = head[0]
            for row in reader:
                if len(row) == 0:
                    continue
                if row[0] == 'N':
                    results[row[1]] = [[], []]
                    current_row = row[1]
                else:
                    results[current_row][0].append(row[0])
                    results[current_row][1].append(row[1])

            plt.title(title)
            plt.ylabel("Time(ms)")
            plt.xlabel("Number of elements")
            for a in results:
                plt.plot(results[a][0], results[a][1], label=a)
            plt.legend()
            plt.grid()
            plt.savefig(png_file)
