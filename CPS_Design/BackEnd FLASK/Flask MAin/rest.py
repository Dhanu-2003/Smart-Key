from flask import Flask, render_template, request
import mysql.connector as msc
from datetime import datetime
import random
import smtplib
from email.mime.text import MIMEText

conn = msc.connect(
    host="127.0.0.1",
    port="3306",
    user="root",
    password="Dhanu@2003",
    database = "keychain"
)

cur = conn.cursor()

app = Flask(__name__)

pen = 0
hol = 0
user = ''
temp = []
data_ = []
total = 0

credential_status = True
email = ''
gen_otp = 0

admin_user = ''


@app.route("/")
def home():
    return "1"

@app.route("/index")
def index():
    return "0"

if __name__ == '__main__':
    app.run(host="0.0.0.0",debug=True,port=5005)
