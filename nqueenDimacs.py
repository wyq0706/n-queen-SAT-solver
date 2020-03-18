n_list=[8,10,13]
name_list=[30,31,32]

for i in range(len(n_list)):
    n=n_list[i]
    filename = 'D://2017011991//class//software analysis and verification//pa1//dpll//tests//test'+str(name_list[i])+'.dimacs'
    list=[]

    def neg(a):
        return str((-1)*a)

    def pos(a):
        return str(a)

    # one queen in a row
    for i in range(n):
        tmp_list=[]
        for j in range(n):
            index=i*n+j
            tmp_list.append(pos(index+1))
            for k in range(j+1,n):
                index_to=i*n+k
                list.append(' '.join([neg(index+1),neg(index_to+1),pos(0)]))
        tmp_list.append(pos(0))
        list.append(' '.join(tmp_list))

    # one queen in a column
    for i in range(n):
        tmp_list=[]
        for j in range(n):
            index=i+j*n
            tmp_list.append(pos(index+1))
            for k in range(j+1,n):
                index_to=i+k*n
                list.append(' '.join([neg(index+1),neg(index_to+1),pos(0)]))
        tmp_list.append(pos(0))
        list.append(' '.join(tmp_list))

    # one queen in a diagonal line
    for i in range(1,n):
        for j in range(n-1):
            index=i*n+j;
            for k in range(i):
                if k+j+1<n:
                    right_top=(i-1-k)*n+k+j+1
                    list.append(' '.join([neg(index+1),neg(right_top+1),pos(0)]))

    for i in range(0,n-1):
        for j in range(n-1):
            index=i*n+j;
            for k in range(n-i-1):
                if k+j+1<n:
                    right_bottom=(i+1+k)*n+k+j+1
                    list.append(' '.join([neg(index+1),neg(right_bottom+1),pos(0)]))


    with open(filename, 'w') as file_object:
        file_object.write("p cnf "+str(n*n)+' '+str(len(list))+'\n')
        for line in list:
            file_object.write(line+'\n')

