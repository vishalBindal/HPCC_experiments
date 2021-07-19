import sys

lines = []
with open(sys.argv[1], 'r') as f:
    c = 0
    line = f.readline()
    while line != '':
        c += 1
        if c>=50:
            words = line.split('\n')[0].split(' ')
            if len(words)<2:
                break
            time = int(words[0]) - 2e9
            flow = "H1->H5"
            if words[2] == "0b000201" and words[3] == "0b000301":
                flow = "H1->H2"
            elif words[2] == "0b000401" and words[3] == "0b000301":
                flow = "H3->H2"
            elif words[2] == "0b000401" and words[3] == "0b000601":
                flow = "H3->H5"
            elif words[2] == "0b000801" and words[3] == "0b000301":
                flow = "H7->H2"
            elif words[2] == "0b000501" and words[3] == "0b000901":
                flow = "H4->H8"
            final = [str(int(time)), words[1], flow]
            if len(words)>14:
                final.append(words[-1])
                final.append(words[7].split('(')[0])
                final.append(words[9].split('(')[0])
                if len(words)>20:
                    final.append(words[12].split('(')[0])
                    final.append(words[14].split('(')[0])
                else:
                    final.append('-1')
                    final.append('-1')
                st = " ".join(final)
                lines.append(st)
        line = f.readline()

for line in lines:
    print(line)
            