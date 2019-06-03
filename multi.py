import pymysql
import threading
import time

table_name = ["test1", "test2", "test3"]

def lookup_func(table_num) :
    i = 0
    db = pymysql.connect(host='localhost', user='root', passwd='capstone!', db='fire_alarm', charset='utf8')
    while(i < 2) :
        cursor = db.cursor()
        sql = "SELECT * FROM " + table_name[table_num]
        cursor.execute(sql)
        result = cursor.fetchall()
        for row_data in result:
            print("Sub Thread : %d"%table_num) 
            print("id : " + str(row_data[0]))
            print("time : " + str(row_data[1]))
            print("temperature : " + str(row_data[2]))
            print("smoke : " + str(row_data[3]))
            break;
        i += 1
        time.sleep(1)

    db.close()


if __name__ == "__main__" :

    for i in range(0, 3) :
        t = threading.Thread(target = lookup_func, args = (i, ))
        t.start()
    print("Main Thread")
