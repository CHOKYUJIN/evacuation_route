#dx = [-1,-1,-1,0,0,0,1,1,1]
#dy = [-1,0,1,-1,0,1,-1,0,1]
def readMap(filename):
    file = open(filename,'r')
    All = file.read()
    file.close()
    lines = All.split('\n')
    return lines

def fire_recognize(row, col, market):   
#   print(market[row][col])
    if row-1 >= 0 and row+1 < 13:
        if col >= 1 and col <21:
            l = list(market[row-1])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row-1] = ''.join(l)
            l = list(market[row])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row] = ''.join(l)
            l = list(market[row+1])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row+1] = ''.join(l)
        elif col == 0:
            l = list(market[row-1])
            l[col] = '#'
            l[col+1] = '#'
            market[row-1] = ''.join(l)
            l = list(market[row])
            l[col] = '#'
            l[col+1] = '#'
            market[row] = ''.join(l)
            l = list(market[row+1])
            l[col] = '#'
            l[col+1] = '#'
            market[row+1] = ''.join(l)
        else:
            l = list(market[row-1])
            l[col] = '#'
            l[col-1] = '#'
            market[row-1] = ''.join(l)
            l = list(market[row])
            l[col] = '#'
            l[col-1] = '#'
            market[row] = ''.join(l)
            l = list(market[row+1])
            l[col] = '#'
            l[col-1] = '#'
            market[row+1] = ''.join(l)
            
    elif row == 0:
        if col >= 1 and col <21:
            l = list(market[row])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row] = ''.join(l)
            l = list(market[row+1])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row+1] = ''.join(l)
        elif col == 0:
            l = list(market[row])
            l[col] = '#'
            l[col+1] = '#'
            market[row] = ''.join(l)
            l = list(market[row+1])
            l[col] = '#'
            l[col+1] = '#'
            market[row+1] = ''.join(l)
        else:
            l = list(market[row])
            l[col] = '#'
            l[col-1] = '#'
            market[row] = ''.join(l)
            l = list(market[row+1])
            l[col] = '#'
            l[col-1] = '#'
            market[row+1] = ''.join(l)
    else:
        if col >= 1 and col <21:
            l = list(market[row])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row] = ''.join(l)
            l = list(market[row-1])
            l[col-1] = '#'
            l[col] = '#'
            l[col+1] = '#'
            market[row-1] = ''.join(l)
        elif col == 0:
            l = list(market[row])
            l[col] = '#'
            l[col+1] = '#'
            market[row] = ''.join(l)
            l = list(market[row-1])
            l[col] = '#'
            l[col+1] = '#'
            market[row-1] = ''.join(l)
        else:
            l = list(market[row])
            l[col] = '#'
            l[col-1] = '#'
            market[row] = ''.join(l)
            l = list(market[row-1])
            l[col] = '#'
            l[col-1] = '#'
            market[row-1] = ''.join(l)

    return market

def find_path(row,col,path):
    x = row
    y = col
    real_path = [[0]*22 for _ in range(13)]
    real_path[row][col] = 5
    while path[x][y] != -1:
        if path[x][y] == 1:
            real_path[x][y+1] = 1
            y+=1
        elif path[x][y] == 2:
            real_path[x][y-1] = 2
            y-=1
        elif path[x][y] == 3:
            real_path[x+1][y] = 3
            x+=1
        elif path[x][y] == 4:
            real_path[x-1][y] = 4
            x-=1
    return real_path
        

def bfs(row,col,Map):
    arr = []
    visit = [[0] * 22 for _ in range(13)]
    parent = [[(-1,-1)]*22 for _ in range(13)]
    path =[[0] * 22 for _ in range(13)]
    dir = [[0, -1],[0, 1], [-1, 0], [1, 0]]
    arr.append([row, col])
    visit[row][col] = 1

    while arr:
        cur = arr.pop(0)
        x = cur[0]
        y = cur[1]
        if (x == 0 or x == 12 or y == 0 or y == 21) and Map[x][y] == '.':
            path[row][col] = -1
            real_path = find_path(x,y,path)
            return real_path
        for i in range(4):
            wx = x + dir[i][0]
            wy = y + dir[i][1]
            if wx > 12 or wy > 21 or wx < 0 or wy < 0:
                continue
            if visit[wx][wy] == 0 and Map[wx][wy] == '.':
                if i == 0:
                    path[wx][wy] = 1
                elif i == 1:
                    path[wx][wy] = 2
                elif i == 2:
                    path[wx][wy] = 3
                else: 
                    path[wx][wy] = 4
                visit[wx][wy] = 1
             
                arr.append([wx, wy])


if __name__=='__main__':
    Map = readMap('./map.txt')
    Map = fire_recognize(7,7,Map)
    for i in Map:
        print(i)
    path = bfs(9,5,Map)
    if path != None:
        for i in path:
            print(i)
