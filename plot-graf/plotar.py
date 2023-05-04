import matplotlib.pyplot as plt


sem_heap_x = []
sem_heap_y = []
com_heap_x = []
com_heap_y = []

with open("output.txt","r") as arquivo:
    for line in arquivo:
        line = line.split()
        #print("({},{}),({},{})".format(line[0],line[1],line[0],line[2]))

        sem_heap_x.append(int(line[0]))
        sem_heap_y.append(int(line[1]))

        com_heap_x.append(int(line[0]))
        com_heap_y.append(int(line[2]))

plt.xlabel('Prioridade')
plt.ylabel('Numero de Comparações')

#plt.plot(sem_heap_x,sem_heap_y,'.',color='red',label='Sem heap')
plt.plot(com_heap_x,com_heap_y,'.',color='blue',label='Sem heap')
plt.show()