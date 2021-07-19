import csv

node_ports = [(3, 500), (6, 501), (6, 502), (3, 503), (3, 504), (9, 505)]
with open('task1_trace.csv', mode='r') as csv_file:
    with open('task1_clean.csv', mode='w') as out_csv:
        reader = csv.reader(csv_file, delimiter=' ')
        writer = csv.writer(out_csv, delimiter=' ')
        for row in reader:
            time = row[0]
            node = int(row[1].split(':')[1])
            port = int(row[9])
            packsize = int(row[14].split('(')[0])
            event = row[4]
            if (node, port) in node_ports and event == 'Recv':
                writer.writerow([time, node, port, packsize])
            