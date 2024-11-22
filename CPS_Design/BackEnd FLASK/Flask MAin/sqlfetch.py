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
cur.execute("select date from access")
temp = cur.fetchall()
print(temp)