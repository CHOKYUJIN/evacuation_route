import pymysql
import threading
import time
import algo 

table_name = ["alarm1", "alarm2", "alarm3"]
direction_table = ["direction1", "direction2","direction3"]
table_pos = [(7,7), (6,13), (3,18)]
fire_pos_row = -1
fire_pos_col = -1
fire_flag = False

def lookup_func(table_num) :
    global fire_flag
    global fire_pos_row
    global fire_pos_col
    while(fire_flag == False) :
        db = pymysql.connect(host='localhost', user='root', passwd='capstone!', db='fire_alarm', charset='utf8')
       # print("thread %d is running"%(table_num+1) )
    # db control
        time.sleep(5)
        cursor = db.cursor()
        sql = "SELECT * FROM " + table_name[table_num]
        row_count = cursor.execute(sql)

        if row_count == 0:
            print("empty table!")
        else:    
            result = cursor.fetchall()
       # print(result)
#        if result == None:
  #          return
         
            for row_data in result:
                if(row_data[1] > 70 and row_data[2] > 400) :
                    print(row_data[1],row_data[2])
                    fire_pos_row = table_pos[table_num][0]
                    fire_pos_col = table_pos[table_num][1]
                    fire_flag = True
                    break;
        db.close()
    # while end

    if(fire_flag) : 
        db = pymysql.connect(host='localhost', user='root', passwd='capstone!', db='fire_alarm', charset='utf8')
        print("Sub : %d"%table_num)
        Map = algo.readMap("map.txt");
        Map = algo.fire_recognize(fire_pos_row, fire_pos_col, Map)
        path = algo.bfs(table_pos[table_num][0],table_pos[table_num][1],Map)
        if path != None:
            for i in path:
                print(i)
#            check again
#            sql2 = "INSERT into " + direction_table[table_num] + "dir VALUES(" + str(path[table_pos[table_num][0]][table_pos[table_num][1]])+")"
#            cursor.execute(sql2)
            print(direction_table[table_num]+": "+str(path[table_pos[table_num][0]][table_pos[table_num][1]]))
            if Map[table_pos[table_num][0]][table_pos[table_num][1]] == '#':
                dir2 = path[table_pos[table_num][0]][table_pos[table_num][1]]
                real_dir = -1
                if dir2 == 1:
                    real_dir = path[table_pos[table_num][0]][table_pos[table_num][1]-1]
                elif dir2 == 2:
                    real_dir = path[table_pos[table_num][0]][table_pos[table_num][1]+1]
                elif dir2 == 3:
                    real_dir = path[table_pos[table_num][0]-1][table_pos[table_num][1]]
                else:
                    real_dir = path[table_pos[table_num][0]+1][table_pos[table_num][1]]

                cursor = db.cursor()
                sql = "INSERT INTO "+ direction_table[table_num]+ "(dir) VALUEs("+ str(real_dir)+ ")"
                cursor.execute(sql)
                db.commit()
            else:
                cursor = db.cursor()
                sql = "INSERT INTO "+ direction_table[table_num]+"(dir) VALUES("+str(path[table_pos[table_num][0]][table_pos[table_num][1]]) +")"
                cursor.execute(sql)
                db.commit()
            print("direction is updated")
        else:
            tmp_dir = algo.find_close_point(table_pos[table_num][0],table_pos[table_num][1],Map)
            cursor = db.cursor()
            sql = "INSERT INTO "+ direction_table[table_num]+"(dir) VALUES("+str(tmp_dir) +")"
            cursor.execute(sql)
            db.commit()
            print("direction is updated")
        
        db.close()

if __name__ == "__main__" :
    for i in range(0, 3) :
        t = threading.Thread(target = lookup_func, args = (i, ))
        t.start()
        time.sleep(1)
    print("Main Thread")
