import csv

def generate_test2_1_data_set():
    
    csv_array = []
    for i in range(292):
        csv_array.append("column" + str(i))
    
    with open("test2_1.csv", "w") as f:
        writer = csv.writer(f, quoting=csv.QUOTE_ALL)
        writer.writerow(csv_array)







def main():
    generate_test2_1_data_set()

if __name__ == "__main__":
    main()