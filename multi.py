import pymysql
import threading
import time
import algo 

table_name = ["test1", "test2", "test3"]
table_pos = [(7,7), (6,13), (3,18)]
fire_pos_row = -1
fire_pos_col = -1
fire_flag = False

def lookup_func(table_num) :
    global fire_flag
    global fire_pos_row
    global fire_pos_col
    db = pymysql.connect(host='localhost', user='root', passwd='capstone!', db='fire_alarm', charset='utf8')
    while(fire_flag == False) :
    # db control
        cursor = db.cursor()
        sql = "SELECT * FROM " + table_name[table_num]
        cursor.execute(sql)
        result = cursor.fetchall()
        
        for row_data in result:
            if(row_data[2] > 60 and row_data[3] > 150) :
                fire_pos_row = table_pos[table_num][0]
                fire_pos_col = table_pos[table_num][1]
                fire_flag = True
                break;
    # while end

    if(fire_flag) :
        print("Sub : %d"%table_num)
        Map = algo.readMap("map.txt");
        Map = algo.fire_recognize(fire_pos_row, fire_pos_col, Map)
        path = algo.bfs(table_pos[table_num][0],table_pos[table_num][1],Map)
        if path != None:
            print("direction: "+str(path[table_pos[table_num][0]][table_pos[table_num][1]]))
    db.close()

if __name__ == "__main__" :
    for i in range(0, 3) :
        t = threading.Thread(target = lookup_func, args = (i, ))
        t.start()
        time.sleep(2)
    print("Main Thread")
