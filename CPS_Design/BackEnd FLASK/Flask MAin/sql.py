import mysql.connector as msc

conn = msc.connect(
    host="localhost",
    port=3306,
    user="root",
    password="Dhanu@2003",
)

cur = conn.cursor()
l = []
l1 = []
try:
    cur.execute("create database keychain;")
    conn.commit()
except:
    print("database exists")
cur.execute("use keychain;")
cur.execute("create table IF NOT EXISTS speed(value int)")
conn.commit()
cur.execute("create table IF NOT EXISTS location(latitude varchar(50), longitude varchar(50))")
conn.commit()
cur.execute("create table IF NOT EXISTS log_data(start varchar(50), stop varchar(50), duration varchar(50), distance varchar(50), start_loc varchar(50), stop_loc varchar(50), avg_speed varchar(50))")
conn.commit()
cur.execute("create table IF NOT EXISTS access(SNo varchar(10),Date Varchar(50), Times varchar(50))")
conn.commit()

cur.execute("select * from access;")
print(cur.fetchall())

